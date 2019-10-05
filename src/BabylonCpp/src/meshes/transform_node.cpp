#include <babylon/meshes/transform_node.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/math/tmp.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

std::unique_ptr<Vector3> TransformNode::_lookAtVectorCache
  = std::make_unique<Vector3>(0.f, 0.f, 0.f);
std::unique_ptr<Quaternion> TransformNode::_rotationAxisCache
  = std::make_unique<Quaternion>();

TransformNode::TransformNode(const std::string& iName, Scene* scene,
                             bool isPure)
    : Node{iName, scene}
    , billboardMode{this, &TransformNode::get_billboardMode,
                    &TransformNode::set_billboardMode}
    , preserveParentRotationForBillboard{this,
                                         &TransformNode::
                                           get_preserveParentRotationForBillboard,
                                         &TransformNode::
                                           set_preserveParentRotationForBillboard}
    , infiniteDistance{this, &TransformNode::get_infiniteDistance,
                       &TransformNode::set_infiniteDistance}
    , scalingDeterminant{1.f}
    , ignoreNonUniformScaling{false}
    , reIntegrateRotationIntoRotationQuaternion{false}
    , _poseMatrix{std::make_unique<Matrix>(Matrix::Identity())}
    , _localMatrix{Matrix::Zero()}
    , _indexInSceneTransformNodesArray{-1}
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
    , _postMultiplyPivotMatrix{false}
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
    , _billboardMode{TransformNode::BILLBOARDMODE_NONE}
    , _preserveParentRotationForBillboard(false)
    , _infiniteDistance{false}
    , _localWorld{Matrix::Zero()}
    , _usePivotMatrix{false}
    , _absolutePosition{Vector3::Zero()}
    , _pivotMatrix{Matrix::Identity()}
    , _pivotMatrixInverse{nullptr}
    , _nonUniformScaling{false}
{
}

TransformNode::~TransformNode()
{
}

void TransformNode::addToScene(const TransformNodePtr& transformNode)
{
  transformNode->addToRootNodes();
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
  if (targetPropertyPath.size() == 1) {
    const auto& target = targetPropertyPath[0];
    if (target == "position") {
      return position();
    }
    if (target == "rotationQuaternion") {
      return rotationQuaternion().value_or(Quaternion());
    }
  }
  else if (targetPropertyPath.size() == 2) {
    const auto& target = targetPropertyPath[0];
    const auto& key    = targetPropertyPath[1];
    // Rotation
    if (target == "rotation") {
      return IAnimatable::getProperty(key.c_str(), rotation());
    }
    // Scaling
    if (target == "scaling") {
      return IAnimatable::getProperty(key.c_str(), scaling());
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
    if (targetPropertyPath.size() == 1) {
      const auto& target = targetPropertyPath[0];
      if (*animationType == Animation::ANIMATIONTYPE_QUATERNION()) {
        auto quaternionValue = value.get<Quaternion>();
        if (target == "rotationQuaternion") {
          rotationQuaternion = quaternionValue;
        }
      }
      else if (*animationType == Animation::ANIMATIONTYPE_VECTOR3()) {
        auto vector3Value = value.get<Vector3>();
        // Position
        if (target == "position") {
          position = vector3Value;
        }
        // Rotation
        if (target == "rotation") {
          rotation = vector3Value;
        }
        // Scaling
        if (target == "scaling") {
          scaling = vector3Value;
        }
      }
    }
    if (targetPropertyPath.size() == 2) {
      const auto& target = targetPropertyPath[0];
      const auto& key    = targetPropertyPath[1];
      if (*animationType == Animation::ANIMATIONTYPE_FLOAT()) {
        const auto& floatValue = value.get<float>();
        // Position
        if (target == "position") {
          IAnimatable::setProperty(key.c_str(), position(), floatValue);
        }
        // Rotation
        if (target == "rotation") {
          IAnimatable::setProperty(key.c_str(), rotation(), floatValue);
        }
        // Scaling
        if (target == "scaling") {
          IAnimatable::setProperty(key.c_str(), scaling(), floatValue);
        }
      }
    }
  }
}

const std::string TransformNode::getClassName() const
{
  return "TransformNode";
}

unsigned int TransformNode::get_billboardMode() const
{
  return _billboardMode;
}

void TransformNode::set_billboardMode(unsigned int value)
{
  if (_billboardMode == value) {
    return;
  }
  _billboardMode = value;
}

bool TransformNode::get_preserveParentRotationForBillboard() const
{
  return _preserveParentRotationForBillboard;
}

void TransformNode::set_preserveParentRotationForBillboard(bool value)
{
  if (value == _preserveParentRotationForBillboard) {
    return;
  }
  _preserveParentRotationForBillboard = value;
}

bool TransformNode::get_infiniteDistance() const
{
  return _infiniteDistance;
}

void TransformNode::set_infiniteDistance(bool value)
{
  if (_infiniteDistance == value) {
    return;
  }

  _infiniteDistance = value;
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
  _rotation           = newRotation;
  _rotationQuaternion = std::nullopt;
  _isDirty            = true;
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
  if (quaternion) {
    _rotation.setAll(0.f);
  }
  _isDirty = true;
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
  if (!_poseMatrix) {
    _poseMatrix = matrix.clone();
    return *this;
  }
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
  auto& cache = _cache;

  if (billboardMode != cache.billboardMode
      || billboardMode != TransformNode::BILLBOARDMODE_NONE) {
    return false;
  }

  if (cache.pivotMatrixUpdated) {
    return false;
  }

  if (infiniteDistance) {
    return false;
  }

  if (!cache.position.equals(_position)) {
    return false;
  }

  if (_rotationQuaternion.has_value()) {
    if (!cache.rotationQuaternion.equals(*_rotationQuaternion)) {
      return false;
    }
  }
  else if (!cache.rotation.equals(_rotation)) {
    return false;
  }

  if (!cache.scaling.equals(_scaling)) {
    return false;
  }

  return true;
}

void TransformNode::_initCache()
{
  Node::_initCache();

  auto& cache              = _cache;
  cache.localMatrixUpdated = false;
  cache.position           = Vector3::Zero();
  cache.scaling            = Vector3::Zero();
  cache.rotation           = Vector3::Zero();
  cache.rotationQuaternion = Quaternion(0.f, 0.f, 0.f, 0.f);
  cache.billboardMode      = AbstractMesh::BILLBOARDMODE_NONE;
  cache.infiniteDistance   = false;
}

TransformNode& TransformNode::markAsDirty(const std::string& /*property*/)
{
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
  _pivotMatrix.copyFrom(matrix);
  _usePivotMatrix = !_pivotMatrix.isIdentity();

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
    auto& invertParentWorldMatrix = Tmp::MatrixArray[0];
    parent()->getWorldMatrix().invertToRef(invertParentWorldMatrix);
    Vector3::TransformCoordinatesFromFloatsToRef(
      absolutePositionX, absolutePositionY, absolutePositionZ,
      invertParentWorldMatrix, position);
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
  position = Vector3::TransformNormal(vector3, _localMatrix);
  return *this;
}

Vector3 TransformNode::getPositionExpressedInLocalSpace()
{
  computeWorldMatrix();
  auto& invLocalWorldMatrix = Tmp::MatrixArray[0];
  _localMatrix.invertToRef(invLocalWorldMatrix);
  return Vector3::TransformNormal(position, invLocalWorldMatrix);
}

TransformNode& TransformNode::locallyTranslate(const Vector3& vector3)
{
  computeWorldMatrix(true);
  position = Vector3::TransformCoordinates(vector3, _localMatrix);
  return *this;
}

TransformNode& TransformNode::lookAt(const Vector3& targetPoint, float yawCor,
                                     float pitchCor, float rollCor, Space space)
{
  auto& dv = *TransformNode::_lookAtVectorCache;
  auto pos = (space == Space::LOCAL) ? position : getAbsolutePosition();
  targetPoint.subtractToRef(pos, dv);
  setDirection(dv, yawCor, pitchCor, rollCor);

  // Correct for parent's rotation offset
  if (space == Space::WORLD && parent()) {
    if (rotationQuaternion().has_value()) {
      // Get local rotation matrix of the looking object
      auto& rotationMatrix = Tmp::MatrixArray[0];
      rotationQuaternion()->toRotationMatrix(rotationMatrix);

      // Offset rotation by parent's inverted rotation matrix to correct in
      // world space
      auto& parentRotationMatrix = Tmp::MatrixArray[1];
      parent()->getWorldMatrix().getRotationMatrixToRef(parentRotationMatrix);
      parentRotationMatrix.invert();
      rotationMatrix.multiplyToRef(parentRotationMatrix, rotationMatrix);
      rotationQuaternion()->fromRotationMatrix(rotationMatrix);
    }
    else {
      // Get local rotation matrix of the looking object
      auto& quaternionRotation = Tmp::QuaternionArray[0];
      Quaternion::FromEulerVectorToRef(rotation, quaternionRotation);
      auto& rotationMatrix = Tmp::MatrixArray[0];
      quaternionRotation.toRotationMatrix(rotationMatrix);

      // Offset rotation by parent's inverted rotation matrix to correct in
      // world space
      auto& parentRotationMatrix = Tmp::MatrixArray[1];
      parent()->getWorldMatrix().getRotationMatrixToRef(parentRotationMatrix);
      parentRotationMatrix.invert();
      rotationMatrix.multiplyToRef(parentRotationMatrix, rotationMatrix);
      quaternionRotation.fromRotationMatrix(rotationMatrix);
      quaternionRotation.toEulerAnglesToRef(rotation());
    }
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

TransformNode& TransformNode::setDirection(const Vector3& localAxis,
                                           float yawCor, float pitchCor,
                                           float rollCor)
{
  const auto yaw = -std::atan2(localAxis.z, localAxis.x) + Math::PI_2;
  const auto len
    = std::sqrt(localAxis.x * localAxis.x + localAxis.z * localAxis.z);
  const auto pitch = -std::atan2(localAxis.y, len);
  if (rotationQuaternion().has_value()) {
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
  const auto& _pivotMatrixM = _pivotMatrix.m();
  result.x                  = -_pivotMatrixM[12];
  result.y                  = -_pivotMatrixM[13];
  result.z                  = -_pivotMatrixM[14];
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
  const auto& _pivotMatrixM = _pivotMatrix.m();
  result.x                  = _pivotMatrixM[12];
  result.y                  = _pivotMatrixM[13];
  result.z                  = _pivotMatrixM[14];
  getPivotPointToRef(result);
  Vector3::TransformCoordinatesToRef(result, getWorldMatrix(), result);
  return *this;
}

TransformNode& TransformNode::setParent(Node* node)
{
  if (!node && !parent()) {
    return *this;
  }

  auto& quatRotation = Tmp::QuaternionArray[0];
  auto& newPosition  = Tmp::Vector3Array[0];
  auto& scale        = Tmp::Vector3Array[1];

  if (!node) {
    if (parent()) {
      parent()->computeWorldMatrix(true);
    }
    computeWorldMatrix(true);
    getWorldMatrix().decompose(scale, quatRotation, newPosition);
  }
  else {
    auto& diffMatrix      = Tmp::MatrixArray[0];
    auto& invParentMatrix = Tmp::MatrixArray[1];

    computeWorldMatrix(true);
    node->computeWorldMatrix(true);

    node->getWorldMatrix().invertToRef(invParentMatrix);
    getWorldMatrix().multiplyToRef(invParentMatrix, diffMatrix);
    diffMatrix.decompose(scale, quatRotation, newPosition);
  }

  if (rotationQuaternion()) {
    rotationQuaternion()->copyFrom(quatRotation);
  }
  else {
    quatRotation.toEulerAnglesToRef(rotation());
  }

  scaling().copyFrom(scale);
  position().copyFrom(newPosition);

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
    rotationQuaternion = rotation().toQuaternion();
    rotation().setAll(0.f);
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
      auto& invertParentWorldMatrix = Tmp::MatrixArray[0];
      parent()->getWorldMatrix().invertToRef(invertParentWorldMatrix);
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
  if (!rotationQuaternion().has_value()) {
    rotationQuaternion = Quaternion::RotationYawPitchRoll(
      rotation().y, rotation().x, rotation().z);
    rotation().setAll(0.f);
  }

  auto& tmpVector        = Tmp::Vector3Array[0];
  auto& finalScale       = Tmp::Vector3Array[1];
  auto& finalTranslation = Tmp::Vector3Array[2];

  auto& finalRotation = Tmp::QuaternionArray[0];

  auto& translationMatrix    = Tmp::MatrixArray[0]; // T
  auto& translationMatrixInv = Tmp::MatrixArray[1]; // T'
  auto& rotationMatrix       = Tmp::MatrixArray[2]; // R
  auto& finalMatrix          = Tmp::MatrixArray[3]; // T' x R x T

  point.subtractToRef(position, tmpVector);
  Matrix::TranslationToRef(tmpVector.x, tmpVector.y, tmpVector.z,
                           translationMatrix); // T
  Matrix::TranslationToRef(-tmpVector.x, -tmpVector.y, -tmpVector.z,
                           translationMatrixInv);          // T'
  Matrix::RotationAxisToRef(axis, amount, rotationMatrix); // R

  translationMatrixInv.multiplyToRef(rotationMatrix, finalMatrix); // T' x R
  finalMatrix.multiplyToRef(translationMatrix, finalMatrix);       // T' x R x T

  finalMatrix.decompose(finalScale, finalRotation, finalTranslation);

  position().addInPlace(finalTranslation);
  finalRotation.multiplyToRef(*rotationQuaternion(), *rotationQuaternion());

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

Node* TransformNode::_getEffectiveParent() const
{
  return parent();
}

Matrix& TransformNode::computeWorldMatrix(bool force,
                                          bool /*useWasUpdatedFlag*/)
{
  if (_isWorldMatrixFrozen && !_isDirty) {
    return _worldMatrix;
  }

  const auto currentRenderId = getScene()->getRenderId();
  if (!_isDirty && !force && isSynchronized()) {
    _currentRenderId = currentRenderId;
    return _worldMatrix;
  }

  updateCache();
  auto& cache              = _cache;
  cache.pivotMatrixUpdated = false;
  cache.billboardMode      = billboardMode();
  cache.infiniteDistance   = infiniteDistance();

  _currentRenderId = currentRenderId;
  _childUpdateId++;
  _isDirty              = false;
  auto iParent          = _getEffectiveParent();
  auto useBillboardPath = _billboardMode != TransformNode::BILLBOARDMODE_NONE
                          && !preserveParentRotationForBillboard();
  auto& camera = getScene()->activeCamera();

  // Scaling
  auto& iScaling     = cache.scaling;
  auto& iTranslation = cache.position;

  // Translation
  if (_infiniteDistance) {
    if (!parent() && camera) {
      auto cameraWorldMatrix = camera->getWorldMatrix();
      auto cameraGlobalPosition
        = Vector3(cameraWorldMatrix.m()[12], cameraWorldMatrix.m()[13],
                  cameraWorldMatrix.m()[14]);

      iTranslation.copyFromFloats(_position.x + cameraGlobalPosition.x,
                                  _position.y + cameraGlobalPosition.y,
                                  _position.z + cameraGlobalPosition.z);
    }
    else {
      iTranslation.copyFrom(_position);
    }
  }
  else {
    iTranslation.copyFrom(_position);
  }

  // Scaling
  iScaling.copyFromFloats(_scaling.x * scalingDeterminant,
                          _scaling.y * scalingDeterminant,
                          _scaling.z * scalingDeterminant);

  // Rotation
  auto& iRotation = cache.rotationQuaternion;
  if (_rotationQuaternion.has_value()) {
    if (reIntegrateRotationIntoRotationQuaternion) {
      const auto len = rotation().lengthSquared();
      if (len != 0.f) {
        _rotationQuaternion->multiplyInPlace(Quaternion::RotationYawPitchRoll(
          _rotation.y, _rotation.x, _rotation.z));
        _rotation.copyFromFloats(0.f, 0.f, 0.f);
      }
    }
    iRotation.copyFrom(*_rotationQuaternion);
  }
  else {
    Quaternion::RotationYawPitchRollToRef(_rotation.y, _rotation.x, _rotation.z,
                                          iRotation);
    cache.rotation.copyFrom(_rotation);
  }

  // Compose
  if (_usePivotMatrix) {
    auto& scaleMatrix = Tmp::MatrixArray[1];
    Matrix::ScalingToRef(iScaling.x, iScaling.y, iScaling.z, scaleMatrix);

    // Rotation
    auto& rotationMatrix = Tmp::MatrixArray[0];
    iRotation.toRotationMatrix(rotationMatrix);

    // Composing transformations
    _pivotMatrix.multiplyToRef(scaleMatrix, Tmp::MatrixArray[4]);
    Tmp::MatrixArray[4].multiplyToRef(rotationMatrix, _localMatrix);

    // Post multiply inverse of pivotMatrix
    if (_postMultiplyPivotMatrix) {
      _localMatrix.multiplyToRef(*_pivotMatrixInverse, _localMatrix);
    }

    _localMatrix.addTranslationFromFloats(iTranslation.x, iTranslation.y,
                                          iTranslation.z);
  }
  else {
    Matrix::ComposeToRef(iScaling, iRotation, iTranslation, _localMatrix);
  }

  // Parent
  if (iParent) {
    if (useBillboardPath) {
      if (_transformToBoneReferal) {
        iParent->getWorldMatrix().multiplyToRef(
          _transformToBoneReferal->getWorldMatrix(), Tmp::MatrixArray[7]);
      }
      else {
        Tmp::MatrixArray[7].copyFrom(iParent->getWorldMatrix());
      }

      // Extract scaling and translation from parent
      auto& translation = Tmp::Vector3Array[5];
      auto& scale       = Tmp::Vector3Array[6];
      Tmp::MatrixArray[7].decompose(scale, std::nullopt, translation);
      Matrix::ScalingToRef(scale.x, scale.y, scale.z, Tmp::MatrixArray[7]);
      Tmp::MatrixArray[7].setTranslation(translation);

      _localMatrix.multiplyToRef(Tmp::MatrixArray[7], _worldMatrix);
    }
    else {
      if (_transformToBoneReferal) {
        _localMatrix.multiplyToRef(iParent->getWorldMatrix(),
                                   Tmp::MatrixArray[6]);
        Tmp::MatrixArray[6].multiplyToRef(
          _transformToBoneReferal->getWorldMatrix(), _worldMatrix);
      }
      else {
        _localMatrix.multiplyToRef(iParent->getWorldMatrix(), _worldMatrix);
      }
    }
    _markSyncedWithParent();
  }
  else {
    _worldMatrix.copyFrom(_localMatrix);
  }

  // Billboarding (testing PG:http://www.babylonjs-playground.com/#UJEIL#13)
  if (useBillboardPath && camera) {
    auto& storedTranslation = Tmp::Vector3Array[0];
    _worldMatrix.getTranslationToRef(storedTranslation); // Save translation

    // Cancel camera rotation
    Tmp::MatrixArray[1].copyFrom(camera->getViewMatrix());
    Tmp::MatrixArray[1].setTranslationFromFloats(0.f, 0.f, 0.f);
    Tmp::MatrixArray[1].invertToRef(Tmp::MatrixArray[0]);

    if ((billboardMode & TransformNode::BILLBOARDMODE_ALL)
        != TransformNode::BILLBOARDMODE_ALL) {
      Tmp::MatrixArray[0].decompose(std::nullopt, Tmp::QuaternionArray[0],
                                    std::nullopt);
      auto& eulerAngles = Tmp::Vector3Array[1];
      Tmp::QuaternionArray[0].toEulerAnglesToRef(eulerAngles);

      if ((billboardMode & TransformNode::BILLBOARDMODE_X)
          != TransformNode::BILLBOARDMODE_X) {
        eulerAngles.x = 0;
      }

      if ((billboardMode & TransformNode::BILLBOARDMODE_Y)
          != TransformNode::BILLBOARDMODE_Y) {
        eulerAngles.y = 0;
      }

      if ((billboardMode & TransformNode::BILLBOARDMODE_Z)
          != TransformNode::BILLBOARDMODE_Z) {
        eulerAngles.z = 0;
      }

      Matrix::RotationYawPitchRollToRef(eulerAngles.y, eulerAngles.x,
                                        eulerAngles.z, Tmp::MatrixArray[0]);
    }
    _worldMatrix.setTranslationFromFloats(0.f, 0.f, 0.f);
    _worldMatrix.multiplyToRef(Tmp::MatrixArray[0], _worldMatrix);

    // Restore translation
    _worldMatrix.setTranslation(Tmp::Vector3Array[0]);
  }

  // Normal matrix
  if (!ignoreNonUniformScaling) {
    if (_scaling.isNonUniform()) {
      _updateNonUniformScalingState(true);
    }
    else if (iParent) {
      auto tnParent = static_cast<TransformNode*>(iParent);
      if (tnParent && tnParent->_nonUniformScaling) {
        _updateNonUniformScalingState(tnParent->_nonUniformScaling);
      }
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
  _absolutePosition.copyFromFloats(_worldMatrix.m()[12], _worldMatrix.m()[13],
                                   _worldMatrix.m()[14]);

  // Callbacks
  onAfterWorldMatrixUpdateObservable.notifyObservers(this);

  if (!_poseMatrix) {
    _poseMatrix = std::make_unique<Matrix>(Matrix::Invert(_worldMatrix));
  }

  // Cache the determinant
  _worldMatrixDeterminantIsDirty = true;

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

Vector3 TransformNode::getPositionInCameraSpace(const CameraPtr& camera) const
{
  if (!camera) {
    return Vector3::TransformCoordinates(
      absolutePosition(), getScene()->activeCamera()->getViewMatrix());
  }
  else {
    return Vector3::TransformCoordinates(absolutePosition(),
                                         camera->getViewMatrix());
  }
}

float TransformNode::getDistanceToCamera(const CameraPtr& camera)
{
  if (!camera) {
    return absolutePosition()
      .subtract(getScene()->activeCamera()->globalPosition())
      .length();
  }
  else {
    return absolutePosition().subtract(camera->globalPosition()).length();
  }
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

std::vector<TransformNodePtr> TransformNode::getChildTransformNodes(
  bool directDescendantsOnly,
  const std::function<bool(const NodePtr& node)>& predicate)
{
  std::vector<TransformNodePtr> results;
  _getDescendants(results, directDescendantsOnly,
                  [&predicate](const NodePtr& node) {
                    return ((!predicate || predicate(node))
                            && (std::static_pointer_cast<TransformNode>(node)));
                  });
  return results;
}

void TransformNode::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  // Animations
  getScene()->stopAnimation(this);

  // Remove from scene
  getScene()->removeTransformNode(this);

  onAfterWorldMatrixUpdateObservable.clear();

  if (doNotRecurse) {
    const auto& transformNodes = getChildTransformNodes(true);
    for (const auto& transformNode : transformNodes) {
      transformNode->parent = nullptr;
      transformNode->computeWorldMatrix(true);
    }
  }

  Node::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
