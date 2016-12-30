#include <babylon/bones/skeleton.h>

#include <babylon/bones/bone.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

Skeleton::Skeleton(const std::string& _name, const std::string& _id,
                   Scene* scene)
    : needInitialSkinMatrix{false}
    , name{_name}
    , id{_id}
    , _scene{scene}
    , _isDirty{true}
    , _identity{Matrix::Identity()}
{
  bones.clear();
  scene->skeletons.emplace_back(this);
  prepare();
  // make sure it will recalculate the matrix next time prepare is called.
  _isDirty = true;
}

Skeleton::~Skeleton()
{
}

// Members
Float32Array& Skeleton::getTransformMatrices(AbstractMesh* mesh)
{
  if (needInitialSkinMatrix && !mesh->_bonesTransformMatrices.empty()) {
    return mesh->_bonesTransformMatrices;
  }
  return _transformMatrices;
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

void Skeleton::createAnimationRange(const std::string& _name, int from, int to)
{
  // check name not already in use
  if (!std_util::contains(_ranges, _name)) {
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
  if (!std_util::contains(_ranges, _name)) {
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
  if (std_util::contains(_ranges, _name) || !source->getAnimationRange(_name)) {
    return false;
  }
  bool ret        = true;
  int frameOffset = _getHighestAnimationFrame() + 1;

  // make a dictionary of source skeleton's bones, so exact same order or
  // doublely nested loop is not required
  std::unordered_map<std::string, Bone*> boneDict;
  auto& sourceBones = source->bones;
  for (auto& bone : sourceBones) {
    boneDict[bone->name] = bone.get();
  }

  if (bones.size() != sourceBones.size()) {
    ret = false;
  }

  Vector3 skelDimensionsRatio;
  bool hasSkelDimensionsRatio = false;
  if (rescaleAsRequired && dimensionsAtRest) {
    dimensionsAtRest->divide(*source->dimensionsAtRest);
  }

  for (auto& bone : bones) {
    if (std_util::contains(boneDict, bone->name)) {
      ret = ret
            && bone->copyAnimationRange(
                 boneDict[bone->name], _name, frameOffset, rescaleAsRequired,
                 skelDimensionsRatio, hasSkelDimensionsRatio);
    }
    else {
      ret = false;
    }
  }
  // do not call createAnimationRange(), since it also is done to bones, which
  // was already done
  AnimationRange* range = source->getAnimationRange(_name);
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
Skeleton::beginAnimation(const std::string& _name, bool /*loop*/,
                         float /*speedRatio*/,
                         const std::function<void()>& /*onAnimationEnd*/)
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
  std_util::erase(_meshesWithPoseMatrix, mesh);
}

void Skeleton::_computeTransformMatrices(Float32Array& targetMatrix)
{
  _computeTransformMatrices(targetMatrix, Matrix(), false);
}

void Skeleton::_computeTransformMatrices(Float32Array& targetMatrix,
                                         const Matrix& initialSkinMatrix,
                                         bool initialSkinMatrixSet)
{
  unsigned int index = 0;
  for (const auto& bone : bones) {
    Bone* parentBone = bone->getParent();

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

    bone->getInvertedAbsoluteTransform().multiplyToArray(
      *bone->getWorldMatrix(), targetMatrix, index * 16);
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

      if (mesh->_bonesTransformMatrices.size() != 16 * (bones.size() + 1)) {
        mesh->_bonesTransformMatrices.resize(16 * (bones.size() + 1));
      }

      Matrix poseMatrix = mesh->getPoseMatrix();

      // Prepare bones
      for (auto& bone : bones) {
        if (!bone->getParent()) {
          auto& tmpMatrix = Tmp::MatrixArray[0];
          auto& matrix    = bone->getBaseMatrix();
          matrix.multiplyToRef(poseMatrix, tmpMatrix);
          bone->_updateDifferenceMatrix(tmpMatrix);
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

  //_scene->_activeBones += bones.size();
}

std::vector<IAnimatable*> Skeleton::getAnimatables()
{
  if (_animatables.size() != bones.size()) {
    _animatables.clear();

    /*for (auto& bone : bones) {
      _animatables.emplace_back(bone);
    }*/
  }

  return _animatables;
}

std::vector<Animation*> Skeleton::getAnimations()
{
  return std::vector<Animation*>();
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
  // getScene()->stopAnimation(this);

  // Remove from scene
  getScene()->removeSkeleton(this);
}

Json::object Skeleton::serialize() const
{
  return Json::object();
}

Skeleton* Skeleton::Parse(const Json::value& /*parsedSkeleton*/,
                          Scene* /*scene*/)
{
  return nullptr;
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

  // if (!_meshesWithPoseMatrix.empty()) {
  //  poseMatrix = _meshesWithPoseMatrix[0]->getPoseMatrix();
  //}

  return poseMatrix;
}

} // end of namespace BABYLON
