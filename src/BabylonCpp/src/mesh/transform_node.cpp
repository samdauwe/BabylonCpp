#include <babylon/mesh/transform_node.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

std::unique_ptr<Vector3> TransformNode::_lookAtVectorCache
  = std::make_unique<Vector3>(0.f, 0.f, 0.f);
std::unique_ptr<Quaternion> TransformNode::_rotationAxisCache
  = std::make_unique<Quaternion>();

TransformNode::TransformNode(const std::string& name, Scene* scene, bool isPure)
    : Node{name, scene}
    , billboardMode{TransformNode::BILLBOARDMODE_NONE}
    , scalingDeterminant{1.f}
    , infiniteDistance{false}
    , ignoreNonUniformScaling{false}
    , _poseMatrix{std::make_unique<Matrix>(Matrix::Zero())}
    , position{this, &TransformNode::get_position, &TransformNode::set_position}
    , rotation{this, &TransformNode::get_rotation, &TransformNode::set_rotation}
    , scaling{this, &TransformNode::get_scaling, &TransformNode::set_scaling}
    , rotationQuaternion{this, &TransformNode::get_rotationQuaternion,
                         &TransformNode::set_rotationQuaternion}
    , forward{this, &TransformNode::get_forward}
    , up{this, &TransformNode::get_up}
    , right{this, &TransformNode::get_right}
    , absolutePosition{this, &TransformNode::get_absolutePosition}
    , isWorldMatrixFrozen{this, &TransformNode::get_isWorldMatrixFrozen}
    , nonUniformScaling{this, &TransformNode::get_nonUniformScaling}
    , _scaling{Vector3::One()}
    , _isDirty{false}
    , _isWorldMatrixFrozen{false}
    , _isPure{isPure}
    , _forward{Vector3{0.f, 0.f, 1.f}}
    , _forwardNormalized{Vector3{0.f, 0.f, 1.f}}
    , _forwardInverted{Vector3{0.f, 0.f, -1.f}}
    , _up{Vector3{0.f, 1.f, 0.f}}
    , _upNormalized{Vector3{0.f, 1.f, 0.f}}
    , _right{Vector3{1.f, 0.f, 0.f}}
    , _rightNormalized{Vector3{1.f, 0.f, 0.f}}
    , _rightInverted{Vector3{-1.f, 0.f, 0.f}}
    , _position{Vector3::Zero()}
    , _rotation{Vector3::Zero()}
    , _rotationQuaternion{std::nullopt}
    , _transformToBoneReferal{nullptr}
    , _localWorld{Matrix::Zero()}
    , _absolutePosition{Vector3::Zero()}
    , _pivotMatrix{Matrix::Identity()}
    , _pivotMatrixInverse{nullptr}
    , _postMultiplyPivotMatrix{false}
    , _nonUniformScaling{false}
{
}

TransformNode::~TransformNode()
{
}

void TransformNode::addToScene(const TransformNodePtr& transformNode)
{
  if (transformNode->_isPure) {
    transformNode->getScene()->addTransformNode(transformNode);
  }
}

Type TransformNode::type() const
{
  return Type::TRANSFORMNODE;
}

AnimationValue
TransformNode::getProperty(const std::vector<std::string>& targetPropertyPath)
{
  if (targetPropertyPath.size() == 2) {
    const auto& target = targetPropertyPath[0];
    const auto& key    = targetPropertyPath[1];
    // Rotation
    if (target == "rotation") {
      return IAnimatable::getProperty(key, rotation());
    }
    // Scaling
    if (target == "scaling") {
      return IAnimatable::getProperty(key, scaling());
    }
  }

  return AnimationValue();
}

void TransformNode::setProperty(
  const std::vector<std::string>& targetPropertyPath,
  const AnimationValue& value)
{
  const auto animationType = value.animationType();
  if (animationType.has_value()) {
    if (targetPropertyPath.size() == 2) {
      const auto& target = targetPropertyPath[0];
      const auto& key    = targetPropertyPath[1];
      if (*animationType == Animation::ANIMATIONTYPE_FLOAT()) {
        auto floatValue = value.get<float>();
        // Rotation
        if (target == "rotation") {
          IAnimatable::setProperty(key, rotation(), floatValue);
        }
        // Scaling
        if (target == "scaling") {
          IAnimatable::setProperty(key, scaling(), floatValue);
        }
      }
    }
  }
}

const std::string TransformNode::getClassName() const
{
  return "TransformNode";
}

Vector3& TransformNode::get_position()
{
  return _position;
}

void TransformNode::set_position(const Vector3& newPosition)
{
  _position = newPosition;
  _isDirty  = true;
}

Vector3& TransformNode::get_rotation()
{
  return _rotation;
}

void TransformNode::set_rotation(const Vector3& newRotation)
{
  _rotation = newRotation;
  _isDirty  = true;
}

Vector3& TransformNode::get_scaling()
{
  return _scaling;
}

void TransformNode::set_scaling(const Vector3& newScaling)
{
  _scaling = newScaling;
  _isDirty = true;
}

std::optional<Quaternion>& TransformNode::get_rotationQuaternion()
{
  return _rotationQuaternion;
}

void TransformNode::set_rotationQuaternion(
  const std::optional<Quaternion>& quaternion)
{
  _rotationQuaternion = quaternion;
  // reset the rotation vector.
  if (quaternion && !stl_util::almost_equal(rotation().length(), 0.f)) {
    rotation().copyFromFloats(0.f, 0.f, 0.f);
  }
}

Vector3& TransformNode::get_forward()
{
  _forwardNormalized = Vector3::Normalize(Vector3::TransformNormal(
    getScene()->useRightHandedSystem() ? _forwardInverted : _forward, //
    getWorldMatrix()                                                  //
    ));
  return _forwardNormalized;
}

Vector3& TransformNode::get_up()
{
  _upNormalized
    = Vector3::Normalize(Vector3::TransformNormal(_up,             //
                                                  getWorldMatrix() //
                                                  ));
  return _upNormalized;
}

Vector3& TransformNode::get_right()
{
  _rightNormalized = Vector3::Normalize(Vector3::TransformNormal(
    getScene()->useRightHandedSystem() ? _rightInverted : _right, //
    getWorldMatrix()                                              //
    ));
  return _rightNormalized;
}

TransformNode& TransformNode::updatePoseMatrix(const Matrix& matrix)
{
  _poseMatrix->copyFrom(matrix);
  return *this;
}

Matrix& TransformNode::getPoseMatrix()
{
  return *_poseMatrix;
}
const Matrix& TransformNode::getPoseMatrix() const
{
  return *_poseMatrix;
}

bool TransformNode::_isSynchronized()
{
  if (_isDirty) {
    return false;
  }

  if (billboardMode != _cache.billboardMode
      || billboardMode != TransformNode::BILLBOARDMODE_NONE) {
    return false;
  }

  if (_cache.pivotMatrixUpdated) {
    return false;
  }

  if (infiniteDistance) {
    return false;
  }

  if (!_cache.position.equals(_position)) {
    return false;
  }

  if (_rotationQuaternion.has_value()) {
    if (!_cache.rotationQuaternion.equals(*_rotationQuaternion)) {
      return false;
    }
  }

  if (!_cache.rotation.equals(_rotation)) {
    return false;
  }

  if (!_cache.scaling.equals(_scaling)) {
    return false;
  }

  return true;
}

void TransformNode::_initCache()
{
  Node::_initCache();

  _cache.localMatrixUpdated = false;
  _cache.position           = Vector3::Zero();
  _cache.scaling            = Vector3::Zero();
  _cache.rotation           = Vector3::Zero();
  _cache.rotationQuaternion = Quaternion(0.f, 0.f, 0.f, 0.f);
  _cache.billboardMode      = AbstractMesh::BILLBOARDMODE_NONE;
}

TransformNode& TransformNode::markAsDirty(const std::string& property)
{
  if (property == "rotation") {
    rotationQuaternion = std::nullopt;
  }
  _currentRenderId = std::numeric_limits<int>::max();
  _isDirty         = true;
  return *this;
}

Vector3& TransformNode::get_absolutePosition()
{
  return _absolutePosition;
}

TransformNode& TransformNode::setPreTransformMatrix(Matrix& matrix)
{
  return setPivotMatrix(matrix, false);
}

TransformNode& TransformNode::setPivotMatrix(Matrix matrix,
                                             bool postMultiplyPivotMatrix)
{
  _pivotMatrix              = matrix;
  _cache.pivotMatrixUpdated = true;
  _postMultiplyPivotMatrix  = postMultiplyPivotMatrix;

  if (_postMultiplyPivotMatrix) {
    if (!_pivotMatrixInverse) {
      _pivotMatrixInverse
        = std::make_unique<Matrix>(Matrix::Invert(_pivotMatrix));
    }
    else {
      _pivotMatrix.invertToRef(*_pivotMatrixInverse);
    }
  }
  return *this;
}

Matrix& TransformNode::getPivotMatrix()
{
  return _pivotMatrix;
}

const Matrix& TransformNode::getPivotMatrix() const
{
  return _pivotMatrix;
}

TransformNode& TransformNode::freezeWorldMatrix()
{
  _isWorldMatrixFrozen
    = false; // no guarantee world is not already frozen, switch off temporarily
  computeWorldMatrix(true);
  _isWorldMatrixFrozen = true;
  return *this;
}

TransformNode& TransformNode::unfreezeWorldMatrix()
{
  _isWorldMatrixFrozen = false;
  computeWorldMatrix(true);
  return *this;
}

bool TransformNode::get_isWorldMatrixFrozen() const
{
  return _isWorldMatrixFrozen;
}

Vector3& TransformNode::getAbsolutePosition()
{
  computeWorldMatrix();
  return _absolutePosition;
}

TransformNode& TransformNode::setAbsolutePosition(
  const std::optional<Vector3>& iAbsolutePosition)
{
  if (!iAbsolutePosition) {
    return *this;
  }
  auto absolutePositionX = (*iAbsolutePosition).x;
  auto absolutePositionY = (*iAbsolutePosition).y;
  auto absolutePositionZ = (*iAbsolutePosition).z;

  if (parent()) {
    auto invertParentWorldMatrix = parent()->getWorldMatrix();
    invertParentWorldMatrix.invert();
    Vector3 worldPosition{absolutePositionX, absolutePositionY,
                          absolutePositionZ};
    position
      = Vector3::TransformCoordinates(worldPosition, invertParentWorldMatrix);
  }
  else {
    position().x = absolutePositionX;
    position().y = absolutePositionY;
    position().z = absolutePositionZ;
  }
  return *this;
}

TransformNode& TransformNode::setPositionWithLocalVector(const Vector3& vector3)
{
  computeWorldMatrix();
  position = Vector3::TransformNormal(vector3, _localWorld);
  return *this;
}

Vector3 TransformNode::getPositionExpressedInLocalSpace()
{
  computeWorldMatrix();
  auto invLocalWorldMatrix = _localWorld;
  invLocalWorldMatrix.invert();

  return Vector3::TransformNormal(position, invLocalWorldMatrix);
}

TransformNode& TransformNode::locallyTranslate(const Vector3& vector3)
{
  computeWorldMatrix(true);
  position = Vector3::TransformCoordinates(vector3, _localWorld);
  return *this;
}

TransformNode& TransformNode::lookAt(const Vector3& targetPoint, float yawCor,
                                     float pitchCor, float rollCor, Space space)
{
  auto& dv = *TransformNode::_lookAtVectorCache;
  auto pos = (space == Space::LOCAL) ? position : getAbsolutePosition();
  targetPoint.subtractToRef(pos, dv);
  auto yaw   = -std::atan2(dv.z, dv.x) - Math::PI_2;
  auto len   = std::sqrt(dv.x * dv.x + dv.z * dv.z);
  auto pitch = std::atan2(dv.y, len);
  if (rotationQuaternion()) {
    Quaternion::RotationYawPitchRollToRef(yaw + yawCor, pitch + pitchCor,
                                          rollCor, *rotationQuaternion());
  }
  else {
    rotation().x = pitch + pitchCor;
    rotation().y = yaw + yawCor;
    rotation().z = rollCor;
  }
  return *this;
}

Vector3 TransformNode::getDirection(const Vector3& localAxis)
{
  auto result = Vector3::Zero();

  getDirectionToRef(localAxis, result);

  return result;
}

TransformNode& TransformNode::getDirectionToRef(const Vector3& localAxis,
                                                Vector3& result)
{
  Vector3::TransformNormalToRef(localAxis, getWorldMatrix(), result);
  return *this;
}

TransformNode& TransformNode::setPivotPoint(Vector3& point, Space space)
{
  if (getScene()->getRenderId() == 0) {
    computeWorldMatrix(true);
  }

  auto wm = getWorldMatrix();

  if (space == Space::WORLD) {
    auto& tmat = Tmp::MatrixArray[0];
    wm.invertToRef(tmat);
    point = Vector3::TransformCoordinates(point, tmat);
  }

  auto pivotMatrix = Matrix::Translation(-point.x, -point.y, -point.z);
  return setPivotMatrix(pivotMatrix, true);
}

Vector3 TransformNode::getPivotPoint()
{
  auto point = Vector3::Zero();
  getPivotPointToRef(point);
  return point;
}

TransformNode& TransformNode::getPivotPointToRef(Vector3& result)
{
  result.x = -_pivotMatrix.m[12];
  result.y = -_pivotMatrix.m[13];
  result.z = -_pivotMatrix.m[14];
  return *this;
}

Vector3 TransformNode::getAbsolutePivotPoint()
{
  auto point = Vector3::Zero();
  getAbsolutePivotPointToRef(point);
  return point;
}

TransformNode& TransformNode::getAbsolutePivotPointToRef(Vector3& result)
{
  result.x = _pivotMatrix.m[12];
  result.y = _pivotMatrix.m[13];
  result.z = _pivotMatrix.m[14];
  getPivotPointToRef(result);
  Vector3::TransformCoordinatesToRef(result, getWorldMatrix(), result);
  return *this;
}

TransformNode& TransformNode::setParent(Node* node)
{
  if (!node && !parent()) {
    return *this;
  }
  if (!node) {
    auto& iRotation   = Tmp::QuaternionArray[0];
    auto& newPosition = Tmp::Vector3Array[0];
    auto& scale       = Tmp::Vector3Array[1];

    if (parent()) {
      parent()->computeWorldMatrix(true);
    }
    computeWorldMatrix(true);
    getWorldMatrix().decompose(scale, iRotation, newPosition);

    if (rotationQuaternion()) {
      (*rotationQuaternion()).copyFrom(iRotation);
    }
    else {
      iRotation.toEulerAnglesToRef(this->rotation());
    }

    scaling().x = scale.x;
    scaling().y = scale.y;
    scaling().z = scale.z;

    position().x = newPosition.x;
    position().y = newPosition.y;
    position().z = newPosition.z;
  }
  else {
    auto& iRotation       = Tmp::QuaternionArray[0];
    auto& newPosition     = Tmp::Vector3Array[0];
    auto& scale           = Tmp::Vector3Array[1];
    auto& diffMatrix      = Tmp::MatrixArray[0];
    auto& invParentMatrix = Tmp::MatrixArray[1];

    computeWorldMatrix(true);
    node->computeWorldMatrix(true);

    node->getWorldMatrix().invertToRef(invParentMatrix);
    getWorldMatrix().multiplyToRef(invParentMatrix, diffMatrix);
    diffMatrix.decompose(scale, iRotation, newPosition);

    if (rotationQuaternion()) {
      (*rotationQuaternion()).copyFrom(iRotation);
    }
    else {
      iRotation.toEulerAnglesToRef(this->rotation());
    }

    position().x = newPosition.x;
    position().y = newPosition.y;
    position().z = newPosition.z;

    scaling().x = scale.x;
    scaling().y = scale.y;
    scaling().z = scale.z;
  }

  Node::set_parent(node);
  return *this;
}

bool TransformNode::get_nonUniformScaling() const
{
  return _nonUniformScaling;
}

bool TransformNode::_updateNonUniformScalingState(bool value)
{
  if (_nonUniformScaling == value) {
    return false;
  }

  _nonUniformScaling = value;
  return true;
}

TransformNode& TransformNode::attachToBone(Bone* bone,
                                           TransformNode* affectedTransformNode)
{
  _transformToBoneReferal = affectedTransformNode;
  Node::set_parent(bone);

  if (bone->getWorldMatrix().determinant() < 0.f) {
    scalingDeterminant *= -1.f;
  }
  return *this;
}

TransformNode& TransformNode::detachFromBone()
{
  if (!parent()) {
    return *this;
  }

  if (parent()->getWorldMatrix().determinant() < 0.f) {
    scalingDeterminant *= -1.f;
  }
  _transformToBoneReferal = nullptr;
  setParent(nullptr);
  return *this;
}

TransformNode& TransformNode::rotate(Vector3 axis, float amount, Space space)
{
  axis.normalize();
  if (!rotationQuaternion()) {
    rotationQuaternion = Quaternion::RotationYawPitchRoll(
      rotation().y, rotation().x, rotation().z);
    rotation = Vector3::Zero();
  }
  Quaternion iRotationQuaternion;
  if (space == Space::LOCAL) {
    iRotationQuaternion = Quaternion::RotationAxisToRef(
      axis, amount, *TransformNode::_rotationAxisCache);
    this->rotationQuaternion()->multiplyToRef(iRotationQuaternion,
                                              *rotationQuaternion());
  }
  else {
    if (parent()) {
      auto invertParentWorldMatrix = parent()->getWorldMatrix();
      invertParentWorldMatrix.invert();
      axis = Vector3::TransformNormal(axis, invertParentWorldMatrix);
    }
    iRotationQuaternion = Quaternion::RotationAxisToRef(
      axis, amount, *TransformNode::_rotationAxisCache);
    iRotationQuaternion.multiplyToRef(*rotationQuaternion(),
                                      *rotationQuaternion());
  }
  return *this;
}

TransformNode& TransformNode::rotateAround(const Vector3& point, Vector3& axis,
                                           float amount)
{
  axis.normalize();
  if (!rotationQuaternion()) {
    rotationQuaternion = Quaternion::RotationYawPitchRoll(
      rotation().y, rotation().x, rotation().z);
    rotation().copyFromFloats(0.f, 0.f, 0.f);
  }
  point.subtractToRef(position, Tmp::Vector3Array[0]);
  Matrix::TranslationToRef(Tmp::Vector3Array[0].x, Tmp::Vector3Array[0].y,
                           Tmp::Vector3Array[0].z, Tmp::MatrixArray[0]);
  Tmp::MatrixArray[0].invertToRef(Tmp::MatrixArray[2]);
  Matrix::RotationAxisToRef(axis, amount, Tmp::MatrixArray[1]);
  Tmp::MatrixArray[2].multiplyToRef(Tmp::MatrixArray[1], Tmp::MatrixArray[2]);
  Tmp::MatrixArray[2].multiplyToRef(Tmp::MatrixArray[0], Tmp::MatrixArray[2]);

  Tmp::MatrixArray[2].decompose(Tmp::Vector3Array[0], Tmp::QuaternionArray[0],
                                Tmp::Vector3Array[1]);

  position().addInPlace(Tmp::Vector3Array[1]);
  Tmp::QuaternionArray[0].multiplyToRef(*rotationQuaternion(),
                                        *rotationQuaternion());

  return *this;
}

TransformNode& TransformNode::translate(const Vector3& axis, float distance,
                                        Space space)
{
  auto displacementVector = axis.scale(distance);
  if (space == Space::LOCAL) {
    auto tempV3 = getPositionExpressedInLocalSpace().add(displacementVector);
    setPositionWithLocalVector(tempV3);
  }
  else {
    setAbsolutePosition(getAbsolutePosition().add(displacementVector));
  }
  return *this;
}

TransformNode& TransformNode::addRotation(float x, float y, float z)
{
  Quaternion rotationQuaternionTmp;
  if (rotationQuaternion()) {
    rotationQuaternionTmp = *rotationQuaternion();
  }
  else {
    rotationQuaternionTmp = Tmp::QuaternionArray[1];
    Quaternion::RotationYawPitchRollToRef(rotation().y, rotation().x,
                                          rotation().z, rotationQuaternionTmp);
  }
  auto& accumulation = Tmp::QuaternionArray[0];
  Quaternion::RotationYawPitchRollToRef(y, x, z, accumulation);
  rotationQuaternionTmp.multiplyInPlace(accumulation);
  if (!rotationQuaternion()) {
    rotationQuaternionTmp.toEulerAnglesToRef(rotation());
  }
  return *this;
}

Matrix& TransformNode::computeWorldMatrix(bool force,
                                          bool /*useWasUpdatedFlag*/)
{
  if (_isWorldMatrixFrozen) {
    return _worldMatrix;
  }

  if (!force && isSynchronized()) {
    _currentRenderId = getScene()->getRenderId();
    return _worldMatrix;
  }

  updateCache();
  _cache.position.copyFrom(position);
  _cache.scaling.copyFrom(scaling());
  _cache.pivotMatrixUpdated = false;
  _cache.billboardMode      = billboardMode;
  _currentRenderId          = getScene()->getRenderId();
  _childRenderId            = getScene()->getRenderId();
  _isDirty                  = false;

  // Scaling
  Matrix::ScalingToRef(scaling().x * scalingDeterminant,
                       scaling().y * scalingDeterminant,
                       scaling().z * scalingDeterminant, Tmp::MatrixArray[1]);

  // Rotation

  // rotate, if quaternion is set and rotation was used
  if (rotationQuaternion()) {
    auto len = rotation().length();
    if (len != 0.f) {
      (*rotationQuaternion())
        .multiplyInPlace(Quaternion::RotationYawPitchRoll(
          rotation().y, rotation().x, rotation().z));
      rotation().copyFromFloats(0.f, 0.f, 0.f);
    }
  }

  if (rotationQuaternion()) {
    (*rotationQuaternion()).toRotationMatrix(Tmp::MatrixArray[0]);
    _cache.rotationQuaternion.copyFrom(*rotationQuaternion());
  }
  else {
    Matrix::RotationYawPitchRollToRef(rotation().y, rotation().x, rotation().z,
                                      Tmp::MatrixArray[0]);
    _cache.rotation.copyFrom(rotation());
  }

  // Translation
  auto& camera = getScene()->activeCamera;

  if (infiniteDistance && !parent() && camera) {

    auto cameraWorldMatrix = camera->getWorldMatrix();

    Vector3 cameraGlobalPosition(cameraWorldMatrix.m[12],
                                 cameraWorldMatrix.m[13],
                                 cameraWorldMatrix.m[14]);

    Matrix::TranslationToRef(position().x + cameraGlobalPosition.x,
                             position().y + cameraGlobalPosition.y,
                             position().z + cameraGlobalPosition.z,
                             Tmp::MatrixArray[2]);
  }
  else {
    Matrix::TranslationToRef(position().x, position().y, position().z,
                             Tmp::MatrixArray[2]);
  }

  // Composing transformations
  _pivotMatrix.multiplyToRef(Tmp::MatrixArray[1], Tmp::MatrixArray[4]);
  Tmp::MatrixArray[4].multiplyToRef(Tmp::MatrixArray[0], Tmp::MatrixArray[5]);

  // Billboarding (testing PG:http://www.babylonjs-playground.com/#UJEIL#13)
  if (billboardMode != TransformNode::BILLBOARDMODE_NONE && camera) {
    if ((billboardMode & TransformNode::BILLBOARDMODE_ALL)
        != AbstractMesh::BILLBOARDMODE_ALL) {
      // Need to decompose each rotation here
      auto& currentPosition = Tmp::Vector3Array[3];

      if (parent()) {
        if (_transformToBoneReferal) {
          parent()->getWorldMatrix().multiplyToRef(
            _transformToBoneReferal->getWorldMatrix(), Tmp::MatrixArray[6]);
          Vector3::TransformCoordinatesToRef(position, Tmp::MatrixArray[6],
                                             currentPosition);
        }
        else {
          Vector3::TransformCoordinatesToRef(
            position, parent()->getWorldMatrix(), currentPosition);
        }
      }
      else {
        currentPosition.copyFrom(position);
      }

      currentPosition.subtractInPlace(camera->globalPosition());

      auto finalEuler = Tmp::Vector3Array[4].copyFromFloats(0.f, 0.f, 0.f);
      if ((billboardMode & TransformNode::BILLBOARDMODE_X)
          == TransformNode::BILLBOARDMODE_X) {
        finalEuler.x = std::atan2(-currentPosition.y, currentPosition.z);
      }

      if ((billboardMode & TransformNode::BILLBOARDMODE_Y)
          == TransformNode::BILLBOARDMODE_Y) {
        finalEuler.y = std::atan2(currentPosition.x, currentPosition.z);
      }

      if ((billboardMode & TransformNode::BILLBOARDMODE_Z)
          == TransformNode::BILLBOARDMODE_Z) {
        finalEuler.z = std::atan2(currentPosition.y, currentPosition.x);
      }

      Matrix::RotationYawPitchRollToRef(finalEuler.y, finalEuler.x,
                                        finalEuler.z, Tmp::MatrixArray[0]);
    }
    else {
      Tmp::MatrixArray[1].copyFrom(camera->getViewMatrix());

      Tmp::MatrixArray[1].setTranslationFromFloats(0, 0, 0);
      Tmp::MatrixArray[1].invertToRef(Tmp::MatrixArray[0]);
    }

    Tmp::MatrixArray[1].copyFrom(Tmp::MatrixArray[5]);
    Tmp::MatrixArray[1].multiplyToRef(Tmp::MatrixArray[0], Tmp::MatrixArray[5]);
  }

  // Post multiply inverse of pivotMatrix
  if (_postMultiplyPivotMatrix) {
    Tmp::MatrixArray[5].multiplyToRef(*_pivotMatrixInverse,
                                      Tmp::MatrixArray[5]);
  }

  // Local world
  Tmp::MatrixArray[5].multiplyToRef(Tmp::MatrixArray[2], _localWorld);

  // Parent
  if (parent()) {
    if (billboardMode != TransformNode::BILLBOARDMODE_NONE) {
      if (_transformToBoneReferal) {
        parent()->getWorldMatrix().multiplyToRef(
          _transformToBoneReferal->getWorldMatrix(), Tmp::MatrixArray[6]);
        Tmp::MatrixArray[5].copyFrom(Tmp::MatrixArray[6]);
      }
      else {
        Tmp::MatrixArray[5].copyFrom(parent()->getWorldMatrix());
      }

      _localWorld.getTranslationToRef(Tmp::Vector3Array[5]);
      Vector3::TransformCoordinatesToRef(
        Tmp::Vector3Array[5], Tmp::MatrixArray[5], Tmp::Vector3Array[5]);
      _worldMatrix.copyFrom(_localWorld);
      _worldMatrix.setTranslation(Tmp::Vector3Array[5]);
    }
    else {
      if (_transformToBoneReferal) {
        _localWorld.multiplyToRef(parent()->getWorldMatrix(),
                                  Tmp::MatrixArray[6]);
        Tmp::MatrixArray[6].multiplyToRef(
          _transformToBoneReferal->getWorldMatrix(), _worldMatrix);
      }
      else {
        _localWorld.multiplyToRef(parent()->getWorldMatrix(), _worldMatrix);
      }
    }
    _markSyncedWithParent();
  }
  else {
    _worldMatrix.copyFrom(_localWorld);
  }

  // Normal matrix
  if (!ignoreNonUniformScaling) {
    if (scaling().isNonUniform()) {
      _updateNonUniformScalingState(true);
    }
    else if (parent() && static_cast<TransformNode*>(parent())
             && (static_cast<TransformNode*>(parent()))->_nonUniformScaling) {
      const auto parentTransformNode = static_cast<TransformNode*>(parent());
      _updateNonUniformScalingState(parentTransformNode->_nonUniformScaling);
    }
    else {
      _updateNonUniformScalingState(false);
    }
  }
  else {
    _updateNonUniformScalingState(false);
  }

  _afterComputeWorldMatrix();

  // Absolute position
  _absolutePosition.copyFromFloats(_worldMatrix.m[12], _worldMatrix.m[13],
                                   _worldMatrix.m[14]);

  // Callbacks
  onAfterWorldMatrixUpdateObservable.notifyObservers(this);

  if (!_poseMatrix) {
    _poseMatrix = std::make_unique<Matrix>(Matrix::Invert(_worldMatrix));
  }

  // Cache the determinant
  _worldMatrixDeterminant = _worldMatrix.determinant();

  return _worldMatrix;
}

void TransformNode::_afterComputeWorldMatrix()
{
}

TransformNode& TransformNode::registerAfterWorldMatrixUpdate(
  const std::function<void(TransformNode* mesh, EventState& es)>& func)
{
  onAfterWorldMatrixUpdateObservable.add(func);
  return *this;
}

TransformNode& TransformNode::unregisterAfterWorldMatrixUpdate(
  const std::function<void(TransformNode* mesh, EventState& es)>& func)
{
  onAfterWorldMatrixUpdateObservable.removeCallback(func);
  return *this;
}

TransformNodePtr TransformNode::clone(const std::string& /*name*/,
                                      Node* /*newParent*/,
                                      bool /*doNotCloneChildren*/)
{
  return nullptr;
}

json TransformNode::serialize(json& /*currentSerializationObject*/)
{
  return nullptr;
}

TransformNodePtr TransformNode::Parse(const json& /*parsedTransformNode*/,
                                      Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
  return nullptr;
}

void TransformNode::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  getScene()->removeTransformNode(this);

  onAfterWorldMatrixUpdateObservable.clear();

  Node::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
