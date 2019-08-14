#include <babylon/bones/bone.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

std::array<Vector3, 2> Bone::_tmpVecs{{Vector3::Zero(), Vector3::Zero()}};
Quaternion Bone::_tmpQuat{Quaternion::Identity()};
std::array<Matrix, 5> Bone::_tmpMats{{Matrix::Identity(), Matrix::Identity(),
                                      Matrix::Identity(), Matrix::Identity(),
                                      Matrix::Identity()}};

Bone::Bone(const std::string& iName, Skeleton* skeleton, Bone* parentBone,
           const std::optional<Matrix>& localMatrix,
           const std::optional<Matrix>& iRestPose,
           const std::optional<Matrix>& baseMatrix, std::optional<int> index)
    : Node{iName, skeleton->getScene(), false}
    , length{-1}
    , _index{index}
    , _linkedTransformNode{nullptr}
    , _matrix{this, &Bone::get__matrix, &Bone::set__matrix}
    , position{this, &Bone::get_position, &Bone::set_position}
    , rotation{this, &Bone::get_rotation, &Bone::set_rotation}
    , rotationQuaternion{this, &Bone::get_rotationQuaternion,
                         &Bone::set_rotationQuaternion}
    , scaling{this, &Bone::get_scaling, &Bone::set_scaling}
    , _skeleton{skeleton}
    , _localMatrix{localMatrix ? *localMatrix : Matrix::Identity()}
    , _restPose{iRestPose ? *iRestPose : Matrix::Identity()}
    , _baseMatrix{baseMatrix ? *baseMatrix : _localMatrix}
    , _invertedAbsoluteTransform{std::make_unique<Matrix>()}
    , _parent{nullptr}
    , _scaleMatrix{Matrix::Identity()}
    , _scaleVector{Vector3::One()}
    , _negateScaleChildren{Vector3::One()}
    , _scalingDeterminant{1.f}
    , _needToDecompose{true}
    , _needToCompose{false}
{
  setParent(parentBone, false);

  if (baseMatrix || localMatrix) {
    _updateDifferenceMatrix();
  }
}

Bone::~Bone()
{
}

Type Bone::type() const
{
  return Type::BONE;
}

void Bone::addToSkeleton(const BonePtr& newBone)
{
  _skeleton->bones.emplace_back(newBone);
}

// Members
Matrix& Bone::get__matrix()
{
  _compose();
  return _localMatrix;
}

void Bone::set__matrix(const Matrix& value)
{
  _localMatrix.copyFrom(value);
  _needToDecompose = true;
}

const std::string Bone::getClassName() const
{
  return "Bone";
}

Skeleton* Bone::getSkeleton() const
{
  return _skeleton;
}

Bone* Bone::getParent() const
{
  return _parent;
}

std::vector<Bone*>& Bone::getChildren()
{
  return children;
}

void Bone::setParent(Bone* iParent, bool updateDifferenceMatrix)
{
  if (_parent == iParent) {
    return;
  }

  if (_parent) {
    stl_util::erase(_parent->children, this);
  }

  _parent = iParent;

  if (_parent) {
    _parent->children.emplace_back(this);
  }

  if (updateDifferenceMatrix) {
    _updateDifferenceMatrix();
  }

  markAsDirty();
}

Matrix& Bone::getLocalMatrix()
{
  _compose();
  return _localMatrix;
}

const Matrix& Bone::getLocalMatrix() const
{
  return _localMatrix;
}

Matrix& Bone::getBaseMatrix()
{
  return _baseMatrix;
}

std::optional<Matrix>& Bone::getRestPose()
{
  return _restPose;
}

Matrix& Bone::getWorldMatrix()
{
  return _worldTransform;
}

void Bone::returnToRest()
{
  updateMatrix(_restPose.value_or(Matrix::Identity()));
}

Matrix& Bone::getInvertedAbsoluteTransform()
{
  return *_invertedAbsoluteTransform;
}

Matrix& Bone::getAbsoluteTransform()
{
  return _absoluteTransform;
}

const Matrix& Bone::getAbsoluteTransform() const
{
  return _absoluteTransform;
}

void Bone::linkTransformNode(const TransformNodePtr& transformNode)
{
  if (_linkedTransformNode) {
    --_skeleton->_numBonesWithLinkedTransformNode;
  }

  _linkedTransformNode = transformNode;

  if (_linkedTransformNode) {
    ++_skeleton->_numBonesWithLinkedTransformNode;
  }
}

Vector3& Bone::get_position()
{
  _decompose();
  return _localPosition;
}

void Bone::set_position(const Vector3& newPosition)
{
  _decompose();
  _localPosition.copyFrom(newPosition);

  _markAsDirtyAndCompose();
}

Vector3& Bone::get_rotation()
{
  return getRotation();
}

void Bone::set_rotation(const Vector3& newRotation)
{
  setRotation(newRotation);
}

std::optional<Quaternion>& Bone::get_rotationQuaternion()
{
  _decompose();
  return _localRotation;
}

void Bone::set_rotationQuaternion(const std::optional<Quaternion>& newRotation)
{
  if (newRotation) {
    setRotationQuaternion(*newRotation);
  }
}

std::optional<Vector3>& Bone::get_scaling()
{
  return getScale();
}

void Bone::set_scaling(const std::optional<Vector3>& newScaling)
{
  setScale(*newScaling);
}

std::vector<AnimationPtr> Bone::getAnimations()
{
  return animations;
}

AnimationPropertiesOverride*& Bone::get_animationPropertiesOverride()
{
  return _skeleton->animationPropertiesOverride;
}

void Bone::_decompose()
{
  if (!_needToDecompose) {
    return;
  }

  _needToDecompose = false;

  if (!_localScaling) {
    _localScaling  = Vector3::Zero();
    _localRotation = Quaternion::Zero();
    _localPosition = Vector3::Zero();
  }
  _localMatrix.decompose(*_localScaling, _localRotation, _localPosition);
}

void Bone::_compose()
{
  if (!_needToCompose) {
    return;
  }

  _needToCompose = false;
  Matrix::ComposeToRef(*_localScaling, *_localRotation, _localPosition,
                       _localMatrix);
}

void Bone::updateMatrix(const Matrix& matrix, bool updateDifferenceMatrix,
                        bool updateLocalMatrix)
{
  _baseMatrix.copyFrom(matrix);

  if (updateDifferenceMatrix) {
    _updateDifferenceMatrix();
  }

  if (updateLocalMatrix) {
    _localMatrix.copyFrom(matrix);
    _markAsDirtyAndDecompose();
  }
  else {
    markAsDirty();
  }
}

void Bone::_updateDifferenceMatrix(const std::optional<Matrix>& rootMatrix,
                                   bool updateChildren)
{
  auto _rootMatrix = rootMatrix ? *rootMatrix : _baseMatrix;

  if (_parent) {
    _rootMatrix.multiplyToRef(_parent->getAbsoluteTransform(),
                              _absoluteTransform);
  }
  else {
    _absoluteTransform.copyFrom(_rootMatrix);
  }

  _absoluteTransform.invertToRef(*_invertedAbsoluteTransform);

  if (updateChildren) {
    for (auto& child : children) {
      child->_updateDifferenceMatrix();
    }
  }

  _scalingDeterminant = (_absoluteTransform.determinant() < 0.f ? -1.f : 1.f);
}

Bone& Bone::markAsDirty(const std::string& /*property*/)
{
  ++_currentRenderId;
  ++_childUpdateId;
  _skeleton->_markAsDirty();

  return *this;
}

void Bone::_markAsDirtyAndCompose()
{
  markAsDirty();
  _needToCompose = true;
}

void Bone::_markAsDirtyAndDecompose()
{
  markAsDirty();
  _needToDecompose = true;
}

bool Bone::copyAnimationRange(Bone* source, const std::string& rangeName,
                              int frameOffset, bool rescaleAsRequired,
                              const Vector3& skelDimensionsRatio,
                              bool hasSkelDimensionsRatio)
{
  // all animation may be coming from a library skeleton, so may need to create
  // animation
  /*if (animations.empty()) {
    animations.emplace_back(std::make_shared<Animation>(
      name, "_matrix", source->animations[0]->framePerSecond,
      Animation::ANIMATIONTYPE_MATRIX, 0));
  }*/

  // get animation info / verify there is such a range from the source bone
  if (source->animations.empty() || !source->animations[0]) {
    return false;
  }

  const auto& sourceRange = source->animations[0]->getRange(rangeName);
  float from              = sourceRange.from;
  float to                = sourceRange.to;
  const auto& sourceKeys  = source->animations[0]->getKeys();

  // rescaling prep
  auto sourceBoneLength  = source->length;
  auto sourceParent      = source->getParent();
  auto parentBone        = getParent();
  bool parentScalingReqd = rescaleAsRequired && sourceParent
                           && sourceBoneLength > 0 && length > 0
                           && !stl_util::almost_equal(sourceBoneLength, length);
  float parentRatio = parentScalingReqd && parentBone && sourceParent ?
                        static_cast<float>(parentBone->length)
                          / static_cast<float>(sourceParent->length) :
                        1.f;

  bool dimensionsScalingReqd
    = rescaleAsRequired && !parentBone && hasSkelDimensionsRatio
      && (!stl_util::almost_equal(skelDimensionsRatio.x, 1.f)
          || !stl_util::almost_equal(skelDimensionsRatio.y, 1.f)
          || !stl_util::almost_equal(skelDimensionsRatio.z, 1.f));

  auto& destKeys = animations[0]->getKeys();

  // loop vars declaration
  Vector3 origTranslation;
  Matrix mat;

  for (const auto& orig : sourceKeys) {
    if (orig.frame >= from && orig.frame <= to) {
      if (rescaleAsRequired) {
        mat = orig.value.get<Matrix>();

        if (parentScalingReqd) {
          // scale based on parent ratio, when bone has parent
          origTranslation = mat.getTranslation();
          mat.setTranslation(origTranslation.scaleInPlace(parentRatio));
        }
        else if (dimensionsScalingReqd) {
          // scale based on skeleton dimension ratio when root bone, and value
          // is passed
          origTranslation = mat.getTranslation();
          mat.setTranslation(
            origTranslation.multiplyInPlace(skelDimensionsRatio));
        }
        else {
          // use original when root bone, and no data for skelDimensionsRatio
          mat = orig.value.get<Matrix>();
        }
      }
      else {
        mat = orig.value.get<Matrix>();
      }
      destKeys.emplace_back(IAnimationKey(orig.frame + frameOffset, mat));
    }
  }
  animations[0]->createRange(rangeName, from + frameOffset, to + frameOffset);
  return true;
}

void Bone::translate(const Vector3& vec, Space space, AbstractMesh* mesh)
{
  auto& lm = getLocalMatrix();

  if (space == Space::LOCAL) {
    lm.addAtIndex(12, vec.x);
    lm.addAtIndex(13, vec.y);
    lm.addAtIndex(14, vec.z);
  }
  else {

    std::optional<Matrix> wm = std::nullopt;
    // mesh.getWorldMatrix() needs to be called before
    // skeleton.computeAbsoluteTransforms()
    if (mesh) {
      wm = mesh->getWorldMatrix();
    }

    _skeleton->computeAbsoluteTransforms();
    auto& tmat = Bone::_tmpMats[0];
    auto& tvec = Bone::_tmpVecs[0];

    if (_parent) {
      if (mesh && wm.has_value()) {
        tmat.copyFrom(_parent->getAbsoluteTransform());
        tmat.multiplyToRef(*wm, tmat);
      }
      else {
        tmat.copyFrom(_parent->getAbsoluteTransform());
      }
    }

    tmat.setTranslationFromFloats(0.f, 0.f, 0.f);
    tmat.invert();
    Vector3::TransformCoordinatesToRef(vec, tmat, tvec);

    lm.addAtIndex(12, tvec.x);
    lm.addAtIndex(13, tvec.y);
    lm.addAtIndex(14, tvec.z);
  }

  _markAsDirtyAndDecompose();
}

void Bone::setPosition(const Vector3& iPosition, Space space,
                       AbstractMesh* mesh)
{
  auto& lm = getLocalMatrix();

  if (space == Space::LOCAL) {
    lm.setTranslationFromFloats(iPosition.x, iPosition.y, iPosition.z);
  }
  else {

    std::optional<Matrix> wm = std::nullopt;

    // mesh.getWorldMatrix() needs to be called before
    // skeleton.computeAbsoluteTransforms()
    if (mesh) {
      wm = mesh->getWorldMatrix();
    }

    _skeleton->computeAbsoluteTransforms();

    auto& tmat = Bone::_tmpMats[0];
    auto& vec  = Bone::_tmpVecs[0];

    if (_parent) {
      if (mesh && wm.has_value()) {
        tmat.copyFrom(_parent->getAbsoluteTransform());
        tmat.multiplyToRef(*wm, tmat);
      }
      else {
        tmat.copyFrom(_parent->getAbsoluteTransform());
      }
    }

    tmat.invert();
    Vector3::TransformCoordinatesToRef(iPosition, tmat, vec);
    lm.setTranslationFromFloats(vec.x, vec.y, vec.z);
  }

  _markAsDirtyAndDecompose();
}

void Bone::setAbsolutePosition(const Vector3& iPosition, AbstractMesh* mesh)
{
  setPosition(iPosition, Space::WORLD, mesh);
}

void Bone::scale(float x, float y, float z, bool scaleChildren)
{
  auto& locMat = getLocalMatrix();

  // Apply new scaling on top of current local matrix
  auto& scaleMat = Bone::_tmpMats[0];
  Matrix::ScalingToRef(x, y, z, scaleMat);
  scaleMat.multiplyToRef(locMat, locMat);

  // Invert scaling matrix and apply the inverse to all children
  scaleMat.invert();

  for (auto& child : children) {
    auto& cm = child->getLocalMatrix();
    cm.multiplyToRef(scaleMat, cm);
    cm.multiplyAtIndex(12, x);
    cm.multiplyAtIndex(13, y);
    cm.multiplyAtIndex(14, z);

    child->_markAsDirtyAndDecompose();
  }

  _markAsDirtyAndDecompose();

  if (scaleChildren) {
    for (auto& child : children) {
      child->scale(x, y, z, scaleChildren);
    }
  }
}

void Bone::setScale(const Vector3& scale)
{
  _decompose();
  auto localScalingCpy = *_localScaling;
  localScalingCpy.copyFrom(scale);
  _localScaling = localScalingCpy;
  _markAsDirtyAndCompose();
}

std::optional<Vector3>& Bone::getScale()
{
  _decompose();
  return _localScaling;
}

void Bone::getScaleToRef(Vector3& result)
{
  _decompose();
  result.copyFrom(*_localScaling);
}

void Bone::setYawPitchRoll(float yaw, float pitch, float roll, Space space,
                           AbstractMesh* mesh)
{
  if (space == Space::LOCAL) {
    auto& quat = Bone::_tmpQuat;
    Quaternion::RotationYawPitchRollToRef(yaw, pitch, roll, quat);
    setRotationQuaternion(quat, space, mesh);
    return;
  }

  auto& rotMatInv = Bone::_tmpMats[0];
  if (!_getNegativeRotationToRef(rotMatInv, mesh)) {
    return;
  }

  auto& rotMat = Bone::_tmpMats[1];
  Matrix::RotationYawPitchRollToRef(yaw, pitch, roll, rotMat);

  rotMatInv.multiplyToRef(rotMat, rotMat);
  _rotateWithMatrix(rotMat, space, mesh);
}

void Bone::rotate(Vector3& axis, float amount, Space space, AbstractMesh* mesh)
{
  auto& rmat = Bone::_tmpMats[0];
  rmat.setTranslationFromFloats(0.f, 0.f, 0.f);
  Matrix::RotationAxisToRef(axis, amount, rmat);
  _rotateWithMatrix(rmat, space, mesh);
}

void Bone::setAxisAngle(Vector3& axis, float angle, Space space,
                        AbstractMesh* mesh)
{
  if (space == Space::LOCAL) {
    auto& quat = Bone::_tmpQuat;
    Quaternion::RotationAxisToRef(axis, angle, quat);

    setRotationQuaternion(quat, space, mesh);
    return;
  }

  auto& rotMatInv = Bone::_tmpMats[0];
  if (!_getNegativeRotationToRef(rotMatInv, mesh)) {
    return;
  }

  auto& rotMat = Bone::_tmpMats[1];
  Matrix::RotationAxisToRef(axis, angle, rotMat);

  rotMatInv.multiplyToRef(rotMat, rotMat);
  _rotateWithMatrix(rotMat, space, mesh);
}

void Bone::setRotation(const Vector3& iRotation, Space space,
                       AbstractMesh* mesh)
{
  setYawPitchRoll(iRotation.y, iRotation.x, iRotation.z, space, mesh);
}

void Bone::setRotationQuaternion(const Quaternion& quat, Space space,
                                 AbstractMesh* mesh)
{
  if (space == Space::LOCAL) {
    _decompose();
    _localRotation->copyFrom(quat);

    _markAsDirtyAndCompose();

    return;
  }

  auto& rotMatInv = Bone::_tmpMats[0];
  if (!_getNegativeRotationToRef(rotMatInv, mesh)) {
    return;
  }

  auto& rotMat = Bone::_tmpMats[1];
  Matrix::FromQuaternionToRef(quat, rotMat);

  rotMatInv.multiplyToRef(rotMat, rotMat);

  _rotateWithMatrix(rotMat, space, mesh);
}

void Bone::setRotationMatrix(const Matrix& rotMat, Space space,
                             AbstractMesh* mesh)
{
  if (space == Space::LOCAL) {
    auto& quat = Bone::_tmpQuat;
    Quaternion::FromRotationMatrixToRef(rotMat, quat);
    setRotationQuaternion(quat, space, mesh);
    return;
  }

  auto& rotMatInv = Bone::_tmpMats[0];
  if (!_getNegativeRotationToRef(rotMatInv, mesh)) {
    return;
  }

  auto& rotMat2 = Bone::_tmpMats[1];
  rotMat2.copyFrom(rotMat);

  rotMatInv.multiplyToRef(rotMat, rotMat2);

  _rotateWithMatrix(rotMat2, space, mesh);
}

void Bone::_rotateWithMatrix(const Matrix& rmat, Space space,
                             AbstractMesh* mesh)
{
  auto& lmat           = getLocalMatrix();
  const auto& lmatM    = lmat.m();
  auto lx              = lmatM[12];
  auto ly              = lmatM[13];
  auto lz              = lmatM[14];
  auto parentBone      = getParent();
  auto& parentScale    = Bone::_tmpMats[3];
  auto& parentScaleInv = Bone::_tmpMats[4];

  if (parentBone && space == Space::WORLD) {
    if (mesh) {
      parentScale.copyFrom(mesh->getWorldMatrix());
      parentBone->getAbsoluteTransform().multiplyToRef(parentScale,
                                                       parentScale);
    }
    else {
      parentScale.copyFrom(parentBone->getAbsoluteTransform());
    }
    parentScaleInv.copyFrom(parentScale);
    parentScaleInv.invert();
    lmat.multiplyToRef(parentScale, lmat);
    lmat.multiplyToRef(rmat, lmat);
    lmat.multiplyToRef(parentScaleInv, lmat);
  }
  else {
    if (space == Space::WORLD && mesh) {
      parentScale.copyFrom(mesh->getWorldMatrix());
      parentScaleInv.copyFrom(parentScale);
      parentScaleInv.invert();
      lmat.multiplyToRef(parentScale, lmat);
      lmat.multiplyToRef(rmat, lmat);
      lmat.multiplyToRef(parentScaleInv, lmat);
    }
    else {
      lmat.multiplyToRef(rmat, lmat);
    }
  }

  lmat.setTranslationFromFloats(lx, ly, lz);

  computeAbsoluteTransforms();
  _markAsDirtyAndDecompose();
}

bool Bone::_getNegativeRotationToRef(Matrix& rotMatInv, AbstractMesh* mesh)
{
  auto& scaleMatrix = Bone::_tmpMats[2];
  rotMatInv.copyFrom(getAbsoluteTransform());

  if (mesh) {
    rotMatInv.multiplyToRef(mesh->getWorldMatrix(), rotMatInv);
    Matrix::ScalingToRef(mesh->scaling().x, mesh->scaling().y,
                         mesh->scaling().z, scaleMatrix);
  }

  rotMatInv.invert();
  if (std::isnan(rotMatInv.m()[0])) {
    // Matrix failed to invert.
    // This can happen if scale is zero for example.
    return false;
  }

  scaleMatrix.multiplyAtIndex(0, _scalingDeterminant);
  rotMatInv.multiplyToRef(scaleMatrix, rotMatInv);

  return true;
}

Vector3 Bone::getPosition(Space space, AbstractMesh* mesh) const
{
  auto pos = Vector3::Zero();

  getPositionToRef(pos, space, mesh);

  return pos;
}

void Bone::getPositionToRef(Vector3& result, Space space,
                            AbstractMesh* mesh) const
{
  if (space == Space::LOCAL) {

    auto& lm = getLocalMatrix();

    const auto& lmM = lm.m();
    result.x        = lmM[12];
    result.y        = lmM[13];
    result.z        = lmM[14];
  }
  else {
    std::optional<Matrix> wm = std::nullopt;
    // mesh.getWorldMatrix() needs to be called before
    // skeleton.computeAbsoluteTransforms()
    if (mesh) {
      wm = mesh->getWorldMatrix();
    }

    _skeleton->computeAbsoluteTransforms();

    auto& tmat = Bone::_tmpMats[0];

    if (mesh && wm.has_value()) {
      tmat.copyFrom(getAbsoluteTransform());
      tmat.multiplyToRef(*wm, tmat);
    }
    else {
      tmat = getAbsoluteTransform();
    }

    const auto& tmatM = tmat.m();
    result.x          = tmatM[12];
    result.y          = tmatM[13];
    result.z          = tmatM[14];
  }
}

Vector3 Bone::getAbsolutePosition(AbstractMesh* mesh) const
{
  auto pos = Vector3::Zero();

  getPositionToRef(pos, Space::WORLD, mesh);

  return pos;
}

void Bone::getAbsolutePositionToRef(AbstractMesh* mesh, Vector3& result) const
{
  getPositionToRef(result, Space::WORLD, mesh);
}

void Bone::computeAbsoluteTransforms()
{
  _compose();

  if (_parent) {
    _localMatrix.multiplyToRef(_parent->_absoluteTransform, _absoluteTransform);
  }
  else {
    _absoluteTransform.copyFrom(_localMatrix);

    auto poseMatrix = _skeleton->getPoseMatrix();

    if (poseMatrix) {
      _absoluteTransform.multiplyToRef(*poseMatrix, _absoluteTransform);
    }
  }

  for (auto& child : children) {
    child->computeAbsoluteTransforms();
  }
}

Vector3 Bone::getDirection(const Vector3& localAxis, AbstractMesh* mesh) const
{
  auto result = Vector3::Zero();

  getDirectionToRef(localAxis, result, mesh);

  return result;
}

void Bone::getDirectionToRef(const Vector3& localAxis, Vector3& result,
                             AbstractMesh* mesh) const
{
  std::optional<Matrix> wm = std::nullopt;

  // mesh.getWorldMatrix() needs to be called before
  // skeleton.computeAbsoluteTransforms()
  if (mesh) {
    wm = mesh->getWorldMatrix();
  }

  _skeleton->computeAbsoluteTransforms();

  auto& mat = Bone::_tmpMats[0];

  mat.copyFrom(getAbsoluteTransform());

  if (mesh && wm.has_value()) {
    mat.multiplyToRef(*wm, mat);
  }

  Vector3::TransformNormalToRef(localAxis, mat, result);

  result.normalize();
}

Vector3& Bone::getRotation(Space space, AbstractMesh* mesh)
{
  _rotationTmp = Vector3::Zero();

  getRotationToRef(_rotationTmp, space, mesh);

  return _rotationTmp;
}

void Bone::getRotationToRef(Vector3& result, Space space, AbstractMesh* mesh)
{
  auto& quat = Bone::_tmpQuat;

  getRotationQuaternionToRef(quat, space, mesh);

  quat.toEulerAnglesToRef(result);
}

Quaternion Bone::getRotationQuaternion(Space space, AbstractMesh* mesh)
{
  auto result = Quaternion::Identity();

  getRotationQuaternionToRef(result, space, mesh);

  return result;
}

void Bone::getRotationQuaternionToRef(Quaternion& result, const Space& space,
                                      AbstractMesh* mesh)
{
  if (space == Space::LOCAL) {
    _decompose();
    result.copyFrom(*_localRotation);
  }
  else {
    auto& mat = Bone::_tmpMats[0];
    auto amat = getAbsoluteTransform();

    if (mesh) {
      amat.multiplyToRef(mesh->getWorldMatrix(), mat);
    }
    else {
      mat.copyFrom(amat);
    }

    mat.multiplyAtIndex(0, _scalingDeterminant);
    mat.multiplyAtIndex(1, _scalingDeterminant);
    mat.multiplyAtIndex(2, _scalingDeterminant);

    mat.decompose(std::nullopt, result, std::nullopt);
  }
}

Matrix Bone::getRotationMatrix(Space space, AbstractMesh* mesh) const
{
  auto result = Matrix::Identity();

  getRotationMatrixToRef(result, space, mesh);

  return result;
}

void Bone::getRotationMatrixToRef(Matrix& result, Space space,
                                  AbstractMesh* mesh) const
{
  if (space == Space::LOCAL) {
    getLocalMatrix().getRotationMatrixToRef(result);
  }
  else {
    auto& mat = Bone::_tmpMats[0];
    auto amat = getAbsoluteTransform();

    if (mesh) {
      amat.multiplyToRef(mesh->getWorldMatrix(), mat);
    }
    else {
      mat.copyFrom(amat);
    }

    mat.multiplyAtIndex(0, _scalingDeterminant);
    mat.multiplyAtIndex(1, _scalingDeterminant);
    mat.multiplyAtIndex(2, _scalingDeterminant);

    mat.getRotationMatrixToRef(result);
  }
}

Vector3 Bone::getAbsolutePositionFromLocal(const Vector3& iPosition,
                                           AbstractMesh* mesh) const
{
  auto result = Vector3::Zero();

  getAbsolutePositionFromLocalToRef(iPosition, mesh, result);

  return result;
}

void Bone::getAbsolutePositionFromLocalToRef(const Vector3& iPosition,
                                             AbstractMesh* mesh,
                                             Vector3& result) const
{
  std::optional<Matrix> wm = std::nullopt;

  // mesh.getWorldMatrix() needs to be called before
  // skeleton.computeAbsoluteTransforms()
  if (mesh) {
    wm = mesh->getWorldMatrix();
  }

  _skeleton->computeAbsoluteTransforms();

  auto& tmat = Bone::_tmpMats[0];

  if (mesh && wm.has_value()) {
    tmat.copyFrom(getAbsoluteTransform());
    tmat.multiplyToRef(*wm, tmat);
  }
  else {
    tmat = getAbsoluteTransform();
  }

  Vector3::TransformCoordinatesToRef(iPosition, tmat, result);
}

Vector3 Bone::getLocalPositionFromAbsolute(const Vector3& iPosition,
                                           AbstractMesh* mesh) const
{
  auto result = Vector3::Zero();

  getLocalPositionFromAbsoluteToRef(iPosition, mesh, result);

  return result;
}

void Bone::getLocalPositionFromAbsoluteToRef(const Vector3& iPosition,
                                             AbstractMesh* mesh,
                                             Vector3& result) const
{
  std::optional<Matrix> wm = std::nullopt;

  // mesh.getWorldMatrix() needs to be called before
  // skeleton.computeAbsoluteTransforms()
  if (mesh) {
    wm = mesh->getWorldMatrix();
  }

  _skeleton->computeAbsoluteTransforms();

  auto tmat = Bone::_tmpMats[0];

  tmat.copyFrom(getAbsoluteTransform());

  if (mesh && wm.has_value()) {
    tmat.multiplyToRef(*wm, tmat);
  }

  tmat.invert();

  Vector3::TransformCoordinatesToRef(iPosition, tmat, result);
}

} // end of namespace BABYLON
