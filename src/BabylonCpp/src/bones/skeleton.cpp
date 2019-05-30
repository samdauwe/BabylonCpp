#include <babylon/bones/skeleton.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/math/tmp.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

Skeleton::Skeleton(const std::string& iName, const std::string& iId,
                   Scene* scene)
    : needInitialSkinMatrix{false}
    , overrideMesh{nullptr}
    , name{iName}
    , id{iId}
    , _numBonesWithLinkedTransformNode{0}
    , _hasWaitingData{std::nullopt}
    , doNotSerialize{false}
    , useTextureToStoreBoneMatrices{this,
                                    &Skeleton::
                                      get_useTextureToStoreBoneMatrices,
                                    &Skeleton::
                                      set_useTextureToStoreBoneMatrices}
    , animationPropertiesOverride{this,
                                  &Skeleton::get_animationPropertiesOverride,
                                  &Skeleton::set_animationPropertiesOverride}
    , isUsingTextureForMatrices{this, &Skeleton::get_isUsingTextureForMatrices}
    , uniqueId{this, &Skeleton::get_uniqueId}
    , _isDirty{true}
    , _transformMatrixTexture{nullptr}
    , _identity{Matrix::Identity()}
    , _lastAbsoluteTransformsUpdateId{-1}
    , _canUseTextureForBones{false}
    , _uniqueId{0}
    , _useTextureToStoreBoneMatrices{true}
    , _animationPropertiesOverride{nullptr}
{
  bones.clear();

  _scene    = scene ? scene : Engine::LastCreatedScene();
  _uniqueId = _scene->getUniqueId();

  // make sure it will recalculate the matrix next time prepare is called.
  _isDirty = true;

  const auto& engineCaps = _scene->getEngine()->getCaps();
  _canUseTextureForBones
    = engineCaps.textureFloat && engineCaps.maxVertexTextureImageUnits > 0;
}

Skeleton::~Skeleton()
{
}

Type Skeleton::type() const
{
  return Type::SKELETON;
}

void Skeleton::addToScene(const SkeletonPtr& newSkeleton)
{
  _scene->skeletons.emplace_back(newSkeleton);
}

const std::string Skeleton::getClassName() const
{
  return "Skeleton";
}

std::vector<BonePtr> Skeleton::getChildren() const
{
  std::vector<BonePtr> children;
  for (const auto& b : bones) {
    if (!b->getParent()) {
      children.emplace_back(b);
    }
  }
  return children;
}

bool Skeleton::get_useTextureToStoreBoneMatrices() const
{
  return _useTextureToStoreBoneMatrices;
}

void Skeleton::set_useTextureToStoreBoneMatrices(bool value)
{
  _useTextureToStoreBoneMatrices = value;
  _markAsDirty();
}

AnimationPropertiesOverride*& Skeleton::get_animationPropertiesOverride()
{
  if (!_animationPropertiesOverride) {
    return _scene->animationPropertiesOverride;
  }
  return _animationPropertiesOverride;
}

void Skeleton ::set_animationPropertiesOverride(
  AnimationPropertiesOverride* const& value)
{
  _animationPropertiesOverride = value;
}

bool Skeleton::get_isUsingTextureForMatrices() const
{
  return useTextureToStoreBoneMatrices && _canUseTextureForBones
         && !needInitialSkinMatrix;
}

size_t Skeleton::get_uniqueId() const
{
  return _uniqueId;
}

// Members
Float32Array& Skeleton::getTransformMatrices(AbstractMesh* mesh)
{
  if (needInitialSkinMatrix && !mesh->_bonesTransformMatrices.empty()) {
    return mesh->_bonesTransformMatrices;
  }

  if (_transformMatrices.empty()) {
    prepare();
  }

  return _transformMatrices;
}

RawTexturePtr& Skeleton::getTransformMatrixTexture()
{
  return _transformMatrixTexture;
}

Scene* Skeleton::getScene()
{
  return _scene;
}

std::string Skeleton::toString(bool fullDetails)
{
  std::ostringstream oss;
  oss << "Name: " << name << ", nBones: " << bones.size();
  oss << ", nAnimationRanges: ";
  if (_ranges.empty()) {
    oss << "none";
  }
  else {
    oss << _ranges.size();
  }
  if (fullDetails) {
    oss << ", Ranges: {";
    bool first = true;
    for (auto& item : _ranges) {
      if (!first) {
        oss << ", ";
        first = false;
      }
      oss << item.first;
    }
    oss << "}";
  }
  return oss.str();
}

int Skeleton::getBoneIndexByName(const std::string& _name)
{
  for (size_t boneIndex = 0, cache = bones.size(); boneIndex < cache;
       ++boneIndex) {
    if (bones[boneIndex]->name == _name) {
      return static_cast<int>(boneIndex);
    }
  }
  return -1;
}

void Skeleton::createAnimationRange(const std::string& _name, float from,
                                    float to)
{
  // check name not already in use
  if (!stl_util::contains(_ranges, _name)) {
    _ranges[_name] = AnimationRange(_name, from, to);
    for (auto& bone : bones) {
      if (!bone->animations.empty() && (bone->animations[0] != nullptr)) {
        bone->animations[0]->createRange(_name, from, to);
      }
    }
  }
}

void Skeleton::deleteAnimationRange(const std::string& _name, bool deleteFrames)
{
  for (auto& bone : bones) {
    if (!bone->animations.empty() && (bone->animations[0] != nullptr)) {
      bone->animations[0]->deleteRange(_name, deleteFrames);
    }
  }
  _ranges.erase(_name);
}

AnimationRange* Skeleton::getAnimationRange(const std::string& _name)
{
  if (!stl_util::contains(_ranges, _name)) {
    return &_ranges[_name];
  }

  return nullptr;
}

std::vector<AnimationRange> Skeleton::getAnimationRanges()
{
  std::vector<AnimationRange> animationRanges;
  for (const auto& range : _ranges) {
    animationRanges.emplace_back(range.second);
  }
  return animationRanges;
}

bool Skeleton::copyAnimationRange(Skeleton* source, const std::string& _name,
                                  bool rescaleAsRequired)
{
  if (stl_util::contains(_ranges, _name) || !source->getAnimationRange(_name)) {
    return false;
  }
  bool ret         = true;
  auto frameOffset = _getHighestAnimationFrame() + 1;

  // make a dictionary of source skeleton's bones, so exact same order or
  // doublely nested loop is not required
  std::unordered_map<std::string, Bone*> boneDict;
  auto& sourceBones = source->bones;
  for (auto& bone : sourceBones) {
    boneDict[bone->name] = bone.get();
  }

  if (bones.size() != sourceBones.size()) {
    BABYLON_LOGF_WARN(
      "Skeleton",
      "copyAnimationRange: this rig has %zu bones, while source as %zu",
      bones.size(), sourceBones.size())
    ret = false;
  }

  Vector3 skelDimensionsRatio;
  bool hasSkelDimensionsRatio = false;
  if (rescaleAsRequired && dimensionsAtRest && source->dimensionsAtRest) {
    dimensionsAtRest->divide(*source->dimensionsAtRest);
  }

  for (auto& bone : bones) {
    if (stl_util::contains(boneDict, bone->name)) {
      ret = ret
            && bone->copyAnimationRange(
              boneDict[bone->name], _name, static_cast<int>(frameOffset),
              rescaleAsRequired, skelDimensionsRatio, hasSkelDimensionsRatio);
    }
    else {
      BABYLON_LOGF_WARN(
        "Skeleton", "copyAnimationRange: not same rig, missing source bone %s",
        bone->name.c_str())
      ret = false;
    }
  }
  // do not call createAnimationRange(), since it also is done to bones, which
  // was already done
  auto range = source->getAnimationRange(_name);
  if (range) {
    _ranges[_name] = AnimationRange(_name, range->from + frameOffset,
                                    range->to + frameOffset);
  }
  return ret;
}

void Skeleton::returnToRest()
{
  for (auto& bone : bones) {
    bone->returnToRest();
  }
}

float Skeleton::_getHighestAnimationFrame()
{
  float ret = 0.f;
  for (auto& bone : bones) {
    if (!bone->animations.empty() && bone->animations[0]) {
      auto highest = bone->animations[0]->getHighestFrame();
      if (ret < highest) {
        ret = highest;
      }
    }
  }
  return ret;
}

Animatable*
Skeleton::beginAnimation(const std::string& _name, bool /*loop*/,
                         float /*speedRatio*/,
                         const std::function<void()>& /*onAnimationEnd*/)
{
  auto range = getAnimationRange(_name);

  if (!range) {
    return nullptr;
  }

  // return _scene->beginAnimation(dynamic_cast<IAnimatable*>(this),
  // range->from,
  //                              range->to, loop, speedRatio, onAnimationEnd);
  return nullptr;
}

// Methods
void Skeleton::_markAsDirty()
{
  _isDirty = true;
}

void Skeleton::_registerMeshWithPoseMatrix(AbstractMesh* mesh)
{
  _meshesWithPoseMatrix.emplace_back(mesh);
}

void Skeleton::_unregisterMeshWithPoseMatrix(AbstractMesh* mesh)
{
  stl_util::erase(_meshesWithPoseMatrix, mesh);
}

void Skeleton::_computeTransformMatrices(
  Float32Array& targetMatrix, const std::optional<Matrix>& initialSkinMatrix)
{
  onBeforeComputeObservable.notifyObservers(this);

  unsigned int index = 0;
  for (const auto& bone : bones) {
    ++bone->_childUpdateId;
    auto parentBone = bone->getParent();

    if (parentBone) {
      bone->getLocalMatrix().multiplyToRef(parentBone->getWorldMatrix(),
                                           bone->getWorldMatrix());
    }
    else {
      if (initialSkinMatrix.has_value()) {
        bone->getLocalMatrix().multiplyToRef(*initialSkinMatrix,
                                             bone->getWorldMatrix());
      }
      else {
        bone->getWorldMatrix().copyFrom(bone->getLocalMatrix());
      }
    }

    if (!bone->_index.has_value() || *bone->_index != -1) {
      auto mappedIndex = !bone->_index.has_value() ?
                           index :
                           static_cast<unsigned int>(*bone->_index);
      bone->getInvertedAbsoluteTransform().multiplyToArray(
        bone->getWorldMatrix(), targetMatrix, mappedIndex * 16);
    }
    ++index;
  }

  _identity.copyToArray(targetMatrix,
                        static_cast<unsigned int>(bones.size()) * 16);
}

void Skeleton::prepare()
{
  // Update the local matrix of bones with linked transform nodes.
  if (_numBonesWithLinkedTransformNode > 0) {
    for (const auto& bone : bones) {
      if (bone->_linkedTransformNode) {
        // Computing the world matrix also computes the local matrix.
        bone->_linkedTransformNode->computeWorldMatrix();
        bone->_matrix = bone->_linkedTransformNode->_localMatrix;
        bone->markAsDirty();
      }
    }
  }

  if (!_isDirty) {
    return;
  }

  if (needInitialSkinMatrix) {
    for (auto& mesh : _meshesWithPoseMatrix) {

      auto poseMatrix = mesh->getPoseMatrix();

      if (mesh->_bonesTransformMatrices.size() != 16 * (bones.size() + 1)) {
        mesh->_bonesTransformMatrices.resize(16 * (bones.size() + 1));
      }

      if (_synchronizedWithMesh != mesh) {
        _synchronizedWithMesh = mesh;
        // Prepare bones
        for (auto& bone : bones) {
          if (!bone->getParent()) {
            auto& tmpMatrix = Tmp::MatrixArray[0];
            auto& matrix    = bone->getBaseMatrix();
            matrix.multiplyToRef(poseMatrix, tmpMatrix);
            bone->_updateDifferenceMatrix(tmpMatrix);
          }
        }
      }

      _computeTransformMatrices(mesh->_bonesTransformMatrices, poseMatrix);
    }
  }
  else {
    if (_transformMatrices.size() != 16 * (bones.size() + 1)) {
      _transformMatrices.resize(16 * (bones.size() + 1));

      if (isUsingTextureForMatrices) {
        if (_transformMatrixTexture) {
          _transformMatrixTexture->dispose();
        }

        _transformMatrixTexture = RawTexture::CreateRGBATexture(
          _transformMatrices, static_cast<int>((bones.size() + 1) * 4), 1,
          _scene, false, false, Constants::TEXTURE_NEAREST_SAMPLINGMODE,
          Constants::TEXTURETYPE_FLOAT);
      }
    }

    _computeTransformMatrices(_transformMatrices);

    if (isUsingTextureForMatrices && _transformMatrixTexture) {
      _transformMatrixTexture->update(_transformMatrices);
    }
  }

  _isDirty = false;

  _scene->_activeBones.addCount(bones.size(), false);
}

std::vector<IAnimatablePtr> Skeleton::getAnimatables()
{
  if (_animatables.size() != bones.size()) {
    _animatables.clear();

    for (auto& bone : bones) {
      _animatables.emplace_back(bone);
    }
  }

  return _animatables;
}

std::vector<AnimationPtr> Skeleton::getAnimations()
{
  return std::vector<AnimationPtr>();
}

std::unique_ptr<Skeleton> Skeleton::clone(const std::string& /*iName*/,
                                          const std::string& /*iId*/) const
{
  /*auto result = Skeleton::New(name, id, this->_scene);

  for (auto source : bones) {
    Bone* parentBone = nullptr;

    if (source->getParent()) {
      auto parentIndex
        = find(begin(bones), end(bones), source->getParent()) - begin(bones);
      parentBone = result->bones[parentIndex];
    }

    auto bone
      = Bone::New(source->name, result, parentBone, source->getBaseMatrix());
    Tools::DeepCopy(source->animations, bone->animations);
  }

  return result;*/

  return nullptr;
}

void Skeleton::enableBlending(float blendingSpeed)
{
  for (const auto& bone : bones) {
    for (const auto& animation : bone->animations) {
      animation->enableBlending = true;
      animation->blendingSpeed  = blendingSpeed;
    }
  }
}

void Skeleton::dispose(bool /*doNotRecurse*/,
                       bool /*disposeMaterialAndTextures*/)
{
  _meshesWithPoseMatrix.clear();

  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  getScene()->removeSkeleton(this);

  if (_transformMatrixTexture) {
    _transformMatrixTexture->dispose();
    _transformMatrixTexture = nullptr;
  }
}

json Skeleton::serialize() const
{
  return json();
}

SkeletonPtr Skeleton::Parse(const json& parsedSkeleton, Scene* scene)
{
  auto skeleton = Skeleton::New(
    json_util::get_string(parsedSkeleton, "name"),
    std::to_string(json_util::get_number(parsedSkeleton, "id", -1)), scene);
  if (json_util::has_valid_key_value(parsedSkeleton, "dimensionsAtRest")) {
    skeleton->dimensionsAtRest = std::make_unique<Vector3>(Vector3::FromArray(
      json_util::get_array<float>(parsedSkeleton, "dimensionsAtRest")));
  }

  skeleton->needInitialSkinMatrix
    = json_util::get_bool(parsedSkeleton, "needInitialSkinMatrix");

  for (const auto& parsedBone :
       json_util::get_array<json>(parsedSkeleton, "bones")) {
    BonePtr parentBone = nullptr;
    if (json_util::get_number<int>(parsedBone, "parentBoneIndex", -1) > -1) {
      auto parentBoneIndex = static_cast<size_t>(
        json_util::get_number<int>(parsedBone, "parentBoneIndex", -1));
      parentBone = skeleton->bones[parentBoneIndex];
    }
    std::optional<Matrix> rest = std::nullopt;
    if (json_util::has_key(parsedBone, "rest")
        && parsedBone["rest"].is_array()) {
      rest = Matrix::FromArray(json_util::get_array<float>(parsedBone, "rest"));
    }

    auto bone = Bone::New(
      json_util::get_string(parsedBone, "name"), skeleton.get(),
      parentBone.get(),
      Matrix::FromArray(json_util::get_array<float>(parsedBone, "matrix")),
      rest);

    if (json_util::has_valid_key_value(parsedBone, "id")) {
      bone->id = json_util::get_string(parsedBone, "id");
    }

    if (json_util::has_valid_key_value(parsedBone, "length")) {
      bone->length = json_util::get_number<float>(parsedBone, "length");
    }

    if (json_util::has_valid_key_value(parsedBone, "metadata")) {
      bone->metadata = parsedBone["metadata"];
    }

    if (json_util::has_valid_key_value(parsedBone, "animation")) {
      bone->animations.emplace_back(Animation::Parse(parsedBone["animation"]));
    }

    if (json_util::has_valid_key_value(parsedBone, "linkedTransformNodeId")) {
      skeleton->_hasWaitingData = true;
      bone->_waitingTransformNodeId
        = json_util::get_string(parsedBone, "linkedTransformNodeId");
    }
  }

  // placed after bones, so createAnimationRange can cascade down
  if (json_util::has_key(parsedSkeleton, "ranges")) {
    for (const auto& data :
         json_util::get_array<json>(parsedSkeleton, "ranges")) {
      skeleton->createAnimationRange(json_util::get_string(data, "name"),
                                     json_util::get_number<float>(data, "from"),
                                     json_util::get_number<float>(data, "to"));
    }
  }
  return skeleton;
}

void Skeleton::computeAbsoluteTransforms(bool forceUpdate)
{
  auto renderId = _scene->getRenderId();

  if ((_lastAbsoluteTransformsUpdateId != renderId || forceUpdate)
      && (!bones.empty())) {
    bones[0]->computeAbsoluteTransforms();
    _lastAbsoluteTransformsUpdateId = renderId;
  }
}

Matrix* Skeleton::getPoseMatrix() const
{
  Matrix* poseMatrix = nullptr;

  if (!_meshesWithPoseMatrix.empty()) {
    poseMatrix = &_meshesWithPoseMatrix[0]->getPoseMatrix();
  }

  return poseMatrix;
}

void Skeleton::sortBones()
{
  std::vector<BonePtr> _bones;
  std::vector<bool> visited(bones.size());
  for (unsigned int index = 0; index < bones.size(); ++index) {
    _sortBones(index, _bones, visited);
  }

  bones = _bones;
}

void Skeleton::_sortBones(unsigned int index, std::vector<BonePtr>& iBones,
                          std::vector<bool>& visited)
{
  if (visited[index]) {
    return;
  }

  visited[index] = true;

  auto& bone = bones[index];
  if (!bone->_index.has_value()) {
    bone->_index = static_cast<int>(index);
  }

  const auto boneIndexOf = [this](Bone* iBone) {
    auto it
      = std::find_if(bones.begin(), bones.end(), [&iBone](const BonePtr& bone) {
          return bone.get() == iBone;
        });
    if (it != bones.end()) {
      return static_cast<int>(it - bones.begin());
    }
    return -1;
  };

  const auto& parentBone = bone->getParent();
  if (parentBone) {
    auto _index = boneIndexOf(parentBone);
    if (_index >= 0) {
      _sortBones(static_cast<unsigned int>(_index), iBones, visited);
    }
  }

  iBones.emplace_back(bone);
}

} // end of namespace BABYLON
