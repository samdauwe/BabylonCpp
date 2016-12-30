#include <babylon/mesh/abstract_mesh.h>

#include <babylon/actions/action_manager.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/frustum.h>
#include <babylon/math/math_tools.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/particles/particle_system.h>
#include <babylon/physics/joint/physics_joint.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/rendering/edges_renderer.h>

namespace BABYLON {

Quaternion AbstractMesh::_rotationAxisCache;
Vector3 AbstractMesh::_lookAtVectorCache = Vector3(0.f, 0.f, 0.f);

AbstractMesh::AbstractMesh(const std::string& _name, Scene* scene)
    : Node(_name, scene)
    , definedFacingForward{true} // orientation for POV movement & rotation
    , position{Vector3(0.f, 0.f, 0.f)}
    , _rotationQuaternionSet{false}
    , billboardMode{AbstractMesh::BILLBOARDMODE_NONE}
    , visibility{1.f}
    , alphaIndex{std::numeric_limits<int>::max()}
    , infiniteDistance{false}
    , isVisible{true}
    , isPickable{true}
    , showBoundingBox{false}
    , showSubMeshesBoundingBox{false}
    , isBlocker{false}
    , renderingGroupId{0}
    , material{nullptr}
    , receiveShadows{false}
    , renderOutline{false}
    , outlineColor{Color3::Red()}
    , outlineWidth{0.02f}
    , renderOverlay{false}
    , overlayColor{Color3::Red()}
    , overlayAlpha{0.5f}
    , hasVertexAlpha{false}
    , useVertexColors{true}
    , applyFog{true}
    , computeBonesUsingShaders{true}
    , scalingDeterminant{1.f}
    , numBoneInfluencers{4}
    , useOctreeForRenderingSelection{true}
    , useOctreeForPicking{true}
    , useOctreeForCollisions{true}
    , layerMask{0x0FFFFFFF}
    , alwaysSelectAsActiveMesh{false}
    , actionManager{nullptr}
    , physicsImpostor{nullptr}
    , ellipsoid{Vector3(0.5f, 1.f, 0.5f)}
    , ellipsoidOffset{Vector3(0, 0, 0)}
    , edgesWidth{1.f}
    , edgesColor{Color4(1.f, 0.f, 0.f, 1.f)}
    , _edgesRenderer{nullptr}
    , _worldMatrix{std_util::make_unique<Matrix>(Matrix::Zero())}
    , _masterMesh{nullptr}
    , _materialDefines{nullptr}
    , _boundingInfo{nullptr}
    , _isDisposed{false}
    , _renderId{0}
    , _submeshesOctree{nullptr}
    , _unIndexed{false}
    , _onCollideObserver{nullptr}
    , _onCollisionPositionChangeObserver{nullptr}
    , _rotation{Vector3(0.f, 0.f, 0.f)}
    , _scaling{Vector3(1.f, 1.f, 1.f)}
    , _checkCollisions{false}
    , _oldPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _diffPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _newPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _meshToBoneReferal{nullptr}
    , _localWorld{Matrix::Zero()}
    , _rotateYByPI{Matrix::RotationY(Math::PI)}
    , _absolutePosition{std_util::make_unique<Vector3>(Vector3::Zero())}
    , _collisionsTransformMatrix{Matrix::Zero()}
    , _collisionsScalingMatrix{Matrix::Zero()}
    , _isDirty{false}
    , _pivotMatrix{Matrix::Identity()}
    , _isWorldMatrixFrozen{false}
    , _skeleton{nullptr}
{
}

AbstractMesh::~AbstractMesh()
{
}

Node::Type AbstractMesh::type() const
{
  return Node::Type::ABSTRACTMESH;
}

void AbstractMesh::addToScene(std::unique_ptr<AbstractMesh>&& newMesh)
{
  getScene()->addMesh(std::move(newMesh));
}

void AbstractMesh::setOnCollide(const std::function<void()>& callback)
{
  if (_onCollideObserver) {
    onCollideObservable.remove(_onCollideObserver);
  }
  _onCollideObserver = onCollideObservable.add(callback);
}

void AbstractMesh::setOnCollisionPositionChange(
  const std::function<void()>& callback)
{
  if (_onCollisionPositionChangeObserver) {
    onCollisionPositionChangeObservable.remove(
      _onCollisionPositionChangeObserver);
  }
  _onCollisionPositionChangeObserver
    = onCollisionPositionChangeObservable.add(callback);
}

std::string AbstractMesh::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name << ", isInstance: ";
  oss << ((type() == Node::Type::INSTANCEDMESH) ? "YES" : "NO");
  oss << ", # of submeshes: " << (subMeshes.size());
  if (_skeleton) {
    oss << ", skeleton: " << _skeleton->name;
  }
  if (fullDetails) {
    oss << ", billboard mode: ";
    std::vector<std::string> billboardModes{"NONE", "X",    "Y",    "null",
                                            "Z",    "null", "null", "ALL"};
    if (billboardMode < billboardModes.size()) {
      oss << billboardModes[billboardMode];
    }
    else {
      oss << "null";
    }
    oss << ", freeze wrld mat: "
        << (_isWorldMatrixFrozen || _waitingFreezeWorldMatrix ? "YES" : "NO");
  }
  return oss.str();
}

Scene* AbstractMesh::getScene()
{
  return Node::getScene();
}

void AbstractMesh::setSkeleton(Skeleton* value)
{
  if (_skeleton && _skeleton->needInitialSkinMatrix) {
    _skeleton->_unregisterMeshWithPoseMatrix(this);
  }

  if (value && value->needInitialSkinMatrix) {
    value->_registerMeshWithPoseMatrix(this);
  }

  _skeleton = value;

  if (!_skeleton) {
    _bonesTransformMatrices.clear();
  }
}

Skeleton* AbstractMesh::skeleton()
{
  return _skeleton;
}

Vector3& AbstractMesh::rotation()
{
  return _rotation;
}

void AbstractMesh::setRotation(const Vector3& newRotation)
{
  _rotation = newRotation;
}

Vector3& AbstractMesh::scaling()
{
  return _scaling;
}

void AbstractMesh::setScaling(const Vector3& newScaling)
{
  _scaling = newScaling;
  if (physicsImpostor) {
    physicsImpostor->forceUpdate();
  }
}

Quaternion& AbstractMesh::rotationQuaternion()
{
  return _rotationQuaternion;
}

bool AbstractMesh::rotationQuaternionSet() const
{
  return _rotationQuaternionSet;
}

void AbstractMesh::setRotationQuaternion(const Quaternion& quaternion)
{
  _rotationQuaternion    = quaternion;
  _rotationQuaternionSet = true;
  // reset the rotation vector.
  if (rotation().length() > 0) {
    rotation().copyFromFloats(0.f, 0.f, 0.f);
  }
}

void AbstractMesh::resetRotationQuaternion()
{
  _rotationQuaternionSet = false;
}

AbstractMesh* AbstractMesh::getParent()
{
  if (parent()->type() == Node::Type::ABSTRACTMESH) {
    return dynamic_cast<AbstractMesh*>(parent());
  }

  return nullptr;
}

Material* AbstractMesh::getMaterial()
{
  return material;
}

std::vector<AbstractMesh*>
AbstractMesh::getChildMeshes(bool directDecendantsOnly,
                             const std::function<bool(Node* node)>& predicate)
{
  return Node::getChildMeshes(directDecendantsOnly, predicate);
}

void AbstractMesh::updatePoseMatrix(const Matrix& matrix)
{
  _poseMatrix->copyFrom(matrix);
}

Matrix& AbstractMesh::getPoseMatrix()
{
  return *_poseMatrix;
}

void AbstractMesh::disableEdgesRendering()
{
  if (_edgesRenderer != nullptr) {
    _edgesRenderer->dispose();
    _edgesRenderer.reset(nullptr);
  }
}

void AbstractMesh::enableEdgesRendering(float epsilon,
                                        bool checkVerticesInsteadOfIndices)
{
  disableEdgesRendering();

  _edgesRenderer = std_util::make_unique<EdgesRenderer>(
    this, epsilon, checkVerticesInsteadOfIndices);
}

bool AbstractMesh::isBlocked() const
{
  return false;
}

AbstractMesh* AbstractMesh::getLOD(Camera* /*camera*/,
                                   BoundingSphere* /*boundingSphere*/)
{
  return this;
}

size_t AbstractMesh::getTotalVertices() const
{
  return 0;
}

Uint32Array AbstractMesh::getIndices(bool /*copyWhenShared*/)
{
  return Uint32Array();
}

Float32Array AbstractMesh::getVerticesData(unsigned int /*kind*/,
                                           bool /*copyWhenShared*/)
{
  return Float32Array();
}

bool AbstractMesh::isVerticesDataPresent(unsigned int /*kind*/)
{
  return false;
}

BoundingInfo* AbstractMesh::getBoundingInfo()
{
  if (_masterMesh) {
    return _masterMesh->getBoundingInfo();
  }

  if (!_boundingInfo) {
    _updateBoundingInfo();
  }
  return _boundingInfo.get();
}

void AbstractMesh::setBoundingInfo(const BoundingInfo& boundingInfo)
{
  _boundingInfo = std_util::make_unique<BoundingInfo>(boundingInfo);
}

bool AbstractMesh::useBones()
{
  return _skeleton && getScene()->skeletonsEnabled
         && isVerticesDataPresent(VertexBuffer::MatricesIndicesKind)
         && isVerticesDataPresent(VertexBuffer::MatricesWeightsKind);
}

void AbstractMesh::_preActivate()
{
}

void AbstractMesh::_preActivateForIntermediateRendering(int /*renderId*/)
{
}

void AbstractMesh::_activate(int renderId)
{
  _renderId = renderId;
}

Matrix* AbstractMesh::getWorldMatrix()
{
  if (_masterMesh) {
    return _masterMesh->getWorldMatrix();
  }

  if (_currentRenderId != getScene()->getRenderId()) {
    computeWorldMatrix();
  }
  return _worldMatrix.get();
}

Matrix& AbstractMesh::worldMatrixFromCache()
{
  return *_worldMatrix.get();
}

Vector3* AbstractMesh::absolutePosition()
{
  return _absolutePosition.get();
}

void AbstractMesh::freezeWorldMatrix()
{
  // no guarantee world is not already frozen, switch off temporarily
  _isWorldMatrixFrozen = false;
  computeWorldMatrix(true);
  _isWorldMatrixFrozen = true;
}

void AbstractMesh::unfreezeWorldMatrix()
{
  _isWorldMatrixFrozen = false;
  computeWorldMatrix(true);
}

bool AbstractMesh::isWorldMatrixFrozen()
{
  return _isWorldMatrixFrozen;
}

void AbstractMesh::rotate(Vector3& axis, float amount, const Space& space)
{
  axis.normalize();

  if (!_rotationQuaternionSet) {
    setRotationQuaternion(
      Quaternion::RotationYawPitchRoll(_rotation.y, _rotation.x, _rotation.z));
    _rotation = Vector3::Zero();
  }
  Quaternion rotationQuaternionTmp;
  if (space == Space::LOCAL) {
    rotationQuaternionTmp = Quaternion::RotationAxisToRef(
      axis, amount, AbstractMesh::_rotationAxisCache);
    _rotationQuaternion.multiplyToRef(rotationQuaternionTmp,
                                      rotationQuaternion());
  }
  else {
    if (parent()) {
      auto invertParentWorldMatrix = parent()->getWorldMatrix();
      invertParentWorldMatrix->invert();

      axis = Vector3::TransformNormal(axis, *invertParentWorldMatrix);
    }
    rotationQuaternionTmp = Quaternion::RotationAxisToRef(
      axis, amount, AbstractMesh::_rotationAxisCache);
    rotationQuaternionTmp.multiplyToRef(rotationQuaternion(),
                                        rotationQuaternion());
  }
}

void AbstractMesh::translate(Vector3& axis, float distance, const Space& space)
{
  auto displacementVector = axis.scale(distance);

  if (space == Space::LOCAL) {
    auto tempV3 = getPositionExpressedInLocalSpace().add(displacementVector);
    setPositionWithLocalVector(tempV3);
  }
  else {
    setAbsolutePosition(getAbsolutePosition()->add(displacementVector));
  }
}

Vector3* AbstractMesh::getAbsolutePosition()
{
  computeWorldMatrix();
  return _absolutePosition.get();
}

void AbstractMesh::setAbsolutePosition(const Vector3& absolutePosition_)
{
  float absolutePositionX = absolutePosition_.x;
  float absolutePositionY = absolutePosition_.y;
  float absolutePositionZ = absolutePosition_.z;

  if (parent()) {
    auto invertParentWorldMatrix = parent()->getWorldMatrix();
    invertParentWorldMatrix->invert();

    Vector3 worldPosition(absolutePositionX, absolutePositionY,
                          absolutePositionZ);

    position
      = Vector3::TransformCoordinates(worldPosition, *invertParentWorldMatrix);
  }
  else {
    position.x = absolutePositionX;
    position.y = absolutePositionY;
    position.z = absolutePositionZ;
  }
}

void AbstractMesh::movePOV(float amountRight, float amountUp,
                           float amountForward)
{
  position.addInPlace(calcMovePOV(amountRight, amountUp, amountForward));
}

Vector3 AbstractMesh::calcMovePOV(float amountRight, float amountUp,
                                  float amountForward)
{
  Matrix rotMatrix;
  auto rotQuaternion
    = _rotationQuaternionSet ?
        rotationQuaternion() :
        Quaternion::RotationYawPitchRoll(_rotation.y, _rotation.x, _rotation.z);
  rotQuaternion.toRotationMatrix(rotMatrix);

  auto translationDelta = Vector3::Zero();
  auto defForwardMult   = definedFacingForward ? 1.f : -1.f;
  Vector3::TransformCoordinatesFromFloatsToRef(
    amountRight * defForwardMult, amountUp, amountForward * defForwardMult,
    rotMatrix, translationDelta);
  return translationDelta;
}

void AbstractMesh::rotatePOV(float flipBack, float twirlClockwise,
                             float tiltRight)
{
  rotation().addInPlace(calcRotatePOV(flipBack, twirlClockwise, tiltRight));
}

Vector3 AbstractMesh::calcRotatePOV(float flipBack, float twirlClockwise,
                                    float tiltRight)
{
  auto defForwardMult = definedFacingForward ? 1.f : -1.f;
  return Vector3(flipBack * defForwardMult, twirlClockwise,
                 tiltRight * defForwardMult);
}

void AbstractMesh::setPivotMatrix(const Matrix& matrix)
{
  _pivotMatrix              = matrix;
  _cache.pivotMatrixUpdated = true;
}

Matrix& AbstractMesh::getPivotMatrix()
{
  return _pivotMatrix;
}

bool AbstractMesh::_isSynchronized()
{
  if (_isDirty) {
    return false;
  }

  if (billboardMode != _cache.billboardMode
      || billboardMode != AbstractMesh::BILLBOARDMODE_NONE) {
    return false;
  }

  if (_cache.pivotMatrixUpdated) {
    return false;
  }

  if (infiniteDistance) {
    return false;
  }

  if (!_cache.position.equals(position)) {
    return false;
  }

  if (_rotationQuaternionSet) {
    if (!_cache.rotationQuaternion.equals(_rotationQuaternion)) {
      return false;
    }
  }
  else {
    if (!_cache.rotation.equals(rotation())) {
      return false;
    }
  }

  if (!_cache.scaling.equals(scaling())) {
    return false;
  }

  return true;
}

void AbstractMesh::_initCache()
{
  Node::_initCache();

  _cache.localMatrixUpdated = false;
  _cache.position           = Vector3::Zero();
  _cache.scaling            = Vector3::Zero();
  _cache.rotation           = Vector3::Zero();
  _cache.rotationQuaternion = Quaternion(0.f, 0.f, 0.f, 0.f);
  _cache.billboardMode      = AbstractMesh::BILLBOARDMODE_NONE;
}

void AbstractMesh::markAsDirty(const std::string& property)
{
  if (property == "rotation") {
    resetRotationQuaternion();
  }
  _currentRenderId = std::numeric_limits<int>::max();
  _isDirty         = true;
}

void AbstractMesh::_updateBoundingInfo()
{
  if (!_boundingInfo) {
    auto absolutePos = absolutePosition();
    _boundingInfo
      = std_util::make_unique<BoundingInfo>(*absolutePos, *absolutePos);
  }

  _boundingInfo->update(worldMatrixFromCache());

  _updateSubMeshesBoundingInfo(worldMatrixFromCache());
}

void AbstractMesh::_updateSubMeshesBoundingInfo(Matrix& matrix)
{
  if (subMeshes.empty()) {
    return;
  }

  for (auto& subMesh : subMeshes) {
    if (!subMesh->isGlobal()) {
      subMesh->updateBoundingInfo(matrix);
    }
  }
}

Matrix AbstractMesh::computeWorldMatrix(bool force)
{
  if (_isWorldMatrixFrozen) {
    return *_worldMatrix;
  }

  if (!force && (_currentRenderId == getScene()->getRenderId()
                 || isSynchronized(true))) {
    _currentRenderId = getScene()->getRenderId();
    return *_worldMatrix;
  }

  _cache.position.copyFrom(position);
  _cache.scaling.copyFrom(scaling());
  _cache.pivotMatrixUpdated = false;
  _cache.billboardMode      = billboardMode;
  _currentRenderId          = getScene()->getRenderId();
  _isDirty                  = false;

  // Scaling
  Matrix::ScalingToRef(_scaling.x * scalingDeterminant,
                       _scaling.y * scalingDeterminant,
                       _scaling.z * scalingDeterminant, Tmp::MatrixArray[1]);

  // Rotation

  // Rotate, if quaternion is set and rotation was used
  if (_rotationQuaternionSet) {
    float len = _rotation.length();
    if (len > 0.f) {
      _rotationQuaternion.multiplyInPlace(Quaternion::RotationYawPitchRoll(
        rotation().y, rotation().x, rotation().z));
      rotation().copyFromFloats(0.f, 0.f, 0.f);
    }
  }

  if (_rotationQuaternionSet) {
    _rotationQuaternion.toRotationMatrix(Tmp::MatrixArray[0]);
    _cache.rotationQuaternion.copyFrom(_rotationQuaternion);
  }
  else {
    Matrix::RotationYawPitchRollToRef(rotation().y, rotation().x, rotation().z,
                                      Tmp::MatrixArray[0]);
    _cache.rotation.copyFrom(rotation());
  }

  // Translation
  if (infiniteDistance && !parent()) {
    auto camera = getScene()->activeCamera;
    if (camera) {
      auto cameraWorldMatrix = camera->getWorldMatrix();

      Vector3 cameraGlobalPosition(cameraWorldMatrix->m[12],
                                   cameraWorldMatrix->m[13],
                                   cameraWorldMatrix->m[14]);

      Matrix::TranslationToRef(position.x + cameraGlobalPosition.x,
                               position.y + cameraGlobalPosition.y,
                               position.z + cameraGlobalPosition.z,
                               Tmp::MatrixArray[2]);
    }
  }
  else {
    Matrix::TranslationToRef(position.x, position.y, position.z,
                             Tmp::MatrixArray[2]);
  }

  // Composing transformations
  _pivotMatrix.multiplyToRef(Tmp::MatrixArray[1], Tmp::MatrixArray[4]);
  Tmp::MatrixArray[4].multiplyToRef(Tmp::MatrixArray[0], Tmp::MatrixArray[5]);

  // Billboarding
  if (billboardMode != AbstractMesh::BILLBOARDMODE_NONE
      && getScene()->activeCamera) {
    Tmp::Vector3Array[0].copyFrom(position);
    Vector3 localPosition = Tmp::Vector3Array[0];

    if (parent() && parent()->getWorldMatrix()) {
      _markSyncedWithParent();

      Matrix parentMatrix;
      if (_meshToBoneReferal) {
        parent()->getWorldMatrix()->multiplyToRef(
          *_meshToBoneReferal->getWorldMatrix(), Tmp::MatrixArray[6]);
        parentMatrix = Tmp::MatrixArray[6];
      }
      else {
        parentMatrix = *parent()->getWorldMatrix();
      }

      Vector3::TransformNormalToRef(localPosition, parentMatrix,
                                    Tmp::Vector3Array[1]);
      localPosition = Tmp::Vector3Array[1];
    }

    Vector3 zero = getScene()->activeCamera->globalPosition();

    if (parent()) {
      AbstractMesh* parentMesh = dynamic_cast<AbstractMesh*>(parent());
      if (parentMesh) {
        localPosition.addInPlace(parentMesh->position);
        Matrix::TranslationToRef(localPosition.x, localPosition.y,
                                 localPosition.z, Tmp::MatrixArray[2]);
      }
    }

    if ((billboardMode & AbstractMesh::BILLBOARDMODE_ALL)
        != AbstractMesh::BILLBOARDMODE_ALL) {
      if (billboardMode & AbstractMesh::BILLBOARDMODE_X) {
        zero.x = localPosition.x + MathTools::Epsilon;
      }
      if (billboardMode & AbstractMesh::BILLBOARDMODE_Y) {
        zero.y = localPosition.y + MathTools::Epsilon;
      }
      if (billboardMode & AbstractMesh::BILLBOARDMODE_Z) {
        zero.z = localPosition.z + MathTools::Epsilon;
      }
    }

    Matrix::LookAtLHToRef(localPosition, zero, Vector3::Up(),
                          Tmp::MatrixArray[3]);
    Tmp::MatrixArray[3].m[12]     = Tmp::MatrixArray[3].m[13]
      = Tmp::MatrixArray[3].m[14] = 0.f;

    Tmp::MatrixArray[3].invert();

    Tmp::MatrixArray[5].multiplyToRef(Tmp::MatrixArray[3], _localWorld);
    _rotateYByPI.multiplyToRef(_localWorld, Tmp::MatrixArray[5]);
  }

  // Local world
  Tmp::MatrixArray[5].multiplyToRef(Tmp::MatrixArray[2], _localWorld);

  // Parent
  if (parent() && parent()->getWorldMatrix()
      && billboardMode == AbstractMesh::BILLBOARDMODE_NONE) {
    _markSyncedWithParent();

    if (_meshToBoneReferal) {
      _localWorld.multiplyToRef(*parent()->getWorldMatrix(),
                                Tmp::MatrixArray[6]);
      Tmp::MatrixArray[6].multiplyToRef(*_meshToBoneReferal->getWorldMatrix(),
                                        *_worldMatrix);
    }
    else {
      _localWorld.multiplyToRef(*parent()->getWorldMatrix(), *_worldMatrix);
    }
  }
  else {
    _worldMatrix->copyFrom(_localWorld);
  }

  // Bounding info
  _updateBoundingInfo();

  // Absolute position
  _absolutePosition->copyFromFloats(_worldMatrix->m[12], _worldMatrix->m[13],
                                    _worldMatrix->m[14]);

  // Callbacks
  onAfterWorldMatrixUpdateObservable.notifyObservers(this);

  if (!_poseMatrix) {
    _poseMatrix = std_util::make_unique<Matrix>(Matrix::Invert(*_worldMatrix));
  }

  return *_worldMatrix;
}

void AbstractMesh::registerAfterWorldMatrixUpdate(
  const std::function<void(AbstractMesh* mesh)>& func)
{
  onAfterWorldMatrixUpdateObservable.add(func);
}

void AbstractMesh::unregisterAfterWorldMatrixUpdate(
  const std::function<void(AbstractMesh* mesh)>& func)
{
  onAfterWorldMatrixUpdateObservable.removeCallback(func);
}

void AbstractMesh::setPositionWithLocalVector(const Vector3& vector3)
{
  computeWorldMatrix();

  position = Vector3(Vector3::TransformNormal(vector3, _localWorld));
}

Vector3 AbstractMesh::getPositionExpressedInLocalSpace()
{
  computeWorldMatrix();
  auto invLocalWorldMatrix = _localWorld;
  invLocalWorldMatrix.invert();

  return Vector3::TransformNormal(position, invLocalWorldMatrix);
}

void AbstractMesh::locallyTranslate(const Vector3& vector3)
{
  computeWorldMatrix(true);

  position = Vector3(Vector3::TransformCoordinates(vector3, _localWorld));
}

void AbstractMesh::lookAt(const Vector3& targetPoint, float yawCor,
                          float pitchCor, float rollCor, Space space)
{
  Vector3& dv = AbstractMesh::_lookAtVectorCache;
  Vector3 pos = (space == Space::LOCAL) ? position : *getAbsolutePosition();
  targetPoint.subtractToRef(pos, dv);
  float yaw   = -std::atan2(dv.z, dv.x) - Math::PI_2;
  float len   = std::sqrt(dv.x * dv.x + dv.z * dv.z);
  float pitch = std::atan2(dv.y, len);
  if (!_rotationQuaternionSet) {
    _rotationQuaternion    = Quaternion();
    _rotationQuaternionSet = true;
  }
  Quaternion::RotationYawPitchRollToRef(yaw + yawCor, pitch + pitchCor, rollCor,
                                        _rotationQuaternion);
}

void AbstractMesh::attachToBone(Bone* bone, AbstractMesh* affectedMesh)
{
  _meshToBoneReferal = affectedMesh;
  setParent(dynamic_cast<Node*>(bone));

  if (bone->getWorldMatrix()->determinant() < 0.f) {
    scalingDeterminant *= -1.f;
  }
}

void AbstractMesh::detachFromBone()
{
  if (parent()->getWorldMatrix()->determinant() < 0.f) {
    scalingDeterminant *= -1.f;
  }

  _meshToBoneReferal = nullptr;
  setParent(nullptr);
}

bool AbstractMesh::isInFrustum(const std::array<Plane, 6>& frustumPlanes)
{
  return _boundingInfo->isInFrustum(frustumPlanes);
}

bool AbstractMesh::isCompletelyInFrustum(
  const std::array<Plane, 6>& frustumPlanes) const
{
  return _boundingInfo->isCompletelyInFrustum(frustumPlanes);
}

bool AbstractMesh::intersectsMesh(AbstractMesh* mesh, bool precise)
{
  if (!_boundingInfo || !mesh->_boundingInfo) {
    return false;
  }

  return _boundingInfo->intersects(*mesh->_boundingInfo, precise);
}

bool AbstractMesh::intersectsMesh(SolidParticle* /*sp*/, bool /*precise*/)
{
  /*if (!_boundingInfo || !sp->_boundingInfo) {
    return false;
  }

  return _boundingInfo->intersects(*sp->_boundingInfo, precise);*/
  return false;
}

bool AbstractMesh::intersectsPoint(const Vector3& point)
{
  if (!_boundingInfo) {
    return false;
  }

  return _boundingInfo->intersectsPoint(point);
}

PhysicsImpostor* AbstractMesh::getPhysicsImpostor()
{
  return physicsImpostor;
}

Vector3 AbstractMesh::getPositionInCameraSpace(Camera* camera)
{
  if (!camera) {
    camera = getScene()->activeCamera;
  }

  return Vector3::TransformCoordinates(*_absolutePosition,
                                       camera->getViewMatrix());
}

float AbstractMesh::getDistanceToCamera(Camera* camera)
{
  if (!camera) {
    camera = getScene()->activeCamera;
  }

  return _absolutePosition->subtract(camera->position).length();
}

void AbstractMesh::applyImpulse(const Vector3& force,
                                const Vector3& contactPoint)
{
  if (!physicsImpostor) {
    return;
  }

  physicsImpostor->applyImpulse(force, contactPoint);
}

void AbstractMesh::setPhysicsLinkWith(Mesh* otherMesh,
                                      const Vector3& /*pivot1*/,
                                      const Vector3& /*pivot2*/,
                                      const PhysicsParams& /*options*/)
{
  if (!physicsImpostor || !otherMesh->physicsImpostor) {
    return;
  }
}

bool AbstractMesh::checkCollisions() const
{
  return _checkCollisions;
}

void AbstractMesh::setCheckCollisions(bool collisionEnabled)
{
  _checkCollisions = collisionEnabled;
  /*if (getScene()->workerCollisions()) {
    getScene()->collisionCoordinator()->onMeshUpdated(this);
  }*/
}

void AbstractMesh::moveWithCollisions(const Vector3& /*velocity*/)
{
  /*var globalPosition = getAbsolutePosition();

  globalPosition.subtractFromFloatsToRef(0, ellipsoid.y, 0,
                                         _oldPositionForCollisions);
  _oldPositionForCollisions.addInPlace(ellipsoidOffset);
  _collider.radius = ellipsoid;

  getScene().collisionCoordinator.getNewPosition(
    _oldPositionForCollisions, velocity, _collider, 3, this,
    _onCollisionPositionChange, uniqueId);*/
}

void AbstractMesh::_onCollisionPositionChange(int /*collisionId*/,
                                              const Vector3& /*newPosition*/,
                                              AbstractMesh* /*collidedMesh*/)
{
  /*if (getScene().workerCollisions) {
    newPosition.multiplyInPlace(_collider.radius);
  }

  newPosition.subtractToRef(_oldPositionForCollisions,
                            _diffPositionForCollisions);

  if (_diffPositionForCollisions.length() > Engine::CollisionsEpsilon) {
    position.addInPlace(_diffPositionForCollisions);
  }

  if (collidedMesh) {
    onCollideObservable.notifyObservers(collidedMesh);
  }

  onCollisionPositionChangeObservable.notifyObservers(position);*/
}

Octree<SubMesh*>*
AbstractMesh::createOrUpdateSubmeshesOctree(size_t maxCapacity, size_t maxDepth)
{
  if (!_submeshesOctree) {
    _submeshesOctree = new Octree<SubMesh*>(
      [](SubMesh* entry, OctreeBlock<SubMesh*>& block) {
        Octree<AbstractMesh*>::CreationFuncForSubMeshes(entry, block);
      },
      maxCapacity, maxDepth);
  }

  computeWorldMatrix(true);

  // Update octree
  // BoundingBox& bbox = getBoundingInfo()->boundingBox;
  //_submeshesOctree->update(bbox.minimumWorld, bbox.maximumWorld, subMeshes);

  return _submeshesOctree;
}

void AbstractMesh::_collideForSubMesh(SubMesh* /*subMesh*/,
                                      const Matrix& /*transformMatrix*/,
                                      Collider* /*collider*/)
{
  /*_generatePointsArray();
  // Transformation
  if (!subMesh._lastColliderWorldVertices
      || !subMesh._lastColliderTransformMatrix.equals(transformMatrix)) {
    subMesh._lastColliderTransformMatrix = transformMatrix.clone();
    subMesh._lastColliderWorldVertices   = [];
    subMesh._trianglePlanes              = [];
    var start                            = subMesh.verticesStart;
    var end = (subMesh.verticesStart + subMesh.verticesCount);
    for (var i = start; i < end; i++) {
      subMesh._lastColliderWorldVertices.push(
        Vector3.TransformCoordinates(_positions[i], transformMatrix));
    }
  }
  // Collide
  collider._collide(subMesh._trianglePlanes, subMesh._lastColliderWorldVertices,
                    getIndices(), subMesh.indexStart,
                    subMesh.indexStart + subMesh.indexCount,
                    subMesh.verticesStart, !!subMesh.getMaterial());
  if (collider.collisionFound) {
    collider.collidedMesh = this;
  }*/
}

void AbstractMesh::_processCollisionsForSubMeshes(
  Collider* /*collider*/, const Matrix& /*transformMatrix*/)
{
  /*var subMeshes : SubMesh[];
  var len : number;

  // Octrees
  if (_submeshesOctree && useOctreeForCollisions) {
    var radius
      = collider.velocityWorldLength
        + Math.max(collider.radius.x, collider.radius.y, collider.radius.z);
    var intersections
      = _submeshesOctree.intersects(collider.basePointWorld, radius);

    len       = intersections.length;
    subMeshes = intersections.data;
  }
  else {
    subMeshes = subMeshes;
    len       = subMeshes.length;
  }

  for (var index = 0; index < len; index++) {
    var subMesh = subMeshes[index];

    // Bounding test
    if (len > 1 && !subMesh._checkCollision(collider))
      continue;

    _collideForSubMesh(subMesh, transformMatrix, collider);
  }*/
}

void AbstractMesh::_checkCollision(Collider* /*collider*/)
{
  // Bounding box test
  /*if (!_boundingInfo->_checkCollision(collider))
    return;

  // Transformation matrix
  Matrix.ScalingToRef(1.0 / collider.radius.x, 1.0 / collider.radius.y,
                      1.0 / collider.radius.z, _collisionsScalingMatrix);
  worldMatrixFromCache.multiplyToRef(_collisionsScalingMatrix,
                                     _collisionsTransformMatrix);

  _processCollisionsForSubMeshes(collider, _collisionsTransformMatrix);*/
}

bool AbstractMesh::_generatePointsArray()
{
  return false;
}

PickingInfo* AbstractMesh::intersects(const Ray& /*ray*/, bool /*fastCheck*/)
{
  /*var pickingInfo = new PickingInfo();

  if (!subMeshes || !_boundingInfo
      || !ray.intersectsSphere(_boundingInfo.boundingSphere)
      || !ray.intersectsBox(_boundingInfo.boundingBox)) {
    return pickingInfo;
  }

  if (!_generatePointsArray()) {
    return pickingInfo;
  }

  var intersectInfo : IntersectionInfo = null;

  // Octrees
  var subMeshes : SubMesh[];
  var len : number;

  if (_submeshesOctree && useOctreeForPicking) {
    var worldRay      = Ray.Transform(ray, getWorldMatrix());
    var intersections = _submeshesOctree.intersectsRay(worldRay);

    len       = intersections.length;
    subMeshes = intersections.data;
  }
  else {
    subMeshes = subMeshes;
    len       = subMeshes.length;
  }

  for (var index = 0; index < len; index++) {
    var subMesh = subMeshes[index];

    // Bounding test
    if (len > 1 && !subMesh.canIntersects(ray))
      continue;

    var currentIntersectInfo
      = subMesh.intersects(ray, _positions, getIndices(), fastCheck);

    if (currentIntersectInfo) {
      if (fastCheck || !intersectInfo
          || currentIntersectInfo.distance < intersectInfo.distance) {
        intersectInfo           = currentIntersectInfo;
        intersectInfo.subMeshId = index;

        if (fastCheck) {
          break;
        }
      }
    }
  }

  if (intersectInfo) {
    // Get picked point
    var world          = getWorldMatrix();
    var worldOrigin    = Vector3.TransformCoordinates(ray.origin, world);
    var direction      = ray.direction.clone();
    direction          = direction.scale(intersectInfo.distance);
    var worldDirection = Vector3.TransformNormal(direction, world);

    var pickedPoint = worldOrigin.add(worldDirection);

    // Return result
    pickingInfo.hit         = true;
    pickingInfo.distance    = Vector3.Distance(worldOrigin, pickedPoint);
    pickingInfo.pickedPoint = pickedPoint;
    pickingInfo.pickedMesh  = this;
    pickingInfo.bu          = intersectInfo.bu;
    pickingInfo.bv          = intersectInfo.bv;
    pickingInfo.faceId      = intersectInfo.faceId;
    pickingInfo.subMeshId   = intersectInfo.subMeshId;
    return pickingInfo;
  }

  return pickingInfo;*/

  return nullptr;
}

AbstractMesh* AbstractMesh::clone(const std::string& /*name*/,
                                  Node* /*newParent*/,
                                  bool /*doNotCloneChildren*/)
{
  return nullptr;
}

void AbstractMesh::releaseSubMeshes()
{
  if (!subMeshes.empty()) {
    for (auto& subMesh : subMeshes) {
      subMesh->dispose();
    }
  }
  else {
    subMeshes.clear();
  }
}

void AbstractMesh::dispose(bool doNotRecurse)
{
  // Action manager
  if (actionManager) {
    actionManager->dispose();
    actionManager = nullptr;
  }

  // Skeleton
  // auto skeletonTmp = skeleton();
  // skeletonTmp      = nullptr;

  // Animations
  getScene()->stopAnimation(this);

  // Physics
  if (physicsImpostor) {
    physicsImpostor->dispose(/*!doNotRecurse*/);
  }

  // Intersections in progress
  for (auto& other : _intersectionsInProgress) {
    std::remove(other->_intersectionsInProgress.begin(),
                other->_intersectionsInProgress.end(), this);
  }

  _intersectionsInProgress.clear();

  // Lights
  for (auto& light : getScene()->lights) {
    // Included meshes
    std::remove(light->includedOnlyMeshes.begin(),
                light->includedOnlyMeshes.end(), this);

    // Excluded meshes
    std::remove(light->excludedMeshes.begin(), light->excludedMeshes.end(),
                this);

    // Shadow generators
    auto generator = light->getShadowGenerator();
    if (generator) {
      std::remove(generator->getShadowMap()->renderList.begin(),
                  generator->getShadowMap()->renderList.end(), this);
    }
  }

  // Edges
  if (_edgesRenderer) {
    _edgesRenderer->dispose();
    _edgesRenderer.reset(nullptr);
  }

  // SubMeshes
  releaseSubMeshes();

  // Engine
  getScene()->getEngine()->unbindAllAttributes();

  // Remove from scene
  getScene()->removeMesh(this);

  if (!doNotRecurse) {
    // Particles
    for (size_t index = 0; index < getScene()->particleSystems.size();
         ++index) {
      if (getScene()->particleSystems[index]->emitter == this) {
        getScene()->particleSystems[index]->dispose();
        --index;
      }
    }

    // Children
    for (auto& object : getDescendants(true)) {
      object->dispose();
    }
  }
  else {
    for (auto& child : getChildMeshes(true)) {
      child->setParent(nullptr);
      child->computeWorldMatrix(true);
    }
  }

  onAfterWorldMatrixUpdateObservable.clear();
  onCollideObservable.clear();
  onCollisionPositionChangeObservable.clear();

  _isDisposed = true;

  Node::dispose();
}

Vector3 AbstractMesh::getDirection(const Vector3& localAxis)
{
  auto result = Vector3::Zero();

  getDirectionToRef(localAxis, result);

  return result;
}

void AbstractMesh::getDirectionToRef(const Vector3& localAxis, Vector3& result)
{
  Vector3::TransformNormalToRef(localAxis, *getWorldMatrix(), result);
}

void AbstractMesh::setPivotPoint(const Vector3& point, const Space& space)
{
  auto _point = point;
  if (getScene()->getRenderId() == 0) {
    computeWorldMatrix(true);
  }

  auto wm = getWorldMatrix();

  if (space == Space::WORLD) {
    auto& tmat = Tmp::MatrixArray[0];
    wm->invertToRef(tmat);
    _point = Vector3::TransformCoordinates(_point, tmat);
  }

  Vector3::TransformCoordinatesToRef(_point, *wm, position);

  _pivotMatrix.m[12] = -_point.x;
  _pivotMatrix.m[13] = -_point.y;
  _pivotMatrix.m[14] = -_point.z;

  _cache.pivotMatrixUpdated = true;
}

Vector3 AbstractMesh::getPivotPoint() const
{
  auto point = Vector3::Zero();

  getPivotPointToRef(point);

  return point;
}

void AbstractMesh::getPivotPointToRef(Vector3& result) const
{
  result.x = -_pivotMatrix.m[12];
  result.y = -_pivotMatrix.m[13];
  result.z = -_pivotMatrix.m[14];
}

Vector3 AbstractMesh::getAbsolutePivotPoint()
{
  auto point = Vector3::Zero();

  getAbsolutePivotPointToRef(point);

  return point;
}

void AbstractMesh::getAbsolutePivotPointToRef(Vector3& result)
{
  result.x = _pivotMatrix.m[12];
  result.y = _pivotMatrix.m[13];
  result.z = _pivotMatrix.m[14];

  getPivotPointToRef(result);

  Vector3::TransformCoordinatesToRef(result, *getWorldMatrix(), result);
}

} // end of namespace BABYLON
