#include <babylon/meshes/transform_node.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

std::unique_ptr<Vector3> TransformNode::_lookAtVectorCache
  = std::make_unique<Vector3>(0.f, 0.f, 0.f);
std::unique_ptr<Quaternion> TransformNode::_rotationAxisCache = std::make_unique<Quaternion>();

TransformNode::TransformNode(const std::string& iName, Scene* scene, bool isPure)
    : Node{iName, scene}
    , billboardMode{this, &TransformNode::get_billboardMode, &TransformNode::set_billboardMode}
    , preserveParentRotationForBillboard{this,
                                         &TransformNode::get_preserveParentRotationForBillboard,
                                         &TransformNode::set_preserveParentRotationForBillboard}
    , infiniteDistance{this, &TransformNode::get_infiniteDistance,
                       &TransformNode::set_infiniteDistance}
    , scalingDeterminant{1.f}
    , ignoreNonUniformScaling{false}
    , reIntegrateRotationIntoRotationQuaternion{false}
    , _poseMatrix{std::make_unique<Matrix>(Matrix::Identity())}
    , _localMatrix{Matrix::Zero()}
    , _postMultiplyPivotMatrix{false}
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
    , absoluteScaling{this, &TransformNode::get_absoluteScaling}
    , absoluteRotationQuaternion{this, &TransformNode::get_absoluteRotationQuaternion}
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
    , _isAbsoluteSynced{false}
    , _billboardMode{TransformNode::BILLBOARDMODE_NONE}
    , _preserveParentRotationForBillboard(false)
    , _infiniteDistance{false}
    , _localWorld{Matrix::Zero()}
    , _usePivotMatrix{false}
    , _absolutePosition{Vector3::Zero()}
    , _absoluteScaling{Vector3::Zero()}
    , _absoluteRotationQuaternion{Quaternion::Identity()}
    , _pivotMatrix{Matrix::Identity()}
    , _pivotMatrixInverse{nullptr}
    , _nonUniformScaling{false}
{
}

TransformNode::~TransformNode() = default;

void TransformNode::addToScene(const TransformNodePtr& transformNode)
{
  if (transformNode->_isPure) {
    transformNode->getScene()->addTransformNode(transformNode);
  }
}

TransformNodePtr TransformNode::_this() const
{
  const auto& transformNodes = getScene()->transformNodes;
  auto it                    = std::find_if(
    transformNodes.begin(), transformNodes.end(),
    [this](const TransformNodePtr& transformNode) { return transformNode.get() == this; });
  return (it != transformNodes.end()) ? (*it) : nullptr;
}

Type TransformNode::type() const
{
  return Type::TRANSFORMNODE;
}

AnimationValue TransformNode::getProperty(const std::vector<std::string>& targetPropertyPath)
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
      return IAnimatable::getProperty(key, rotation());
    }
    // Scaling
    if (target == "scaling") {
      return IAnimatable::getProperty(key, scaling());
    }
  }

  return AnimationValue();
}

void TransformNode::setProperty(const std::vector<std::string>& targetPropertyPath,
                                const AnimationValue& value)
{
  const auto animationType = value.animationType();
  if (animationType.has_value()) {
    if (targetPropertyPath.size() == 1) {
      const auto& target = targetPropertyPath[0];
      if (*animationType == Animation::ANIMATIONTYPE_QUATERNION) {
        auto quaternionValue = value.get<Quaternion>();
        if (target == "rotationQuaternion") {
          rotationQuaternion = quaternionValue;
        }
      }
      else if (*animationType == Animation::ANIMATIONTYPE_VECTOR3) {
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
      if (*animationType == Animation::ANIMATIONTYPE_FLOAT) {
        const auto& floatValue = value.get<float>();
        // Position
        if (target == "position") {
          IAnimatable::setProperty(key, position(), floatValue);
        }
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

std::string TransformNode::getClassName() const
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

void TransformNode::set_rotationQuaternion(const std::optional<Quaternion>& quaternion)
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
  _forwardNormalized = Vector3::Normalize(
    Vector3::TransformNormal(getScene()->useRightHandedSystem() ? _forwardInverted : _forward, //
                             getWorldMatrix()                                                  //
                             ));
  return _forwardNormalized;
}

Vector3& TransformNode::get_up()
{
  _upNormalized = Vector3::Normalize(Vector3::TransformNormal(_up,             //
                                                              getWorldMatrix() //
                                                              ));
  return _upNormalized;
}

Vector3& TransformNode::get_right()
{
  _rightNormalized = Vector3::Normalize(
    Vector3::TransformNormal(getScene()->useRightHandedSystem() ? _rightInverted : _right, //
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

  if (billboardMode != cache.billboardMode || billboardMode != TransformNode::BILLBOARDMODE_NONE) {
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

Vector3& TransformNode::get_absoluteScaling()
{
  _syncAbsoluteScalingAndRotation();
  return _absoluteScaling;
}

Quaternion& TransformNode::get_absoluteRotationQuaternion()
{
  _syncAbsoluteScalingAndRotation();
  return _absoluteRotationQuaternion;
}

TransformNode& TransformNode::setPreTransformMatrix(Matrix& matrix)
{
  return setPivotMatrix(matrix, false);
}

TransformNode& TransformNode::setPivotMatrix(Matrix matrix, bool postMultiplyPivotMatrix)
{
  _pivotMatrix.copyFrom(matrix);
  _usePivotMatrix = !_pivotMatrix.isIdentity();

  _cache.pivotMatrixUpdated = true;
  _postMultiplyPivotMatrix  = postMultiplyPivotMatrix;

  if (_postMultiplyPivotMatrix) {
    if (!_pivotMatrixInverse) {
      _pivotMatrixInverse = std::make_unique<Matrix>(Matrix::Invert(_pivotMatrix));
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

TransformNodePtr TransformNode::instantiateHierarchy(
  TransformNode* newParent, const std::optional<InstantiateHierarychyOptions>& options,
  const std::function<void(TransformNode* source, TransformNode* clone)>& onNewNodeCreated)
{
  auto cloneTransformNode
    = clone("Clone of " + (!name.empty() ? name : id), newParent ? newParent : parent(), true);

  if (cloneTransformNode) {
    if (onNewNodeCreated) {
      onNewNodeCreated(this, cloneTransformNode.get());
    }
  }

  for (const auto& child : getChildTransformNodes(true)) {
    child->instantiateHierarchy(cloneTransformNode.get(), options, onNewNodeCreated);
  }

  return cloneTransformNode;
}

TransformNode& TransformNode::freezeWorldMatrix(const std::optional<Matrix>& newWorldMatrix)
{
  if (newWorldMatrix) {
    _worldMatrix = *newWorldMatrix;
  }
  else {
    _isWorldMatrixFrozen = false; // no guarantee world is not already frozen,
                                  // switch off temporarily
    computeWorldMatrix(true);
  }
  _isDirty             = false;
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

TransformNode& TransformNode::setAbsolutePosition(const std::optional<Vector3>& iAbsolutePosition)
{
  if (!iAbsolutePosition) {
    return *this;
  }
  auto absolutePositionX = (*iAbsolutePosition).x;
  auto absolutePositionY = (*iAbsolutePosition).y;
  auto absolutePositionZ = (*iAbsolutePosition).z;

  if (parent()) {
    auto& invertParentWorldMatrix = TmpVectors::MatrixArray[0];
    parent()->getWorldMatrix().invertToRef(invertParentWorldMatrix);
    Vector3::TransformCoordinatesFromFloatsToRef(
      absolutePositionX, absolutePositionY, absolutePositionZ, invertParentWorldMatrix, position);
  }
  else {
    position().x = absolutePositionX;
    position().y = absolutePositionY;
    position().z = absolutePositionZ;
  }

  _absolutePosition.copyFrom(*iAbsolutePosition);
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
  auto& invLocalWorldMatrix = TmpVectors::MatrixArray[0];
  _localMatrix.invertToRef(invLocalWorldMatrix);
  return Vector3::TransformNormal(position, invLocalWorldMatrix);
}

TransformNode& TransformNode::locallyTranslate(const Vector3& vector3)
{
  computeWorldMatrix(true);
  position = Vector3::TransformCoordinates(vector3, _localMatrix);
  return *this;
}

TransformNode& TransformNode::lookAt(const Vector3& targetPoint, float yawCor, float pitchCor,
                                     float rollCor, Space space)
{
  auto& dv = *TransformNode::_lookAtVectorCache;
  auto pos = (space == Space::LOCAL) ? position : getAbsolutePosition();
  targetPoint.subtractToRef(pos, dv);
  setDirection(dv, yawCor, pitchCor, rollCor);

  // Correct for parent's rotation offset
  if (space == Space::WORLD && parent()) {
    if (rotationQuaternion().has_value()) {
      // Get local rotation matrix of the looking object
      auto& rotationMatrix = TmpVectors::MatrixArray[0];
      rotationQuaternion()->toRotationMatrix(rotationMatrix);

      // Offset rotation by parent's inverted rotation matrix to correct in
      // world space
      auto& parentRotationMatrix = TmpVectors::MatrixArray[1];
      parent()->getWorldMatrix().getRotationMatrixToRef(parentRotationMatrix);
      parentRotationMatrix.invert();
      rotationMatrix.multiplyToRef(parentRotationMatrix, rotationMatrix);
      rotationQuaternion()->fromRotationMatrix(rotationMatrix);
    }
    else {
      // Get local rotation matrix of the looking object
      auto& quaternionRotation = TmpVectors::QuaternionArray[0];
      Quaternion::FromEulerVectorToRef(rotation, quaternionRotation);
      auto& rotationMatrix = TmpVectors::MatrixArray[0];
      quaternionRotation.toRotationMatrix(rotationMatrix);

      // Offset rotation by parent's inverted rotation matrix to correct in
      // world space
      auto& parentRotationMatrix = TmpVectors::MatrixArray[1];
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

TransformNode& TransformNode::getDirectionToRef(const Vector3& localAxis, Vector3& result)
{
  Vector3::TransformNormalToRef(localAxis, getWorldMatrix(), result);
  return *this;
}

TransformNode& TransformNode::setDirection(const Vector3& localAxis, float yawCor, float pitchCor,
                                           float rollCor)
{
  const auto yaw   = -std::atan2(localAxis.z, localAxis.x) + Math::PI_2;
  const auto len   = std::sqrt(localAxis.x * localAxis.x + localAxis.z * localAxis.z);
  const auto pitch = -std::atan2(localAxis.y, len);
  if (rotationQuaternion().has_value()) {
    Quaternion::RotationYawPitchRollToRef(yaw + yawCor, pitch + pitchCor, rollCor,
                                          *rotationQuaternion());
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
    auto& tmat = TmpVectors::MatrixArray[0];
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
  getPivotPointToRef(result);
  Vector3::TransformCoordinatesToRef(result, getWorldMatrix(), result);
  return *this;
}

TransformNode& TransformNode::setParent(Node* node)
{
  if (!node && !parent()) {
    return *this;
  }

  std::optional<Quaternion> quatRotation = TmpVectors::QuaternionArray[0];
  std::optional<Vector3> newPosition     = TmpVectors::Vector3Array[0];
  std::optional<Vector3> scale           = TmpVectors::Vector3Array[1];

  if (!node) {
    computeWorldMatrix(true);
    getWorldMatrix().decompose(scale, quatRotation, newPosition);
  }
  else {
    auto& diffMatrix      = TmpVectors::MatrixArray[0];
    auto& invParentMatrix = TmpVectors::MatrixArray[1];

    computeWorldMatrix(true);
    node->computeWorldMatrix(true);

    node->getWorldMatrix().invertToRef(invParentMatrix);
    getWorldMatrix().multiplyToRef(invParentMatrix, diffMatrix);
    diffMatrix.decompose(scale, quatRotation, newPosition);
  }

  if (rotationQuaternion()) {
    rotationQuaternion()->copyFrom(*quatRotation);
  }
  else {
    quatRotation->toEulerAnglesToRef(rotation());
  }

  scaling().copyFrom(*scale);
  position().copyFrom(*newPosition);

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

TransformNode& TransformNode::attachToBone(Bone* bone, TransformNode* affectedTransformNode)
{
  _transformToBoneReferal = affectedTransformNode;
  Node::set_parent(bone);

  bone->getSkeleton()->prepare();

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
    iRotationQuaternion
      = Quaternion::RotationAxisToRef(axis, amount, *TransformNode::_rotationAxisCache);
    this->rotationQuaternion()->multiplyToRef(iRotationQuaternion, *rotationQuaternion());
  }
  else {
    if (parent()) {
      auto& invertParentWorldMatrix = TmpVectors::MatrixArray[0];
      parent()->getWorldMatrix().invertToRef(invertParentWorldMatrix);
      axis = Vector3::TransformNormal(axis, invertParentWorldMatrix);
    }
    iRotationQuaternion
      = Quaternion::RotationAxisToRef(axis, amount, *TransformNode::_rotationAxisCache);
    iRotationQuaternion.multiplyToRef(*rotationQuaternion(), *rotationQuaternion());
  }
  return *this;
}

TransformNode& TransformNode::rotateAround(const Vector3& point, Vector3& axis, float amount)
{
  axis.normalize();
  if (!rotationQuaternion().has_value()) {
    rotationQuaternion = Quaternion::RotationYawPitchRoll(rotation().y, rotation().x, rotation().z);
    rotation().setAll(0.f);
  }

  auto& tmpVector                         = TmpVectors::Vector3Array[0];
  std::optional<Vector3> finalScale       = TmpVectors::Vector3Array[1];
  std::optional<Vector3> finalTranslation = TmpVectors::Vector3Array[2];

  std::optional<Quaternion> finalRotation = TmpVectors::QuaternionArray[0];

  auto& translationMatrix    = TmpVectors::MatrixArray[0]; // T
  auto& translationMatrixInv = TmpVectors::MatrixArray[1]; // T'
  auto& rotationMatrix       = TmpVectors::MatrixArray[2]; // R
  auto& finalMatrix          = TmpVectors::MatrixArray[3]; // T' x R x T

  point.subtractToRef(position, tmpVector);
  Matrix::TranslationToRef(tmpVector.x, tmpVector.y, tmpVector.z,
                           translationMatrix); // T
  Matrix::TranslationToRef(-tmpVector.x, -tmpVector.y, -tmpVector.z,
                           translationMatrixInv);          // T'
  Matrix::RotationAxisToRef(axis, amount, rotationMatrix); // R

  translationMatrixInv.multiplyToRef(rotationMatrix, finalMatrix); // T' x R
  finalMatrix.multiplyToRef(translationMatrix, finalMatrix);       // T' x R x T

  finalMatrix.decompose(finalScale, finalRotation, finalTranslation);

  position().addInPlace(*finalTranslation);
  finalRotation->multiplyToRef(*rotationQuaternion(), *rotationQuaternion());

  return *this;
}

TransformNode& TransformNode::translate(const Vector3& axis, float distance, Space space)
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
    rotationQuaternionTmp = TmpVectors::QuaternionArray[1];
    Quaternion::RotationYawPitchRollToRef(rotation().y, rotation().x, rotation().z,
                                          rotationQuaternionTmp);
  }
  auto& accumulation = TmpVectors::QuaternionArray[0];
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

Matrix& TransformNode::computeWorldMatrix(bool force, bool /*useWasUpdatedFlag*/)
{
  if (_isWorldMatrixFrozen && !_isDirty) {
    return _worldMatrix;
  }

  const auto currentRenderId = getScene()->getRenderId();
  if (!_isDirty && !force && isSynchronized()) {
    _currentRenderId = currentRenderId;
    return _worldMatrix;
  }

  auto& camera = getScene()->activeCamera();
  const auto useBillboardPosition
    = (_billboardMode & TransformNode::BILLBOARDMODE_USE_POSITION) != 0;
  const auto useBillboardPath
    = _billboardMode != TransformNode::BILLBOARDMODE_NONE && !preserveParentRotationForBillboard;

  // Billboarding based on camera position
  if (useBillboardPath && camera && useBillboardPosition) {
    lookAt(camera->position());

    if ((billboardMode & TransformNode::BILLBOARDMODE_X) != TransformNode::BILLBOARDMODE_X) {
      rotation().x = 0.f;
    }

    if ((billboardMode & TransformNode::BILLBOARDMODE_Y) != TransformNode::BILLBOARDMODE_Y) {
      rotation().y = 0.f;
    }

    if ((billboardMode & TransformNode::BILLBOARDMODE_Z) != TransformNode::BILLBOARDMODE_Z) {
      rotation().z = 0.f;
    }
  }

  updateCache();
  auto& cache              = _cache;
  cache.pivotMatrixUpdated = false;
  cache.billboardMode      = billboardMode();
  cache.infiniteDistance   = infiniteDistance();

  _currentRenderId = currentRenderId;
  _childUpdateId++;
  _isDirty     = false;
  auto iParent = _getEffectiveParent();

  // Scaling
  auto& iScaling     = cache.scaling;
  auto& iTranslation = cache.position;

  // Translation
  if (_infiniteDistance) {
    if (!parent() && camera) {
      auto cameraWorldMatrix = camera->getWorldMatrix();
      auto cameraGlobalPosition
        = Vector3(cameraWorldMatrix.m()[12], cameraWorldMatrix.m()[13], cameraWorldMatrix.m()[14]);

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
  iScaling.copyFromFloats(_scaling.x * scalingDeterminant, _scaling.y * scalingDeterminant,
                          _scaling.z * scalingDeterminant);

  // Rotation
  auto& iRotation = cache.rotationQuaternion;
  if (_rotationQuaternion.has_value()) {
    if (reIntegrateRotationIntoRotationQuaternion) {
      const auto len = rotation().lengthSquared();
      if (len != 0.f) {
        _rotationQuaternion->multiplyInPlace(
          Quaternion::RotationYawPitchRoll(_rotation.y, _rotation.x, _rotation.z));
        _rotation.copyFromFloats(0.f, 0.f, 0.f);
      }
    }
    iRotation.copyFrom(*_rotationQuaternion);
  }
  else {
    Quaternion::RotationYawPitchRollToRef(_rotation.y, _rotation.x, _rotation.z, iRotation);
    cache.rotation.copyFrom(_rotation);
  }

  // Compose
  if (_usePivotMatrix) {
    auto& scaleMatrix = TmpVectors::MatrixArray[1];
    Matrix::ScalingToRef(iScaling.x, iScaling.y, iScaling.z, scaleMatrix);

    // Rotation
    auto& rotationMatrix = TmpVectors::MatrixArray[0];
    iRotation.toRotationMatrix(rotationMatrix);

    // Composing transformations
    _pivotMatrix.multiplyToRef(scaleMatrix, TmpVectors::MatrixArray[4]);
    TmpVectors::MatrixArray[4].multiplyToRef(rotationMatrix, _localMatrix);

    // Post multiply inverse of pivotMatrix
    if (_postMultiplyPivotMatrix) {
      _localMatrix.multiplyToRef(*_pivotMatrixInverse, _localMatrix);
    }

    _localMatrix.addTranslationFromFloats(iTranslation.x, iTranslation.y, iTranslation.z);
  }
  else {
    Matrix::ComposeToRef(iScaling, iRotation, iTranslation, _localMatrix);
  }

  // Parent
  if (iParent) {
    if (force) {
      iParent->computeWorldMatrix(force);
    }
    if (useBillboardPath) {
      if (_transformToBoneReferal) {
        iParent->getWorldMatrix().multiplyToRef(_transformToBoneReferal->getWorldMatrix(),
                                                TmpVectors::MatrixArray[7]);
      }
      else {
        TmpVectors::MatrixArray[7].copyFrom(iParent->getWorldMatrix());
      }

      // Extract scaling and translation from parent
      std::optional<Vector3> scale        = TmpVectors::Vector3Array[6];
      std::optional<Quaternion> rotation_ = std::nullopt;
      std::optional<Vector3> translation  = TmpVectors::Vector3Array[5];
      TmpVectors::MatrixArray[7].decompose(scale, rotation_, translation);
      Matrix::ScalingToRef(scale->x, scale->y, scale->z, TmpVectors::MatrixArray[7]);
      TmpVectors::MatrixArray[7].setTranslation(*translation);

      _localMatrix.multiplyToRef(TmpVectors::MatrixArray[7], _worldMatrix);
    }
    else {
      if (_transformToBoneReferal) {
        _localMatrix.multiplyToRef(iParent->getWorldMatrix(), TmpVectors::MatrixArray[6]);
        TmpVectors::MatrixArray[6].multiplyToRef(_transformToBoneReferal->getWorldMatrix(),
                                                 _worldMatrix);
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

  // Billboarding based on camera orientation (testing
  // PG:http://www.babylonjs-playground.com/#UJEIL#13)
  if (useBillboardPath && camera && billboardMode && !useBillboardPosition) {
    auto& storedTranslation = TmpVectors::Vector3Array[0];
    _worldMatrix.getTranslationToRef(storedTranslation); // Save translation

    // Cancel camera rotation
    TmpVectors::MatrixArray[1].copyFrom(camera->getViewMatrix());
    TmpVectors::MatrixArray[1].setTranslationFromFloats(0.f, 0.f, 0.f);
    TmpVectors::MatrixArray[1].invertToRef(TmpVectors::MatrixArray[0]);

    if ((billboardMode & TransformNode::BILLBOARDMODE_ALL) != TransformNode::BILLBOARDMODE_ALL) {
      std::optional<Vector3> scale        = std::nullopt;
      std::optional<Quaternion> rotation_ = TmpVectors::QuaternionArray[0];
      std::optional<Vector3> translation  = std::nullopt;
      TmpVectors::MatrixArray[0].decompose(scale, rotation_, translation);
      TmpVectors::QuaternionArray[0] = *rotation_;
      auto& eulerAngles              = TmpVectors::Vector3Array[1];
      TmpVectors::QuaternionArray[0].toEulerAnglesToRef(eulerAngles);

      if ((billboardMode & TransformNode::BILLBOARDMODE_X) != TransformNode::BILLBOARDMODE_X) {
        eulerAngles.x = 0.f;
      }

      if ((billboardMode & TransformNode::BILLBOARDMODE_Y) != TransformNode::BILLBOARDMODE_Y) {
        eulerAngles.y = 0.f;
      }

      if ((billboardMode & TransformNode::BILLBOARDMODE_Z) != TransformNode::BILLBOARDMODE_Z) {
        eulerAngles.z = 0.f;
      }

      Matrix::RotationYawPitchRollToRef(eulerAngles.y, eulerAngles.x, eulerAngles.z,
                                        TmpVectors::MatrixArray[0]);
    }
    _worldMatrix.setTranslationFromFloats(0.f, 0.f, 0.f);
    _worldMatrix.multiplyToRef(TmpVectors::MatrixArray[0], _worldMatrix);

    // Restore translation
    _worldMatrix.setTranslation(TmpVectors::Vector3Array[0]);
  }

  // Normal matrix
  if (!ignoreNonUniformScaling) {
    if (_scaling.isNonUniformWithinEpsilon(0.000001f)) {
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
  _isAbsoluteSynced = false;

  // Callbacks
  onAfterWorldMatrixUpdateObservable.notifyObservers(this);

  if (!_poseMatrix) {
    _poseMatrix = std::make_unique<Matrix>(Matrix::Invert(_worldMatrix));
  }

  // Cache the determinant
  _worldMatrixDeterminantIsDirty = true;

  return _worldMatrix;
}

void TransformNode::resetLocalMatrix(bool independentOfChildren)
{
  computeWorldMatrix();
  if (independentOfChildren) {
    const auto children = getChildren();
    for (size_t i = 0; i < children.size(); ++i) {
      const auto child = std::static_pointer_cast<TransformNode>(children[i]);
      if (child) {
        child->computeWorldMatrix();
        auto& bakedMatrix = TmpVectors::MatrixArray[0];
        child->_localMatrix.multiplyToRef(_localMatrix, bakedMatrix);
        auto& tmpRotationQuaternion                   = TmpVectors::QuaternionArray[0];
        std::optional<Vector3> iScaling               = child->scaling();
        std::optional<Quaternion> iRotationQuaternion = tmpRotationQuaternion;
        std::optional<Vector3> iPosition              = child->position();
        bakedMatrix.decompose(iScaling, iRotationQuaternion, iPosition);
        // Copy back
        child->scaling        = *iScaling;
        tmpRotationQuaternion = *iRotationQuaternion;
        child->position       = *iPosition;
        if (child->rotationQuaternion()) {
          child->rotationQuaternion = tmpRotationQuaternion;
        }
        else {
          tmpRotationQuaternion.toEulerAnglesToRef(child->rotation());
        }
      }
    }
  }
  scaling().copyFromFloats(1.f, 1.f, 1.f);
  position().copyFromFloats(0.f, 0.f, 0.f);
  rotation().copyFromFloats(0.f, 0.f, 0.f);

  // only if quaternion is already set
  if (rotationQuaternion()) {
    rotationQuaternion = Quaternion::Identity();
  }
  _worldMatrix = Matrix::Identity();
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

Vector3 TransformNode::getPositionInCameraSpace(const CameraPtr& camera)
{
  if (!camera) {
    return Vector3::TransformCoordinates(getAbsolutePosition(),
                                         getScene()->activeCamera()->getViewMatrix());
  }
  else {
    return Vector3::TransformCoordinates(getAbsolutePosition(), camera->getViewMatrix());
  }
}

float TransformNode::getDistanceToCamera(const CameraPtr& camera)
{
  if (!camera) {
    return getAbsolutePosition().subtract(getScene()->activeCamera()->globalPosition()).length();
  }
  else {
    return getAbsolutePosition().subtract(camera->globalPosition()).length();
  }
}

TransformNodePtr TransformNode::clone(const std::string& /*name*/, Node* /*newParent*/,
                                      bool /*doNotCloneChildren*/)
{
  return nullptr;
}

json TransformNode::serialize(json& /*currentSerializationObject*/)
{
  return nullptr;
}

TransformNodePtr TransformNode::Parse(const json& /*parsedTransformNode*/, Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
  return nullptr;
}

std::vector<TransformNodePtr>
TransformNode::getChildTransformNodes(bool directDescendantsOnly,
                                      const std::function<bool(const NodePtr& node)>& predicate)
{
  std::vector<TransformNodePtr> results;
  _getDescendants(results, directDescendantsOnly, [&predicate](const NodePtr& node) {
    return ((!predicate || predicate(node)) && (std::static_pointer_cast<TransformNode>(node)));
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

TransformNode& TransformNode::normalizeToUnitCube(
  bool includeDescendants, bool ignoreRotation,
  const std::function<bool(const AbstractMeshPtr& node)>& predicate)
{
  std::optional<Vector3> storedRotation              = std::nullopt;
  std::optional<Quaternion> storedRotationQuaternion = std::nullopt;

  if (ignoreRotation) {
    if (rotationQuaternion()) {
      storedRotationQuaternion = rotationQuaternion();
      rotationQuaternion()->copyFromFloats(0.f, 0.f, 0.f, 1.f);
    }
    else /* if (rotation()) */ {
      storedRotation = rotation;
      rotation().copyFromFloats(0.f, 0.f, 0.f);
    }
  }

  const auto boundingVectors = getHierarchyBoundingVectors(includeDescendants, predicate);
  const auto sizeVec         = boundingVectors.max.subtract(boundingVectors.min);
  const auto maxDimension    = std::max(sizeVec.x, std::max(sizeVec.y, sizeVec.z));

  if (maxDimension == 0.f) {
    return *this;
  }

  const auto scale = 1.f / maxDimension;

  scaling().scaleInPlace(scale);

  if (ignoreRotation) {
    if (rotationQuaternion() && storedRotationQuaternion) {
      rotationQuaternion()->copyFrom(*storedRotationQuaternion);
    }
    else if (/*rotation() && */ storedRotation) {
      rotation().copyFrom(*storedRotation);
    }
  }

  return *this;
}

void TransformNode::_syncAbsoluteScalingAndRotation()
{
  if (!_isAbsoluteSynced) {
    std::optional<Vector3> scale        = _absoluteScaling;
    std::optional<Quaternion> rotation_ = _absoluteRotationQuaternion;
    std::optional<Vector3> translation  = std::nullopt;
    _worldMatrix.decompose(scale, rotation_, translation);
    _absoluteScaling            = *scale;
    _absoluteRotationQuaternion = *rotation_;
    _isAbsoluteSynced           = true;
  }
}

} // end of namespace BABYLON
