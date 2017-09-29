#include <babylon/bones/skeleton.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

Skeleton::Skeleton(const string_t& iName, const string_t& iId, Scene* scene)
    : needInitialSkinMatrix{false}
    , name{iName}
    , id{iId}
    , _scene{scene}
    , _isDirty{true}
    , _identity{Matrix::Identity()}
    , _lastAbsoluteTransformsUpdateId{-1}
{
  bones.clear();
  scene->skeletons.emplace_back(this);
  // make sure it will recalculate the matrix next time prepare is called.
  _isDirty = true;
}

Skeleton::~Skeleton()
{
}

IReflect::Type Skeleton::type() const
{
  return IReflect::Type::SKELETON;
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

Scene* Skeleton::getScene()
{
  return _scene;
}

string_t Skeleton::toString(bool fullDetails)
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

int Skeleton::getBoneIndexByName(const string_t& _name)
{
  for (size_t boneIndex = 0, cache = bones.size(); boneIndex < cache;
       ++boneIndex) {
    if (bones[boneIndex]->name == _name) {
      return static_cast<int>(boneIndex);
    }
  }
  return -1;
}

void Skeleton::createAnimationRange(const string_t& _name, float from, float to)
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

void Skeleton::deleteAnimationRange(const string_t& _name, bool deleteFrames)
{
  for (auto& bone : bones) {
    if (!bone->animations.empty() && (bone->animations[0] != nullptr)) {
      bone->animations[0]->deleteRange(_name, deleteFrames);
    }
  }
  _ranges.erase(_name);
}

AnimationRange* Skeleton::getAnimationRange(const string_t& _name)
{
  if (!stl_util::contains(_ranges, _name)) {
    return &_ranges[_name];
  }

  return nullptr;
}

vector_t<AnimationRange> Skeleton::getAnimationRanges()
{
  vector_t<AnimationRange> animationRanges;
  for (const auto& range : _ranges) {
    animationRanges.emplace_back(range.second);
  }
  return animationRanges;
}

bool Skeleton::copyAnimationRange(Skeleton* source, const string_t& _name,
                                  bool rescaleAsRequired)
{
  if (stl_util::contains(_ranges, _name) || !source->getAnimationRange(_name)) {
    return false;
  }
  bool ret        = true;
  int frameOffset = _getHighestAnimationFrame() + 1;

  // make a dictionary of source skeleton's bones, so exact same order or
  // doublely nested loop is not required
  unordered_map_t<string_t, Bone*> boneDict;
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
                 boneDict[bone->name], _name, frameOffset, rescaleAsRequired,
                 skelDimensionsRatio, hasSkelDimensionsRatio);
    }
    else {
      BABYLON_LOGF_WARN(
        "Skeleton", "copyAnimationRange: not same rig, missing source bone %s",
        bone->name.c_str());
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

int Skeleton::_getHighestAnimationFrame()
{
  int ret = 0;
  for (auto& bone : bones) {
    if (!bone->animations.empty() && bone->animations[0]) {
      int highest = bone->animations[0]->getHighestFrame();
      if (ret < highest) {
        ret = highest;
      }
    }
  }
  return ret;
}

Animatable*
Skeleton::beginAnimation(const string_t& _name, bool /*loop*/,
                         float /*speedRatio*/,
                         const ::std::function<void()>& /*onAnimationEnd*/)
{
  AnimationRange* range = getAnimationRange(_name);

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

void Skeleton::_computeTransformMatrices(Float32Array& targetMatrix)
{
  _computeTransformMatrices(targetMatrix, Matrix(), false);
}

void Skeleton::_computeTransformMatrices(Float32Array& targetMatrix,
                                         const Matrix& initialSkinMatrix,
                                         bool initialSkinMatrixSet)
{
  onBeforeComputeObservable.notifyObservers(this);

  unsigned int index = 0;
  for (const auto& bone : bones) {
    auto parentBone = bone->getParent();

    if (parentBone) {
      bone->getLocalMatrix().multiplyToRef(*parentBone->getWorldMatrix(),
                                           *bone->getWorldMatrix());
    }
    else {
      if (initialSkinMatrixSet) {
        bone->getLocalMatrix().multiplyToRef(initialSkinMatrix,
                                             *bone->getWorldMatrix());
      }
      else {
        bone->getWorldMatrix()->copyFrom(bone->getLocalMatrix());
      }
    }

    if (*bone->_index != -1) {
      auto mappedIndex = bone->_index.isNull() ?
                           index :
                           static_cast<unsigned int>(*bone->_index);
      bone->getInvertedAbsoluteTransform().multiplyToArray(
        *bone->getWorldMatrix(), targetMatrix, mappedIndex * 16);
    }
    ++index;
  }

  _identity.copyToArray(targetMatrix,
                        static_cast<unsigned int>(bones.size()) * 16);
}

void Skeleton::prepare()
{
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
    }

    _computeTransformMatrices(_transformMatrices);
  }

  _isDirty = false;

  _scene->_activeBones.addCount(bones.size(), false);
}

vector_t<IAnimatable*> Skeleton::getAnimatables()
{
  if (_animatables.size() != bones.size()) {
    _animatables.clear();

    /*for (auto& bone : bones) {
      _animatables.emplace_back(bone);
    }*/
  }

  return _animatables;
}

vector_t<Animation*> Skeleton::getAnimations()
{
  return vector_t<Animation*>();
}

unique_ptr_t<Skeleton> Skeleton::clone(const string_t& /*iName*/,
                                       const string_t& /*iId*/) const
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
  for (auto& bone : bones) {
    for (auto& animation : bone->animations) {
      animation->enableBlending = true;
      animation->blendingSpeed  = blendingSpeed;
    }
  }
}

void Skeleton::dispose(bool /*doNotRecurse*/)
{
  _meshesWithPoseMatrix.clear();

  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  getScene()->removeSkeleton(this);
}

Json::object Skeleton::serialize() const
{
  return Json::object();
}

Skeleton* Skeleton::Parse(const Json::value& parsedSkeleton, Scene* scene)
{
  auto skeleton = new Skeleton(Json::GetString(parsedSkeleton, "name"),
                               Json::GetString(parsedSkeleton, "id"), scene);

  if (parsedSkeleton.contains("dimensionsAtRest")) {
    skeleton->dimensionsAtRest = ::std::make_unique<Vector3>(Vector3::FromArray(
      Json::ToArray<float>(parsedSkeleton, "dimensionsAtRest")));
  }

  skeleton->needInitialSkinMatrix
    = Json::GetBool(parsedSkeleton, "needInitialSkinMatrix");

  for (auto& parsedBone : Json::GetArray(parsedSkeleton, "bones")) {
    Bone* parentBone = nullptr;
    if (parsedBone.contains("parentBoneIndex")) {
      auto parentBoneIndex
        = Json::GetNumber<int>(parsedBone, "parentBoneIndex", -1);
      if (parentBoneIndex > -1) {
        auto _parentBoneIndex = static_cast<unsigned>(parentBoneIndex);
        if (_parentBoneIndex < skeleton->bones.size()) {
          parentBone = skeleton->bones[_parentBoneIndex].get();
        }
      }
    }

    Bone* bone = nullptr;
    if (parsedBone.contains("rest")) {
      Matrix rest = Matrix::FromArray(Json::ToArray<float>(parsedBone, "rest"));
      bone        = Bone::New(
        Json::GetString(parsedSkeleton, "name"), skeleton, parentBone,
        Matrix::FromArray(Json::ToArray<float>(parsedBone, "matrix")), rest);
    }
    else {
      bone = Bone::New(
        Json::GetString(parsedSkeleton, "name"), skeleton, parentBone,
        Matrix::FromArray(Json::ToArray<float>(parsedBone, "matrix")));
    }

    if (parsedBone.contains("length")) {
      bone->length = Json::GetNumber(parsedBone, "length", 0);
    }

    if (parsedBone.contains("animation")) {
      bone->animations.emplace_back(
        Animation::Parse(parsedBone.get("animation")));
    }
  }

  // placed after bones, so createAnimationRange can cascade down
  for (auto& range : Json::GetArray(parsedSkeleton, "ranges")) {
    skeleton->createAnimationRange(Json::GetString(range, "name"),
                                   Json::GetNumber<float>(range, "from", 0),
                                   Json::GetNumber<float>(range, "to", 0));
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
  vector_t<Bone*> bones;
  vector_t<bool> visited(bones.size());
  for (unsigned int index = 0; index < bones.size(); ++index) {
    _sortBones(index, bones, visited);
  }

  bones = bones;
}

void Skeleton::_sortBones(unsigned int index, vector_t<Bone*>& iBones,
                          vector_t<bool>& visited)
{
  if (visited[index]) {
    return;
  }

  visited[index] = true;

  auto& bone = bones[index];
  if (bone->_index.isNull()) {
    bone->_index = static_cast<int>(index);
  }

  const auto boneIndexOf = [this](Bone* iBone) {
    auto it = ::std::find_if(
      bones.begin(), bones.end(),
      [&iBone](const unique_ptr_t<Bone>& bone) { return bone.get() == iBone; });
    if (it != bones.end()) {
      return static_cast<int>(it - bones.begin());
    }
    return -1;
  };

  const auto& parentBone = bone->getParent();
  if (parentBone) {
    auto index = boneIndexOf(parentBone);
    if (index >= 0) {
      _sortBones(static_cast<unsigned int>(index), iBones, visited);
    }
  }

  iBones.emplace_back(bone.get());
}

} // end of namespace BABYLON
