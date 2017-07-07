#include <babylon/mesh/abstract_mesh.h>

#include <babylon/actions/action_manager.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/frustum.h>
#include <babylon/math/math_tools.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/particles/particle_system.h>
#include <babylon/physics/joint/physics_joint.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/rendering/edges_renderer.h>

namespace BABYLON {

Quaternion AbstractMesh::_rotationAxisCache;
Vector3 AbstractMesh::_lookAtVectorCache = Vector3(0.f, 0.f, 0.f);

AbstractMesh::AbstractMesh(const std::string& iName, Scene* scene)
    : Node(iName, scene)
    , definedFacingForward{true} // orientation for POV movement & rotation
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
    , renderOutline{false}
    , outlineColor{Color3::Red()}
    , outlineWidth{0.02f}
    , renderOverlay{false}
    , overlayColor{Color3::Red()}
    , overlayAlpha{0.5f}
    , scalingDeterminant{1.f}
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
    , _worldMatrix{std::make_unique<Matrix>(Matrix::Zero())}
    , _masterMesh{nullptr}
    , _materialDefines{nullptr}
    , _boundingInfo{nullptr}
    , _isDisposed{false}
    , _renderId{0}
    , _submeshesOctree{nullptr}
    , _unIndexed{false}
    , _facetNb{0}
    , _partitioningSubdivisions{10}
    , _partitioningBBoxRatio{1.01f}
    , _facetDataEnabled{false}
    , _bbSize{Vector3::Zero()}
    , _subDiv{1, 1, 1, 1}
    , _onCollideObserver{nullptr}
    , _onCollisionPositionChangeObserver{nullptr}
    , _position{Vector3::Zero()}
    , _rotation{Vector3::Zero()}
    , _rotationQuaternionSet{false}
    , _scaling{Vector3::One()}
    , _material{nullptr}
    , _receiveShadows{false}
    , _hasVertexAlpha{false}
    , _useVertexColors{true}
    , _computeBonesUsingShaders{true}
    , _numBoneInfluencers{4}
    , _applyFog{true}
    , _checkCollisions{false}
    , _collisionMask{-1}
    , _collisionGroup{-1}
    , _collider{nullptr}
    , _oldPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _diffPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _newPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _meshToBoneReferal{nullptr}
    , _localWorld{Matrix::Zero()}
    , _absolutePosition{std::make_unique<Vector3>(Vector3::Zero())}
    , _collisionsTransformMatrix{Matrix::Zero()}
    , _collisionsScalingMatrix{Matrix::Zero()}
    , _isDirty{false}
    , _pivotMatrix{Matrix::Identity()}
    , _isWorldMatrixFrozen{false}
    , _skeleton{nullptr}
{
  _resyncLightSources();
}

AbstractMesh::~AbstractMesh()
{
}

IReflect::Type AbstractMesh::type() const
{
  return IReflect::Type::ABSTRACTMESH;
}

void AbstractMesh::addToScene(std::unique_ptr<AbstractMesh>&& newMesh)
{
  getScene()->addMesh(std::move(newMesh));
}

size_t AbstractMesh::facetNb() const
{
  return _facetNb;
}

unsigned int AbstractMesh::partitioningSubdivisions() const
{
  return _partitioningSubdivisions;
}

void AbstractMesh::setPartitioningSubdivisions(unsigned int nb)
{
  _partitioningSubdivisions = nb;
}

float AbstractMesh::partitioningBBoxRatio() const
{
  return _partitioningBBoxRatio;
}

void AbstractMesh::setPartitioningBBoxRatio(float ratio)
{
  _partitioningBBoxRatio = ratio;
}

bool AbstractMesh::isFacetDataEnabled() const
{
  return _facetDataEnabled;
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

Material* AbstractMesh::material()
{
  return _material;
}

void AbstractMesh::setMaterial(Material* value)
{
  if (_material == value) {
    return;
  }

  _material = value;
  if (subMeshes.empty()) {
    return;
  }

  for (auto& subMesh : subMeshes) {
    subMesh->setEffect(nullptr);
  }
}

bool AbstractMesh::receiveShadows() const
{
  return _receiveShadows;
}

void AbstractMesh::setReceiveShadows(bool value)
{
  if (_receiveShadows == value) {
    return;
  }

  _receiveShadows = value;
  _markSubMeshesAsLightDirty();
}

bool AbstractMesh::hasVertexAlpha() const
{
  return _hasVertexAlpha;
}

void AbstractMesh::setHasVertexAlpha(bool value)
{
  if (_hasVertexAlpha == value) {
    return;
  }

  _hasVertexAlpha = value;
  _markSubMeshesAsAttributesDirty();
}

bool AbstractMesh::useVertexColors() const
{
  return _useVertexColors;
}

void AbstractMesh::setUseVertexColors(bool value)
{
  if (_useVertexColors == value) {
    return;
  }

  _useVertexColors = value;
  _markSubMeshesAsAttributesDirty();
}

bool AbstractMesh::computeBonesUsingShaders() const
{
  return _computeBonesUsingShaders;
}

void AbstractMesh::setComputeBonesUsingShaders(bool value)
{
  if (_computeBonesUsingShaders == value) {
    return;
  }

  _computeBonesUsingShaders = value;
  _markSubMeshesAsAttributesDirty();
}

unsigned int AbstractMesh::numBoneInfluencers() const
{
  return _numBoneInfluencers;
}

void AbstractMesh::setNumBoneInfluencers(unsigned int value)
{
  if (_numBoneInfluencers == value) {
    return;
  }

  _numBoneInfluencers = value;
  _markSubMeshesAsAttributesDirty();
}

bool AbstractMesh::applyFog() const
{
  return _applyFog;
}

void AbstractMesh::setApplyFog(bool value)
{
  if (_applyFog == value) {
    return;
  }

  _applyFog = value;
  _markSubMeshesAsMiscDirty();
}

int AbstractMesh::collisionMask() const
{
  return _collisionMask;
}

void AbstractMesh::setCollisionMask(int mask)
{
  _collisionMask = !std::isnan(mask) ? mask : -1;
}

int AbstractMesh::collisionGroup() const
{
  return _collisionGroup;
}

void AbstractMesh::setCollisionGroup(int mask)
{
  _collisionGroup = !std::isnan(mask) ? mask : -1;
}

const char* AbstractMesh::getClassName() const
{
  return "AbstractMesh";
}

std::string AbstractMesh::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name << ", isInstance: ";
  oss << ((type() == IReflect::Type::INSTANCEDMESH) ? "YES" : "NO");
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

void AbstractMesh::_resyncLightSources()
{
  _lightSources.clear();

  for (auto& light : getScene()->lights) {
    if (!light->isEnabled()) {
      continue;
    }

    if (light->canAffectMesh(this)) {
      _lightSources.emplace_back(light.get());
    }
  }

  _markSubMeshesAsLightDirty();
}

void AbstractMesh::_resyncLighSource(Light* light)
{
  bool isIn = light->isEnabled() && light->canAffectMesh(this);

  auto index = std::find(_lightSources.begin(), _lightSources.end(), light);

  if (index != _lightSources.end()) {
    if (!isIn) {
      return;
    }
    _lightSources.emplace_back(light);
  }
  else {
    if (isIn) {
      return;
    }
    _lightSources.erase(index);
  }

  _markSubMeshesAsLightDirty();
}

void AbstractMesh::_removeLightSource(Light* light)
{
  auto index = std::find(_lightSources.begin(), _lightSources.end(), light);

  if (index == _lightSources.end()) {
    return;
  }

  _lightSources.erase(index);
}

void AbstractMesh::_markSubMeshesAsDirty(
  const std::function<void(const MaterialDefines& defines)>& func)
{
  if (subMeshes.empty()) {
    return;
  }

  for (auto& subMesh : subMeshes) {
    if (subMesh->_materialDefines) {
      func(*subMesh->_materialDefines);
    }
  }
}

void AbstractMesh::_markSubMeshesAsLightDirty()
{
}

void AbstractMesh::_markSubMeshesAsAttributesDirty()
{
}

void AbstractMesh::_markSubMeshesAsMiscDirty()
{
  if (subMeshes.empty()) {
    return;
  }

  for (auto& subMesh : subMeshes) {
    auto material = subMesh->getMaterial();
    if (material) {
      material->markAsDirty(Material::MiscDirtyFlag);
    }
  }
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

  _markSubMeshesAsAttributesDirty();
}

Skeleton* AbstractMesh::skeleton()
{
  return _skeleton;
}

std::vector<Vector3>& AbstractMesh::_positions()
{
  return _emptyPositions;
}

Vector3& AbstractMesh::position()
{
  return _position;
}

void AbstractMesh::setPosition(const Vector3& newPosition)
{
  _position = newPosition;
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

void AbstractMesh::nullifyRotationQuaternion()
{
  _rotationQuaternionSet = false;
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
  if (parent()->type() == IReflect::Type::ABSTRACTMESH) {
    return dynamic_cast<AbstractMesh*>(parent());
  }

  return nullptr;
}

Material* AbstractMesh::getMaterial()
{
  return material();
}

std::vector<AbstractMesh*>
AbstractMesh::getChildMeshes(bool directDecendantsOnly,
                             const std::function<bool(Node* node)>& predicate)
{
  return Node::getChildMeshes(directDecendantsOnly, predicate);
}

AbstractMesh& AbstractMesh::updatePoseMatrix(const Matrix& matrix)
{
  _poseMatrix->copyFrom(matrix);
  return *this;
}

Matrix& AbstractMesh::getPoseMatrix()
{
  return *_poseMatrix;
}

AbstractMesh& AbstractMesh::disableEdgesRendering()
{
  if (_edgesRenderer != nullptr) {
    _edgesRenderer->dispose();
    _edgesRenderer.reset(nullptr);
  }
  return *this;
}

AbstractMesh&
AbstractMesh::enableEdgesRendering(float epsilon,
                                   bool checkVerticesInsteadOfIndices)
{
  disableEdgesRendering();

  _edgesRenderer = std::make_unique<EdgesRenderer>(
    this, epsilon, checkVerticesInsteadOfIndices);

  return *this;
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
                                           bool /*copyWhenShared*/,
                                           bool /*forceCopy*/)
{
  return Float32Array();
}

Mesh* AbstractMesh::setVerticesData(unsigned int /*kind*/,
                                    const Float32Array& /*data*/,
                                    bool /*updatable*/, int /*stride*/)
{
  return nullptr;
}

Mesh* AbstractMesh::updateVerticesData(unsigned int /*kind*/,
                                       const Float32Array& /*data*/,
                                       bool /*updateExtends*/,
                                       bool /*makeItUnique*/)
{
  return nullptr;
}

Mesh* AbstractMesh::setIndices(const IndicesArray& /*indices*/,
                               size_t /*totalVertices*/)
{
  return nullptr;
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

AbstractMesh& AbstractMesh::setBoundingInfo(const BoundingInfo& boundingInfo)
{
  _boundingInfo = std::make_unique<BoundingInfo>(boundingInfo);
  return *this;
}

bool AbstractMesh::useBones()
{
  return _skeleton && getScene()->skeletonsEnabled()
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

  if (_currentRenderId != getScene()->getRenderId() || !isSynchronized()) {
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

AbstractMesh& AbstractMesh::freezeWorldMatrix()
{
  // no guarantee world is not already frozen, switch off temporarily
  _isWorldMatrixFrozen = false;
  computeWorldMatrix(true);
  _isWorldMatrixFrozen = true;
  return *this;
}

AbstractMesh& AbstractMesh::unfreezeWorldMatrix()
{
  _isWorldMatrixFrozen = false;
  computeWorldMatrix(true);
  return *this;
}

bool AbstractMesh::isWorldMatrixFrozen()
{
  return _isWorldMatrixFrozen;
}

AbstractMesh& AbstractMesh::rotate(Vector3& axis, float amount,
                                   const Space& space)
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
  return *this;
}

AbstractMesh& AbstractMesh::rotateAround(const Vector3& point, Vector3& axis,
                                         float amount)
{
  axis.normalize();
  if (!rotationQuaternionSet()) {
    const auto rotationValue = rotation();
    setRotationQuaternion(Quaternion::RotationYawPitchRoll(
      rotationValue.y, rotationValue.x, rotationValue.z));
    rotation().copyFromFloats(0.f, 0.f, 0.f);
  }
  point.subtractToRef(position(), Tmp::Vector3Array[0]);
  Matrix::TranslationToRef(Tmp::Vector3Array[0].x, Tmp::Vector3Array[0].y,
                           Tmp::Vector3Array[0].z, Tmp::MatrixArray[0]);
  Tmp::MatrixArray[0].invertToRef(Tmp::MatrixArray[2]);
  Matrix::RotationAxisToRef(axis, amount, Tmp::MatrixArray[1]);
  Tmp::MatrixArray[2].multiplyToRef(Tmp::MatrixArray[1], Tmp::MatrixArray[2]);
  Tmp::MatrixArray[2].multiplyToRef(Tmp::MatrixArray[0], Tmp::MatrixArray[2]);

  Tmp::MatrixArray[2].decompose(Tmp::Vector3Array[0], Tmp::QuaternionArray[0],
                                Tmp::Vector3Array[1]);

  position().addInPlace(Tmp::Vector3Array[1]);
  rotationQuaternion().multiplyInPlace(Tmp::QuaternionArray[0]);

  return *this;
}

AbstractMesh& AbstractMesh::translate(Vector3& axis, float distance,
                                      const Space& space)
{
  auto displacementVector = axis.scale(distance);

  if (space == Space::LOCAL) {
    auto tempV3 = getPositionExpressedInLocalSpace().add(displacementVector);
    setPositionWithLocalVector(tempV3);
  }
  else {
    setAbsolutePosition(getAbsolutePosition()->add(displacementVector));
  }
  return *this;
}

AbstractMesh& AbstractMesh::addRotation(float x, float y, float z)
{
  Quaternion rotationQuaternionTmp;
  if (rotationQuaternionSet()) {
    rotationQuaternionTmp = rotationQuaternion();
  }
  else {
    rotationQuaternionTmp = Tmp::QuaternionArray[1];
    Quaternion::RotationYawPitchRollToRef(rotation().y, rotation().x,
                                          rotation().z, rotationQuaternionTmp);
  }
  auto accumulation = Tmp::QuaternionArray[0];
  Quaternion::RotationYawPitchRollToRef(y, x, z, accumulation);
  rotationQuaternionTmp.multiplyInPlace(accumulation);
  if (!rotationQuaternionSet()) {
    rotationQuaternionTmp.toEulerAnglesToRef(rotation());
  }
  return *this;
}

Vector3* AbstractMesh::getAbsolutePosition()
{
  computeWorldMatrix();
  return _absolutePosition.get();
}

AbstractMesh&
AbstractMesh::setAbsolutePosition(const Vector3& absolutePosition_)
{
  const float absolutePositionX = absolutePosition_.x;
  const float absolutePositionY = absolutePosition_.y;
  const float absolutePositionZ = absolutePosition_.z;

  if (parent()) {
    auto invertParentWorldMatrix = parent()->getWorldMatrix();
    invertParentWorldMatrix->invert();

    Vector3 worldPosition(absolutePositionX, absolutePositionY,
                          absolutePositionZ);

    _position
      = Vector3::TransformCoordinates(worldPosition, *invertParentWorldMatrix);
  }
  else {
    _position.x = absolutePositionX;
    _position.y = absolutePositionY;
    _position.z = absolutePositionZ;
  }

  return *this;
}

AbstractMesh& AbstractMesh::movePOV(float amountRight, float amountUp,
                                    float amountForward)
{
  _position.addInPlace(calcMovePOV(amountRight, amountUp, amountForward));
  return *this;
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

AbstractMesh& AbstractMesh::rotatePOV(float flipBack, float twirlClockwise,
                                      float tiltRight)
{
  rotation().addInPlace(calcRotatePOV(flipBack, twirlClockwise, tiltRight));
  return *this;
}

Vector3 AbstractMesh::calcRotatePOV(float flipBack, float twirlClockwise,
                                    float tiltRight)
{
  auto defForwardMult = definedFacingForward ? 1.f : -1.f;
  return Vector3(flipBack * defForwardMult, twirlClockwise,
                 tiltRight * defForwardMult);
}

AbstractMesh& AbstractMesh::setPivotMatrix(const Matrix& matrix)
{
  _pivotMatrix              = matrix;
  _cache.pivotMatrixUpdated = true;
  return *this;
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

  if (!_cache.position.equals(_position)) {
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

void AbstractMesh::markAsDirty(unsigned int property)
{
  if (property == static_cast<unsigned>(PropertyType::ROTATION)) {
    resetRotationQuaternion();
  }
  _currentRenderId = std::numeric_limits<int>::max();
  _isDirty         = true;
}

AbstractMesh& AbstractMesh::_updateBoundingInfo()
{
  if (!_boundingInfo) {
    auto absolutePos = absolutePosition();
    _boundingInfo = std::make_unique<BoundingInfo>(*absolutePos, *absolutePos);
  }

  _boundingInfo->update(worldMatrixFromCache());

  _updateSubMeshesBoundingInfo(worldMatrixFromCache());

  return *this;
}

AbstractMesh& AbstractMesh::_updateSubMeshesBoundingInfo(Matrix& matrix)
{
  if (subMeshes.empty()) {
    return *this;
  }

  for (auto& subMesh : subMeshes) {
    if (!subMesh->isGlobal()) {
      subMesh->updateBoundingInfo(matrix);
    }
  }

  return *this;
}

Matrix AbstractMesh::computeWorldMatrix(bool force)
{
  if (_isWorldMatrixFrozen) {
    return *_worldMatrix;
  }

  if (!force && isSynchronized(true)) {
    _currentRenderId = getScene()->getRenderId();
    return *_worldMatrix;
  }

  _cache.position.copyFrom(_position);
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

      Matrix::TranslationToRef(_position.x + cameraGlobalPosition.x,
                               _position.y + cameraGlobalPosition.y,
                               _position.z + cameraGlobalPosition.z,
                               Tmp::MatrixArray[2]);
    }
  }
  else {
    Matrix::TranslationToRef(_position.x, _position.y, _position.z,
                             Tmp::MatrixArray[2]);
  }

  // Composing transformations
  _pivotMatrix.multiplyToRef(Tmp::MatrixArray[1], Tmp::MatrixArray[4]);
  Tmp::MatrixArray[4].multiplyToRef(Tmp::MatrixArray[0], Tmp::MatrixArray[5]);

  // Billboarding (testing PG:http://www.babylonjs-playground.com/#UJEIL#13)
  if (billboardMode != AbstractMesh::BILLBOARDMODE_NONE
      && getScene()->activeCamera) {
    if ((billboardMode & AbstractMesh::BILLBOARDMODE_ALL)
        != AbstractMesh::BILLBOARDMODE_ALL) {
      // Need to decompose each rotation here
      auto currentPosition = Tmp::Vector3Array[3];

      if (parent() && parent()->getWorldMatrix()) {
        if (_meshToBoneReferal) {
          parent()->getWorldMatrix()->multiplyToRef(
            *_meshToBoneReferal->getWorldMatrix(), Tmp::MatrixArray[6]);
          Vector3::TransformCoordinatesToRef(position(), Tmp::MatrixArray[6],
                                             currentPosition);
        }
        else {
          Vector3::TransformCoordinatesToRef(
            position(), *parent()->getWorldMatrix(), currentPosition);
        }
      }
      else {
        currentPosition.copyFrom(position());
      }

      currentPosition.subtractInPlace(
        getScene()->activeCamera->globalPosition());

      auto finalEuler = Tmp::Vector3Array[4].copyFromFloats(0.f, 0.f, 0.f);
      if ((billboardMode & AbstractMesh::BILLBOARDMODE_X)
          == AbstractMesh::BILLBOARDMODE_X) {
        finalEuler.x = std::atan2(-currentPosition.y, currentPosition.z);
      }

      if ((billboardMode & AbstractMesh::BILLBOARDMODE_Y)
          == AbstractMesh::BILLBOARDMODE_Y) {
        finalEuler.y = std::atan2(currentPosition.x, currentPosition.z);
      }

      if ((billboardMode & AbstractMesh::BILLBOARDMODE_Z)
          == AbstractMesh::BILLBOARDMODE_Z) {
        finalEuler.z = std::atan2(currentPosition.y, currentPosition.x);
      }

      Matrix::RotationYawPitchRollToRef(finalEuler.y, finalEuler.x,
                                        finalEuler.z, Tmp::MatrixArray[0]);
    }
    else {
      Tmp::MatrixArray[1].copyFrom(getScene()->activeCamera->getViewMatrix());

      Tmp::MatrixArray[1].setTranslationFromFloats(0, 0, 0);
      Tmp::MatrixArray[1].invertToRef(Tmp::MatrixArray[0]);
    }

    Tmp::MatrixArray[1].copyFrom(Tmp::MatrixArray[5]);
    Tmp::MatrixArray[1].multiplyToRef(Tmp::MatrixArray[0], Tmp::MatrixArray[5]);
  }

  // Local world
  Tmp::MatrixArray[5].multiplyToRef(Tmp::MatrixArray[2], _localWorld);

  // Parent
  if (parent() && parent()->getWorldMatrix()) {
    if (billboardMode != AbstractMesh::BILLBOARDMODE_NONE) {
      if (_meshToBoneReferal) {
        parent()->getWorldMatrix()->multiplyToRef(
          *_meshToBoneReferal->getWorldMatrix(), Tmp::MatrixArray[6]);
        Tmp::MatrixArray[5].copyFrom(Tmp::MatrixArray[6]);
      }
      else {
        Tmp::MatrixArray[5].copyFrom(*parent()->getWorldMatrix());
      }

      _localWorld.getTranslationToRef(Tmp::Vector3Array[5]);
      Vector3::TransformCoordinatesToRef(
        Tmp::Vector3Array[5], Tmp::MatrixArray[5], Tmp::Vector3Array[5]);
      _worldMatrix->copyFrom(_localWorld);
      _worldMatrix->setTranslation(Tmp::Vector3Array[5]);
    }
    else {
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
    _markSyncedWithParent();
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
    _poseMatrix = std::make_unique<Matrix>(Matrix::Invert(*_worldMatrix));
  }

  return *_worldMatrix;
}

AbstractMesh& AbstractMesh::registerAfterWorldMatrixUpdate(
  const std::function<void(AbstractMesh* mesh)>& func)
{
  onAfterWorldMatrixUpdateObservable.add(func);
  return *this;
}

AbstractMesh& AbstractMesh::unregisterAfterWorldMatrixUpdate(
  const std::function<void(AbstractMesh* mesh)>& func)
{
  onAfterWorldMatrixUpdateObservable.removeCallback(func);
  return *this;
}

AbstractMesh& AbstractMesh::setPositionWithLocalVector(const Vector3& vector3)
{
  computeWorldMatrix();

  _position = Vector3(Vector3::TransformNormal(vector3, _localWorld));

  return *this;
}

Vector3 AbstractMesh::getPositionExpressedInLocalSpace()
{
  computeWorldMatrix();
  auto invLocalWorldMatrix = _localWorld;
  invLocalWorldMatrix.invert();

  return Vector3::TransformNormal(_position, invLocalWorldMatrix);
}

AbstractMesh& AbstractMesh::locallyTranslate(const Vector3& vector3)
{
  computeWorldMatrix(true);

  _position = Vector3(Vector3::TransformCoordinates(vector3, _localWorld));

  return *this;
}

AbstractMesh& AbstractMesh::lookAt(const Vector3& targetPoint, float yawCor,
                                   float pitchCor, float rollCor, Space space)
{
  Vector3& dv = AbstractMesh::_lookAtVectorCache;
  Vector3 pos = (space == Space::LOCAL) ? _position : *getAbsolutePosition();
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
  return *this;
}

AbstractMesh& AbstractMesh::attachToBone(Bone* bone, AbstractMesh* affectedMesh)
{
  _meshToBoneReferal = affectedMesh;
  Node::setParent(bone);

  if (bone->getWorldMatrix()->determinant() < 0.f) {
    scalingDeterminant *= -1.f;
  }

  return *this;
}

AbstractMesh& AbstractMesh::detachFromBone()
{
  if (parent()->getWorldMatrix()->determinant() < 0.f) {
    scalingDeterminant *= -1.f;
  }

  _meshToBoneReferal = nullptr;
  Node::setParent(nullptr);

  return *this;
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
  return physicsImpostor.get();
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

AbstractMesh& AbstractMesh::applyImpulse(const Vector3& force,
                                         const Vector3& contactPoint)
{
  if (!physicsImpostor) {
    return *this;
  }

  physicsImpostor->applyImpulse(force, contactPoint);

  return *this;
}

AbstractMesh& AbstractMesh::setPhysicsLinkWith(Mesh* otherMesh,
                                               const Vector3& /*pivot1*/,
                                               const Vector3& /*pivot2*/,
                                               const PhysicsParams& /*options*/)
{
  if (!physicsImpostor || !otherMesh->physicsImpostor) {
    return *this;
  }

  return *this;
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

AbstractMesh& AbstractMesh::moveWithCollisions(const Vector3& /*velocity*/)
{
  auto globalPosition = getAbsolutePosition();

  globalPosition->subtractFromFloatsToRef(0.f, ellipsoid.y, 0.f,
                                          _oldPositionForCollisions);
  _oldPositionForCollisions.addInPlace(ellipsoidOffset);

  if (!_collider) {
    _collider = std::make_unique<Collider>();
  }

  _collider->radius = ellipsoid;

  // getScene()->collisionCoordinator->getNewPosition(
  //  _oldPositionForCollisions, velocity, _collider.get(), 3, this,
  //  _onCollisionPositionChange(), uniqueId);

  return *this;
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
  AbstractMesh::createOrUpdateSubmeshesOctree(size_t /*maxCapacity*/,
                                              size_t /*maxDepth*/)
{
  // if (!_submeshesOctree) {
  //  _submeshesOctree = new Octree<SubMesh*>(
  //    [](SubMesh* entry, OctreeBlock<SubMesh*>& block) {
  //      Octree<AbstractMesh*>::CreationFuncForSubMeshes(entry, block);
  //    },
  //    maxCapacity, maxDepth);
  //}

  computeWorldMatrix(true);

  // Update octree
  // BoundingBox& bbox = getBoundingInfo()->boundingBox;
  //_submeshesOctree->update(bbox.minimumWorld, bbox.maximumWorld, subMeshes);

  return _submeshesOctree;
}

AbstractMesh&
AbstractMesh::_collideForSubMesh(SubMesh* /*subMesh*/,
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
  return *this;
}

AbstractMesh&
AbstractMesh::_processCollisionsForSubMeshes(Collider* /*collider*/,
                                             const Matrix& /*transformMatrix*/)
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
  return *this;
}

AbstractMesh& AbstractMesh::_checkCollision(Collider* /*collider*/)
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
  return *this;
}

bool AbstractMesh::_generatePointsArray()
{
  return false;
}

PickingInfo AbstractMesh::intersects(const Ray& /*ray*/, bool /*fastCheck*/)
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

  return PickingInfo();
}

AbstractMesh* AbstractMesh::clone(const std::string& /*name*/,
                                  Node* /*newParent*/,
                                  bool /*doNotCloneChildren*/)
{
  return nullptr;
}

AbstractMesh& AbstractMesh::releaseSubMeshes()
{
  if (!subMeshes.empty()) {
    for (auto& subMesh : subMeshes) {
      subMesh->dispose();
    }
  }

  subMeshes.clear();

  return *this;
}

void AbstractMesh::dispose(bool doNotRecurse)
{
  // Action manager
  if (actionManager) {
    actionManager->dispose();
    actionManager = nullptr;
  }

  // Skeleton
  auto skeletonTmp = skeleton();
  if (skeletonTmp) {
    skeletonTmp = nullptr;
  }

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
    std::remove(light->includedOnlyMeshes().begin(),
                light->includedOnlyMeshes().end(), this);

    // Excluded meshes
    std::remove(light->excludedMeshes().begin(), light->excludedMeshes().end(),
                this);

    // Shadow generators
    auto generator = light->getShadowGenerator();
    if (generator) {
      auto shadowMap = generator->getShadowMap();
      std::remove(shadowMap->renderList.begin(), shadowMap->renderList.end(),
                  this);
    }
  }

  // Edges
  if (_edgesRenderer) {
    _edgesRenderer->dispose();
    _edgesRenderer.reset(nullptr);
  }

  // SubMeshes
  if (type() != IReflect::Type::INSTANCEDMESH) {
    releaseSubMeshes();
  }

  // Octree
  auto sceneOctree = getScene()->selectionOctree();
  if (sceneOctree) {
    sceneOctree->dynamicContent.erase(
      std::remove(sceneOctree->dynamicContent.begin(),
                  sceneOctree->dynamicContent.end(), this),
      sceneOctree->dynamicContent.end());
  }

  // Engine
  getScene()->getEngine()->wipeCaches();

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

  // facet data
  if (_facetDataEnabled) {
    disableFacetData();
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

AbstractMesh& AbstractMesh::getDirectionToRef(const Vector3& localAxis,
                                              Vector3& result)
{
  Vector3::TransformNormalToRef(localAxis, *getWorldMatrix(), result);
  return *this;
}

AbstractMesh& AbstractMesh::setPivotPoint(const Vector3& point,
                                          const Space& space)
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

  Vector3::TransformCoordinatesToRef(_point, *wm, _position);

  _pivotMatrix.m[12] = -_point.x;
  _pivotMatrix.m[13] = -_point.y;
  _pivotMatrix.m[14] = -_point.z;

  _cache.pivotMatrixUpdated = true;

  return *this;
}

Vector3 AbstractMesh::getPivotPoint() const
{
  auto point = Vector3::Zero();

  getPivotPointToRef(point);

  return point;
}

const AbstractMesh& AbstractMesh::getPivotPointToRef(Vector3& result) const
{
  result.x = -_pivotMatrix.m[12];
  result.y = -_pivotMatrix.m[13];
  result.z = -_pivotMatrix.m[14];

  return *this;
}

Vector3 AbstractMesh::getAbsolutePivotPoint()
{
  auto point = Vector3::Zero();

  getAbsolutePivotPointToRef(point);

  return point;
}

AbstractMesh& AbstractMesh::setParent(AbstractMesh* mesh)
{
  auto child  = this;
  auto parent = mesh;

  if (mesh == nullptr) {
    auto& rotation = Tmp::QuaternionArray[0];
    auto& position = Tmp::Vector3Array[0];
    auto& scale    = Tmp::Vector3Array[1];

    child->getWorldMatrix()->decompose(scale, rotation, position);

    if (child->_rotationQuaternionSet) {
      child->rotationQuaternion().copyFrom(rotation);
    }
    else {
      rotation.toEulerAnglesToRef(child->rotation());
    }

    child->position().x = position.x;
    child->position().y = position.y;
    child->position().z = position.z;
  }
  else {
    auto& rotation = Tmp::QuaternionArray[0];
    auto& position = Tmp::Vector3Array[0];
    auto& scale    = Tmp::Vector3Array[1];
    auto& m1       = Tmp::MatrixArray[0];
    auto& m2       = Tmp::MatrixArray[1];

    parent->getWorldMatrix()->decompose(scale, rotation, position);

    rotation.toRotationMatrix(m1);
    m2.setTranslation(position);

    m2.multiplyToRef(m1, m1);

    auto invParentMatrix = Matrix::Invert(m1);

    auto m = child->getWorldMatrix()->multiply(invParentMatrix);
    m.decompose(scale, rotation, position);

    if (child->_rotationQuaternionSet) {
      child->rotationQuaternion().copyFrom(rotation);
    }
    else {
      rotation.toEulerAnglesToRef(child->rotation());
    }

    invParentMatrix = Matrix::Invert(*parent->getWorldMatrix());

    m = child->getWorldMatrix()->multiply(invParentMatrix);
    m.decompose(scale, rotation, position);

    child->position().x = position.x;
    child->position().y = position.y;
    child->position().z = position.z;
  }
  static_cast<Node*>(child)->setParent(parent);
  return *this;
}

AbstractMesh& AbstractMesh::addChild(AbstractMesh* mesh)
{
  mesh->setParent(this);
  return *this;
}

AbstractMesh& AbstractMesh::removeChild(AbstractMesh* mesh)
{
  mesh->setParent(nullptr);
  return *this;
}

AbstractMesh& AbstractMesh::getAbsolutePivotPointToRef(Vector3& result)
{
  result.x = _pivotMatrix.m[12];
  result.y = _pivotMatrix.m[13];
  result.z = _pivotMatrix.m[14];

  getPivotPointToRef(result);

  Vector3::TransformCoordinatesToRef(result, *getWorldMatrix(), result);

  return *this;
}

AbstractMesh& AbstractMesh::_initFacetData()
{
  _facetNb = getIndices().size() / 3;

  _facetNormals.resize(_facetNb);
  std::fill(_facetNormals.begin(), _facetNormals.end(), Vector3::Zero());

  _facetPositions.resize(_facetNb);
  std::fill(_facetPositions.begin(), _facetPositions.end(), Vector3::Zero());

  _facetDataEnabled = true;

  return *this;
}

AbstractMesh& AbstractMesh::updateFacetData()
{
  if (!_facetDataEnabled) {
    _initFacetData();
  }
  auto positions = getVerticesData(VertexBuffer::PositionKind);
  auto indices   = getIndices();
  auto normals   = getVerticesData(VertexBuffer::NormalKind);
  auto bInfo     = getBoundingInfo();
  _bbSize.x      = (bInfo->maximum.x - bInfo->minimum.x > MathTools::Epsilon) ?
                bInfo->maximum.x - bInfo->minimum.x :
                MathTools::Epsilon;
  _bbSize.y = (bInfo->maximum.y - bInfo->minimum.y > MathTools::Epsilon) ?
                bInfo->maximum.y - bInfo->minimum.y :
                MathTools::Epsilon;
  _bbSize.z = (bInfo->maximum.z - bInfo->minimum.z > MathTools::Epsilon) ?
                bInfo->maximum.z - bInfo->minimum.z :
                MathTools::Epsilon;
  auto bbSizeMax = (_bbSize.x > _bbSize.y) ? _bbSize.x : _bbSize.y;
  bbSizeMax      = (bbSizeMax > _bbSize.z) ? bbSizeMax : _bbSize.z;
  _subDiv.max    = _partitioningSubdivisions;
  // adjust the number of subdivisions per axis
  _subDiv.X
    = static_cast<unsigned>(std::floor(_subDiv.max * _bbSize.x / bbSizeMax));
  // according to each bbox size per axis
  _subDiv.Y
    = static_cast<unsigned>(std::floor(_subDiv.max * _bbSize.y / bbSizeMax));
  _subDiv.Z
    = static_cast<unsigned>(std::floor(_subDiv.max * _bbSize.z / bbSizeMax));
  _subDiv.X = _subDiv.X < 1 ? 1 : _subDiv.X; // at least one subdivision
  _subDiv.Y = _subDiv.Y < 1 ? 1 : _subDiv.Y;
  _subDiv.Z = _subDiv.Z < 1 ? 1 : _subDiv.Z;
  // set the parameters for ComputeNormals()
  _facetParameters.facetNormals      = getFacetLocalNormals();
  _facetParameters.facetPositions    = getFacetLocalPositions();
  _facetParameters.facetPartitioning = getFacetLocalPartitioning();
  _facetParameters.bInfo             = *bInfo;
  _facetParameters.bbSize            = _bbSize;
  _facetParameters.subDiv            = _subDiv;
  _facetParameters.ratio             = partitioningBBoxRatio();
  // VertexData::ComputeNormals(positions, indices, normals, _facetParameters);
  return *this;
}

std::vector<Vector3>& AbstractMesh::getFacetLocalNormals()
{
  if (_facetNormals.empty()) {
    updateFacetData();
  }
  return _facetNormals;
}

std::vector<Vector3>& AbstractMesh::getFacetLocalPositions()
{
  if (_facetPositions.empty()) {
    updateFacetData();
  }
  return _facetPositions;
}

std::vector<Uint32Array>& AbstractMesh::getFacetLocalPartitioning()
{
  if (_facetPartitioning.empty()) {
    updateFacetData();
  }
  return _facetPartitioning;
}

Vector3 AbstractMesh::getFacetPosition(unsigned int i)
{
  auto pos = Vector3::Zero();
  getFacetPositionToRef(i, pos);
  return pos;
}

AbstractMesh& AbstractMesh::getFacetPositionToRef(unsigned int i, Vector3& ref)
{
  auto localPos = (getFacetLocalPositions())[i];
  auto world    = getWorldMatrix();
  Vector3::TransformCoordinatesToRef(localPos, *world, ref);
  return *this;
}

Vector3 AbstractMesh::getFacetNormal(unsigned int i)
{
  auto norm = Vector3::Zero();
  getFacetNormalToRef(i, norm);
  return norm;
}

AbstractMesh& AbstractMesh::getFacetNormalToRef(unsigned int i, Vector3& ref)
{
  auto localNorm = (getFacetLocalNormals())[i];
  Vector3::TransformNormalToRef(localNorm, *getWorldMatrix(), ref);
  return *this;
}

Uint32Array AbstractMesh::getFacetsAtLocalCoordinates(float x, float y, float z)
{
  auto bInfo = getBoundingInfo();
  int ox     = static_cast<int>(
    std::floor((x - bInfo->minimum.x * _partitioningBBoxRatio) * _subDiv.X
               * _partitioningBBoxRatio / _bbSize.x));
  int oy = static_cast<int>(
    std::floor((y - bInfo->minimum.y * _partitioningBBoxRatio) * _subDiv.Y
               * _partitioningBBoxRatio / _bbSize.y));
  int oz = static_cast<int>(
    std::floor((z - bInfo->minimum.z * _partitioningBBoxRatio) * _subDiv.Z
               * _partitioningBBoxRatio / _bbSize.z));

  if (ox < 0 || oy < 0 || oz < 0) {
    return Uint32Array();
  }

  unsigned int _ox = static_cast<unsigned>(ox);
  unsigned int _oy = static_cast<unsigned>(oy);
  unsigned int _oz = static_cast<unsigned>(oz);
  if (_ox > _subDiv.max || _oy > _subDiv.max || _oz > _subDiv.max) {
    return Uint32Array();
  }

  return _facetPartitioning[_ox + _subDiv.max * _oy
                            + _subDiv.max * _subDiv.max * _oz];
}

int AbstractMesh::getClosestFacetAtCoordinates(float x, float y, float z,
                                               Vector3& projected,
                                               bool projectedSet,
                                               bool checkFace, bool facing)
{
  auto world   = getWorldMatrix();
  auto& invMat = Tmp::MatrixArray[5];
  world->invertToRef(invMat);
  auto& invVect = Tmp::Vector3Array[8];
  int closest   = -1;
  // transform (x,y,z) to coordinates in the mesh local space
  Vector3::TransformCoordinatesFromFloatsToRef(x, y, z, invMat, invVect);
  closest = getClosestFacetAtLocalCoordinates(invVect.x, invVect.y, invVect.z,
                                              projected, checkFace, facing);
  if (projectedSet) {
    // tranform the local computed projected vector to world coordinates
    Vector3::TransformCoordinatesFromFloatsToRef(
      projected.x, projected.y, projected.z, *world, projected);
  }
  return closest;
}

int AbstractMesh::getClosestFacetAtLocalCoordinates(float x, float y, float z,
                                                    Vector3& projected,
                                                    bool projectedSet,
                                                    bool checkFace, bool facing)
{
  int closest = -1;
  float tmpx  = 0.f;
  float tmpy  = 0.f;
  float tmpz  = 0.f;
  float d     = 0.f; // tmp dot facet normal * facet position
  float t0    = 0.f;
  float projx = 0.f;
  float projy = 0.f;
  float projz = 0.f;
  // Get all the facets in the same partitioning block than (x, y, z)
  auto facetPositions = getFacetLocalPositions();
  auto facetNormals   = getFacetLocalNormals();
  auto facetsInBlock  = getFacetsAtLocalCoordinates(x, y, z);
  if (facetsInBlock.empty()) {
    return closest;
  }
  // Get the closest facet to (x, y, z)
  float shortest    = std::numeric_limits<float>::max(); // init distance vars
  float tmpDistance = shortest;
  size_t fib;   // current facet in the block
  Vector3 norm; // current facet normal
  Vector3 p0;   // current facet barycenter position
  // loop on all the facets in the current partitioning block
  for (size_t idx = 0; idx < facetsInBlock.size(); ++idx) {
    fib  = facetsInBlock[idx];
    norm = facetNormals[fib];
    p0   = facetPositions[fib];

    d = (x - p0.x) * norm.x + (y - p0.y) * norm.y + (z - p0.z) * norm.z;
    if (!checkFace || (checkFace && facing && d >= 0.f)
        || (checkFace && !facing && d <= 0.f)) {
      // compute (x,y,z) projection on the facet = (projx, projy, projz)
      d  = norm.x * p0.x + norm.y * p0.y + norm.z * p0.z;
      t0 = -(norm.x * x + norm.y * y + norm.z * z - d)
           / (norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);
      projx = x + norm.x * t0;
      projy = y + norm.y * t0;
      projz = z + norm.z * t0;

      tmpx = projx - x;
      tmpy = projy - y;
      tmpz = projz - z;
      // compute length between (x, y, z) and its projection on the facet
      tmpDistance = tmpx * tmpx + tmpy * tmpy + tmpz * tmpz;
      // just keep the closest facet to (x, y, z)
      if (tmpDistance < shortest) {
        shortest = tmpDistance;
        closest  = static_cast<int>(fib);
        if (projectedSet) {
          projected.x = projx;
          projected.y = projy;
          projected.z = projz;
        }
      }
    }
  }
  return closest;
}

FacetParameters& AbstractMesh::getFacetDataParameters()
{
  return _facetParameters;
}

AbstractMesh& AbstractMesh::disableFacetData()
{
  if (_facetDataEnabled) {
    _facetDataEnabled = false;
    _facetPositions.clear();
    _facetNormals.clear();
    _facetPartitioning.clear();
    _facetParameters = FacetParameters();
  }
  return *this;
}

void AbstractMesh::createNormals(bool updatable)
{
  auto positions = getVerticesData(VertexBuffer::PositionKind);
  auto indices   = getIndices();
  Float32Array normals;

  if (isVerticesDataPresent(VertexBuffer::NormalKind)) {
    normals = getVerticesData(VertexBuffer::NormalKind);
  }

  FacetParameters options;
  options.useRightHandedSystem = getScene()->useRightHandedSystem();
  VertexData::ComputeNormals(positions, indices, normals, options);
  setVerticesData(VertexBuffer::NormalKind, normals, updatable);
}

} // end of namespace BABYLON
