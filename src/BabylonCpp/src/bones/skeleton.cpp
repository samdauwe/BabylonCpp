#include <babylon/bones/skeleton.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

Skeleton::Skeleton(const std::string& iName, const std::string& iId, Scene* scene)
    : needInitialSkinMatrix{false}
    , overrideMesh{nullptr}
    , name{iName}
    , id{iId}
    , _numBonesWithLinkedTransformNode{0}
    , _hasWaitingData{std::nullopt}
    , doNotSerialize{false}
    , useTextureToStoreBoneMatrices{this, &Skeleton::get_useTextureToStoreBoneMatrices,
                                    &Skeleton::set_useTextureToStoreBoneMatrices}
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
  _canUseTextureForBones = engineCaps.textureFloat && engineCaps.maxVertexTextureImageUnits > 0;
}

Skeleton::~Skeleton() = default;

Type Skeleton::type() const
{
  return Type::SKELETON;
}

void Skeleton::addToScene(const SkeletonPtr& newSkeleton)
{
  _scene->skeletons.emplace_back(newSkeleton);
}

std::string Skeleton::getClassName() const
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

AnimationPropertiesOverridePtr& Skeleton::get_animationPropertiesOverride()
{
  if (!_animationPropertiesOverride) {
    return _scene->animationPropertiesOverride;
  }
  return _animationPropertiesOverride;
}

void Skeleton ::set_animationPropertiesOverride(const AnimationPropertiesOverridePtr& value)
{
  _animationPropertiesOverride = value;
}

bool Skeleton::get_isUsingTextureForMatrices() const
{
  return useTextureToStoreBoneMatrices && _canUseTextureForBones;
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

RawTexturePtr& Skeleton::getTransformMatrixTexture(AbstractMesh* mesh)
{
  if (needInitialSkinMatrix && mesh->_transformMatrixTexture) {
    return mesh->_transformMatrixTexture;
  }

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
    for (const auto& item : _ranges) {
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

int Skeleton::getBoneIndexByName(const std::string& iName)
{
  for (size_t boneIndex = 0, cache = bones.size(); boneIndex < cache; ++boneIndex) {
    if (bones[boneIndex]->name == iName) {
      return static_cast<int>(boneIndex);
    }
  }
  return -1;
}

void Skeleton::createAnimationRange(const std::string& iName, float from, float to)
{
  // check name not already in use
  if (!stl_util::contains(_ranges, iName)) {
    _ranges[iName] = std::make_shared<AnimationRange>(iName, from, to);
    for (const auto& bone : bones) {
      if (!bone->animations.empty() && (bone->animations[0] != nullptr)) {
        bone->animations[0]->createRange(iName, from, to);
      }
    }
  }
}

void Skeleton::deleteAnimationRange(const std::string& iName, bool deleteFrames)
{
  for (const auto& bone : bones) {
    if (!bone->animations.empty() && (bone->animations[0] != nullptr)) {
      bone->animations[0]->deleteRange(iName, deleteFrames);
    }
  }
  _ranges.erase(iName);
}

AnimationRangePtr Skeleton::getAnimationRange(const std::string& iName)
{
  if (!stl_util::contains(_ranges, iName)) {
    return _ranges[iName];
  }

  return nullptr;
}

std::vector<AnimationRangePtr> Skeleton::getAnimationRanges()
{
  std::vector<AnimationRangePtr> animationRanges;
  for (const auto& range : _ranges) {
    animationRanges.emplace_back(range.second);
  }
  return animationRanges;
}

bool Skeleton::copyAnimationRange(Skeleton* source, const std::string& iName,
                                  bool rescaleAsRequired)
{
  if (stl_util::contains(_ranges, iName) || !source->getAnimationRange(iName)) {
    return false;
  }
  auto ret         = true;
  auto frameOffset = _getHighestAnimationFrame() + 1;

  // make a dictionary of source skeleton's bones, so exact same order or
  // doublely nested loop is not required
  std::unordered_map<std::string, Bone*> boneDict;
  auto& sourceBones = source->bones;
  for (const auto& bone : sourceBones) {
    boneDict[bone->name] = bone.get();
  }

  if (bones.size() != sourceBones.size()) {
    BABYLON_LOGF_WARN("Skeleton", "copyAnimationRange: this rig has %zu bones, while source as %zu",
                      bones.size(), sourceBones.size())
    ret = false;
  }

  std::optional<Vector3> skelDimensionsRatio = std::nullopt;
  if (rescaleAsRequired && dimensionsAtRest && source->dimensionsAtRest) {
    skelDimensionsRatio = dimensionsAtRest->divide(*source->dimensionsAtRest);
  }

  for (const auto& bone : bones) {
    if (stl_util::contains(boneDict, bone->name)) {
      ret = ret
            && bone->copyAnimationRange(boneDict[bone->name], iName, static_cast<int>(frameOffset),
                                        rescaleAsRequired, skelDimensionsRatio);
    }
    else {
      BABYLON_LOGF_WARN("Skeleton", "copyAnimationRange: not same rig, missing source bone %s",
                        bone->name.c_str())
      ret = false;
    }
  }
  // do not call createAnimationRange(), since it also is done to bones, which was already done
  auto range = source->getAnimationRange(iName);
  if (range) {
    _ranges[iName]
      = std::make_shared<AnimationRange>(iName, range->from + frameOffset, range->to + frameOffset);
  }
  return ret;
}

void Skeleton::returnToRest()
{
  for (const auto& bone : bones) {
    bone->returnToRest();
  }
}

float Skeleton::_getHighestAnimationFrame()
{
  auto ret = 0.f;
  for (const auto& bone : bones) {
    if (!bone->animations.empty() && bone->animations[0]) {
      auto highest = bone->animations[0]->getHighestFrame();
      if (ret < highest) {
        ret = highest;
      }
    }
  }
  return ret;
}

Animatable* Skeleton::beginAnimation(const std::string& iName, bool /*loop*/, float /*speedRatio*/,
                                     const std::function<void()>& /*onAnimationEnd*/)
{
  auto range = getAnimationRange(iName);

  if (!range) {
    return nullptr;
  }

  // return _scene->beginAnimation(dynamic_cast<IAnimatable*>(this),
  // range->from,
  //                              range->to, loop, speedRatio, onAnimationEnd);
  return nullptr;
}

SkeletonPtr Skeleton::MakeAnimationAdditive(const SkeletonPtr& skeleton, int referenceFrame,
                                            const std::string& range)
{
  auto rangeValue = skeleton->getAnimationRange(/*name*/ range);

  // We can't make a range additive if it doesn't exist
  if (!rangeValue) {
    return nullptr;
  }

  // Find any current scene-level animatable belonging to the target that matches the range
  auto sceneAnimatables         = skeleton->_scene->getAllAnimatablesByTarget(skeleton);
  AnimatablePtr rangeAnimatable = nullptr;

  for (const auto& sceneAnimatable : sceneAnimatables) {
    if (sceneAnimatable->fromFrame == rangeValue->from
        && sceneAnimatable->toFrame == rangeValue->to) {
      rangeAnimatable = sceneAnimatable;
      break;
    }
  }

  // Convert the animations belonging to the skeleton to additive keyframes
  auto animatables = skeleton->getAnimatables();

  for (const auto& animatable : animatables) {
    auto animations = animatable->getAnimations();

    if (animations.empty()) {
      continue;
    }

    for (const auto& animation : animations) {
      Animation::MakeAnimationAdditive(animation, referenceFrame, range);
    }
  }

  // Mark the scene-level animatable as additive
  if (rangeAnimatable) {
    rangeAnimatable->isAdditive = true;
  }

  return skeleton;
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

void Skeleton::_computeTransformMatrices(Float32Array& targetMatrix,
                                         const std::optional<Matrix>& initialSkinMatrix)
{
  onBeforeComputeObservable.notifyObservers(this);

  unsigned int index = 0;
  for (const auto& bone : bones) {
    ++bone->_childUpdateId;
    auto parentBone = bone->getParent();

    if (parentBone) {
      bone->getLocalMatrix().multiplyToRef(parentBone->getWorldMatrix(), bone->getWorldMatrix());
    }
    else {
      if (initialSkinMatrix.has_value()) {
        bone->getLocalMatrix().multiplyToRef(*initialSkinMatrix, bone->getWorldMatrix());
      }
      else {
        bone->getWorldMatrix().copyFrom(bone->getLocalMatrix());
      }
    }

    if (!bone->_index.has_value() || *bone->_index != -1) {
      auto mappedIndex
        = !bone->_index.has_value() ? index : static_cast<unsigned int>(*bone->_index);
      bone->getInvertedAbsoluteTransform().multiplyToArray(bone->getWorldMatrix(), targetMatrix,
                                                           mappedIndex * 16);
    }
    ++index;
  }

  _identity.copyToArray(targetMatrix, static_cast<unsigned int>(bones.size()) * 16);
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
    for (const auto& mesh : _meshesWithPoseMatrix) {

      auto poseMatrix = mesh->getPoseMatrix();

      if (mesh->_bonesTransformMatrices.size() != 16 * (bones.size() + 1)) {
        mesh->_bonesTransformMatrices.resize(16 * (bones.size() + 1));
      }

      if (_synchronizedWithMesh != mesh) {
        _synchronizedWithMesh = mesh;
        // Prepare bones
        for (const auto& bone : bones) {
          if (!bone->getParent()) {
            auto& tmpMatrix = TmpVectors::MatrixArray[0];
            auto& matrix    = bone->getBaseMatrix();
            matrix.multiplyToRef(poseMatrix, tmpMatrix);
            bone->_updateDifferenceMatrix(tmpMatrix);
          }
        }

        if (isUsingTextureForMatrices()) {
          const auto textureWidth = static_cast<int>((bones.size() + 1) * 4);
          if (!mesh->_transformMatrixTexture
              || mesh->_transformMatrixTexture->getSize().width != textureWidth) {

            if (mesh->_transformMatrixTexture) {
              mesh->_transformMatrixTexture->dispose();
            }

            mesh->_transformMatrixTexture = RawTexture::CreateRGBATexture(
              mesh->_bonesTransformMatrices, static_cast<int>((bones.size() + 1) * 4), 1, _scene,
              false, false, Constants::TEXTURE_NEAREST_SAMPLINGMODE, Constants::TEXTURETYPE_FLOAT);
          }
        }
      }

      _computeTransformMatrices(mesh->_bonesTransformMatrices, poseMatrix);

      if (isUsingTextureForMatrices && mesh->_transformMatrixTexture) {
        mesh->_transformMatrixTexture->update(mesh->_bonesTransformMatrices);
      }
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
          _transformMatrices, static_cast<int>((bones.size() + 1) * 4), 1, _scene, false, false,
          Constants::TEXTURE_NEAREST_SAMPLINGMODE, Constants::TEXTURETYPE_FLOAT);
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

    for (const auto& bone : bones) {
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

void Skeleton::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
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
  auto skeleton
    = Skeleton::New(json_util::get_string(parsedSkeleton, "name"),
                    std::to_string(json_util::get_number(parsedSkeleton, "id", -1)), scene);
  if (json_util::has_valid_key_value(parsedSkeleton, "dimensionsAtRest")) {
    skeleton->dimensionsAtRest = std::make_unique<Vector3>(
      Vector3::FromArray(json_util::get_array<float>(parsedSkeleton, "dimensionsAtRest")));
  }

  skeleton->needInitialSkinMatrix = json_util::get_bool(parsedSkeleton, "needInitialSkinMatrix");

  for (const auto& parsedBone : json_util::get_array<json>(parsedSkeleton, "bones")) {
    BonePtr parentBone = nullptr;
    if (json_util::get_number<int>(parsedBone, "parentBoneIndex", -1) > -1) {
      auto parentBoneIndex
        = static_cast<size_t>(json_util::get_number<int>(parsedBone, "parentBoneIndex", -1));
      parentBone = skeleton->bones[parentBoneIndex];
    }
    std::optional<Matrix> rest = std::nullopt;
    if (json_util::has_key(parsedBone, "rest") && parsedBone["rest"].is_array()) {
      rest = Matrix::FromArray(json_util::get_array<float>(parsedBone, "rest"));
    }

    auto bone
      = Bone::New(json_util::get_string(parsedBone, "name"), skeleton.get(), parentBone.get(),
                  Matrix::FromArray(json_util::get_array<float>(parsedBone, "matrix")), rest);

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
      skeleton->_hasWaitingData     = true;
      bone->_waitingTransformNodeId = json_util::get_string(parsedBone, "linkedTransformNodeId");
    }
  }

  // placed after bones, so createAnimationRange can cascade down
  if (json_util::has_key(parsedSkeleton, "ranges")) {
    for (const auto& data : json_util::get_array<json>(parsedSkeleton, "ranges")) {
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

  if ((_lastAbsoluteTransformsUpdateId != renderId || forceUpdate) && (!bones.empty())) {
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
    auto it = std::find_if(bones.begin(), bones.end(),
                           [&iBone](const BonePtr& bone) { return bone.get() == iBone; });
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
