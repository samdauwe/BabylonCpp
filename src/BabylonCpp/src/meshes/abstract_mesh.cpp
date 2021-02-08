#include <babylon/meshes/abstract_mesh.h>

#include <babylon/actions/abstract_action_manager.h>
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
#include <babylon/engines/engine.h>
#include <babylon/engines/extensions/occlusion_query_extension.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/maths/frustum.h>
#include <babylon/maths/functions.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/misc/string_tools.h>
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
    : TransformNode{iName, scene, false}
    , cullingStrategy{AbstractMesh::CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY}
    , facetNb{this, &AbstractMesh::get_facetNb}
    , partitioningSubdivisions{this, &AbstractMesh::get_partitioningSubdivisions,
                               &AbstractMesh::set_partitioningSubdivisions}
    , partitioningBBoxRatio{this, &AbstractMesh::get_partitioningBBoxRatio,
                            &AbstractMesh::set_partitioningBBoxRatio}
    , mustDepthSortFacets{this, &AbstractMesh::get_mustDepthSortFacets,
                          &AbstractMesh::set_mustDepthSortFacets}
    , facetDepthSortFrom{this, &AbstractMesh::get_facetDepthSortFrom,
                         &AbstractMesh::set_facetDepthSortFrom}
    , isFacetDataEnabled{this, &AbstractMesh::get_isFacetDataEnabled}
    , onCollide{this, &AbstractMesh::set_onCollide}
    , onCollisionPositionChange{this, &AbstractMesh::set_onCollisionPositionChange}
    , definedFacingForward{true} // orientation for POV movement & rotation
    , _occlusionQuery{nullptr}
    , _renderingGroup{nullptr}
    , _occlusionDataStorage{this, &AbstractMesh::get__occlusionDataStorage}
    , occlusionRetryCount{this, &AbstractMesh::get_occlusionRetryCount,
                          &AbstractMesh::set_occlusionRetryCount}
    , occlusionType{this, &AbstractMesh::get_occlusionType, &AbstractMesh::set_occlusionType}
    , occlusionQueryAlgorithmType{this, &AbstractMesh::get_occlusionQueryAlgorithmType,
                                  &AbstractMesh::set_occlusionQueryAlgorithmType}
    , isOccluded{this, &AbstractMesh::get_isOccluded, &AbstractMesh::set_isOccluded}
    , isOcclusionQueryInProgress{this, &AbstractMesh::get_isOcclusionQueryInProgress,
                                 &AbstractMesh::set_isOcclusionQueryInProgress}
    , visibility{this, &AbstractMesh::get_visibility, &AbstractMesh::set_visibility}
    , alphaIndex{std::numeric_limits<int>::max()}
    , isVisible{true}
    , isPickable{true}
    , showBoundingBox{this, &AbstractMesh::get_showBoundingBox, &AbstractMesh::set_showBoundingBox}
    , showSubMeshesBoundingBox{false}
    , isBlocker{false}
    , enablePointerMoveEvents{false}
    , renderingGroupId{this, &AbstractMesh::get_renderingGroupId,
                       &AbstractMesh::set_renderingGroupId}
    , material{this, &AbstractMesh::get_material, &AbstractMesh::set_material}
    , _savedMaterial{nullptr}
    , receiveShadows{this, &AbstractMesh::get_receiveShadows, &AbstractMesh::set_receiveShadows}
    , renderOutline{this, &AbstractMesh::get_renderOutline, &AbstractMesh::set_renderOutline}
    , outlineColor{Color3::Red()}
    , outlineWidth{0.02f}
    , renderOverlay{this, &AbstractMesh::get_renderOverlay, &AbstractMesh::set_renderOverlay}
    , overlayColor{Color3::Red()}
    , overlayAlpha{0.5f}
    , hasVertexAlpha{this, &AbstractMesh::get_hasVertexAlpha, &AbstractMesh::set_hasVertexAlpha}
    , useVertexColors{this, &AbstractMesh::get_useVertexColors, &AbstractMesh::set_useVertexColors}
    , computeBonesUsingShaders{this, &AbstractMesh::get_computeBonesUsingShaders,
                               &AbstractMesh::set_computeBonesUsingShaders}
    , numBoneInfluencers{this, &AbstractMesh::get_numBoneInfluencers,
                         &AbstractMesh::set_numBoneInfluencers}
    , applyFog{this, &AbstractMesh::get_applyFog, &AbstractMesh::set_applyFog}
    , useOctreeForRenderingSelection{true}
    , useOctreeForPicking{true}
    , useOctreeForCollisions{true}
    , layerMask{this, &AbstractMesh::get_layerMask, &AbstractMesh::set_layerMask}
    , alwaysSelectAsActiveMesh{false}
    , doNotSyncBoundingInfo{false}
    , actionManager{nullptr}
    , physicsImpostor{this, &AbstractMesh::get_physicsImpostor, &AbstractMesh::set_physicsImpostor}
    , ellipsoid{Vector3(0.5f, 1.f, 0.5f)}
    , ellipsoidOffset{Vector3(0, 0, 0)}
    , collisionMask{this, &AbstractMesh::get_collisionMask, &AbstractMesh::set_collisionMask}
    , collisionResponse{this, &AbstractMesh::get_collisionResponse,
                        &AbstractMesh::set_collisionResponse}
    , collisionGroup{this, &AbstractMesh::get_collisionGroup, &AbstractMesh::set_collisionGroup}
    , surroundingMeshes{this, &AbstractMesh::get_surroundingMeshes,
                        &AbstractMesh::set_surroundingMeshes}
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
    , _positions{this, &AbstractMesh::get__positions}
    , _waitingData{_WaitingData{
        std::nullopt, // lods
        {},           // actions
        std::nullopt  // freezeWorldMatrix
      }}
    , _transformMatrixTexture{nullptr}
    , skeleton{this, &AbstractMesh::get_skeleton, &AbstractMesh::set_skeleton}
    , edgesRenderer{this, &AbstractMesh::get_edgesRenderer}
    , isBlocked{this, &AbstractMesh::get_isBlocked}
    , useBones{this, &AbstractMesh::get_useBones}
    , isAnInstance{this, &AbstractMesh::get_isAnInstance}
    , hasInstances{this, &AbstractMesh::get_hasInstances}
    , hasThinInstances{this, &AbstractMesh::get_hasThinInstances}
    , checkCollisions{this, &AbstractMesh::get_checkCollisions, &AbstractMesh::set_checkCollisions}
    , collider{this, &AbstractMesh::get_collider}
    , _renderingGroupId{0}
    , _material{nullptr}
    , _renderOutline{false}
    , _renderOverlay{false}
    , _physicsImpostor{nullptr}
    , _disposePhysicsObserver{nullptr}
    , __occlusionDataStorage{nullptr}
    , _collisionsTransformMatrix{Matrix::Zero()}
    , _collisionsScalingMatrix{Matrix::Zero()}
    , _showBoundingBox{false}
{
  _resyncLightSources();

  // Mesh Uniform Buffer.
  _uniformBuffer
    = std::make_unique<UniformBuffer>(getScene()->getEngine(), Float32Array(), std::nullopt, iName);
  _buildUniformLayout();
}

AbstractMesh::~AbstractMesh() = default;

Type AbstractMesh::type() const
{
  return Type::ABSTRACTMESH;
}

void AbstractMesh::addToScene(const AbstractMeshPtr& newMesh)
{
  getScene()->addMesh(newMesh);
}

AbstractMeshPtr AbstractMesh::_this() const
{
  const auto& meshes = getScene()->meshes;
  auto it            = std::find_if(meshes.begin(), meshes.end(),
                         [this](const AbstractMeshPtr& mesh) { return mesh.get() == this; });
  return (it != meshes.end()) ? (*it) : nullptr;
}

size_t AbstractMesh::get_facetNb() const
{
  return _internalAbstractMeshDataInfo._facetData.facetNb;
}

unsigned int AbstractMesh::get_partitioningSubdivisions() const
{
  return _internalAbstractMeshDataInfo._facetData.partitioningSubdivisions;
}

void AbstractMesh::set_partitioningSubdivisions(unsigned int nb)
{
  _internalAbstractMeshDataInfo._facetData.partitioningSubdivisions = nb;
}

float AbstractMesh::get_partitioningBBoxRatio() const
{
  return _internalAbstractMeshDataInfo._facetData.partitioningBBoxRatio;
}

void AbstractMesh::set_partitioningBBoxRatio(float ratio)
{
  _internalAbstractMeshDataInfo._facetData.partitioningBBoxRatio = ratio;
}

bool AbstractMesh::get_mustDepthSortFacets() const
{
  return _internalAbstractMeshDataInfo._facetData.facetDepthSort;
}

void AbstractMesh::set_mustDepthSortFacets(bool sort)
{
  _internalAbstractMeshDataInfo._facetData.facetDepthSort = sort;
}

Vector3& AbstractMesh::get_facetDepthSortFrom()
{
  return *_internalAbstractMeshDataInfo._facetData.facetDepthSortFrom;
}

void AbstractMesh::set_facetDepthSortFrom(const Vector3& location)
{
  _internalAbstractMeshDataInfo._facetData.facetDepthSortFrom = location;
}

bool AbstractMesh::get_isFacetDataEnabled() const
{
  return _internalAbstractMeshDataInfo._facetData.facetDataEnabled;
}

bool AbstractMesh::_updateNonUniformScalingState(bool value)
{
  if (!TransformNode::_updateNonUniformScalingState(value)) {
    return false;
  }
  _markSubMeshesAsMiscDirty();
  return true;
}

void AbstractMesh::set_onCollide(const std::function<void(AbstractMesh*, EventState&)>& callback)
{
  if (_meshCollisionData._onCollideObserver) {
    onCollideObservable.remove(_meshCollisionData._onCollideObserver);
  }
  _meshCollisionData._onCollideObserver = onCollideObservable.add(callback);
}

void AbstractMesh::set_onCollisionPositionChange(
  const std::function<void(Vector3*, EventState&)>& callback)
{
  if (_meshCollisionData._onCollisionPositionChangeObserver) {
    onCollisionPositionChangeObservable.remove(
      _meshCollisionData._onCollisionPositionChangeObserver);
  }
  _meshCollisionData._onCollisionPositionChangeObserver
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

  // remove from material mesh map id needed
  auto uniqueIdStr = std::to_string(uniqueId);
  if (_material && stl_util::contains(_material->meshMap, uniqueIdStr)) {
    _material->meshMap.erase(uniqueIdStr);
  }

  _material = value;

  if (value) {
    value->meshMap[uniqueIdStr] = shared_from_base<AbstractMesh>();
  }

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
  return _internalAbstractMeshDataInfo._receiveShadows;
}

void AbstractMesh::set_receiveShadows(bool value)
{
  if (_internalAbstractMeshDataInfo._receiveShadows == value) {
    return;
  }

  _internalAbstractMeshDataInfo._receiveShadows = value;
  _markSubMeshesAsLightDirty();
}

bool AbstractMesh::get_hasVertexAlpha() const
{
  return _internalAbstractMeshDataInfo._hasVertexAlpha;
}

void AbstractMesh::set_hasVertexAlpha(bool value)
{
  if (_internalAbstractMeshDataInfo._hasVertexAlpha == value) {
    return;
  }

  _internalAbstractMeshDataInfo._hasVertexAlpha = value;
  _markSubMeshesAsAttributesDirty();
  _markSubMeshesAsMiscDirty();
}

bool AbstractMesh::get_useVertexColors() const
{
  return _internalAbstractMeshDataInfo._useVertexColors;
}

void AbstractMesh::set_useVertexColors(bool value)
{
  if (_internalAbstractMeshDataInfo._useVertexColors == value) {
    return;
  }

  _internalAbstractMeshDataInfo._useVertexColors = value;
  _markSubMeshesAsAttributesDirty();
}

bool AbstractMesh::get_computeBonesUsingShaders() const
{
  return _internalAbstractMeshDataInfo._computeBonesUsingShaders;
}

void AbstractMesh::set_computeBonesUsingShaders(bool value)
{
  if (_internalAbstractMeshDataInfo._computeBonesUsingShaders == value) {
    return;
  }

  _internalAbstractMeshDataInfo._computeBonesUsingShaders = value;
  _markSubMeshesAsAttributesDirty();
}

unsigned int AbstractMesh::get_numBoneInfluencers() const
{
  return _internalAbstractMeshDataInfo._numBoneInfluencers;
}

void AbstractMesh::set_numBoneInfluencers(unsigned int value)
{
  if (_internalAbstractMeshDataInfo._numBoneInfluencers == value) {
    return;
  }

  _internalAbstractMeshDataInfo._numBoneInfluencers = value;
  _markSubMeshesAsAttributesDirty();
}

bool AbstractMesh::get_applyFog() const
{
  return _internalAbstractMeshDataInfo._applyFog;
}

void AbstractMesh::set_applyFog(bool value)
{
  if (_internalAbstractMeshDataInfo._applyFog == value) {
    return;
  }

  _internalAbstractMeshDataInfo._applyFog = value;
  _markSubMeshesAsMiscDirty();
}

unsigned int AbstractMesh::get_layerMask() const
{
  return _internalAbstractMeshDataInfo._layerMask;
}

void AbstractMesh::set_layerMask(unsigned int value)
{
  if (value == _internalAbstractMeshDataInfo._layerMask) {
    return;
  }

  _internalAbstractMeshDataInfo._layerMask = value;
  _resyncLightSources();
}

bool AbstractMesh::get_isOcclusionQueryInProgress() const
{
  return _occlusionDataStorage()->isOcclusionQueryInProgress;
}

void AbstractMesh::set_isOcclusionQueryInProgress(bool value)
{
  _occlusionDataStorage()->isOcclusionQueryInProgress = value;
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
  return _internalAbstractMeshDataInfo._visibility;
}

void AbstractMesh::set_visibility(float value)
{
  if (stl_util::almost_equal(_internalAbstractMeshDataInfo._visibility, value)) {
    return;
  }

  _internalAbstractMeshDataInfo._visibility = value;
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
  return _meshCollisionData._collisionMask;
}

void AbstractMesh::set_collisionMask(int mask)
{
  _meshCollisionData._collisionMask = !isNan(mask) ? mask : -1;
}

bool AbstractMesh::get_collisionResponse() const
{
  return _meshCollisionData._collisionResponse;
}

void AbstractMesh::set_collisionResponse(bool response)
{
  _meshCollisionData._collisionResponse = response;
}

int AbstractMesh::get_collisionGroup() const
{
  return _meshCollisionData._collisionGroup;
}

void AbstractMesh::set_collisionGroup(int mask)
{
  _meshCollisionData._collisionGroup = !isNan(mask) ? mask : -1;
}

std::vector<AbstractMeshPtr>& AbstractMesh::get_surroundingMeshes()
{
  return _meshCollisionData._surroundingMeshes;
}

void AbstractMesh::set_surroundingMeshes(const std::vector<AbstractMeshPtr>& meshes)
{
  _meshCollisionData._surroundingMeshes = meshes;
}

std::vector<LightPtr>& AbstractMesh::get_lightSources()
{
  return _lightSources;
}

void AbstractMesh::_buildUniformLayout()
{
  _uniformBuffer->addUniform("world", 16);
  _uniformBuffer->addUniform("visibility", 1);
  _uniformBuffer->create();
}

void AbstractMesh::transferToEffect(const Matrix& world)
{
  const auto& ubo = _uniformBuffer;

  ubo->updateMatrix("world", world);
  ubo->updateFloat("visibility", _internalAbstractMeshDataInfo._visibility);

  ubo->update();
}

UniformBufferPtr& AbstractMesh::getMeshUniformBuffer()
{
  return _uniformBuffer;
}

std::string AbstractMesh::getClassName() const
{
  return "AbstractMesh";
}

std::string AbstractMesh::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name << ", isInstance: ";
  oss << (getClassName() == "InstancedMesh" ? "YES" : "NO");
  oss << ", # of submeshes: " << (subMeshes.size());
  auto& iSkeleton = _internalAbstractMeshDataInfo._skeleton;
  if (iSkeleton) {
    oss << ", skeleton: " << iSkeleton->name;
  }
  if (fullDetails) {
    oss << ", billboard mode: ";
    const std::vector<std::string> billboardModes{"NONE", "X",    "Y",    "null",
                                                  "Z",    "null", "null", "ALL"};
    if (billboardMode < billboardModes.size()) {
      oss << billboardModes[billboardMode];
    }
    else {
      oss << "null";
    }
    oss << ", freeze wrld mat: "
        << (_isWorldMatrixFrozen
                || (_waitingData.freezeWorldMatrix.has_value() && *_waitingData.freezeWorldMatrix) ?
              "YES" :
              "NO");
  }
  return oss.str();
}

Node* AbstractMesh::_getEffectiveParent()
{
  if (_masterMesh && billboardMode != TransformNode::BILLBOARDMODE_NONE) {
    return _masterMesh;
  }

  return TransformNode::_getEffectiveParent();
}

AbstractActionManagerPtr
AbstractMesh::_getActionManagerForTrigger(const std::optional<unsigned int>& trigger,
                                          bool initialCall)
{
  if (actionManager && (initialCall || actionManager->isRecursive)) {
    if (trigger) {
      if (actionManager->hasSpecificTrigger(*trigger)) {
        return actionManager;
      }
    }
    else {
      return actionManager;
    }
  }

  if (!parent()) {
    return nullptr;
  }

  return TransformNode::_getActionManagerForTrigger(trigger, false);
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

  for (const auto& subMesh : subMeshes) {
    subMesh->_rebuild();
  }
}

void AbstractMesh::_resyncLightSources()
{
  _lightSources.clear();

  for (const auto& light : getScene()->lights) {
    if (!light->isEnabled()) {
      continue;
    }

    if (light->canAffectMesh(this)) {
      _lightSources.emplace_back(light);
    }
  }

  _markSubMeshesAsLightDirty();
}

void AbstractMesh::_resyncLightSource(const LightPtr& light)
{
  const auto isIn = light && light->isEnabled() && light->canAffectMesh(this);

  auto index = std::find(_lightSources.begin(), _lightSources.end(), light);

  auto removed = false;
  if (index == _lightSources.end()) {
    if (!isIn) {
      return;
    }
    _lightSources.emplace_back(light);
  }
  else {
    if (isIn) {
      return;
    }
    removed = true;
    _lightSources.erase(index);
  }

  _markSubMeshesAsLightDirty(removed);
}

void AbstractMesh::_unBindEffect()
{
  for (const auto& subMesh : subMeshes) {
    subMesh->setEffect(nullptr);
  }
}

void AbstractMesh::_removeLightSource(const LightPtr& light, bool dispose)
{
  return _removeLightSource(light.get(), dispose);
}

void AbstractMesh::_removeLightSource(Light* light, bool dispose)
{
  auto index = std::find_if(_lightSources.begin(), _lightSources.end(),
                            [light](const LightPtr& _light) { return _light.get() == light; });

  if (index == _lightSources.end()) {
    return;
  }

  _lightSources.erase(index);

  _markSubMeshesAsLightDirty(dispose);
}

void AbstractMesh::_markSubMeshesAsDirty(const std::function<void(MaterialDefines& defines)>& func)
{
  if (subMeshes.empty()) {
    return;
  }

  for (const auto& subMesh : subMeshes) {
    if (subMesh->_materialDefines) {
      func(*subMesh->_materialDefines);
    }
  }
}

void AbstractMesh::_markSubMeshesAsLightDirty(bool dispose)
{
  const auto func
    = [dispose](MaterialDefines& defines) -> void { defines.markAsLightDirty(dispose); };
  _markSubMeshesAsDirty(func);
}

void AbstractMesh::_markSubMeshesAsAttributesDirty()
{
  const auto func = [](MaterialDefines& defines) -> void { defines.markAsAttributesDirty(); };
  _markSubMeshesAsDirty(func);
}

void AbstractMesh::_markSubMeshesAsMiscDirty()
{
  const auto func = [](MaterialDefines& defines) -> void { defines.markAsMiscDirty(); };
  _markSubMeshesAsDirty(func);
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
  auto& iSkeleton = _internalAbstractMeshDataInfo._skeleton;
  if (iSkeleton && iSkeleton->needInitialSkinMatrix) {
    iSkeleton->_unregisterMeshWithPoseMatrix(this);
  }

  if (value && value->needInitialSkinMatrix) {
    value->_registerMeshWithPoseMatrix(this);
  }

  _internalAbstractMeshDataInfo._skeleton = value;

  if (!_internalAbstractMeshDataInfo._skeleton) {
    _bonesTransformMatrices.clear();
  }

  _markSubMeshesAsAttributesDirty();
}

SkeletonPtr& AbstractMesh::get_skeleton()
{
  return _internalAbstractMeshDataInfo._skeleton;
}

Vector3& AbstractMesh::get_scaling()
{
  return _scaling;
}

void AbstractMesh::set_scaling(const Vector3& newScaling)
{
  _scaling = newScaling;
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
AbstractMesh::enableEdgesRendering(float epsilon, bool checkVerticesInsteadOfIndices,
                                   const std::optional<IEdgesRendererOptions>& options)
{
  disableEdgesRendering();
  _edgesRenderer = std::make_unique<EdgesRenderer>(shared_from_base<AbstractMesh>(), epsilon,
                                                   checkVerticesInsteadOfIndices, true, options);

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

AbstractMesh* AbstractMesh::getLOD(const CameraPtr& /*camera*/, BoundingSphere* /*boundingSphere*/)
{
  return this;
}

size_t AbstractMesh::getTotalVertices() const
{
  return 0;
}

Uint32Array AbstractMesh::getIndices(bool /*copyWhenShared*/, bool /*forceCopy*/)
{
  return Uint32Array();
}

Float32Array AbstractMesh::getVerticesData(const std::string& /*kind*/, bool /*copyWhenShared*/,
                                           bool /*forceCopy*/)
{
  return Float32Array();
}

AbstractMesh* AbstractMesh::setVerticesData(const std::string& /*kind*/,
                                            const Float32Array& /*data*/, bool /*updatable*/,
                                            const std::optional<size_t>& /*stride*/)
{
  return this;
}

AbstractMesh* AbstractMesh::updateVerticesData(const std::string& /*kind*/,
                                               const Float32Array& /*data*/, bool /*updateExtends*/,
                                               bool /*makeItUnique*/)
{
  return this;
}

AbstractMesh* AbstractMesh::setIndices(const IndicesArray& /*indices*/, size_t /*totalVertices*/,
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

AbstractMesh&
AbstractMesh::normalizeToUnitCube(bool includeDescendants, bool ignoreRotation,
                                  const std::function<bool(const AbstractMeshPtr& node)>& predicate)
{
  TransformNode::normalizeToUnitCube(includeDescendants, ignoreRotation, predicate);
  return *this;
}

AbstractMesh& AbstractMesh::setBoundingInfo(const BoundingInfo& boundingInfo)
{
  _boundingInfo = std::make_unique<BoundingInfo>(boundingInfo);
  return *this;
}

bool AbstractMesh::get_useBones() const
{
  return skeleton() && getScene()->skeletonsEnabled()
         && isVerticesDataPresent(VertexBuffer::MatricesIndicesKind)
         && isVerticesDataPresent(VertexBuffer::MatricesWeightsKind);
}

void AbstractMesh::_preActivate()
{
}

Mesh* AbstractMesh::_preActivateForIntermediateRendering(int /*renderId*/)
{
  return nullptr;
}

bool AbstractMesh::_activate(int renderId, bool /*intermediateRendering*/)
{
  _renderId = renderId;
  return true;
}

void AbstractMesh::_postActivate()
{
  // Do nothing
}

void AbstractMesh::_freeze()
{
  // Do nothing
}

void AbstractMesh::_unFreeze()
{
  // Do nothing
}

Matrix& AbstractMesh::getWorldMatrix()
{
  if (_masterMesh && billboardMode == TransformNode::BILLBOARDMODE_NONE) {
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

bool AbstractMesh::get_hasInstances() const
{
  return false;
}

bool AbstractMesh::get_hasThinInstances() const
{
  return false;
}

AbstractMesh& AbstractMesh::movePOV(float amountRight, float amountUp, float amountForward)
{
  position().addInPlace(calcMovePOV(amountRight, amountUp, amountForward));
  return *this;
}

Vector3 AbstractMesh::calcMovePOV(float amountRight, float amountUp, float amountForward)
{
  Matrix rotMatrix;
  auto rotQuaternion = rotationQuaternion() ?
                         *rotationQuaternion() :
                         Quaternion::RotationYawPitchRoll(rotation().y, rotation().x, rotation().z);
  rotQuaternion.toRotationMatrix(rotMatrix);

  auto translationDelta = Vector3::Zero();
  auto defForwardMult   = definedFacingForward ? 1.f : -1.f;
  Vector3::TransformCoordinatesFromFloatsToRef(amountRight * defForwardMult, amountUp,
                                               amountForward * defForwardMult, rotMatrix,
                                               translationDelta);
  return translationDelta;
}

AbstractMesh& AbstractMesh::rotatePOV(float flipBack, float twirlClockwise, float tiltRight)
{
  rotation().addInPlace(calcRotatePOV(flipBack, twirlClockwise, tiltRight));
  return *this;
}

Vector3 AbstractMesh::calcRotatePOV(float flipBack, float twirlClockwise, float tiltRight)
{
  auto defForwardMult = definedFacingForward ? 1.f : -1.f;
  return Vector3(flipBack * defForwardMult, twirlClockwise, tiltRight * defForwardMult);
}

AbstractMesh& AbstractMesh::refreshBoundingInfo(bool applySkeleton)
{
  if (_boundingInfo && _boundingInfo->isLocked()) {
    return *this;
  }

  _refreshBoundingInfo(_getPositionData(applySkeleton), std::nullopt);
  return *this;
}

void AbstractMesh::_refreshBoundingInfo(const Float32Array& data,
                                        const std::optional<Vector2>& bias)
{
  if (!data.empty()) {
    auto extend = extractMinAndMax(data, 0, getTotalVertices(), bias);
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
    _generatePointsArray();
    auto matricesIndicesData = getVerticesData(VertexBuffer::MatricesIndicesKind);
    auto matricesWeightsData = getVerticesData(VertexBuffer::MatricesWeightsKind);
    if (!matricesWeightsData.empty() && !matricesIndicesData.empty()) {
      auto needExtras = numBoneInfluencers() > 4;
      auto matricesIndicesExtraData
        = needExtras ? getVerticesData(VertexBuffer::MatricesIndicesExtraKind) : Float32Array();
      auto matricesWeightsExtraData
        = needExtras ? getVerticesData(VertexBuffer::MatricesWeightsExtraKind) : Float32Array();

      skeleton()->prepare();
      auto skeletonMatrices = skeleton()->getTransformMatrices(this);

      auto& tempVector  = TmpVectors::Vector3Array[0];
      auto& finalMatrix = TmpVectors::MatrixArray[0];
      auto& tempMatrix  = TmpVectors::MatrixArray[1];

      auto matWeightIdx = 0u;
      for (unsigned int index = 0; index < data.size(); index += 3, matWeightIdx += 4) {
        finalMatrix.reset();

        auto inf    = 0u;
        auto weight = 0.f;
        for (inf = 0; inf < 4; inf++) {
          weight = matricesWeightsData[matWeightIdx + inf];
          if ((weight > 0.f) && (matWeightIdx + inf < matricesIndicesData.size())) {
            Matrix::FromFloat32ArrayToRefScaled(
              skeletonMatrices,
              static_cast<unsigned int>(std::floor(matricesIndicesData[matWeightIdx + inf] * 16)),
              weight, tempMatrix);
            finalMatrix.addToSelf(tempMatrix);
          }
        }
        if (needExtras) {
          for (inf = 0; inf < 4; ++inf) {
            weight = matricesWeightsExtraData[matWeightIdx + inf];
            if ((weight > 0.f) && (matWeightIdx + inf < matricesIndicesData.size())) {
              Matrix::FromFloat32ArrayToRefScaled(
                skeletonMatrices,
                static_cast<unsigned int>(
                  std::floor(matricesIndicesExtraData[matWeightIdx + inf] * 16)),
                weight, tempMatrix);
              finalMatrix.addToSelf(tempMatrix);
            }
          }
        }

        Vector3::TransformCoordinatesFromFloatsToRef(data[index], data[index + 1], data[index + 2],
                                                     finalMatrix, tempVector);
        tempVector.toArray(data, index);

        if ((index / 3) < _positions().size()) {
          _positions()[index / 3].copyFrom(tempVector);
        }
      }
    }
  }

  return data;
}

AbstractMesh& AbstractMesh::_updateBoundingInfo()
{
  auto effectiveMesh = _effectiveMesh();
  if (_boundingInfo) {
    _boundingInfo->update(effectiveMesh->worldMatrixFromCache());
  }
  else {
    _boundingInfo = std::make_unique<BoundingInfo>(absolutePosition(), absolutePosition(),
                                                   effectiveMesh->worldMatrixFromCache());
  }
  _updateSubMeshesBoundingInfo(effectiveMesh->worldMatrixFromCache());
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
  if (doNotSyncBoundingInfo) {
    return;
  }
  // Bounding info
  _updateBoundingInfo();
}

AbstractMesh* AbstractMesh::_effectiveMesh()
{
  return (skeleton() && skeleton()->overrideMesh) ? skeleton()->overrideMesh.get() : this;
}

bool AbstractMesh::isInFrustum(const std::array<Plane, 6>& frustumPlanes, unsigned int /*strategy*/)
{
  return _boundingInfo != nullptr && _boundingInfo->isInFrustum(frustumPlanes, cullingStrategy);
}

bool AbstractMesh::isCompletelyInFrustum(const std::array<Plane, 6>& frustumPlanes)
{
  return _boundingInfo != nullptr && _boundingInfo->isCompletelyInFrustum(frustumPlanes);
}

bool AbstractMesh::intersectsMesh(AbstractMesh& mesh, bool precise, bool includeDescendants)
{
  if (!_boundingInfo || !mesh._boundingInfo) {
    return false;
  }

  if (_boundingInfo->intersects(*mesh._boundingInfo, precise)) {
    return true;
  }

  if (includeDescendants) {
    for (const auto& child : getChildMeshes()) {
      if (child->intersectsMesh(mesh, precise, true)) {
        return true;
      }
    }
  }

  return false;
}

bool AbstractMesh::intersectsMesh(SolidParticle& sp, bool precise, bool includeDescendants)
{
  if (!_boundingInfo || !sp._boundingInfo) {
    return false;
  }

  if (_boundingInfo->intersects(*sp._boundingInfo, precise)) {
    return true;
  }

  if (includeDescendants) {
    for (const auto& child : getChildMeshes()) {
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

AbstractMesh& AbstractMesh::applyImpulse(const Vector3& force, const Vector3& contactPoint)
{
  if (!_physicsImpostor) {
    return *this;
  }

  _physicsImpostor->applyImpulse(force, contactPoint);
  return *this;
}

AbstractMesh& AbstractMesh::setPhysicsLinkWith(Mesh* otherMesh, const Vector3& pivot1,
                                               const Vector3& pivot2, const PhysicsParams& options)
{
  if (!_physicsImpostor || !otherMesh->_physicsImpostor) {
    return *this;
  }

  PhysicsJointData jointData;
  jointData.mainPivot      = pivot1;
  jointData.connectedPivot = pivot2;
  jointData.nativeParams   = options;
  _physicsImpostor->createJoint(otherMesh->physicsImpostor(), PhysicsJoint::HingeJoint, jointData);

  return *this;
}

bool AbstractMesh::get_checkCollisions() const
{
  return _meshCollisionData._checkCollisions;
}

void AbstractMesh::set_checkCollisions(bool collisionEnabled)
{
  _meshCollisionData._checkCollisions = collisionEnabled;
}

ColliderPtr& AbstractMesh::get_collider()
{
  return _meshCollisionData._collider;
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
    _disposePhysicsObserver = onDisposeObservable.add([this](Node* /*node*/, EventState& /*es*/) {
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

  globalPosition.addToRef(ellipsoidOffset, _meshCollisionData._oldPositionForCollisions);
  auto& coordinator = getScene()->collisionCoordinator();

  if (!_meshCollisionData._collider) {
    _meshCollisionData._collider = coordinator->createCollider();
  }

  _meshCollisionData._collider->_radius = ellipsoid;

  coordinator->getNewPosition(
    _meshCollisionData._oldPositionForCollisions, displacement, _meshCollisionData._collider, 3,
    shared_from_base<AbstractMesh>(),
    [this](size_t collisionId, Vector3& newPosition, const AbstractMeshPtr& collidedMesh) {
      _onCollisionPositionChange(static_cast<int>(collisionId), newPosition, collidedMesh);
    },
    uniqueId);
  return *this;
}

void AbstractMesh::_onCollisionPositionChange(int /*collisionId*/, Vector3& newPosition,
                                              const AbstractMeshPtr& collidedMesh)
{
  newPosition.subtractToRef(_meshCollisionData._oldPositionForCollisions,
                            _meshCollisionData._diffPositionForCollisions);

  if (_meshCollisionData._diffPositionForCollisions.length() > Engine::CollisionsEpsilon) {
    position().addInPlace(_meshCollisionData._diffPositionForCollisions);
  }

  if (collidedMesh) {
    onCollideObservable.notifyObservers(collidedMesh.get());
  }

  onCollisionPositionChangeObservable.notifyObservers(&position());
}

Octree<SubMesh*>* AbstractMesh::createOrUpdateSubmeshesOctree(size_t maxCapacity, size_t maxDepth)
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
  _submeshesOctree->update(bbox.minimumWorld, bbox.maximumWorld, rawSubMeshPtrs);

  return _submeshesOctree;
}

AbstractMesh& AbstractMesh::_collideForSubMesh(SubMesh* subMesh, const Matrix& transformMatrix,
                                               Collider& /*iCollider*/)
{
  _generatePointsArray();

  if (_positions().empty()) {
    return *this;
  }

  // Transformation
  if (subMesh->_lastColliderWorldVertices.empty()
      || !subMesh->_lastColliderTransformMatrix->equals(transformMatrix)) {
    subMesh->_lastColliderTransformMatrix = std::make_unique<Matrix>(transformMatrix);
    subMesh->_lastColliderWorldVertices   = {};
    subMesh->_trianglePlanes              = {};
    auto start                            = subMesh->verticesStart;
    auto end                              = (subMesh->verticesStart + subMesh->verticesCount);
    for (unsigned int i = start; i < end; i++) {
      subMesh->_lastColliderWorldVertices.emplace_back(
        Vector3::TransformCoordinates(_positions()[i], transformMatrix));
    }
  }
  // Collide
#if 0
  iCollider._collide(
    subMesh->_trianglePlanes, subMesh->_lastColliderWorldVertices, getIndices(),
    subMesh->indexStart, subMesh->indexStart + subMesh->indexCount,
    subMesh->verticesStart, subMesh->getMaterial() != nullptr);
#endif
  return *this;
}

AbstractMesh& AbstractMesh::_processCollisionsForSubMeshes(Collider& iCollider,
                                                           const Matrix& transformMatrix)
{
  auto iSubMeshes = _scene->getCollidingSubMeshCandidates(this, iCollider);
  auto len        = iSubMeshes.size();

  for (size_t index = 0; index < len; ++index) {
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
  auto& collisionsScalingMatrix   = TmpVectors::MatrixArray[0];
  auto& collisionsTransformMatrix = TmpVectors::MatrixArray[1];
  Matrix::ScalingToRef(1.f / iCollider._radius.x, 1.f / iCollider._radius.y,
                       1.f / iCollider._radius.z, collisionsScalingMatrix);
  worldMatrixFromCache().multiplyToRef(collisionsScalingMatrix, collisionsTransformMatrix);
  _processCollisionsForSubMeshes(iCollider, collisionsTransformMatrix);
  return *this;
}

bool AbstractMesh::_generatePointsArray()
{
  return false;
}

PickingInfo AbstractMesh::intersects(Ray& ray, const std::optional<bool>& iFastCheck,
                                     const TrianglePickingPredicate& trianglePredicate,
                                     const std::optional<bool>& onlyBoundingInfo,
                                     const std::optional<Matrix>& worldToUse, bool skipBoundingInfo)
{
  const auto fastCheck = iFastCheck.value_or(true);
  PickingInfo pickingInfo;
  auto intersectionThreshold
    = getClassName() == "InstancedLinesMesh" || getClassName() == "LinesMesh" ?
        static_cast<LinesMesh*>(this)->intersectionThreshold :
        0.f;
  const auto& boundingInfo = _boundingInfo;

  if (subMeshes.empty() || !boundingInfo) {
    return pickingInfo;
  }
  if (!skipBoundingInfo
      && (!ray.intersectsSphere(boundingInfo->boundingSphere, intersectionThreshold)
          || !ray.intersectsBox(boundingInfo->boundingBox, intersectionThreshold))) {
    return pickingInfo;
  }

  if (onlyBoundingInfo && *onlyBoundingInfo) {
    pickingInfo.hit        = skipBoundingInfo ? false : true;
    pickingInfo.pickedMesh = skipBoundingInfo ? nullptr : shared_from_base<AbstractMesh>();
    pickingInfo.distance
      = skipBoundingInfo ? 0.f : Vector3::Distance(ray.origin, boundingInfo->boundingSphere.center);
    pickingInfo.subMeshId = 0;
    return pickingInfo;
  }

  if (!_generatePointsArray()) {
    return pickingInfo;
  }

  std::optional<IntersectionInfo> intersectInfo = std::nullopt;

  const auto _subMeshes = _scene->getIntersectingSubMeshCandidates(this, ray);
  const auto len        = _subMeshes.size();

  // Check if all submeshes are using a material that don't allow picking (point/lines rendering)
  // if no submesh can be picked that way, then fallback to BBox picking
  auto anySubmeshSupportIntersect = false;
  for (size_t index = 0; index < len; ++index) {
    const auto& subMesh   = _subMeshes[index];
    const auto& iMaterial = subMesh->getMaterial();
    if (!iMaterial) {
      continue;
    }
    if (!getIndices().empty()
        && (iMaterial->fillMode() == Constants::MATERIAL_TriangleStripDrawMode
            || iMaterial->fillMode() == Constants::MATERIAL_TriangleFillMode
            || iMaterial->fillMode() == Constants::MATERIAL_WireFrameFillMode
            || iMaterial->fillMode() == Constants::MATERIAL_PointFillMode)) {
      anySubmeshSupportIntersect = true;
      break;
    }
  }

  // no sub mesh support intersection, fallback to BBox that has already be done
  if (!anySubmeshSupportIntersect) {
    pickingInfo.hit        = true;
    pickingInfo.pickedMesh = shared_from_base<AbstractMesh>();
    pickingInfo.distance   = Vector3::Distance(ray.origin, boundingInfo->boundingSphere.center);
    pickingInfo.subMeshId  = -1;
    return pickingInfo;
  }

  // at least 1 submesh supports intersection, keep going
  for (size_t index = 0; index < len; ++index) {
    const auto& subMesh = _subMeshes[index];

    // Bounding test
    if (len > 1 && !subMesh->canIntersects(ray)) {
      continue;
    }

    auto currentIntersectInfo
      = subMesh->intersects(ray, _positions(), getIndices(), fastCheck, trianglePredicate);

    if (currentIntersectInfo) {
      if (fastCheck || !intersectInfo || currentIntersectInfo->distance < intersectInfo->distance) {
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
    auto world        = worldToUse.value_or((skeleton() && skeleton()->overrideMesh ?
                                               skeleton()->overrideMesh->getWorldMatrix() :
                                               getWorldMatrix()));
    auto& worldOrigin = TmpVectors::Vector3Array[0];
    auto& direction   = TmpVectors::Vector3Array[1];
    Vector3::TransformCoordinatesToRef(ray.origin, world, worldOrigin);
    ray.direction.scaleToRef(intersectInfo->distance, direction);
    auto worldDirection = Vector3::TransformNormal(direction, world);
    auto pickedPoint    = worldDirection.addInPlace(worldOrigin);

    // Return result
    pickingInfo.hit           = true;
    pickingInfo.distance      = Vector3::Distance(worldOrigin, pickedPoint);
    pickingInfo.pickedPoint   = pickedPoint;
    pickingInfo.pickedMesh    = shared_from_base<AbstractMesh>();
    pickingInfo.bu            = intersectInfo->bu.value_or(0.f);
    pickingInfo.bv            = intersectInfo->bv.value_or(0.f);
    pickingInfo.subMeshFaceId = static_cast<int>(intersectInfo->faceId);
    pickingInfo.faceId        = static_cast<int>(intersectInfo->faceId)
                         + static_cast<int>(subMeshes[intersectInfo->subMeshId]->indexStart)
                             / (StringTools::indexOf(getClassName(), "LinesMesh") != -1 ? 2 : 3);
    pickingInfo.subMeshId = static_cast<int>(intersectInfo->subMeshId);
    return pickingInfo;
  }

  return pickingInfo;
}

AbstractMesh* AbstractMesh::clone(const std::string& /*name*/, Node* /*newParent*/,
                                  bool /*doNotCloneChildren*/)
{
  return nullptr;
}

AbstractMesh& AbstractMesh::releaseSubMeshes()
{
  if (!subMeshes.empty()) {
    auto subMeshesCopy = subMeshes; // copy because subMesh->dispose can erase from subMeshes
    for (const auto& subMesh : subMeshesCopy) {
      subMesh->dispose();
    }
  }

  subMeshes.clear();

  return *this;
}

void AbstractMesh::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  // mesh map release.
  if (_scene->useMaterialMeshMap) {
    // remove from material mesh map id needed
    if (_material && !_material->meshMap.empty()) {
      _material->meshMap.erase(std::to_string(uniqueId));
    }
  }

  // Smart Array Retainers.
  getScene()->freeActiveMeshes();
  getScene()->freeRenderingGroups();

  // Action manager
  if (actionManager) {
    actionManager->dispose();
    actionManager = nullptr;
  }

  // Skeleton
  _internalAbstractMeshDataInfo._skeleton = nullptr;

  if (_transformMatrixTexture) {
    _transformMatrixTexture->dispose();
    _transformMatrixTexture = nullptr;
  }

  // Intersections in progress
  for (const auto& other : _intersectionsInProgress) {
    other->_intersectionsInProgress.erase(std::remove(other->_intersectionsInProgress.begin(),
                                                      other->_intersectionsInProgress.end(), this),
                                          other->_intersectionsInProgress.end());
  }

  _intersectionsInProgress.clear();

  // Lights
  auto lightsCopy = getScene()->lights;
  for (const auto& light : lightsCopy) {
    // Included meshes
    stl_util::remove_vector_elements_equal_sharedptr_wrapped(light->includedOnlyMeshes,
                                                             this); // Checked

    // Excluded meshes
    stl_util::remove_vector_elements_equal_sharedptr_wrapped(light->excludedMeshes,
                                                             this); // Checked

    // Shadow generators
    auto generator = light->getShadowGenerator();
    if (generator) {
      auto shadowMap = generator->getShadowMap();
      if (shadowMap && !shadowMap->renderList().empty()) {
        stl_util::remove_vector_elements_equal_ptr_wrapped(shadowMap->renderList, this);
      }
    }
  }

  // Edges
  if (_edgesRenderer) {
    _edgesRenderer->dispose();
    _edgesRenderer = nullptr;
  }

  // SubMeshes
  if (getClassName() != "InstancedMesh" || getClassName() != "InstancedLinesMesh") {
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
      if (material()->getClassName() == "MultiMaterial") {
        material()->dispose(false, true, true);
      }
      else {
        material()->dispose(false, true);
      }
    }
  }

  if (!doNotRecurse) {
    // Particles
    for (size_t index = 0; index < getScene()->particleSystems.size(); ++index) {
      auto& emitter = getScene()->particleSystems[index]->emitter;
      if (std::holds_alternative<AbstractMeshPtr>(emitter)
          && (std::get<AbstractMeshPtr>(emitter).get() == this)) {
        getScene()->particleSystems[index]->dispose();
        --index;
      }
    }
  }

  // facet data
  if (_internalAbstractMeshDataInfo._facetData.facetDataEnabled) {
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
  auto& data   = _internalAbstractMeshDataInfo._facetData;
  data.facetNb = getIndices().size() / 3;

  // default nb of partitioning subdivisions = 10
  data.partitioningSubdivisions
    = (data.partitioningSubdivisions) ? data.partitioningSubdivisions : 10;
  // default ratio 1.01 = the partitioning is 1% bigger than the bounding box
  data.partitioningBBoxRatio
    = (data.partitioningBBoxRatio != 0.f) ? data.partitioningBBoxRatio : 1.01f;

  data.facetNormals.resize(data.facetNb);
  std::fill(data.facetNormals.begin(), data.facetNormals.end(), Vector3::Zero());

  data.facetPositions.resize(data.facetNb);
  std::fill(data.facetPositions.begin(), data.facetPositions.end(), Vector3::Zero());

  data.facetDataEnabled = true;

  return *this;
}

AbstractMesh& AbstractMesh::updateFacetData()
{
  auto& data = _internalAbstractMeshDataInfo._facetData;
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

    data.facetDepthSortFunction = [](const DepthSortedFacet& f1, const DepthSortedFacet& f2) {
      const auto diff = f2.sqDistance - f1.sqDistance;
      return (diff < 0.f) ? -1 : (diff > 0.f) ? 1 : 0;
    };
    if (!data.facetDepthSortFrom) {
      auto& camera            = getScene()->activeCamera();
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

  data.bbSize.x   = (bInfo.maximum().x - bInfo.minimum().x > Math::Epsilon) ?
                      bInfo.maximum().x - bInfo.minimum().x :
                      Math::Epsilon;
  data.bbSize.y   = (bInfo.maximum().y - bInfo.minimum().y > Math::Epsilon) ?
                      bInfo.maximum().y - bInfo.minimum().y :
                      Math::Epsilon;
  data.bbSize.z   = (bInfo.maximum().z - bInfo.minimum().z > Math::Epsilon) ?
                      bInfo.maximum().z - bInfo.minimum().z :
                      Math::Epsilon;
  auto bbSizeMax  = (data.bbSize.x > data.bbSize.y) ? data.bbSize.x : data.bbSize.y;
  bbSizeMax       = (bbSizeMax > data.bbSize.z) ? bbSizeMax : data.bbSize.z;
  data.subDiv.max = data.partitioningSubdivisions;
  // adjust the number of subdivisions per axis
  data.subDiv.X = static_cast<unsigned>(std::floor(data.subDiv.max * data.bbSize.x / bbSizeMax));
  // according to each bbox size per axis
  data.subDiv.Y = static_cast<unsigned>(std::floor(data.subDiv.max * data.bbSize.y / bbSizeMax));
  // at least one subdivision
  data.subDiv.Z = static_cast<unsigned>(std::floor(data.subDiv.max * data.bbSize.z / bbSizeMax));
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
    Vector3::TransformCoordinatesToRef(*data.facetDepthSortFrom, data.invertedMatrix,
                                       data.facetDepthSortOrigin);
    data.facetParameters.distanceTo = data.facetDepthSortOrigin;
  }
  data.facetParameters.depthSortedFacets = data.depthSortedFacets;
  VertexData::ComputeNormals(positions, indices, normals, data.facetParameters);

  if (data.facetDepthSort && data.facetDepthSortEnabled) {
    BABYLON::stl_util::sort_js_style(data.depthSortedFacets, data.facetDepthSortFunction);
    auto l = (data.depthSortedIndices.size() / 3);
    for (size_t f = 0; f < l; f++) {
      const auto& sind                   = data.depthSortedFacets[f].ind;
      data.depthSortedIndices[f * 3]     = indices[sind];
      data.depthSortedIndices[f * 3 + 1] = indices[sind + 1];
      data.depthSortedIndices[f * 3 + 2] = indices[sind + 2];
    }
    updateIndices(data.depthSortedIndices, std::nullopt, true);
  }

  return *this;
}

std::vector<Vector3>& AbstractMesh::getFacetLocalNormals()
{
  auto& facetData = _internalAbstractMeshDataInfo._facetData;
#if 0
  if (facetData.facetNormals.empty()) {
    updateFacetData();
  }
#endif
  return facetData.facetNormals;
}

std::vector<Vector3>& AbstractMesh::getFacetLocalPositions()
{
  auto& facetData = _internalAbstractMeshDataInfo._facetData;
#if 0
  if (facetData.facetPositions.empty()) {
    updateFacetData();
  }
#endif
  return facetData.facetPositions;
}

std::vector<Uint32Array>& AbstractMesh::getFacetLocalPartitioning()
{
  auto& facetData = _internalAbstractMeshDataInfo._facetData;
#if 0
  if (facetData.facetPartitioning.empty()) {
    updateFacetData();
  }
#endif
  return facetData.facetPartitioning;
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
  auto& data        = _internalAbstractMeshDataInfo._facetData;

  auto ox
    = static_cast<int>(std::floor((x - bInfo.minimum().x * data.partitioningBBoxRatio)
                                  * data.subDiv.X * data.partitioningBBoxRatio / data.bbSize.x));
  auto oy
    = static_cast<int>(std::floor((y - bInfo.minimum().y * data.partitioningBBoxRatio)
                                  * data.subDiv.Y * data.partitioningBBoxRatio / data.bbSize.y));
  auto oz
    = static_cast<int>(std::floor((z - bInfo.minimum().z * data.partitioningBBoxRatio)
                                  * data.subDiv.Z * data.partitioningBBoxRatio / data.bbSize.z));

  if (ox < 0 || oy < 0 || oz < 0) {
    return Uint32Array();
  }

  auto _ox = static_cast<unsigned>(ox);
  auto _oy = static_cast<unsigned>(oy);
  auto _oz = static_cast<unsigned>(oz);
  if (_ox > data.subDiv.max || _oy > data.subDiv.max || _oz > data.subDiv.max) {
    return Uint32Array();
  }

  return data
    .facetPartitioning[_ox + data.subDiv.max * _oy + data.subDiv.max * data.subDiv.max * _oz];
}

int AbstractMesh::getClosestFacetAtCoordinates(float x, float y, float z, Vector3& projected,
                                               bool projectedSet, bool checkFace, bool facing)
{
  auto world   = getWorldMatrix();
  auto& invMat = TmpVectors::MatrixArray[5];
  world.invertToRef(invMat);
  auto& invVect = TmpVectors::Vector3Array[8];
  auto closest  = -1;
  // transform (x,y,z) to coordinates in the mesh local space
  Vector3::TransformCoordinatesFromFloatsToRef(x, y, z, invMat, invVect);
  closest = getClosestFacetAtLocalCoordinates(invVect.x, invVect.y, invVect.z, projected, checkFace,
                                              facing);
  if (projectedSet) {
    // tranform the local computed projected vector to world coordinates
    Vector3::TransformCoordinatesFromFloatsToRef(projected.x, projected.y, projected.z, world,
                                                 projected);
  }
  return closest;
}

int AbstractMesh::getClosestFacetAtLocalCoordinates(float x, float y, float z, Vector3& projected,
                                                    bool projectedSet, bool checkFace, bool facing)
{
  auto closest = -1;
  auto tmpx    = 0.f;
  auto tmpy    = 0.f;
  auto tmpz    = 0.f;
  auto d       = 0.f; // tmp dot facet normal * facet position
  auto t0      = 0.f;
  auto projx   = 0.f;
  auto projy   = 0.f;
  auto projz   = 0.f;
  // Get all the facets in the same partitioning block than (x, y, z)
  auto facetPositions = getFacetLocalPositions();
  auto facetNormals   = getFacetLocalNormals();
  auto facetsInBlock  = getFacetsAtLocalCoordinates(x, y, z);
  if (facetsInBlock.empty()) {
    return closest;
  }
  // Get the closest facet to (x, y, z)
  auto shortest = std::numeric_limits<float>::max(); // init distance vars
  auto fib      = 0ull;                              // current facet in the block
  Vector3 norm;                                      // current facet normal
  Vector3 p0;                                        // current facet barycenter position
  // loop on all the facets in the current partitioning block
  for (unsigned int idx : facetsInBlock) {
    fib  = idx;
    norm = facetNormals[fib];
    p0   = facetPositions[fib];

    d = (x - p0.x) * norm.x + (y - p0.y) * norm.y + (z - p0.z) * norm.z;
    if (!checkFace || (checkFace && facing && d >= 0.f) || (checkFace && !facing && d <= 0.f)) {
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
      float tmpDistance = tmpx * tmpx + tmpy * tmpy + tmpz * tmpz;
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
  return _internalAbstractMeshDataInfo._facetData.facetParameters;
}

AbstractMesh& AbstractMesh::disableFacetData()
{
  auto& facetData = _internalAbstractMeshDataInfo._facetData;
  if (facetData.facetDataEnabled) {
    facetData.facetDataEnabled = false;
    facetData.facetPositions.clear();
    facetData.facetNormals.clear();
    facetData.facetPartitioning.clear();
    facetData.facetParameters = FacetParameters();
    facetData.depthSortedIndices.clear();
  }
  return *this;
}

AbstractMesh& AbstractMesh::updateIndices(const IndicesArray& /*indices*/,
                                          const std::optional<int>& /*offset*/,
                                          bool /*gpuMemoryOnly*/)
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

AbstractMesh& AbstractMesh::alignWithNormal(Vector3& normal, const Vector3& upDirection)
{
  auto& axisX = TmpVectors::Vector3Array[0];
  auto& axisZ = TmpVectors::Vector3Array[1];
  Vector3::CrossToRef(upDirection, normal, axisZ);
  Vector3::CrossToRef(normal, axisZ, axisX);

  if (rotationQuaternion()) {
    auto rotationQuat = *rotationQuaternion();
    Quaternion::RotationQuaternionFromAxisToRef(axisX, normal, axisZ, rotationQuat);
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

    auto isOcclusionQueryAvailable = engine->isQueryResultAvailable(_occlusionQuery);
    if (isOcclusionQueryAvailable) {
      auto occlusionQueryResult = engine->getQueryResult(_occlusionQuery);

      dataStorage.isOcclusionQueryInProgress    = false;
      dataStorage.occlusionInternalRetryCounter = 0;
      dataStorage.isOccluded                    = occlusionQueryResult != 1;
    }
    else {

      dataStorage.occlusionInternalRetryCounter++;

      if (dataStorage.occlusionRetryCount != -1
          && dataStorage.occlusionInternalRetryCounter > dataStorage.occlusionRetryCount) {
        dataStorage.isOcclusionQueryInProgress    = false;
        dataStorage.occlusionInternalRetryCounter = 0;

        // if optimistic set isOccluded to false regardless of the status of
        // isOccluded. (Render in the current render loop) if strict continue
        // the last state of the object.
        dataStorage.isOccluded
          = dataStorage.occlusionType == AbstractMesh::OCCLUSION_TYPE_OPTIMISTIC ?
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

    engine->beginOcclusionQuery(dataStorage.occlusionQueryAlgorithmType, _occlusionQuery);
    occlusionBoundingBoxRenderer->renderOcclusionBoundingBox(this);
    engine->endOcclusionQuery(dataStorage.occlusionQueryAlgorithmType);
    dataStorage.isOcclusionQueryInProgress = true;
  }

  return dataStorage.isOccluded;
}

std::vector<IParticleSystemPtr> AbstractMesh::getConnectedParticleSystems()
{
  std::vector<IParticleSystemPtr> particleSystems;
  for (const auto& particleSystem : _scene->particleSystems) {
    if (std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)
        && std::get<AbstractMeshPtr>(particleSystem->emitter).get() == this) {
      particleSystems.emplace_back(particleSystem);
    }
  }
  return particleSystems;
}

} // end of namespace BABYLON
