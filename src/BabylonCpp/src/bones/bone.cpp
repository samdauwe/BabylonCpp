#include <babylon/bones/bone.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

array_t<Vector3, 2> Bone::_tmpVecs{{Vector3::Zero(), Vector3::Zero()}};
Quaternion Bone::_tmpQuat{Quaternion::Identity()};
array_t<Matrix, 5> Bone::_tmpMats{{Matrix::Identity(), Matrix::Identity(),
                                   Matrix::Identity(), Matrix::Identity(),
                                   Matrix::Identity()}};

Bone::Bone(const string_t& iName, Skeleton* skeleton, Bone* parentBone,
           const Nullable<Matrix>& localMatrix,
           const Nullable<Matrix>& restPose, const Nullable<Matrix>& baseMatrix,
           Nullable<int> index)
    : Node{iName, skeleton->getScene()}
    , length{-1}
    , _index{index}
    , _skeleton{skeleton}
    , _localMatrix{localMatrix ? *localMatrix : Matrix::Identity()}
    , _restPose{restPose ? *restPose : Matrix::Identity()}
    , _baseMatrix{baseMatrix ? *baseMatrix : _localMatrix}
    , _worldTransform{::std::make_unique<Matrix>()}
    , _invertedAbsoluteTransform{::std::make_unique<Matrix>()}
    , _scaleMatrix{Matrix::Identity()}
    , _scaleVector{Vector3::One()}
    , _negateScaleChildren{Vector3::One()}
    , _scalingDeterminant{1.f}
{
  setParent(parentBone, false);

  _updateDifferenceMatrix();
}

Bone::~Bone()
{
}

IReflect::Type Bone::type() const
{
  return IReflect::Type::BONE;
}

void Bone::addToSkeleton(std::unique_ptr<Bone>&& newBone)
{
  _skeleton->bones.emplace_back(::std::move(newBone));
}

// Members
Matrix& Bone::_matrix()
{
  return _localMatrix;
}

const Matrix& Bone::_matrix() const
{
  return _localMatrix;
}

void Bone::setMatrix(const Matrix& val)
{
  _localMatrix = val;
}

Skeleton* Bone::getSkeleton() const
{
  return _skeleton;
}

Bone* Bone::getParent()
{
  return _parent;
}

void Bone::setParent(Bone* parent, bool updateDifferenceMatrix)
{
  if (_parent == parent) {
    return;
  }

  if (_parent) {
    stl_util::erase(parent->children, this);
  }

  _parent = parent;

  if (_parent) {
    _parent->children.emplace_back(this);
  }

  if (updateDifferenceMatrix) {
    _updateDifferenceMatrix();
  }
}

Matrix& Bone::getLocalMatrix()
{
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

Matrix& Bone::getRestPose()
{
  return _restPose;
}

void Bone::returnToRest()
{
  updateMatrix(_restPose);
}

Matrix* Bone::getWorldMatrix()
{
  return _worldTransform.get();
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

// Properties (matches AbstractMesh properties)
Vector3 Bone::position() const
{
  return getPosition();
}

Vector3 Bone::rotation() const
{
  return getRotation();
}

Quaternion Bone::rotationQuaternion() const
{
  return getRotationQuaternion();
}

Vector3 Bone::scaling() const
{
  return getScale();
}

void Bone::setScaling(const Vector3& newScaling)
{
  setScale(newScaling.x, newScaling.y, newScaling.z);
}

// Methods
vector_t<Animation*> Bone::getAnimations()
{
  return animations;
}

void Bone::updateMatrix(const Matrix& matrix, bool updateDifferenceMatrix)
{
  _baseMatrix  = matrix;
  _localMatrix = matrix;

  _skeleton->_markAsDirty();

  if (updateDifferenceMatrix) {
    _updateDifferenceMatrix();
  }
}

void Bone::_updateDifferenceMatrix()
{
  _updateDifferenceMatrix(_baseMatrix);
}

void Bone::_updateDifferenceMatrix(Matrix& rootMatrix)
{
  if (_parent) {
    rootMatrix.multiplyToRef(_parent->getAbsoluteTransform(),
                             _absoluteTransform);
  }
  else {
    _absoluteTransform.copyFrom(rootMatrix);
  }

  _absoluteTransform.invertToRef(*_invertedAbsoluteTransform);

  for (auto& child : children) {
    child->_updateDifferenceMatrix();
  }

  _scalingDeterminant = (_absoluteTransform.determinant() < 0.f ? -1.f : 1.f);
}

void Bone::markAsDirty(unsigned int /*property*/)
{
  ++_currentRenderId;
  _skeleton->_markAsDirty();
}

bool Bone::copyAnimationRange(Bone* source, const string_t& rangeName,
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
  if (source->animations.empty() && !source->animations[0]) {
    return false;
  }

  const auto& sourceRange = source->animations[0]->getRange(rangeName);
  float from              = sourceRange.from;
  float to                = sourceRange.to;
  const auto& sourceKeys  = source->animations[0]->getKeys();

  // rescaling prep
  int sourceBoneLength   = source->length;
  auto sourceParent      = source->getParent();
  auto parent            = getParent();
  bool parentScalingReqd = rescaleAsRequired && sourceParent
                           && sourceBoneLength > 0 && length > 0
                           && sourceBoneLength != length;
  float parentRatio = parentScalingReqd ?
                        static_cast<float>(parent->length)
                          / static_cast<float>(sourceParent->length) :
                        0.f;

  bool dimensionsScalingReqd
    = rescaleAsRequired && !parent && hasSkelDimensionsRatio
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
        mat = orig.value.matrixData;

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
          mat = orig.value.matrixData;
        }
      }
      else {
        mat = orig.value.matrixData;
      }
      destKeys.emplace_back(AnimationKey(orig.frame + frameOffset, mat));
    }
  }
  animations[0]->createRange(rangeName, from + frameOffset, to + frameOffset);
  return true;
}

void Bone::translate(const Vector3& vec, Space space, AbstractMesh* mesh)
{
  auto& lm = getLocalMatrix();

  if (space == Space::LOCAL) {

    lm.m[12] += vec.x;
    lm.m[13] += vec.y;
    lm.m[14] += vec.z;
  }
  else {

    Matrix* wm = nullptr;
    // mesh.getWorldMatrix() needs to be called before
    // skeleton.computeAbsoluteTransforms()
    if (mesh) {
      wm = mesh->getWorldMatrix();
    }

    _skeleton->computeAbsoluteTransforms();
    auto& tmat = Bone::_tmpMats[0];
    auto& tvec = Bone::_tmpVecs[0];

    if (mesh) {
      tmat.copyFrom(_parent->getAbsoluteTransform());
      if (wm) {
        tmat.multiplyToRef(*wm, tmat);
      }
    }
    else {
      tmat.copyFrom(_parent->getAbsoluteTransform());
    }

    tmat.m[12] = 0.f;
    tmat.m[13] = 0.f;
    tmat.m[14] = 0.f;

    tmat.invert();
    Vector3::TransformCoordinatesToRef(vec, tmat, tvec);

    lm.m[12] += tvec.x;
    lm.m[13] += tvec.y;
    lm.m[14] += tvec.z;
  }

  markAsDirty();
}

void Bone::setPosition(const Vector3& position, Space space, AbstractMesh* mesh)
{
  auto& lm = getLocalMatrix();

  if (space == Space::LOCAL) {

    lm.m[12] = position.x;
    lm.m[13] = position.y;
    lm.m[14] = position.z;
  }
  else {

    Matrix* wm = nullptr;
    // mesh.getWorldMatrix() needs to be called before
    // skeleton.computeAbsoluteTransforms()
    if (mesh) {
      wm = mesh->getWorldMatrix();
    }

    _skeleton->computeAbsoluteTransforms();

    auto& tmat = Bone::_tmpMats[0];
    auto& vec  = Bone::_tmpVecs[0];

    if (mesh) {
      tmat.copyFrom(_parent->getAbsoluteTransform());
      if (wm) {
        tmat.multiplyToRef(*wm, tmat);
      }
    }
    else {
      tmat.copyFrom(_parent->getAbsoluteTransform());
    }

    tmat.invert();
    Vector3::TransformCoordinatesToRef(position, tmat, vec);

    lm.m[12] = vec.x;
    lm.m[13] = vec.y;
    lm.m[14] = vec.z;
  }

  markAsDirty();
}

void Bone::setAbsolutePosition(const Vector3& position, AbstractMesh* mesh)
{
  setPosition(position, Space::WORLD, mesh);
}

void Bone::setScale(float x, float y, float z, bool scaleChildren)
{
  if (!animations.empty() && animations[0] && !animations[0]->isStopped()) {
    if (!scaleChildren) {
      _negateScaleChildren.x = 1.f / x;
      _negateScaleChildren.y = 1.f / y;
      _negateScaleChildren.z = 1.f / z;
    }
    _syncScaleVector();
  }

  scale(x / _scaleVector.x, y / _scaleVector.y, z / _scaleVector.z,
        scaleChildren);
}

void Bone::scale(float x, float y, float z, bool scaleChildren)
{
  auto& locMat     = getLocalMatrix();
  auto& origLocMat = Bone::_tmpMats[0];
  origLocMat.copyFrom(locMat);

  auto& origLocMatInv = Bone::_tmpMats[1];
  origLocMatInv.copyFrom(origLocMat);
  origLocMatInv.invert();

  auto& scaleMat = Bone::_tmpMats[2];
  Matrix::FromValuesToRef(x, 0, 0, 0, // M11-M14
                          0, y, 0, 0, // M21-M24
                          0, 0, z, 0, // M31-M34
                          0, 0, 0, 1, // M41-M44
                          scaleMat);
  _scaleMatrix.multiplyToRef(scaleMat, _scaleMatrix);
  _scaleVector.x *= x;
  _scaleVector.y *= y;
  _scaleVector.z *= z;

  locMat.multiplyToRef(origLocMatInv, locMat);
  locMat.multiplyToRef(scaleMat, locMat);
  locMat.multiplyToRef(origLocMat, locMat);

  auto parent = getParent();

  if (parent) {
    locMat.multiplyToRef(parent->getAbsoluteTransform(),
                         getAbsoluteTransform());
  }
  else {
    getAbsoluteTransform().copyFrom(locMat);
  }

  scaleMat.invert();

  for (auto& child : children) {
    auto& cm = child->getLocalMatrix();
    cm.multiplyToRef(scaleMat, cm);
    auto& lm = child->getLocalMatrix();
    lm.m[12] *= x;
    lm.m[13] *= y;
    lm.m[14] *= z;
  }

  computeAbsoluteTransforms();

  if (scaleChildren) {
    for (auto& child : children) {
      child->scale(x, y, z, scaleChildren);
    }
  }

  markAsDirty();
}

void Bone::setYawPitchRoll(float yaw, float pitch, float roll, Space space,
                           AbstractMesh* mesh)
{
  auto& rotMat = Bone::_tmpMats[0];
  Matrix::RotationYawPitchRollToRef(yaw, pitch, roll, rotMat);

  auto& rotMatInv = Bone::_tmpMats[1];

  _getNegativeRotationToRef(rotMatInv, space, mesh);

  rotMatInv.multiplyToRef(rotMat, rotMat);

  _rotateWithMatrix(rotMat, space, mesh);
}

void Bone::rotate(Vector3& axis, float amount, Space space, AbstractMesh* mesh)
{
  auto& rmat = Bone::_tmpMats[0];
  rmat.m[12] = 0.f;
  rmat.m[13] = 0.f;
  rmat.m[14] = 0.f;

  Matrix::RotationAxisToRef(axis, amount, rmat);

  _rotateWithMatrix(rmat, space, mesh);
}

void Bone::setAxisAngle(Vector3& axis, float angle, Space space,
                        AbstractMesh* mesh)
{
  auto& rotMat = Bone::_tmpMats[0];
  Matrix::RotationAxisToRef(axis, angle, rotMat);
  auto& rotMatInv = Bone::_tmpMats[1];

  _getNegativeRotationToRef(rotMatInv, space, mesh);

  rotMatInv.multiplyToRef(rotMat, rotMat);
  _rotateWithMatrix(rotMat, space, mesh);
}

void Bone::setRotation(const Vector3& rotation, Space space, AbstractMesh* mesh)
{
  setYawPitchRoll(rotation.y, rotation.x, rotation.z, space, mesh);
}

void Bone::setRotationQuaternion(const Quaternion& quat, Space space,
                                 AbstractMesh* mesh)
{
  auto& rotMatInv = Bone::_tmpMats[0];

  _getNegativeRotationToRef(rotMatInv, space, mesh);

  auto& rotMat = Bone::_tmpMats[1];
  Matrix::FromQuaternionToRef(quat, rotMat);

  rotMatInv.multiplyToRef(rotMat, rotMat);

  _rotateWithMatrix(rotMat, space, mesh);
}

void Bone::setRotationMatrix(const Matrix& rotMat, Space space,
                             AbstractMesh* mesh)
{
  auto& rotMatInv = Bone::_tmpMats[0];

  _getNegativeRotationToRef(rotMatInv, space, mesh);

  auto& rotMat2 = Bone::_tmpMats[1];
  rotMat2.copyFrom(rotMat);

  rotMatInv.multiplyToRef(rotMat, rotMat2);

  _rotateWithMatrix(rotMat2, space, mesh);
}

void Bone::_rotateWithMatrix(const Matrix& rmat, Space space,
                             AbstractMesh* mesh)
{
  auto& lmat           = getLocalMatrix();
  float lx             = lmat.m[12];
  float ly             = lmat.m[13];
  float lz             = lmat.m[14];
  auto parent          = getParent();
  auto& parentScale    = Bone::_tmpMats[3];
  auto& parentScaleInv = Bone::_tmpMats[4];

  if (parent) {
    if (space == Space::WORLD) {
      if (mesh) {
        parentScale.copyFrom(*mesh->getWorldMatrix());
        parent->getAbsoluteTransform().multiplyToRef(parentScale, parentScale);
      }
      else {
        parentScale.copyFrom(parent->getAbsoluteTransform());
      }
    }
    else {
      parentScale = parent->_scaleMatrix;
    }
    parentScaleInv.copyFrom(parentScale);
    parentScaleInv.invert();
    lmat.multiplyToRef(parentScale, lmat);
    lmat.multiplyToRef(rmat, lmat);
    lmat.multiplyToRef(parentScaleInv, lmat);
  }
  else {
    if (space == Space::WORLD && mesh) {
      parentScale.copyFrom(*mesh->getWorldMatrix());
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

  lmat.m[12] = lx;
  lmat.m[13] = ly;
  lmat.m[14] = lz;

  computeAbsoluteTransforms();

  markAsDirty();
}

void Bone::_getNegativeRotationToRef(Matrix& rotMatInv, Space space,
                                     AbstractMesh* mesh)
{
  if (space == Space::WORLD) {
    auto& scaleMatrix = Bone::_tmpMats[2];
    scaleMatrix.copyFrom(_scaleMatrix);
    rotMatInv.copyFrom(getAbsoluteTransform());

    if (mesh) {
      rotMatInv.multiplyToRef(*mesh->getWorldMatrix(), rotMatInv);
      auto& meshScale = Bone::_tmpMats[3];
      Matrix::ScalingToRef(mesh->scaling().x, mesh->scaling().y,
                           mesh->scaling().z, meshScale);
      scaleMatrix.multiplyToRef(meshScale, scaleMatrix);
    }

    rotMatInv.invert();
    scaleMatrix.m[0] *= _scalingDeterminant;
    rotMatInv.multiplyToRef(scaleMatrix, rotMatInv);
  }
  else {
    rotMatInv.copyFrom(getLocalMatrix());
    rotMatInv.invert();
    auto& scaleMatrix = Bone::_tmpMats[2];
    scaleMatrix.copyFrom(_scaleMatrix);

    if (_parent) {
      auto& pscaleMatrix = Bone::_tmpMats[3];
      pscaleMatrix.copyFrom(_parent->_scaleMatrix);
      pscaleMatrix.invert();
      pscaleMatrix.multiplyToRef(rotMatInv, rotMatInv);
    }
    else {
      scaleMatrix.m[0] *= _scalingDeterminant;
    }

    rotMatInv.multiplyToRef(scaleMatrix, rotMatInv);
  }
}

Vector3 Bone::getScale() const
{
  return _scaleVector;
}

void Bone::getScaleToRef(Vector3& result) const
{
  result.copyFrom(_scaleVector);
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

    result.x = lm.m[12];
    result.y = lm.m[13];
    result.z = lm.m[14];
  }
  else {

    Matrix* wm = nullptr;
    // mesh.getWorldMatrix() needs to be called before
    // skeleton.computeAbsoluteTransforms()
    if (mesh) {
      wm = mesh->getWorldMatrix();
    }

    _skeleton->computeAbsoluteTransforms();

    auto& tmat = Bone::_tmpMats[0];

    if (mesh) {
      tmat.copyFrom(getAbsoluteTransform());
      if (wm) {
        tmat.multiplyToRef(*wm, tmat);
      }
    }
    else {
      tmat = getAbsoluteTransform();
    }

    result.x = tmat.m[12];
    result.y = tmat.m[13];
    result.z = tmat.m[14];
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

void Bone::_syncScaleVector()
{
  const auto& lm = getLocalMatrix();

  const float xsq = (lm.m[0] * lm.m[0] + lm.m[1] * lm.m[1] + lm.m[2] * lm.m[2]);
  const float ysq = (lm.m[4] * lm.m[4] + lm.m[5] * lm.m[5] + lm.m[6] * lm.m[6]);
  const float zsq
    = (lm.m[8] * lm.m[8] + lm.m[9] * lm.m[9] + lm.m[10] * lm.m[10]);

  const float xs = lm.m[0] * lm.m[1] * lm.m[2] * lm.m[3] < 0 ? -1 : 1;
  const float ys = lm.m[4] * lm.m[5] * lm.m[6] * lm.m[7] < 0 ? -1 : 1;
  const float zs = lm.m[8] * lm.m[9] * lm.m[10] * lm.m[11] < 0 ? -1 : 1;

  _scaleVector.x = xs * ::std::sqrt(xsq);
  _scaleVector.y = ys * ::std::sqrt(ysq);
  _scaleVector.z = zs * ::std::sqrt(zsq);

  if (_parent) {
    _scaleVector.x /= _parent->_negateScaleChildren.x;
    _scaleVector.y /= _parent->_negateScaleChildren.y;
    _scaleVector.z /= _parent->_negateScaleChildren.z;
  }

  Matrix::FromValuesToRef(_scaleVector.x, 0.f, 0.f, 0.f, // M11-M14
                          0.f, _scaleVector.y, 0.f, 0.f, // M21-M24
                          0.f, 0.f, _scaleVector.z, 0.f, // M31-M34
                          0.f, 0.f, 0.f, 1.f,            // M41-M44
                          _scaleMatrix);
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
  Matrix* wm = nullptr;
  // mesh.getWorldMatrix() needs to be called before
  // skeleton.computeAbsoluteTransforms()
  if (mesh) {
    wm = mesh->getWorldMatrix();
  }

  _skeleton->computeAbsoluteTransforms();

  auto& mat = Bone::_tmpMats[0];

  mat.copyFrom(getAbsoluteTransform());

  if (mesh && wm) {
    mat.multiplyToRef(*wm, mat);
  }

  Vector3::TransformNormalToRef(localAxis, mat, result);

  result.normalize();
}

Vector3 Bone::getRotation(Space space, AbstractMesh* mesh) const
{
  auto result = Vector3::Zero();

  getRotationToRef(result, space, mesh);

  return result;
}

void Bone::getRotationToRef(Vector3& result, Space space,
                            AbstractMesh* mesh) const
{
  auto& quat = Bone::_tmpQuat;

  getRotationQuaternionToRef(quat, space, mesh);

  quat.toEulerAnglesToRef(result);
}

Quaternion Bone::getRotationQuaternion(Space space, AbstractMesh* mesh) const
{
  auto result = Quaternion::Identity();

  getRotationQuaternionToRef(result, space, mesh);

  return result;
}

void Bone::getRotationQuaternionToRef(Quaternion& result, Space space,
                                      AbstractMesh* mesh) const
{
  if (space == Space::LOCAL) {
    getLocalMatrix().decompose(Bone::_tmpVecs[0], result, Bone::_tmpVecs[1]);
  }
  else {
    auto& mat = Bone::_tmpMats[0];
    auto amat = getAbsoluteTransform();

    if (mesh) {
      amat.multiplyToRef(*mesh->getWorldMatrix(), mat);
    }
    else {
      mat.copyFrom(amat);
    }

    mat.m[0] *= _scalingDeterminant;
    mat.m[1] *= _scalingDeterminant;
    mat.m[2] *= _scalingDeterminant;

    mat.decompose(Bone::_tmpVecs[0], result, Bone::_tmpVecs[1]);
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
      amat.multiplyToRef(*mesh->getWorldMatrix(), mat);
    }
    else {
      mat.copyFrom(amat);
    }

    mat.m[0] *= _scalingDeterminant;
    mat.m[1] *= _scalingDeterminant;
    mat.m[2] *= _scalingDeterminant;

    mat.getRotationMatrixToRef(result);
  }
}

Vector3 Bone::getAbsolutePositionFromLocal(const Vector3& position,
                                           AbstractMesh* mesh) const
{
  auto result = Vector3::Zero();

  getAbsolutePositionFromLocalToRef(position, mesh, result);

  return result;
}

void Bone::getAbsolutePositionFromLocalToRef(const Vector3& position,
                                             AbstractMesh* mesh,
                                             Vector3& result) const
{
  Matrix* wm = nullptr;
  // mesh.getWorldMatrix() needs to be called before
  // skeleton.computeAbsoluteTransforms()
  if (mesh) {
    wm = mesh->getWorldMatrix();
  }

  _skeleton->computeAbsoluteTransforms();

  auto& tmat = Bone::_tmpMats[0];

  if (mesh) {
    tmat.copyFrom(getAbsoluteTransform());
    if (wm) {
      tmat.multiplyToRef(*wm, tmat);
    }
  }
  else {
    tmat = getAbsoluteTransform();
  }

  Vector3::TransformCoordinatesToRef(position, tmat, result);
}

Vector3 Bone::getLocalPositionFromAbsolute(const Vector3& position,
                                           AbstractMesh* mesh) const
{
  auto result = Vector3::Zero();

  getLocalPositionFromAbsoluteToRef(position, mesh, result);

  return result;
}

void Bone::getLocalPositionFromAbsoluteToRef(const Vector3& position,
                                             AbstractMesh* mesh,
                                             Vector3& result) const
{
  Matrix* wm = nullptr;
  // mesh.getWorldMatrix() needs to be called before
  // skeleton.computeAbsoluteTransforms()
  if (mesh) {
    wm = mesh->getWorldMatrix();
  }

  _skeleton->computeAbsoluteTransforms();

  auto tmat = Bone::_tmpMats[0];

  tmat.copyFrom(getAbsoluteTransform());

  if (mesh) {
    if (wm) {
      tmat.multiplyToRef(*wm, tmat);
    }
  }

  tmat.invert();

  Vector3::TransformCoordinatesToRef(position, tmat, result);
}

} // end of namespace BABYLON
