#include <babylon/meshes/abstract_mesh.h>

#include <babylon/actions/action_manager.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/collisions/intersection_info.h>
#include <babylon/collisions/picking_info.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/octrees/octree_scene_component.h>
#include <babylon/culling/ray.h>
#include <babylon/engines/_occlusion_data_storage.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/frustum.h>
#include <babylon/math/tmp.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/misc/tools.h>
#include <babylon/particles/particle_system.h>
#include <babylon/particles/solid_particle.h>
#include <babylon/physics/joint/physics_joint.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/rendering/bounding_box_renderer.h>
#include <babylon/rendering/edges_renderer.h>

namespace BABYLON {

Vector3 AbstractMesh::_lookAtVectorCache = Vector3(0.f, 0.f, 0.f);

AbstractMesh::AbstractMesh(const std::string& iName, Scene* scene)
    : TransformNode(iName, scene, false)
    , cullingStrategy{AbstractMesh::CULLINGSTRATEGY_STANDARD}
    , facetNb{this, &AbstractMesh::get_facetNb}
    , partitioningSubdivisions{this,
                               &AbstractMesh::get_partitioningSubdivisions,
                               &AbstractMesh::set_partitioningSubdivisions}
    , partitioningBBoxRatio{this, &AbstractMesh::get_partitioningBBoxRatio,
                            &AbstractMesh::set_partitioningBBoxRatio}
    , mustDepthSortFacets{this, &AbstractMesh::get_mustDepthSortFacets,
                          &AbstractMesh::set_mustDepthSortFacets}
    , facetDepthSortFrom{this, &AbstractMesh::get_facetDepthSortFrom,
                         &AbstractMesh::set_facetDepthSortFrom}
    , isFacetDataEnabled{this, &AbstractMesh::get_isFacetDataEnabled}
    , onCollide{this, &AbstractMesh::set_onCollide}
    , onCollisionPositionChange{this,
                                &AbstractMesh::set_onCollisionPositionChange}
    , definedFacingForward{true} // orientation for POV movement & rotation
    , _occlusionQuery{nullptr}
    , _renderingGroup{nullptr}
    , _occlusionDataStorage{this, &AbstractMesh::get__occlusionDataStorage}
    , occlusionRetryCount{this, &AbstractMesh::get_occlusionRetryCount,
                          &AbstractMesh::set_occlusionRetryCount}
    , occlusionType{this, &AbstractMesh::get_occlusionType,
                    &AbstractMesh::set_occlusionType}
    , occlusionQueryAlgorithmType{this,
                                  &AbstractMesh::
                                    get_occlusionQueryAlgorithmType,
                                  &AbstractMesh::
                                    set_occlusionQueryAlgorithmType}
    , isOccluded{this, &AbstractMesh::get_isOccluded,
                 &AbstractMesh::set_isOccluded}
    , isOcclusionQueryInProgress{this,
                                 &AbstractMesh::get_isOcclusionQueryInProgress}
    , visibility{this, &AbstractMesh::get_visibility,
                 &AbstractMesh::set_visibility}
    , alphaIndex{std::numeric_limits<int>::max()}
    , isVisible{true}
    , isPickable{true}
    , showBoundingBox{this, &AbstractMesh::get_showBoundingBox,
                      &AbstractMesh::set_showBoundingBox}
    , showSubMeshesBoundingBox{false}
    , isBlocker{false}
    , enablePointerMoveEvents{false}
    , renderingGroupId{this, &AbstractMesh::get_renderingGroupId,
                       &AbstractMesh::set_renderingGroupId}
    , material{this, &AbstractMesh::get_material, &AbstractMesh::set_material}
    , _savedMaterial{nullptr}
    , receiveShadows{this, &AbstractMesh::get_receiveShadows,
                     &AbstractMesh::set_receiveShadows}
    , renderOutline{this, &AbstractMesh::get_renderOutline,
                    &AbstractMesh::set_renderOutline}
    , outlineColor{Color3::Red()}
    , outlineWidth{0.02f}
    , renderOverlay{this, &AbstractMesh::get_renderOverlay,
                    &AbstractMesh::set_renderOverlay}
    , overlayColor{Color3::Red()}
    , overlayAlpha{0.5f}
    , hasVertexAlpha{this, &AbstractMesh::get_hasVertexAlpha,
                     &AbstractMesh::set_hasVertexAlpha}
    , useVertexColors{this, &AbstractMesh::get_useVertexColors,
                      &AbstractMesh::set_useVertexColors}
    , computeBonesUsingShaders{this,
                               &AbstractMesh::get_computeBonesUsingShaders,
                               &AbstractMesh::set_computeBonesUsingShaders}
    , numBoneInfluencers{this, &AbstractMesh::get_numBoneInfluencers,
                         &AbstractMesh::set_numBoneInfluencers}
    , applyFog{this, &AbstractMesh::get_applyFog, &AbstractMesh::set_applyFog}
    , useOctreeForRenderingSelection{true}
    , useOctreeForPicking{true}
    , useOctreeForCollisions{true}
    , layerMask{this, &AbstractMesh::get_layerMask,
                &AbstractMesh::set_layerMask}
    , alwaysSelectAsActiveMesh{false}
    , actionManager{nullptr}
    , physicsImpostor{this, &AbstractMesh::get_physicsImpostor,
                      &AbstractMesh::set_physicsImpostor}
    , ellipsoid{Vector3(0.5f, 1.f, 0.5f)}
    , ellipsoidOffset{Vector3(0, 0, 0)}
    , collisionMask{this, &AbstractMesh::get_collisionMask,
                    &AbstractMesh::set_collisionMask}
    , collisionGroup{this, &AbstractMesh::get_collisionGroup,
                     &AbstractMesh::set_collisionGroup}
    , edgesWidth{1.f}
    , edgesColor{Color4(1.f, 0.f, 0.f, 1.f)}
    , _edgesRenderer{nullptr}
    , _masterMesh{nullptr}
    , _materialDefines{nullptr}
    , _boundingInfo{nullptr}
    , _renderId{0}
    , _submeshesOctree{nullptr}
    , _unIndexed{false}
    , lightSources{this, &AbstractMesh::get_lightSources}
    , _waitingFreezeWorldMatrix{std::nullopt}
    , _positions{this, &AbstractMesh::get__positions}
    , skeleton{this, &AbstractMesh::get_skeleton, &AbstractMesh::set_skeleton}
    , edgesRenderer{this, &AbstractMesh::get_edgesRenderer}
    , isBlocked{this, &AbstractMesh::get_isBlocked}
    , useBones{this, &AbstractMesh::get_useBones}
    , isAnInstance{this, &AbstractMesh::get_isAnInstance}
    , checkCollisions{this, &AbstractMesh::get_checkCollisions,
                      &AbstractMesh::set_checkCollisions}
    , collider{this, &AbstractMesh::get_collider}
    , _onCollideObserver{nullptr}
    , _onCollisionPositionChangeObserver{nullptr}
    , _visibility{1.f}
    , _renderingGroupId{0}
    , _material{nullptr}
    , _receiveShadows{false}
    , _renderOutline{false}
    , _renderOverlay{false}
    , _hasVertexAlpha{false}
    , _useVertexColors{true}
    , _computeBonesUsingShaders{true}
    , _numBoneInfluencers{4}
    , _applyFog{true}
    , _layerMask{0x0FFFFFFF}
    , _checkCollisions{false}
    , _collisionMask{-1}
    , _collisionGroup{-1}
    , _collider{nullptr}
    , _oldPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _diffPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _physicsImpostor{nullptr}
    , _disposePhysicsObserver{nullptr}
    , __occlusionDataStorage{nullptr}
    , _collisionsTransformMatrix{Matrix::Zero()}
    , _collisionsScalingMatrix{Matrix::Zero()}
    , _skeleton{nullptr}
    , _showBoundingBox{false}
{
  _resyncLightSources();
}

AbstractMesh::~AbstractMesh()
{
}

Type AbstractMesh::type() const
{
  return Type::ABSTRACTMESH;
}

void AbstractMesh::addToScene(const AbstractMeshPtr& newMesh)
{
  getScene()->addMesh(newMesh);
}

size_t AbstractMesh::get_facetNb() const
{
  return _facetData.facetNb;
}

unsigned int AbstractMesh::get_partitioningSubdivisions() const
{
  return _facetData.partitioningSubdivisions;
}

void AbstractMesh::set_partitioningSubdivisions(unsigned int nb)
{
  _facetData.partitioningSubdivisions = nb;
}

float AbstractMesh::get_partitioningBBoxRatio() const
{
  return _facetData.partitioningBBoxRatio;
}

void AbstractMesh::set_partitioningBBoxRatio(float ratio)
{
  _facetData.partitioningBBoxRatio = ratio;
}

bool AbstractMesh::get_mustDepthSortFacets() const
{
  return _facetData.facetDepthSort;
}

void AbstractMesh::set_mustDepthSortFacets(bool sort)
{
  _facetData.facetDepthSort = sort;
}

Vector3& AbstractMesh::get_facetDepthSortFrom()
{
  return *_facetData.facetDepthSortFrom;
}

void AbstractMesh::set_facetDepthSortFrom(const Vector3& location)
{
  _facetData.facetDepthSortFrom = location;
}

bool AbstractMesh::get_isFacetDataEnabled() const
{
  return _facetData.facetDataEnabled;
}

bool AbstractMesh::_updateNonUniformScalingState(bool value)
{
  if (!TransformNode::_updateNonUniformScalingState(value)) {
    return false;
  }
  _markSubMeshesAsMiscDirty();
  return true;
}

void AbstractMesh::set_onCollide(
  const std::function<void(AbstractMesh*, EventState&)>& callback)
{
  if (_onCollideObserver) {
    onCollideObservable.remove(_onCollideObserver);
  }
  _onCollideObserver = onCollideObservable.add(callback);
}

void AbstractMesh::set_onCollisionPositionChange(
  const std::function<void(Vector3*, EventState&)>& callback)
{
  if (_onCollisionPositionChangeObserver) {
    onCollisionPositionChangeObservable.remove(
      _onCollisionPositionChangeObserver);
  }
  _onCollisionPositionChangeObserver
    = onCollisionPositionChangeObservable.add(callback);
}

MaterialPtr& AbstractMesh::get_material()
{
  return _material;
}

void AbstractMesh::set_material(const MaterialPtr& value)
{
  if (_material == value) {
    return;
  }

  _material = value;

  if (onMaterialChangedObservable.hasObservers()) {
    onMaterialChangedObservable.notifyObservers(this);
  }

  if (subMeshes.empty()) {
    return;
  }

  _unBindEffect();
}

bool AbstractMesh::get_receiveShadows() const
{
  return _receiveShadows;
}

void AbstractMesh::set_receiveShadows(bool value)
{
  if (_receiveShadows == value) {
    return;
  }

  _receiveShadows = value;
  _markSubMeshesAsLightDirty();
}

bool AbstractMesh::get_hasVertexAlpha() const
{
  return _hasVertexAlpha;
}

void AbstractMesh::set_hasVertexAlpha(bool value)
{
  if (_hasVertexAlpha == value) {
    return;
  }

  _hasVertexAlpha = value;
  _markSubMeshesAsAttributesDirty();
  _markSubMeshesAsMiscDirty();
}

bool AbstractMesh::get_useVertexColors() const
{
  return _useVertexColors;
}

void AbstractMesh::set_useVertexColors(bool value)
{
  if (_useVertexColors == value) {
    return;
  }

  _useVertexColors = value;
  _markSubMeshesAsAttributesDirty();
}

bool AbstractMesh::get_computeBonesUsingShaders() const
{
  return _computeBonesUsingShaders;
}

void AbstractMesh::set_computeBonesUsingShaders(bool value)
{
  if (_computeBonesUsingShaders == value) {
    return;
  }

  _computeBonesUsingShaders = value;
  _markSubMeshesAsAttributesDirty();
}

unsigned int AbstractMesh::get_numBoneInfluencers() const
{
  return _numBoneInfluencers;
}

void AbstractMesh::set_numBoneInfluencers(unsigned int value)
{
  if (_numBoneInfluencers == value) {
    return;
  }

  _numBoneInfluencers = value;
  _markSubMeshesAsAttributesDirty();
}

bool AbstractMesh::get_applyFog() const
{
  return _applyFog;
}

void AbstractMesh::set_applyFog(bool value)
{
  if (_applyFog == value) {
    return;
  }

  _applyFog = value;
  _markSubMeshesAsMiscDirty();
}

unsigned int AbstractMesh::get_layerMask() const
{
  return _layerMask;
}

void AbstractMesh::set_layerMask(unsigned int value)
{
  if (value == _layerMask) {
    return;
  }

  _layerMask = value;
  _resyncLightSources();
}

bool AbstractMesh::get_isOcclusionQueryInProgress() const
{
  return _occlusionDataStorage()->isOcclusionQueryInProgress;
}

_OcclusionDataStoragePtr& AbstractMesh::get__occlusionDataStorage()
{
  if (!__occlusionDataStorage) {
    __occlusionDataStorage = std::make_shared<_OcclusionDataStorage>();
  }
  return __occlusionDataStorage;
}

bool AbstractMesh::get_isOccluded() const
{
  return _occlusionDataStorage()->isOccluded;
}

void AbstractMesh::set_isOccluded(bool value)
{
  _occlusionDataStorage()->isOccluded = value;
}

unsigned int AbstractMesh::get_occlusionQueryAlgorithmType() const
{
  return _occlusionDataStorage()->occlusionQueryAlgorithmType;
}

void AbstractMesh::set_occlusionQueryAlgorithmType(unsigned int value)
{
  _occlusionDataStorage()->occlusionQueryAlgorithmType = value;
}

unsigned int AbstractMesh::get_occlusionType() const
{
  return _occlusionDataStorage()->occlusionType;
}

void AbstractMesh::set_occlusionType(unsigned int value)
{
  _occlusionDataStorage()->occlusionType = value;
}

int AbstractMesh::get_occlusionRetryCount() const
{
  return _occlusionDataStorage()->occlusionRetryCount;
}

void AbstractMesh::set_occlusionRetryCount(int value)
{
  _occlusionDataStorage()->occlusionRetryCount = value;
}

float AbstractMesh::get_visibility() const
{
  return _visibility;
}

void AbstractMesh::set_visibility(float value)
{
  if (stl_util::almost_equal(_visibility, value)) {
    return;
  }

  _visibility = value;
  _markSubMeshesAsMiscDirty();
}

bool AbstractMesh::get_showBoundingBox() const
{
  return _showBoundingBox;
}

void AbstractMesh::set_showBoundingBox(bool value)
{
  _showBoundingBox = value;
  // Lazyly creates a BB renderer if needed.
  if (value) {
    getScene()->getBoundingBoxRenderer();
  }
}

int AbstractMesh::get_renderingGroupId() const
{
  return _renderingGroupId;
}

void AbstractMesh::set_renderingGroupId(int value)
{
  _renderingGroupId = value;
}

int AbstractMesh::get_collisionMask() const
{
  return _collisionMask;
}

void AbstractMesh::set_collisionMask(int mask)
{
  _collisionMask = !isNan(mask) ? mask : -1;
}

int AbstractMesh::get_collisionGroup() const
{
  return _collisionGroup;
}

void AbstractMesh::set_collisionGroup(int mask)
{
  _collisionGroup = !isNan(mask) ? mask : -1;
}

std::vector<LightPtr>& AbstractMesh::get_lightSources()
{
  return _lightSources;
}

const std::string AbstractMesh::getClassName() const
{
  return "AbstractMesh";
}

std::string AbstractMesh::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name << ", isInstance: ";
  oss << ((type() == Type::INSTANCEDMESH) ? "YES" : "NO");
  oss << ", # of submeshes: " << (subMeshes.size());
  if (_skeleton) {
    oss << ", skeleton: " << _skeleton->name;
  }
  if (fullDetails) {
    oss << ", billboard mode: ";
    const std::vector<std::string> billboardModes{
      "NONE", "X", "Y", "null", "Z", "null", "null", "ALL"};
    if (billboardMode < billboardModes.size()) {
      oss << billboardModes[billboardMode];
    }
    else {
      oss << "null";
    }
    oss << ", freeze wrld mat: "
        << (_isWorldMatrixFrozen
                || (_waitingFreezeWorldMatrix.has_value()
                    && *_waitingFreezeWorldMatrix) ?
              "YES" :
              "NO");
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
      _lightSources.emplace_back(light);
    }
  }

  _markSubMeshesAsLightDirty();
}

void AbstractMesh::_rebuild()
{
  onRebuildObservable.notifyObservers(this);

  if (_occlusionQuery) {
    _occlusionQuery = nullptr;
  }

  if (subMeshes.empty()) {
    return;
  }

  for (auto& subMesh : subMeshes) {
    subMesh->_rebuild();
  }
}

void AbstractMesh::_resyncLighSource(const LightPtr& light)
{
  bool isIn = light && light->isEnabled() && light->canAffectMesh(this);

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

void AbstractMesh::_unBindEffect()
{
  for (auto& subMesh : subMeshes) {
    subMesh->setEffect(nullptr);
  }
}

void AbstractMesh::_removeLightSource(const LightPtr& light)
{
  return _removeLightSource(light.get());
}

void AbstractMesh::_removeLightSource(Light* light)
{
  auto index = std::find_if(
    _lightSources.begin(), _lightSources.end(),
    [light](const LightPtr& _light) { return _light.get() == light; });

  if (index == _lightSources.end()) {
    return;
  }

  _lightSources.erase(index);

  _markSubMeshesAsLightDirty();
}

void AbstractMesh::_markSubMeshesAsDirty(
  const std::function<void(MaterialDefines& defines)>& func)
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
  const auto func
    = [](MaterialDefines& defines) { defines.markAsLightDirty(); };
  _markSubMeshesAsDirty(func);
}

void AbstractMesh::_markSubMeshesAsAttributesDirty()
{
  const auto func
    = [](MaterialDefines& defines) { defines.markAsAttributesDirty(); };
  _markSubMeshesAsDirty(func);
}

void AbstractMesh::_markSubMeshesAsMiscDirty()
{
  if (subMeshes.empty()) {
    return;
  }

  for (auto& subMesh : subMeshes) {
    auto iMaterial = subMesh->getMaterial();
    if (iMaterial) {
      iMaterial->markAsDirty(Material::MiscDirtyFlag);
    }
  }
}

Scene* AbstractMesh::getScene() const
{
  return Node::getScene();
}

std::vector<Vector3>& AbstractMesh::get__positions()
{
  return _emptyPositions;
}

void AbstractMesh::set_skeleton(const SkeletonPtr& value)
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

SkeletonPtr& AbstractMesh::get_skeleton()
{
  return _skeleton;
}

Vector3& AbstractMesh::get_scaling()
{
  return _scaling;
}

void AbstractMesh::set_scaling(const Vector3& newScaling)
{
  _scaling = newScaling;
  if (_physicsImpostor) {
    _physicsImpostor->forceUpdate();
  }
}

AbstractMesh* AbstractMesh::getParent()
{
  if (parent()->type() == Type::ABSTRACTMESH) {
    return dynamic_cast<AbstractMesh*>(parent());
  }

  return nullptr;
}

MaterialPtr AbstractMesh::getMaterial()
{
  return material();
}

AbstractMesh& AbstractMesh::disableEdgesRendering()
{
  if (_edgesRenderer) {
    _edgesRenderer->dispose();
    _edgesRenderer = nullptr;
  }
  return *this;
}

AbstractMesh&
AbstractMesh::enableEdgesRendering(float epsilon,
                                   bool checkVerticesInsteadOfIndices)
{
  disableEdgesRendering();

  _edgesRenderer = std::make_unique<EdgesRenderer>(
    shared_from_base<AbstractMesh>(), epsilon, checkVerticesInsteadOfIndices);

  return *this;
}

IEdgesRendererPtr& AbstractMesh::get_edgesRenderer()
{
  return _edgesRenderer;
}

bool AbstractMesh::get_isBlocked() const
{
  return false;
}

AbstractMesh* AbstractMesh::getLOD(const CameraPtr& /*camera*/,
                                   BoundingSphere* /*boundingSphere*/)
{
  return this;
}

size_t AbstractMesh::getTotalVertices() const
{
  return 0;
}

Uint32Array AbstractMesh::getIndices(bool /*copyWhenShared*/,
                                     bool /*forceCopy*/)
{
  return Uint32Array();
}

Float32Array AbstractMesh::getVerticesData(const std::string& /*kind*/,
                                           bool /*copyWhenShared*/,
                                           bool /*forceCopy*/)
{
  return Float32Array();
}

AbstractMesh*
AbstractMesh::setVerticesData(const std::string& /*kind*/,
                              const Float32Array& /*data*/, bool /*updatable*/,
                              const std::optional<size_t>& /*stride*/)
{
  return this;
}

AbstractMesh* AbstractMesh::updateVerticesData(const std::string& /*kind*/,
                                               const Float32Array& /*data*/,
                                               bool /*updateExtends*/,
                                               bool /*makeItUnique*/)
{
  return this;
}

AbstractMesh* AbstractMesh::setIndices(const IndicesArray& /*indices*/,
                                       size_t /*totalVertices*/,
                                       bool /*updatable*/)
{
  return this;
}

bool AbstractMesh::isVerticesDataPresent(const std::string& /*kind*/) const
{
  return false;
}

BoundingInfoPtr& AbstractMesh::getBoundingInfo()
{
  if (_masterMesh) {
    return _masterMesh->getBoundingInfo();
  }

  if (!_boundingInfo) {
    // _boundingInfo is being created here
    _updateBoundingInfo();
  }
  // cannot be null.
  return _boundingInfo;
}

AbstractMesh& AbstractMesh::normalizeToUnitCube(bool includeDescendants)
{
  auto boundingVectors = getHierarchyBoundingVectors(includeDescendants);
  auto sizeVec         = boundingVectors.max.subtract(boundingVectors.min);
  auto maxDimension    = stl_util::max(sizeVec.x, sizeVec.y, sizeVec.z);

  if (maxDimension == 0.f) {
    return *this;
  }

  auto scale = 1.f / maxDimension;

  scaling().scaleInPlace(scale);

  return *this;
}

AbstractMesh& AbstractMesh::setBoundingInfo(const BoundingInfo& boundingInfo)
{
  _boundingInfo = std::make_unique<BoundingInfo>(boundingInfo);
  return *this;
}

bool AbstractMesh::get_useBones() const
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

bool AbstractMesh::_activate(int renderId, bool /*intermediateRendering*/)
{
  _renderId = renderId;
  return true;
}

Matrix& AbstractMesh::getWorldMatrix()
{
  if (_masterMesh) {
    return _masterMesh->getWorldMatrix();
  }

  return TransformNode::getWorldMatrix();
}

float AbstractMesh::_getWorldMatrixDeterminant()
{
  if (_masterMesh) {
    return _masterMesh->_getWorldMatrixDeterminant();
  }

  return TransformNode::_getWorldMatrixDeterminant();
}

bool AbstractMesh::get_isAnInstance() const
{
  return false;
}

AbstractMesh& AbstractMesh::movePOV(float amountRight, float amountUp,
                                    float amountForward)
{
  position().addInPlace(calcMovePOV(amountRight, amountUp, amountForward));
  return *this;
}

Vector3 AbstractMesh::calcMovePOV(float amountRight, float amountUp,
                                  float amountForward)
{
  Matrix rotMatrix;
  auto rotQuaternion = rotationQuaternion() ?
                         *rotationQuaternion() :
                         Quaternion::RotationYawPitchRoll(
                           rotation().y, rotation().x, rotation().z);
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

MinMax AbstractMesh::getHierarchyBoundingVectors(
  bool includeDescendants,
  const std::function<bool(AbstractMesh* abstractMesh)>& predicate)
{
  // Ensures that all world matrix will be recomputed.
  getScene()->incrementRenderId();

  computeWorldMatrix(true);

  Vector3 min;
  Vector3 max;
  const auto& boundingInfo = *getBoundingInfo();

  if (subMeshes.empty()) {
    min = Vector3(std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max());
    max = Vector3(std::numeric_limits<float>::lowest(),
                  std::numeric_limits<float>::lowest(),
                  std::numeric_limits<float>::lowest());
  }
  else {
    min = boundingInfo.boundingBox.minimumWorld;
    max = boundingInfo.boundingBox.maximumWorld;
  }

  if (includeDescendants) {
    auto descendants = getDescendants(false);

    for (auto& descendant : descendants) {
      auto childMesh = std::static_pointer_cast<class AbstractMesh>(descendant);

      childMesh->computeWorldMatrix(true);

      // Filters meshes based on custom predicate function.
      if (predicate && !predicate(childMesh.get())) {
        continue;
      }

      // make sure we have the needed params to get mix and max
      if (childMesh->getTotalVertices() == 0) {
        continue;
      }

      const auto& childBoundingInfo = *childMesh->getBoundingInfo();
      const auto& boundingBox       = childBoundingInfo.boundingBox;

      auto minBox = boundingBox.minimumWorld;
      auto maxBox = boundingBox.maximumWorld;

      Tools::CheckExtends(minBox, min, max);
      Tools::CheckExtends(maxBox, min, max);
    }
  }

  return {min, max};
}

AbstractMesh& AbstractMesh::refreshBoundingInfo(bool /*applySkeleton*/)
{
  return *this;
}

void AbstractMesh::_refreshBoundingInfo(const Float32Array& data,
                                        const std::optional<Vector2>& bias)
{
  if (!data.empty()) {
    auto extend = Tools::ExtractMinAndMax(data, 0, getTotalVertices(), bias);
    if (_boundingInfo) {
      _boundingInfo->reConstruct(extend.min, extend.max);
    }
    else {
      _boundingInfo = std::make_shared<BoundingInfo>(extend.min, extend.max);
    }
  }

  if (!subMeshes.empty()) {
    for (const auto& subMesh : subMeshes) {
      subMesh->refreshBoundingInfo(data);
    }
  }

  _updateBoundingInfo();
}

Float32Array AbstractMesh::_getPositionData(bool applySkeleton)
{
  auto data = getVerticesData(VertexBuffer::PositionKind);

  if (!data.empty() && applySkeleton && skeleton()) {
    auto matricesIndicesData
      = getVerticesData(VertexBuffer::MatricesIndicesKind);
    auto matricesWeightsData
      = getVerticesData(VertexBuffer::MatricesWeightsKind);
    if (!matricesWeightsData.empty() && !matricesIndicesData.empty()) {
      auto needExtras = numBoneInfluencers() > 4;
      auto matricesIndicesExtraData
        = needExtras ? getVerticesData(VertexBuffer::MatricesIndicesExtraKind) :
                       Float32Array();
      auto matricesWeightsExtraData
        = needExtras ? getVerticesData(VertexBuffer::MatricesWeightsExtraKind) :
                       Float32Array();

      auto skeletonMatrices = skeleton()->getTransformMatrices(this);

      auto& tempVector  = Tmp::Vector3Array[0];
      auto& finalMatrix = Tmp::MatrixArray[0];
      auto& tempMatrix  = Tmp::MatrixArray[1];

      unsigned int matWeightIdx = 0;
      for (unsigned int index = 0; index < data.size();
           index += 3, matWeightIdx += 4) {
        finalMatrix.reset();

        unsigned int inf = 0;
        float weight     = 0.f;
        for (inf = 0; inf < 4; inf++) {
          weight = matricesWeightsData[matWeightIdx + inf];
          if (weight > 0) {
            Matrix::FromFloat32ArrayToRefScaled(
              skeletonMatrices,
              static_cast<unsigned int>(
                std::floor(matricesIndicesData[matWeightIdx + inf] * 16)),
              weight, tempMatrix);
            finalMatrix.addToSelf(tempMatrix);
          }
        }
        if (needExtras) {
          for (inf = 0; inf < 4; inf++) {
            weight = matricesWeightsExtraData[matWeightIdx + inf];
            if (weight > 0) {
              Matrix::FromFloat32ArrayToRefScaled(
                skeletonMatrices,
                static_cast<unsigned int>(std::floor(
                  matricesIndicesExtraData[matWeightIdx + inf] * 16)),
                weight, tempMatrix);
              finalMatrix.addToSelf(tempMatrix);
            }
          }
        }

        Vector3::TransformCoordinatesFromFloatsToRef(
          data[index], data[index + 1], data[index + 2], finalMatrix,
          tempVector);
        tempVector.toArray(data, index);
      }
    }
  }

  return data;
}

AbstractMesh& AbstractMesh::_updateBoundingInfo()
{
  if (!_boundingInfo) {
    _boundingInfo
      = std::make_unique<BoundingInfo>(absolutePosition(), absolutePosition());
  }

  _boundingInfo->update(worldMatrixFromCache());
  _updateSubMeshesBoundingInfo(worldMatrixFromCache());

  return *this;
}

AbstractMesh& AbstractMesh::_updateSubMeshesBoundingInfo(const Matrix& matrix)
{
  if (subMeshes.empty()) {
    return *this;
  }

  auto count = subMeshes.size();
  for (size_t subIndex = 0; subIndex < count; ++subIndex) {
    auto& subMesh = subMeshes[subIndex];
    if (count > 1 || !subMesh->isGlobal()) {
      subMesh->updateBoundingInfo(matrix);
    }
  }

  return *this;
}

void AbstractMesh::_afterComputeWorldMatrix()
{
  // Bounding info
  _updateBoundingInfo();
}

AbstractMesh* AbstractMesh::_effectiveMesh()
{
  return (skeleton() && skeleton()->overrideMesh) ?
           skeleton()->overrideMesh.get() :
           this;
}

bool AbstractMesh::isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                               unsigned int /*strategy*/)
{
  return _boundingInfo != nullptr
         && _boundingInfo->isInFrustum(frustumPlanes, cullingStrategy);
}

bool AbstractMesh::isCompletelyInFrustum(
  const std::array<Plane, 6>& frustumPlanes)
{
  return _boundingInfo != nullptr
         && _boundingInfo->isCompletelyInFrustum(frustumPlanes);
}

bool AbstractMesh::intersectsMesh(AbstractMesh& mesh, bool precise,
                                  bool includeDescendants)
{
  if (!_boundingInfo || !mesh._boundingInfo) {
    return false;
  }

  if (_boundingInfo->intersects(*mesh._boundingInfo, precise)) {
    return true;
  }

  if (includeDescendants) {
    for (auto& child : getChildMeshes()) {
      if (child->intersectsMesh(mesh, precise, true)) {
        return true;
      }
    }
  }

  return false;
}

bool AbstractMesh::intersectsMesh(SolidParticle& sp, bool precise,
                                  bool includeDescendants)
{
  if (!_boundingInfo || !sp._boundingInfo) {
    return false;
  }

  if (_boundingInfo->intersects(*sp._boundingInfo, precise)) {
    return true;
  }

  if (includeDescendants) {
    for (auto& child : getChildMeshes()) {
      if (child->intersectsMesh(sp, precise, true)) {
        return true;
      }
    }
  }

  return false;
}

bool AbstractMesh::intersectsPoint(const Vector3& point)
{
  if (!_boundingInfo) {
    return false;
  }

  return _boundingInfo->intersectsPoint(point);
}

PhysicsImpostorPtr& AbstractMesh::getPhysicsImpostor()
{
  return physicsImpostor;
}

Vector3 AbstractMesh::getPositionInCameraSpace(CameraPtr camera)
{
  if (!camera) {
    camera = getScene()->activeCamera;
  }

  return Vector3::TransformCoordinates(absolutePosition(),
                                       camera->getViewMatrix());
}

float AbstractMesh::getDistanceToCamera(CameraPtr camera)
{
  if (!camera) {
    camera = getScene()->activeCamera;
  }

  return absolutePosition().subtract(camera->position).length();
}

AbstractMesh& AbstractMesh::applyImpulse(const Vector3& force,
                                         const Vector3& contactPoint)
{
  if (!_physicsImpostor) {
    return *this;
  }

  _physicsImpostor->applyImpulse(force, contactPoint);
  return *this;
}

AbstractMesh& AbstractMesh::setPhysicsLinkWith(Mesh* otherMesh,
                                               const Vector3& pivot1,
                                               const Vector3& pivot2,
                                               const PhysicsParams& options)
{
  if (!_physicsImpostor || !otherMesh->_physicsImpostor) {
    return *this;
  }

  PhysicsJointData jointData;
  jointData.mainPivot      = pivot1;
  jointData.connectedPivot = pivot2;
  jointData.nativeParams   = options;
  _physicsImpostor->createJoint(otherMesh->physicsImpostor(),
                                PhysicsJoint::HingeJoint, jointData);

  return *this;
}

bool AbstractMesh::get_checkCollisions() const
{
  return _checkCollisions;
}

void AbstractMesh::set_checkCollisions(bool collisionEnabled)
{
  _checkCollisions = collisionEnabled;
}

ColliderPtr& AbstractMesh::get_collider()
{
  return _collider;
}

bool AbstractMesh::get_renderOutline() const
{
  return _renderOutline;
}

PhysicsImpostorPtr& AbstractMesh::get_physicsImpostor()
{
  return _physicsImpostor;
}

void AbstractMesh::set_physicsImpostor(const PhysicsImpostorPtr& value)
{
  if (_physicsImpostor == value) {
    return;
  }
  if (_disposePhysicsObserver) {
    onDisposeObservable.remove(_disposePhysicsObserver);
  }

  _physicsImpostor = value;

  if (value) {
    _disposePhysicsObserver
      = onDisposeObservable.add([this](Node* /*node*/, EventState& /*es*/) {
          // Physics
          if (_physicsImpostor) {
            _physicsImpostor->dispose(/*!doNotRecurse*/);
            physicsImpostor = nullptr;
          }
        });
  }
}

void AbstractMesh::set_renderOutline(bool value)
{
  if (value) {
    // Lazy Load the component.
    getScene()->getOutlineRenderer();
  }
  _renderOutline = value;
}

bool AbstractMesh::get_renderOverlay() const
{
  return _renderOverlay;
}

void AbstractMesh::set_renderOverlay(bool value)
{
  if (value) {
    // Lazy Load the component.
    getScene()->getOutlineRenderer();
  }
  _renderOverlay = value;
}

AbstractMesh& AbstractMesh::moveWithCollisions(Vector3& displacement)
{
  auto globalPosition = getAbsolutePosition();

  globalPosition.addToRef(ellipsoidOffset, _oldPositionForCollisions);

  if (!_collider) {
    _collider = std::make_shared<Collider>();
  }

  _collider->_radius = ellipsoid;

  getScene()->collisionCoordinator()->getNewPosition(
    _oldPositionForCollisions, displacement, _collider, 3,
    shared_from_base<AbstractMesh>(),
    [this](int collisionId, Vector3& newPosition,
           const AbstractMeshPtr& collidedMesh) {
      _onCollisionPositionChange(collisionId, newPosition, collidedMesh);
    },
    static_cast<unsigned int>(uniqueId));

  return *this;
}

void AbstractMesh::_onCollisionPositionChange(
  int /*collisionId*/, Vector3& newPosition,
  const AbstractMeshPtr& collidedMesh)
{
  newPosition.subtractToRef(_oldPositionForCollisions,
                            _diffPositionForCollisions);

  if (_diffPositionForCollisions.length() > Engine::CollisionsEpsilon) {
    position().addInPlace(_diffPositionForCollisions);
  }

  if (collidedMesh) {
    onCollideObservable.notifyObservers(collidedMesh.get());
  }

  onCollisionPositionChangeObservable.notifyObservers(&position());
}

Octree<SubMesh*>*
AbstractMesh::createOrUpdateSubmeshesOctree(size_t maxCapacity, size_t maxDepth)
{
  auto scene     = getScene();
  auto component = scene->_getComponent(SceneComponentConstants::NAME_OCTREE);
  if (!component) {
    component = OctreeSceneComponent::New(scene);
    scene->_addComponent(component);
  }

  if (!_submeshesOctree) {
    _submeshesOctree = new Octree<SubMesh*>(
      [](SubMesh* entry, OctreeBlock<SubMesh*>& block) {
        Octree<AbstractMesh*>::CreationFuncForSubMeshes(entry, block);
      },
      maxCapacity, maxDepth);
  }

  computeWorldMatrix(true);

  const auto& boundingInfo = *getBoundingInfo();

  // Update octree
  auto& bbox          = boundingInfo.boundingBox;
  auto rawSubMeshPtrs = stl_util::to_raw_ptr_vector(subMeshes);
  _submeshesOctree->update(bbox.minimumWorld, bbox.maximumWorld,
                           rawSubMeshPtrs);

  return _submeshesOctree;
}

AbstractMesh& AbstractMesh::_collideForSubMesh(SubMesh* subMesh,
                                               const Matrix& transformMatrix,
                                               Collider& iCollider)
{
  _generatePointsArray();

  if (_positions().empty()) {
    return *this;
  }

  // Transformation
  if (subMesh->_lastColliderWorldVertices.empty()
      || !subMesh->_lastColliderTransformMatrix->equals(transformMatrix)) {
    subMesh->_lastColliderTransformMatrix
      = std::make_unique<Matrix>(transformMatrix);
    subMesh->_lastColliderWorldVertices = {};
    subMesh->_trianglePlanes            = {};
    auto start                          = subMesh->verticesStart;
    auto end = (subMesh->verticesStart + subMesh->verticesCount);
    for (unsigned int i = start; i < end; i++) {
      subMesh->_lastColliderWorldVertices.emplace_back(
        Vector3::TransformCoordinates(_positions()[i], transformMatrix));
    }
  }
  // Collide
  iCollider._collide(
    subMesh->_trianglePlanes, subMesh->_lastColliderWorldVertices, getIndices(),
    subMesh->indexStart, subMesh->indexStart + subMesh->indexCount,
    subMesh->verticesStart, subMesh->getMaterial() != nullptr);
  if (iCollider.collisionFound) {
    iCollider.collidedMesh = shared_from_base<AbstractMesh>();
  }
  return *this;
}

AbstractMesh&
AbstractMesh::_processCollisionsForSubMeshes(Collider& iCollider,
                                             const Matrix& transformMatrix)
{
  auto iSubMeshes = _scene->getCollidingSubMeshCandidates(this, iCollider);
  auto len        = iSubMeshes.size();

  for (size_t index = 0; index < len; index++) {
    auto& subMesh = iSubMeshes[index];

    // Bounding test
    if (len > 1 && !subMesh->_checkCollision(iCollider)) {
      continue;
    }

    _collideForSubMesh(subMesh, transformMatrix, iCollider);
  }
  return *this;
}

AbstractMesh& AbstractMesh::_checkCollision(Collider& iCollider)
{
  // Bounding box test
  if (!_boundingInfo->_checkCollision(iCollider)) {
    return *this;
  }

  // Transformation matrix
  auto& collisionsScalingMatrix   = Tmp::MatrixArray[0];
  auto& collisionsTransformMatrix = Tmp::MatrixArray[1];
  Matrix::ScalingToRef(1.f / iCollider._radius.x, 1.f / iCollider._radius.y,
                       1.f / iCollider._radius.z, collisionsScalingMatrix);
  worldMatrixFromCache().multiplyToRef(collisionsScalingMatrix,
                                       collisionsTransformMatrix);
  _processCollisionsForSubMeshes(iCollider, collisionsTransformMatrix);
  return *this;
}

bool AbstractMesh::_generatePointsArray()
{
  return false;
}

PickingInfo AbstractMesh::intersects(Ray& ray, bool fastCheck)
{
  PickingInfo pickingInfo;

  if (subMeshes.empty() || !_boundingInfo
      || !ray.intersectsSphere(_boundingInfo->boundingSphere)
      || !ray.intersectsBox(_boundingInfo->boundingBox)) {
    return pickingInfo;
  }

  if (!_generatePointsArray() || !_scene->getIntersectingSubMeshCandidates) {
    return pickingInfo;
  }

  std::optional<IntersectionInfo> intersectInfo = std::nullopt;

  // Octrees
  auto _subMeshes = _scene->getIntersectingSubMeshCandidates(this, ray);
  size_t len      = _subMeshes.size();

  for (size_t index = 0; index < len; ++index) {
    auto& subMesh = _subMeshes[index];

    // Bounding test
    if (len > 1 && !subMesh->canIntersects(ray)) {
      continue;
    }

    std::optional<IntersectionInfo> currentIntersectInfo
      = std::nullopt; // subMesh->intersects(ray, _positions(), getIndices(),
                      // fastCheck);

    if (currentIntersectInfo) {
      if (fastCheck || !intersectInfo
          || currentIntersectInfo->distance < intersectInfo->distance) {
        intersectInfo            = currentIntersectInfo;
        intersectInfo->subMeshId = index;

        if (fastCheck) {
          break;
        }
      }
    }
  }

  if (intersectInfo) {
    // Get picked point
    auto world          = getWorldMatrix();
    auto worldOrigin    = Vector3::TransformCoordinates(ray.origin, world);
    auto direction      = ray.direction;
    direction           = direction.scale(intersectInfo->distance);
    auto worldDirection = Vector3::TransformNormal(direction, world);

    auto pickedPoint = worldOrigin.add(worldDirection);

    // Return result
    pickingInfo.hit         = true;
    pickingInfo.distance    = Vector3::Distance(worldOrigin, pickedPoint);
    pickingInfo.pickedPoint = pickedPoint;
    pickingInfo.pickedMesh  = shared_from_base<AbstractMesh>();
    pickingInfo.bu          = intersectInfo->bu.value_or(0.f);
    pickingInfo.bv          = intersectInfo->bv.value_or(0.f);
    pickingInfo.faceId      = static_cast<unsigned>(intersectInfo->faceId);
    pickingInfo.subMeshId   = static_cast<unsigned>(intersectInfo->subMeshId);
    return pickingInfo;
  }

  return pickingInfo;
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

void AbstractMesh::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  // Smart Array Retainers.
  getScene()->freeActiveMeshes();
  getScene()->freeRenderingGroups();

  // Action manager
  if (actionManager) {
    actionManager->dispose();
    actionManager = nullptr;
  }

  // Skeleton
  _skeleton = nullptr;

  // Intersections in progress
  for (auto& other : _intersectionsInProgress) {
    other->_intersectionsInProgress.erase(
      std::remove(other->_intersectionsInProgress.begin(),
                  other->_intersectionsInProgress.end(), this),
      other->_intersectionsInProgress.end());
  }

  _intersectionsInProgress.clear();

  // Lights
  for (auto& light : getScene()->lights) {
    // Included meshes
    light->includedOnlyMeshes().erase(
      std::remove_if(light->includedOnlyMeshes().begin(),
                     light->includedOnlyMeshes().end(),
                     [this](const AbstractMeshPtr& includedOnlyMesh) {
                       return includedOnlyMesh.get() == this;
                     }),
      light->includedOnlyMeshes().end());

    // Excluded meshes
    light->excludedMeshes().erase(
      std::remove_if(light->excludedMeshes().begin(),
                     light->excludedMeshes().end(),
                     [this](const AbstractMeshPtr& excludedMesh) {
                       return excludedMesh.get() == this;
                     }),
      light->excludedMeshes().end());

    // Shadow generators
    auto generator = light->getShadowGenerator();
    if (generator) {
      auto shadowMap = generator->getShadowMap();
      if (shadowMap && !shadowMap->renderList().empty()) {
        shadowMap->renderList().erase(
          std::remove_if(
            shadowMap->renderList().begin(), shadowMap->renderList().end(),
            [this](const AbstractMesh* mesh) { return mesh == this; }),
          shadowMap->renderList().end());
      }
    }
  }

  // Edges
  if (_edgesRenderer) {
    _edgesRenderer->dispose();
    _edgesRenderer = nullptr;
  }

  // SubMeshes
  if (type() != Type::INSTANCEDMESH) {
    releaseSubMeshes();
  }

  // Query
  auto engine = getScene()->getEngine();
  if (_occlusionQuery) {
    _occlusionDataStorage()->isOcclusionQueryInProgress = false;
    engine->deleteQuery(_occlusionQuery);
    _occlusionQuery = nullptr;
  }

  // Engine
  engine->wipeCaches();

  if (disposeMaterialAndTextures) {
    if (material()) {
      material()->dispose(false, true);
    }
  }

  if (!doNotRecurse) {
    // Particles
    for (size_t index = 0; index < getScene()->particleSystems.size();
         ++index) {
      auto& emitter = getScene()->particleSystems[index]->emitter;
      if (std::holds_alternative<AbstractMeshPtr>(emitter)
          && (std::get<AbstractMeshPtr>(emitter).get() == this)) {
        getScene()->particleSystems[index]->dispose();
        --index;
      }
    }
  }

  // facet data
  if (_facetData.facetDataEnabled) {
    disableFacetData();
  }

  onAfterWorldMatrixUpdateObservable.clear();
  onCollideObservable.clear();
  onCollisionPositionChangeObservable.clear();
  onRebuildObservable.clear();

  _isDisposed = true;

  TransformNode::dispose(doNotRecurse, disposeMaterialAndTextures);

  // Remove from scene
  getScene()->removeMesh(this);
}

AbstractMesh& AbstractMesh::addChild(AbstractMesh& mesh)
{
  mesh.setParent(this);
  return *this;
}

AbstractMesh& AbstractMesh::removeChild(AbstractMesh& mesh)
{
  mesh.setParent(nullptr);
  return *this;
}

AbstractMesh& AbstractMesh::_initFacetData()
{
  auto& data   = _facetData;
  data.facetNb = getIndices().size() / 3;

  // default nb of partitioning subdivisions = 10
  data.partitioningSubdivisions
    = (data.partitioningSubdivisions) ? data.partitioningSubdivisions : 10;
  // default ratio 1.01 = the partitioning is 1% bigger than the bounding box
  data.partitioningBBoxRatio
    = (data.partitioningBBoxRatio != 0.f) ? data.partitioningBBoxRatio : 1.01f;

  data.facetNormals.resize(data.facetNb);
  std::fill(data.facetNormals.begin(), data.facetNormals.end(),
            Vector3::Zero());

  data.facetPositions.resize(data.facetNb);
  std::fill(data.facetPositions.begin(), data.facetPositions.end(),
            Vector3::Zero());

  data.facetDataEnabled = true;

  return *this;
}

AbstractMesh& AbstractMesh::updateFacetData()
{
  auto& data = _facetData;
  if (!data.facetDataEnabled) {
    _initFacetData();
  }
  auto positions    = getVerticesData(VertexBuffer::PositionKind);
  auto indices      = getIndices();
  auto normals      = getVerticesData(VertexBuffer::NormalKind);
  const auto& bInfo = *getBoundingInfo();

  if (data.facetDepthSort && !data.facetDepthSortEnabled) {
    // init arrays, matrix and sort function on first call
    data.facetDepthSortEnabled = true;
    // indices instanceof Uint32Array
    {
      data.depthSortedIndices = Uint32Array(indices);
    }

    data.facetDepthSortFunction
      = [](const DepthSortedFacet& f1, const DepthSortedFacet& f2) {
          const auto diff = f2.sqDistance - f1.sqDistance;
          return (diff < 0.f) ? -1 : (diff > 0.f) ? 1 : 0;
        };
    if (!data.facetDepthSortFrom) {
      auto& camera            = getScene()->activeCamera;
      data.facetDepthSortFrom = (camera) ? camera->position : Vector3::Zero();
    }
    data.depthSortedFacets.clear();
    for (unsigned int f = 0; f < data.facetNb; f++) {
      DepthSortedFacet depthSortedFacet;
      depthSortedFacet.ind        = f * 3;
      depthSortedFacet.sqDistance = 0.f;
      data.depthSortedFacets.emplace_back(depthSortedFacet);
    }
    data.invertedMatrix       = Matrix::Identity();
    data.facetDepthSortOrigin = Vector3::Zero();
  }

  data.bbSize.x = (bInfo.maximum().x - bInfo.minimum().x > Math::Epsilon) ?
                    bInfo.maximum().x - bInfo.minimum().x :
                    Math::Epsilon;
  data.bbSize.y = (bInfo.maximum().y - bInfo.minimum().y > Math::Epsilon) ?
                    bInfo.maximum().y - bInfo.minimum().y :
                    Math::Epsilon;
  data.bbSize.z = (bInfo.maximum().z - bInfo.minimum().z > Math::Epsilon) ?
                    bInfo.maximum().z - bInfo.minimum().z :
                    Math::Epsilon;
  auto bbSizeMax
    = (data.bbSize.x > data.bbSize.y) ? data.bbSize.x : data.bbSize.y;
  bbSizeMax       = (bbSizeMax > data.bbSize.z) ? bbSizeMax : data.bbSize.z;
  data.subDiv.max = data.partitioningSubdivisions;
  // adjust the number of subdivisions per axis
  data.subDiv.X = static_cast<unsigned>(
    std::floor(data.subDiv.max * data.bbSize.x / bbSizeMax));
  // according to each bbox size per axis
  data.subDiv.Y = static_cast<unsigned>(
    std::floor(data.subDiv.max * data.bbSize.y / bbSizeMax));
  // at least one subdivision
  data.subDiv.Z = static_cast<unsigned>(
    std::floor(data.subDiv.max * data.bbSize.z / bbSizeMax));
  data.subDiv.X = data.subDiv.X < 1 ? 1 : data.subDiv.X;
  data.subDiv.Y = data.subDiv.Y < 1 ? 1 : data.subDiv.Y;
  data.subDiv.Z = data.subDiv.Z < 1 ? 1 : data.subDiv.Z;
  // set the parameters for ComputeNormals()
  data.facetParameters.facetNormals      = getFacetLocalNormals();
  data.facetParameters.facetPositions    = getFacetLocalPositions();
  data.facetParameters.facetPartitioning = getFacetLocalPartitioning();
  data.facetParameters.bInfo             = bInfo;
  data.facetParameters.bbSize            = data.bbSize;
  data.facetParameters.subDiv            = data.subDiv;
  data.facetParameters.ratio             = partitioningBBoxRatio();
  data.facetParameters.depthSort         = data.facetDepthSort;
  if (data.facetDepthSort && data.facetDepthSortEnabled) {
    computeWorldMatrix(true);
    _worldMatrix.invertToRef(data.invertedMatrix);
    Vector3::TransformCoordinatesToRef(
      *data.facetDepthSortFrom, data.invertedMatrix, data.facetDepthSortOrigin);
    data.facetParameters.distanceTo = data.facetDepthSortOrigin;
  }
  data.facetParameters.depthSortedFacets = data.depthSortedFacets;
  VertexData::ComputeNormals(positions, indices, normals, data.facetParameters);

  if (data.facetDepthSort && data.facetDepthSortEnabled) {
    std::sort(data.depthSortedFacets.begin(), data.depthSortedFacets.end(),
              data.facetDepthSortFunction);
    auto l = (data.depthSortedIndices.size() / 3);
    for (size_t f = 0; f < l; f++) {
      const auto& sind                   = data.depthSortedFacets[f].ind;
      data.depthSortedIndices[f * 3]     = indices[sind];
      data.depthSortedIndices[f * 3 + 1] = indices[sind + 1];
      data.depthSortedIndices[f * 3 + 2] = indices[sind + 2];
    }
    updateIndices(data.depthSortedIndices);
  }

  return *this;
}

std::vector<Vector3>& AbstractMesh::getFacetLocalNormals()
{
  if (_facetData.facetNormals.empty()) {
    updateFacetData();
  }
  return _facetData.facetNormals;
}

std::vector<Vector3>& AbstractMesh::getFacetLocalPositions()
{
  if (_facetData.facetPositions.empty()) {
    updateFacetData();
  }
  return _facetData.facetPositions;
}

std::vector<Uint32Array>& AbstractMesh::getFacetLocalPartitioning()
{
#if 0
  if (_facetPartitioning.empty()) {
    updateFacetData();
  }
#endif
  return _facetData.facetPartitioning;
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
  Vector3::TransformCoordinatesToRef(localPos, world, ref);
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
  Vector3::TransformNormalToRef(localNorm, getWorldMatrix(), ref);
  return *this;
}

Uint32Array AbstractMesh::getFacetsAtLocalCoordinates(float x, float y, float z)
{
  const auto& bInfo = *getBoundingInfo();
  auto& data        = _facetData;

  int ox = static_cast<int>(
    std::floor((x - bInfo.minimum().x * data.partitioningBBoxRatio)
               * data.subDiv.X * data.partitioningBBoxRatio / data.bbSize.x));
  int oy = static_cast<int>(
    std::floor((y - bInfo.minimum().y * data.partitioningBBoxRatio)
               * data.subDiv.Y * data.partitioningBBoxRatio / data.bbSize.y));
  int oz = static_cast<int>(
    std::floor((z - bInfo.minimum().z * data.partitioningBBoxRatio)
               * data.subDiv.Z * data.partitioningBBoxRatio / data.bbSize.z));

  if (ox < 0 || oy < 0 || oz < 0) {
    return Uint32Array();
  }

  unsigned int _ox = static_cast<unsigned>(ox);
  unsigned int _oy = static_cast<unsigned>(oy);
  unsigned int _oz = static_cast<unsigned>(oz);
  if (_ox > data.subDiv.max || _oy > data.subDiv.max || _oz > data.subDiv.max) {
    return Uint32Array();
  }

  return data.facetPartitioning[_ox + data.subDiv.max * _oy
                                + data.subDiv.max * data.subDiv.max * _oz];
}

int AbstractMesh::getClosestFacetAtCoordinates(float x, float y, float z,
                                               Vector3& projected,
                                               bool projectedSet,
                                               bool checkFace, bool facing)
{
  auto world   = getWorldMatrix();
  auto& invMat = Tmp::MatrixArray[5];
  world.invertToRef(invMat);
  auto& invVect = Tmp::Vector3Array[8];
  int closest   = -1;
  // transform (x,y,z) to coordinates in the mesh local space
  Vector3::TransformCoordinatesFromFloatsToRef(x, y, z, invMat, invVect);
  closest = getClosestFacetAtLocalCoordinates(invVect.x, invVect.y, invVect.z,
                                              projected, checkFace, facing);
  if (projectedSet) {
    // tranform the local computed projected vector to world coordinates
    Vector3::TransformCoordinatesFromFloatsToRef(projected.x, projected.y,
                                                 projected.z, world, projected);
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
  size_t fib        = 0; // current facet in the block
  Vector3 norm;          // current facet normal
  Vector3 p0;            // current facet barycenter position
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
  return _facetData.facetParameters;
}

AbstractMesh& AbstractMesh::disableFacetData()
{
  if (_facetData.facetDataEnabled) {
    _facetData.facetDataEnabled = false;
    _facetData.facetPositions.clear();
    _facetData.facetNormals.clear();
    _facetData.facetPartitioning.clear();
    _facetData.facetParameters = FacetParameters();
    _facetData.depthSortedIndices.clear();
  }
  return *this;
}

AbstractMesh& AbstractMesh::updateIndices(const IndicesArray& /*indices*/)
{
  return *this;
}

AbstractMesh& AbstractMesh::createNormals(bool updatable)
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

  return *this;
}

AbstractMesh& AbstractMesh::alignWithNormal(Vector3& normal,
                                            const Vector3& upDirection)
{
  auto& axisX = Tmp::Vector3Array[0];
  auto& axisZ = Tmp::Vector3Array[1];
  Vector3::CrossToRef(upDirection, normal, axisZ);
  Vector3::CrossToRef(normal, axisZ, axisX);

  if (rotationQuaternion()) {
    auto rotationQuat = *rotationQuaternion();
    Quaternion::RotationQuaternionFromAxisToRef(axisX, normal, axisZ,
                                                rotationQuat);
    rotationQuaternion = rotationQuat;
  }
  else {
    Vector3::RotationFromAxisToRef(axisX, normal, axisZ, rotation());
  }
  return *this;
}

bool AbstractMesh::_checkOcclusionQuery()
{
  auto dataStorage = *_occlusionDataStorage();

  if (dataStorage.occlusionType == AbstractMesh::OCCLUSION_TYPE_NONE) {
    dataStorage.isOccluded = false;
    return false;
  }

  auto engine = getEngine();

  if (engine->webGLVersion() < 2.f) {
    dataStorage.isOccluded = false;
    return false;
  }

  if (isOcclusionQueryInProgress && _occlusionQuery) {

    auto isOcclusionQueryAvailable
      = engine->isQueryResultAvailable(_occlusionQuery);
    if (isOcclusionQueryAvailable) {
      auto occlusionQueryResult = engine->getQueryResult(_occlusionQuery);

      dataStorage.isOcclusionQueryInProgress    = false;
      dataStorage.occlusionInternalRetryCounter = 0;
      dataStorage.isOccluded = occlusionQueryResult == 1 ? false : true;
    }
    else {

      dataStorage.occlusionInternalRetryCounter++;

      if (dataStorage.occlusionRetryCount != -1
          && dataStorage.occlusionInternalRetryCounter
               > dataStorage.occlusionRetryCount) {
        dataStorage.isOcclusionQueryInProgress    = false;
        dataStorage.occlusionInternalRetryCounter = 0;

        // if optimistic set isOccluded to false regardless of the status of
        // isOccluded. (Render in the current render loop) if strict continue
        // the last state of the object.
        dataStorage.isOccluded
          = dataStorage.occlusionType
                == AbstractMesh::OCCLUSION_TYPE_OPTIMISTIC ?
              false :
              dataStorage.isOccluded;
      }
      else {
        return false;
      }
    }
  }

  auto scene = getScene();
  if (scene->getBoundingBoxRenderer()) {
    auto occlusionBoundingBoxRenderer = scene->getBoundingBoxRenderer();

    if (!_occlusionQuery) {
      _occlusionQuery = engine->createQuery();
    }

    engine->beginOcclusionQuery(dataStorage.occlusionQueryAlgorithmType,
                                _occlusionQuery);
    occlusionBoundingBoxRenderer->renderOcclusionBoundingBox(this);
    engine->endOcclusionQuery(dataStorage.occlusionQueryAlgorithmType);
    dataStorage.isOcclusionQueryInProgress = true;
  }

  return dataStorage.isOccluded;
}

} // end of namespace BABYLON
