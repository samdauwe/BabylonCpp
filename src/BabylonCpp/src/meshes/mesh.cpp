#include <babylon/meshes/sub_mesh.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/layers/highlight_layer.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/loading/scene_loader_flags.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/maths/vector2.h>
#include <babylon/meshes/_creation_data_storage.h>
#include <babylon/meshes/_instance_data_storage.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/_internal_mesh_data_info.h>
#include <babylon/meshes/_thin_instance_data_storage.h>
#include <babylon/meshes/_visible_instances.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/builders/box_builder.h>
#include <babylon/meshes/builders/capsule_builder.h>
#include <babylon/meshes/builders/cylinder_builder.h>
#include <babylon/meshes/builders/decal_builder.h>
#include <babylon/meshes/builders/disc_builder.h>
#include <babylon/meshes/builders/ground_builder.h>
#include <babylon/meshes/builders/hemisphere_builder.h>
#include <babylon/meshes/builders/ico_sphere_builder.h>
#include <babylon/meshes/builders/lathe_builder.h>
#include <babylon/meshes/builders/lines_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/plane_builder.h>
#include <babylon/meshes/builders/polygon_builder.h>
#include <babylon/meshes/builders/polyhedron_builder.h>
#include <babylon/meshes/builders/ribbon_builder.h>
#include <babylon/meshes/builders/shape_builder.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/builders/torus_builder.h>
#include <babylon/meshes/builders/torus_knot_builder.h>
#include <babylon/meshes/builders/tube_builder.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh_lod_level.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/misc/file_tools.h>
#include <babylon/misc/string_tools.h>
#include <babylon/morph/morph_target.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/particles/particle_system.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/rendering/edges_renderer.h>
#include <babylon/rendering/outline_renderer.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/rendering_group.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

Mesh::Mesh(const std::string& iName, Scene* scene, Node* iParent, Mesh* source,
           bool doNotCloneChildren, bool clonePhysicsImpostor, bool postInitialize)
    : AbstractMesh{iName, scene}
    , computeBonesUsingShaders{this, &Mesh::get_computeBonesUsingShaders,
                               &Mesh::set_computeBonesUsingShaders}
    , onBeforeRenderObservable{this, &Mesh::get_onBeforeDrawObservable}
    , onBeforeBindObservable{this, &Mesh::get_onBeforeBindObservable}
    , onAfterRenderObservable{this, &Mesh::get_onAfterRenderObservable}
    , onBeforeDrawObservable{this, &Mesh::get_onBeforeDrawObservable}
    , onBeforeDraw{this, &Mesh::set_onBeforeDraw}
    , delayLoadState{Constants::DELAYLOADSTATE_NONE}
    , edgesShareWithInstances{false}
    , onLODLevelSelection{nullptr}
    , morphTargetManager{this, &Mesh::get_morphTargetManager, &Mesh::set_morphTargetManager}
    , _creationDataStorage{std::make_shared<_CreationDataStorage>()}
    , _geometry{nullptr}
    , _shouldGenerateFlatShading{false}
    , _originalBuilderSideOrientation{Mesh::DEFAULTSIDE}
    , overrideMaterialSideOrientation{std::nullopt}
    , ignoreCameraMaxZ{false}
    , _source{this, &Mesh::get_source}
    , cloneMeshMap{this, &Mesh::get_cloneMeshMap}
    , isUnIndexed{this, &Mesh::get_isUnIndexed, &Mesh::set_isUnIndexed}
    , worldMatrixInstancedBuffer{this, &Mesh::get_worldMatrixInstancedBuffer}
    , manualUpdateOfWorldMatrixInstancedBuffer{this,
                                               &Mesh::get_manualUpdateOfWorldMatrixInstancedBuffer,
                                               &Mesh::set_manualUpdateOfWorldMatrixInstancedBuffer}
    , _isMesh{this, &Mesh::get__isMesh}
    , hasLODLevels{this, &Mesh::get_hasLODLevels}
    , geometry{this, &Mesh::get_geometry}
    , areNormalsFrozen{this, &Mesh::get_areNormalsFrozen}
    , overridenInstanceCount{this, &Mesh::set_overridenInstanceCount}
    , thinInstanceEnablePicking{false}
    , thinInstanceCount{this, &Mesh::get_thinInstanceCount, &Mesh::set_thinInstanceCount}
    , _instanceDataStorage{std::make_unique<_InstanceDataStorage>()}
    , _internalMeshDataInfo{std::make_unique<_InternalMeshDataInfo>()}
    , _onBeforeDrawObserver{nullptr}
    , _thinInstanceDataStorage{std::make_unique<_ThinInstanceDataStorage>()}
    , _userThinInstanceBuffersStorage{nullptr}
    , _effectiveMaterial{nullptr}
    , _userInstancedBuffersStorage{std::nullopt}
    , _tessellation{0}
    , _arc{1.f}
{
  if (!postInitialize) {
    _initialize(scene, iParent, source, doNotCloneChildren, clonePhysicsImpostor);
  }
}

void Mesh::_initialize(Scene* scene, Node* iParent, Mesh* source, bool doNotCloneChildren,
                       bool clonePhysicsImpostor)
{
  _boundingInfo                    = nullptr;
  _instanceDataStorage->batchCache = std::make_shared<_InstancesBatch>();

  scene = getScene();

  if (source) {
    // Geometry
    if (source->_geometry) {
      source->_geometry->applyToMesh(this);
    }

    // Deep copy
#if 0
    Tools::DeepCopy(source, this,
                    [
                      "name", "material", "skeleton", "instances", "parent",
                      "uniqueId", "source", "metadata", "hasLODLevels",
                      "geometry", "isBlocked", "areNormalsFrozen",
                      "onBeforeDrawObservable", "onBeforeRenderObservable",
                      "onAfterRenderObservable", "onBeforeDraw"
                    ],
                    ["_poseMatrix"]);
#endif

    // Source mesh
    _internalMeshDataInfo->_source = source;
    if (scene->useClonedMeshMap) {
      source->_internalMeshDataInfo->meshMap[std::to_string(uniqueId)] = this;
    }

    // Construction Params
    // Clone parameters allowing mesh to be updated in case of parametric
    // shapes.
    _originalBuilderSideOrientation = source->_originalBuilderSideOrientation;
    _creationDataStorage            = source->_creationDataStorage;

    // Enabled
    setEnabled(source->isEnabled());

    // Parent
    Node::set_parent(source->parent());

    // Pivot
    setPivotMatrix(source->getPivotMatrix());

    id = name + "." + source->id;

    // Material
    material = source->getMaterial();
    if (!doNotCloneChildren) {
      // Children
      for (const auto& mesh : scene->meshes) {
        if (mesh->parent() == source) {
          // doNotCloneChildren is always going to be False
          mesh->clone(name + "." + mesh->name, this, doNotCloneChildren);
        }
      }
    }

    // Morphs
    if (source->morphTargetManager()) {
      morphTargetManager = source->morphTargetManager();
    }

    // Physics clone
    if (scene->getPhysicsEngine()) {
      auto physicsEngine = getScene()->getPhysicsEngine();
      if (clonePhysicsImpostor && physicsEngine) {
        // auto impostor = physicsEngine->getImpostorForPhysicsObject(source);
        // if (impostor) {
        //  physicsImpostor = impostor->clone(this);
        //}
      }
    }

    // Particles
    for (const auto& system : scene->particleSystems) {
      auto& emitter = system->emitter;
      if (std::holds_alternative<AbstractMeshPtr>(emitter)
          && (std::get<AbstractMeshPtr>(emitter).get() == source)) {
        system->clone(system->name, this);
      }
    }
    refreshBoundingInfo();
    computeWorldMatrix(true);
  }

  // Parent
  if (iParent != nullptr) {
    Node::set_parent(iParent);
  }

  _instanceDataStorage->hardwareInstancedRendering = getEngine()->getCaps().instancedArrays;
}

Mesh::~Mesh() = default;

MorphTargetManagerPtr& Mesh::get_morphTargetManager()
{
  return _internalMeshDataInfo->_morphTargetManager;
}

void Mesh::set_morphTargetManager(const MorphTargetManagerPtr& value)
{
  if (_internalMeshDataInfo->_morphTargetManager == value) {
    return;
  }
  _internalMeshDataInfo->_morphTargetManager = value;
  _syncGeometryWithMorphTargetManager();
}

Mesh*& Mesh::get_source()
{
  return _internalMeshDataInfo->_source;
}

std::unordered_map<std::string, Mesh*>& Mesh::get_cloneMeshMap()
{
  return _internalMeshDataInfo->meshMap;
}

bool Mesh::get_isUnIndexed() const
{
  return _unIndexed;
}

void Mesh::set_isUnIndexed(bool value)
{
  if (_unIndexed != value) {
    _unIndexed = value;
    _markSubMeshesAsAttributesDirty();
  }
}

Float32Array& Mesh::get_worldMatrixInstancedBuffer()
{
  return _instanceDataStorage->instancesData;
}

bool Mesh::get_manualUpdateOfWorldMatrixInstancedBuffer() const
{
  return _instanceDataStorage->manualUpdate;
}

void Mesh::set_manualUpdateOfWorldMatrixInstancedBuffer(bool value)
{
  _instanceDataStorage->manualUpdate = value;
}

TransformNodePtr Mesh::instantiateHierarchy(
  TransformNode* newParent, const std::optional<InstantiateHierarychyOptions>& options,
  const std::function<void(TransformNode* source, TransformNode* clone)>& onNewNodeCreated)
{
  auto instance
    = (getTotalVertices() > 0 && (!options || !options->doNotInstantiate)) ?
        std::static_pointer_cast<TransformNode>(
          createInstance("instance of " + (!name.empty() ? name : id))) :
        std::static_pointer_cast<TransformNode>(
          clone("Clone of " + (!name.empty() ? name : id), newParent ? newParent : parent(), true));

  if (instance) {
    instance->parent   = newParent ? newParent : parent();
    instance->position = position().copy();
    instance->scaling  = scaling().copy();
    if (rotationQuaternion()) {
      instance->rotationQuaternion = rotationQuaternion()->copy();
    }
    else {
      instance->rotation = rotation().copy();
    }

    if (onNewNodeCreated) {
      onNewNodeCreated(this, instance.get());
    }
  }

  for (const auto& child : getChildTransformNodes(true)) {
    child->instantiateHierarchy(instance.get(), options, onNewNodeCreated);
  }

  return instance;
}

std::string Mesh::getClassName() const
{
  return "Mesh";
}

Type Mesh::type() const
{
  return Type::MESH;
}

bool Mesh::get__isMesh() const
{
  return true;
}

bool Mesh::get_computeBonesUsingShaders() const
{
  return _internalAbstractMeshDataInfo._computeBonesUsingShaders;
}

void Mesh::set_computeBonesUsingShaders(bool value)
{
  if (_internalAbstractMeshDataInfo._computeBonesUsingShaders == value) {
    return;
  }

  if (value && !_internalMeshDataInfo->_sourcePositions.empty()) {
    // switch from software to GPU computation: we need to reset the vertex and normal buffers that
    // have been updated by the software process
    setVerticesData(VertexBuffer::PositionKind, _internalMeshDataInfo->_sourcePositions, true);
    if (!_internalMeshDataInfo->_sourceNormals.empty()) {
      setVerticesData(VertexBuffer::NormalKind, _internalMeshDataInfo->_sourceNormals, true);
    }
  }

  _internalAbstractMeshDataInfo._computeBonesUsingShaders = value;
  _markSubMeshesAsAttributesDirty();
}

Observable<Mesh>& Mesh::get_onBeforeRenderObservable()
{
  return _internalMeshDataInfo->_onBeforeRenderObservable;
}

Observable<Mesh>& Mesh::get_onBeforeBindObservable()
{
  return _internalMeshDataInfo->_onBeforeBindObservable;
}

Observable<Mesh>& Mesh::get_onAfterRenderObservable()
{
  return _internalMeshDataInfo->_onAfterRenderObservable;
}

Observable<Mesh>& Mesh::get_onBeforeDrawObservable()
{
  return _internalMeshDataInfo->_onBeforeDrawObservable;
}

void Mesh::set_onBeforeDraw(const std::function<void(Mesh*, EventState&)>& callback)
{
  if (_onBeforeDrawObserver) {
    onBeforeDrawObservable().remove(_onBeforeDrawObserver);
  }
  _onBeforeDrawObserver = onBeforeDrawObservable().add(callback);
}

bool Mesh::get_hasInstances() const
{
  return !instances.empty();
}

bool Mesh::get_hasThinInstances() const
{
  return _thinInstanceDataStorage->instancesCount > 0;
}

std::string Mesh::toString(bool fullDetails)
{
  std::ostringstream oss;
  oss << AbstractMesh::toString(fullDetails);
  oss << ", n vertices: " << getTotalVertices();
  oss << ", parent: "
      << ((!_waitingParentId.empty()) ? _waitingParentId : (parent() ? parent()->name : "NONE"));

  if (!animations.empty()) {
    for (const auto& animation : animations) {
      oss << ", animation[0]: " << animation->toString(fullDetails);
    }
  }

  if (fullDetails) {
    if (_geometry) {
      auto ib = getIndices();
      auto vb = getVerticesData(VertexBuffer::PositionKind);

      if (!vb.empty() && !ib.empty()) {
        oss << ", flat shading: " << (vb.size() / 3 == ib.size() ? "YES" : "NO");
      }
    }
    else {
      oss << ", flat shading: UNKNOWN";
    }
  }
  return oss.str();
}

void Mesh::addSubMesh(SubMesh* subMesh)
{
  subMeshes.emplace_back(subMesh);
}

void Mesh::_unBindEffect()
{
  AbstractMesh::_unBindEffect();

  for (const auto& instance : instances) {
    instance->_unBindEffect();
  }
}

bool Mesh::get_hasLODLevels() const
{
  return !_internalMeshDataInfo->_LODLevels.empty();
}

std::vector<MeshLODLevelPtr>& Mesh::getLODLevels()
{
  return _internalMeshDataInfo->_LODLevels;
}

void Mesh::_sortLODLevels()
{
  auto& _LODLevels = _internalMeshDataInfo->_LODLevels;
  BABYLON::stl_util::sort_js_style(_LODLevels,
                                   [](const MeshLODLevelPtr& a, const MeshLODLevelPtr& b) {
                                     if (a->distance < b->distance) {
                                       return 1;
                                     }
                                     if (a->distance > b->distance) {
                                       return -1;
                                     }
                                     return 0;
                                   });
}

Mesh& Mesh::addLODLevel(float distance, const MeshPtr& mesh)
{
  if (mesh && mesh->_masterMesh) {
    BABYLON_LOG_WARN("Mesh", "You cannot use a mesh as LOD level twice")
    return *this;
  }

  auto level = std::make_unique<MeshLODLevel>(distance, mesh);

  _internalMeshDataInfo->_LODLevels.emplace_back(std::move(level));

  if (mesh) {
    mesh->_masterMesh = this;
  }

  _sortLODLevels();

  return *this;
}

MeshPtr Mesh::getLODLevelAtDistance(float distance)
{
  auto& _LODLevels = _internalMeshDataInfo->_LODLevels;
  for (const auto& level : _LODLevels) {
    if (stl_util::almost_equal(level->distance, distance)) {
      return level->mesh;
    }
  }
  return nullptr;
}

Mesh& Mesh::removeLODLevel(const MeshPtr& mesh)
{
  auto& _LODLevels = _internalMeshDataInfo->_LODLevels;
  for (const auto& level : _LODLevels) {
    if (level->mesh == mesh) {
      stl_util::erase(_LODLevels, level);
      if (mesh) {
        mesh->_masterMesh = nullptr;
      }
    }
  }

  _sortLODLevels();
  return *this;
}

AbstractMesh* Mesh::getLOD(const CameraPtr& camera, BoundingSphere* boundingSphere)
{
  auto& _LODLevels = _internalMeshDataInfo->_LODLevels;
  if (_LODLevels.empty()) {
    return this;
  }

  BoundingSphere* bSphere = nullptr;

  if (boundingSphere) {
    bSphere = boundingSphere;
  }
  else {
    auto boundingInfo = getBoundingInfo();

    bSphere = &boundingInfo->boundingSphere;
  }

  auto distanceToCamera = bSphere->centerWorld.subtract(camera->globalPosition()).length();

  if (_LODLevels.back()->distance > distanceToCamera) {
    if (onLODLevelSelection) {
      onLODLevelSelection(distanceToCamera, this, this);
    }
    return this;
  }

  for (const auto& level : _LODLevels) {
    if (level->distance < distanceToCamera) {
      if (level->mesh) {
        if (level->mesh->delayLoadState == Constants::DELAYLOADSTATE_NOTLOADED) {
          level->mesh->_checkDelayState();
          return this;
        }

        if (level->mesh->delayLoadState == Constants::DELAYLOADSTATE_LOADING) {
          return this;
        }

        level->mesh->_preActivate();
        level->mesh->_updateSubMeshesBoundingInfo(worldMatrixFromCache());
      }

      if (onLODLevelSelection) {
        onLODLevelSelection(distanceToCamera, this, level->mesh.get());
      }
      return level->mesh.get();
    }
  }

  if (onLODLevelSelection) {
    onLODLevelSelection(distanceToCamera, this, this);
  }
  return this;
}

GeometryPtr& Mesh::get_geometry()
{
  return _geometry;
}

void Mesh::setGeometry(const GeometryPtr& iGeometry)
{
  _geometry = iGeometry;
}

size_t Mesh::getTotalVertices() const
{
  if (!_geometry) {
    return 0;
  }
  return _geometry->getTotalVertices();
}

Float32Array Mesh::getVerticesData(const std::string& kind, bool copyWhenShared, bool forceCopy)
{
  if (!_geometry) {
    return Float32Array();
  }
  return _geometry->getVerticesData(kind, copyWhenShared, forceCopy);
}

VertexBufferPtr Mesh::getVertexBuffer(const std::string& kind) const
{
  if (!_geometry) {
    return nullptr;
  }
  return _geometry->getVertexBuffer(kind);
}

bool Mesh::isVerticesDataPresent(const std::string& kind) const
{
  if (!_geometry) {
    if (!_delayInfo.empty()) {
      return std::find_if(_delayInfo.begin(), _delayInfo.end(),
                          [&](VertexBuffer* item) { return item->getKind() == kind; })
             != _delayInfo.end();
    }
    return false;
  }

  return _geometry->isVerticesDataPresent(kind);
}

bool Mesh::isVertexBufferUpdatable(const std::string& kind) const
{
  if (!_geometry) {
    if (!_delayInfo.empty()) {
      return std::find_if(_delayInfo.begin(), _delayInfo.end(),
                          [&](VertexBuffer* item) { return item->getKind() == kind; })
             != _delayInfo.end();
    }
    return false;
  }

  return _geometry->isVertexBufferUpdatable(kind);
}

std::vector<std::string> Mesh::getVerticesDataKinds() const
{
  if (!_geometry) {
    std::vector<std::string> result;
    if (!_delayInfo.empty()) {
      for (const auto& vertexBuffer : _delayInfo) {
        result.emplace_back(vertexBuffer->getKind());
      }
    }
    return result;
  }
  return _geometry->getVerticesDataKinds();
}

size_t Mesh::getTotalIndices() const
{
  if (!_geometry) {
    return 0;
  }
  return _geometry->getTotalIndices();
}

IndicesArray Mesh::getIndices(bool copyWhenShared, bool forceCopy)
{
  if (!_geometry) {
    return Uint32Array();
  }
  return _geometry->getIndices(copyWhenShared, forceCopy);
}

bool Mesh::get_isBlocked() const
{
  return _masterMesh != nullptr;
}

bool Mesh::isReady(bool completeCheck, bool forceInstanceSupport)
{
  if (delayLoadState == Constants::DELAYLOADSTATE_LOADING) {
    return false;
  }

  if (!AbstractMesh::isReady(completeCheck)) {
    return false;
  }

  if (subMeshes.empty()) {
    return true;
  }

  if (!completeCheck) {
    return true;
  }

  auto engine = getEngine();
  auto scene  = getScene();
  auto hardwareInstancedRendering
    = forceInstanceSupport
      || (engine->getCaps().instancedArrays && (!instances.empty() || hasThinInstances()));

  computeWorldMatrix();

  auto mat = material() ? material() : scene->defaultMaterial();
  if (mat) {
    if (mat->_storeEffectOnSubMeshes) {
      for (const auto& subMesh : subMeshes) {
        auto effectiveMaterial = subMesh->getMaterial();
        if (effectiveMaterial) {
          if (effectiveMaterial->_storeEffectOnSubMeshes) {
            if (!effectiveMaterial->isReadyForSubMesh(this, subMesh.get(),
                                                      hardwareInstancedRendering)) {
              return false;
            }
          }
          else {
            if (!effectiveMaterial->isReady(this, hardwareInstancedRendering)) {
              return false;
            }
          }
        }
      }
    }
    else {
      if (!mat->isReady(this, hardwareInstancedRendering)) {
        return false;
      }
    }
  }

  // Shadows
  for (const auto& light : _lightSources) {
    auto generator = light->getShadowGenerator();

    if (generator
        && (generator->getShadowMap()->renderList().empty()
            || stl_util::contains(generator->getShadowMap()->renderList(), this))) {
      for (const auto& subMesh : subMeshes) {
        const auto subMeshMaterial = subMesh->getMaterial();
        if (!generator->isReady(subMesh.get(), hardwareInstancedRendering,
                                subMeshMaterial ? subMeshMaterial->needAlphaBlendingForMesh(*this) :
                                                  false)) {
          return false;
        }
      }
    }
  }

  // LOD
  for (const auto& lod : _internalMeshDataInfo->_LODLevels) {
    if (lod->mesh && !lod->mesh->isReady(hardwareInstancedRendering)) {
      return false;
    }
  }

  return true;
}

bool Mesh::get_areNormalsFrozen() const
{
  return _internalMeshDataInfo->_areNormalsFrozen;
}

Mesh& Mesh::freezeNormals()
{
  _internalMeshDataInfo->_areNormalsFrozen = true;
  return *this;
}

Mesh& Mesh::unfreezeNormals()
{
  _internalMeshDataInfo->_areNormalsFrozen = false;
  return *this;
}

void Mesh::set_overridenInstanceCount(size_t count)
{
  _instanceDataStorage->overridenInstanceCount = count;
}

size_t Mesh::get_thinInstanceCount() const
{
  return _thinInstanceDataStorage->instancesCount;
}

void Mesh::set_thinInstanceCount(size_t value)
{
  const auto numMaxInstances = (_thinInstanceDataStorage->matrixData.size()) / 16;

  if (value <= numMaxInstances) {
    _thinInstanceDataStorage->instancesCount = value;
  }
}

void Mesh::_preActivate()
{
  auto& internalDataInfo = *_internalMeshDataInfo;
  auto sceneRenderId     = getScene()->getRenderId();
  if (internalDataInfo._preActivateId == sceneRenderId) {
    return;
  }

  internalDataInfo._preActivateId        = sceneRenderId;
  _instanceDataStorage->visibleInstances = nullptr;
}

Mesh* Mesh::_preActivateForIntermediateRendering(int renderId)
{
  if (_instanceDataStorage->visibleInstances) {
    _instanceDataStorage->visibleInstances->intermediateDefaultRenderId = renderId;
  }
  return this;
}

Mesh& Mesh::_registerInstanceForRenderId(InstancedMesh* instance, int renderId)
{
  if (!_instanceDataStorage->visibleInstances) {
    _instanceDataStorage->visibleInstances                  = std::make_shared<_VisibleInstances>();
    _instanceDataStorage->visibleInstances->defaultRenderId = renderId;
    _instanceDataStorage->visibleInstances->selfDefaultRenderId = _renderId;
  }

  if (_instanceDataStorage->visibleInstances->meshes.find(renderId)
      == _instanceDataStorage->visibleInstances->meshes.end()) {
    if (_instanceDataStorage->previousRenderId.has_value() && _instanceDataStorage->isFrozen
        && *_instanceDataStorage->previousRenderId
             < _instanceDataStorage->visibleInstances->meshes.size()) {
      _instanceDataStorage->visibleInstances->meshes[*_instanceDataStorage->previousRenderId] = {};
    }
    _instanceDataStorage->previousRenderId                   = static_cast<unsigned int>(renderId);
    _instanceDataStorage->visibleInstances->meshes[renderId] = std::vector<InstancedMesh*>();
  }

  _instanceDataStorage->visibleInstances->meshes[renderId].emplace_back(instance);

  return *this;
}

void Mesh::_afterComputeWorldMatrix()
{
  AbstractMesh::_afterComputeWorldMatrix();

  if (!hasThinInstances()) {
    return;
  }

  if (!doNotSyncBoundingInfo) {
    thinInstanceRefreshBoundingInfo(false);
  }
}

void Mesh::_postActivate()
{
  if (edgesShareWithInstances && edgesRenderer() && edgesRenderer()->isEnabled && _renderingGroup) {
    if (!stl_util::contains(_renderingGroup->_edgesRenderers, edgesRenderer())) {
      _renderingGroup->_edgesRenderers.emplace_back(edgesRenderer());
    }
    edgesRenderer()->customInstances.emplace_back(getWorldMatrix());
  }
}

Mesh& Mesh::refreshBoundingInfo(bool applySkeleton)
{
  if (_boundingInfo && _boundingInfo->isLocked()) {
    return *this;
  }

  std::optional<Vector2> bias = geometry() ? geometry()->boundingBias() : std::nullopt;
  _refreshBoundingInfo(_getPositionData(applySkeleton), bias);
  return *this;
}

SubMeshPtr Mesh::_createGlobalSubMesh(bool force)
{
  auto totalVertices = getTotalVertices();
  if (!totalVertices || (!_geometry->isReady() && getIndices().empty())) {
    return nullptr;
  }

  // Check if we need to recreate the submeshes
  if (!subMeshes.empty()) {
    auto ib = getIndices();

    if (ib.empty()) {
      return nullptr;
    }

    const auto totalIndices = ib.size();
    auto needToRecreate     = false;

    if (force) {
      needToRecreate = true;
    }
    else {
      for (const auto& submesh : subMeshes) {
        if (submesh->indexStart + submesh->indexCount > totalIndices) {
          needToRecreate = true;
          break;
        }

        if (submesh->verticesStart + submesh->verticesCount > totalVertices) {
          needToRecreate = true;
          break;
        }
      }
    }

    if (!needToRecreate) {
      return subMeshes[0];
    }
  }

  releaseSubMeshes();
  return SubMesh::New(0, 0, totalVertices, 0, getTotalIndices(), shared_from_base<Mesh>());
}

void Mesh::subdivide(size_t count)
{
  if (count < 1) {
    return;
  }

  size_t totalIndices    = getTotalIndices();
  size_t subdivisionSize = (totalIndices / count);
  size_t offset          = 0;

  // Ensure that subdivisionSize is a multiple of 3
  while (subdivisionSize % 3 != 0) {
    ++subdivisionSize;
  }

  releaseSubMeshes();
  for (unsigned int index = 0; index < count; ++index) {
    if (offset >= totalIndices) {
      break;
    }

    SubMesh::CreateFromIndices(0, static_cast<unsigned>(offset),
                               static_cast<int>(index) == static_cast<int>(count) - 1 ?
                                 totalIndices - offset :
                                 subdivisionSize,
                               shared_from_base<Mesh>());

    offset += subdivisionSize;
  }

  synchronizeInstances();
}

AbstractMesh* Mesh::setVerticesData(const std::string& kind, const Float32Array& data,
                                    bool updatable, const std::optional<size_t>& stride)
{
  if (!_geometry) {
    auto vertexData = std::make_unique<VertexData>();
    vertexData->set(data, kind);

    auto scene = getScene();

    Geometry::New(Geometry::RandomId(), scene, vertexData.get(), updatable, this);
  }
  else {
    _geometry->setVerticesData(kind, data, updatable, stride);
  }

  return this;
}

void Mesh::removeVerticesData(const std::string& kind)
{
  if (!_geometry) {
    return;
  }

  _geometry->removeVerticesData(kind);
}

void Mesh::markVerticesDataAsUpdatable(const std::string& kind, bool updatable)
{
  auto vb = getVertexBuffer(kind);

  if (!vb || vb->isUpdatable() == updatable) {
    return;
  }

  setVerticesData(kind, getVerticesData(kind), updatable);
}

Mesh& Mesh::setVerticesBuffer(const VertexBufferPtr& buffer)
{
  if (!_geometry) {
    _geometry = Geometry::CreateGeometryForMesh(this);
  }

  _geometry->setVerticesBuffer(std::move(buffer));

  return *this;
}

AbstractMesh* Mesh::updateVerticesData(const std::string& kind, const Float32Array& data,
                                       bool updateExtends, bool makeItUnique)
{
  if (!_geometry) {
    return this;
  }
  if (!makeItUnique) {
    _geometry->updateVerticesData(kind, data, updateExtends);
  }
  else {
    makeGeometryUnique();
    updateVerticesData(kind, data, updateExtends, false);
  }

  return this;
}

Mesh& Mesh::updateMeshPositions(std::function<void(Float32Array& positions)> positionFunction,
                                bool computeNormals)
{
  auto positions = getVerticesData(VertexBuffer::PositionKind);
  if (positions.empty()) {
    return *this;
  }

  positionFunction(positions);
  updateVerticesData(VertexBuffer::PositionKind, positions, false, false);

  if (computeNormals) {
    auto indices = getIndices();
    auto normals = getVerticesData(VertexBuffer::NormalKind);

    if (normals.empty()) {
      return *this;
    }

    VertexData::ComputeNormals(positions, indices, normals);
    updateVerticesData(VertexBuffer::NormalKind, normals, false, false);
  }
  return *this;
}

Mesh& Mesh::makeGeometryUnique()
{
  if (!_geometry) {
    return *this;
  }

  if (_geometry->meshes().size() == 1) {
    return *this;
  }

  auto oldGeometry = _geometry;
  auto geometry_   = _geometry->copy(Geometry::RandomId());
  oldGeometry->releaseForMesh(this, true);
  geometry_->applyToMesh(this);

  return *this;
}

AbstractMesh* Mesh::setIndices(const IndicesArray& indices, size_t totalVertices, bool updatable)
{
  if (!_geometry) {
    auto vertexData     = std::make_unique<VertexData>();
    vertexData->indices = indices;

    auto scene = getScene();

    Geometry(Geometry::RandomId(), scene, vertexData.get(), updatable, this);
  }
  else {
    _geometry->setIndices(indices, totalVertices, updatable);
  }

  return this;
}

AbstractMesh& Mesh::updateIndices(const IndicesArray& indices, const std::optional<int>& offset,
                                  bool gpuMemoryOnly)
{
  if (!_geometry) {
    return *this;
  }

  _geometry->updateIndices(indices, offset.value_or(0), gpuMemoryOnly);
  return *this;
}

Mesh& Mesh::toLeftHanded()
{
  if (!_geometry) {
    return *this;
  }

  _geometry->toLeftHanded();

  return *this;
}

void Mesh::_bind(SubMesh* subMesh, const EffectPtr& effect, unsigned int fillMode)
{
  if (!_geometry) {
    return;
  }

  auto engine = getScene()->getEngine();

  // Wireframe
  WebGLDataBufferPtr indexToBind = nullptr;

  if (_unIndexed) {
    indexToBind = nullptr;
  }
  else {
    switch (fillMode) {
      case Material::PointFillMode:
        indexToBind = nullptr;
        break;
      case Material::WireFrameFillMode: {
        const auto& linesIndexBuffer = subMesh->_getLinesIndexBuffer(getIndices(), engine);
        indexToBind                  = linesIndexBuffer ? linesIndexBuffer : nullptr;
      } break;
      default:
      case Material::TriangleFillMode:
        indexToBind = _geometry->getIndexBuffer();
        break;
    }
  }

  // VBOs
  _geometry->_bind(effect, indexToBind);
}

void Mesh::_draw(SubMesh* subMesh, int fillMode, size_t instancesCount, bool /*alternate*/)
{
  if (!_geometry || _geometry->getVertexBuffers().empty()
      || (!_unIndexed && !_geometry->getIndexBuffer())) {
    return;
  }

  {
    _internalMeshDataInfo->_onBeforeDrawObservable.notifyObservers(this);
  }

  auto scene  = getScene();
  auto engine = scene->getEngine();

  const auto _fillMode = static_cast<unsigned int>(fillMode);
  if (_unIndexed || _fillMode == Material::PointFillMode) {
    // or triangles as points
    engine->drawArraysType(_fillMode, static_cast<int>(subMesh->verticesStart),
                           static_cast<int>(subMesh->verticesCount),
                           static_cast<int>(instancesCount));
  }
  else if (_fillMode == Material::WireFrameFillMode) {
    // Triangles as wireframe
    engine->drawElementsType(_fillMode, 0, static_cast<int>(subMesh->_linesIndexCount),
                             static_cast<int>(instancesCount));
  }
  else {
    engine->drawElementsType(_fillMode, static_cast<int>(subMesh->indexStart),
                             static_cast<int>(subMesh->indexCount),
                             static_cast<int>(instancesCount));
  }
}

Mesh& Mesh::registerBeforeRender(const std::function<void(Mesh* mesh, EventState&)>& func)
{
  onBeforeRenderObservable().add(func);
  return *this;
}

Mesh& Mesh::unregisterBeforeRender(const std::function<void(Mesh* mesh, EventState&)>& func)
{
  onBeforeRenderObservable().removeCallback(func);
  return *this;
}

Mesh& Mesh::registerAfterRender(const std::function<void(Mesh* mesh, EventState&)>& func)
{
  onAfterRenderObservable().add(func);
  return *this;
}

Mesh& Mesh::unregisterAfterRender(const std::function<void(Mesh* mesh, EventState&)>& func)
{
  onAfterRenderObservable().removeCallback(func);
  return *this;
}

_InstancesBatchPtr Mesh::_getInstancesRenderList(size_t subMeshId, bool isReplacementMode)
{
  if (_instanceDataStorage->isFrozen && _instanceDataStorage->previousBatch) {
    return _instanceDataStorage->previousBatch;
  }
  auto scene                     = getScene();
  auto isInIntermediateRendering = scene->_isInIntermediateRendering();
  auto onlyForInstances          = isInIntermediateRendering ?
                                     _internalAbstractMeshDataInfo._onlyForInstancesIntermediate :
                                     _internalAbstractMeshDataInfo._onlyForInstances;
  auto& batchCache               = _instanceDataStorage->batchCache;
  batchCache->mustReturn         = false;
  batchCache->renderSelf[subMeshId]
    = isReplacementMode || (!onlyForInstances && isEnabled() && isVisible);
  batchCache->visibleInstances[subMeshId] = std::vector<InstancedMesh*>();

  if (_instanceDataStorage->visibleInstances && !isReplacementMode) {
    auto& visibleInstances = _instanceDataStorage->visibleInstances;
    auto currentRenderId   = scene->getRenderId();
    auto defaultRenderId
      = (isInIntermediateRendering ? visibleInstances->intermediateDefaultRenderId :
                                     visibleInstances->defaultRenderId);
    batchCache->visibleInstances[subMeshId] = visibleInstances->meshes[currentRenderId];

    if ((batchCache->visibleInstances.find(subMeshId) == batchCache->visibleInstances.end())
        && batchCache->visibleInstances[subMeshId].empty() && defaultRenderId) {
      batchCache->visibleInstances[subMeshId] = visibleInstances->meshes[defaultRenderId];
    }
  }

  if (subMeshId >= batchCache->hardwareInstancedRendering.size()) {
    batchCache->hardwareInstancedRendering.resize(subMeshId + 1);
  }

  batchCache->hardwareInstancedRendering[subMeshId]
    = !isReplacementMode && _instanceDataStorage->hardwareInstancedRendering
      && (batchCache->visibleInstances.find(subMeshId) != batchCache->visibleInstances.end())
      && (!batchCache->visibleInstances[subMeshId].empty());
  _instanceDataStorage->previousBatch = batchCache;

  return batchCache;
}

Mesh& Mesh::_renderWithInstances(SubMesh* subMesh, unsigned int fillMode,
                                 const _InstancesBatchPtr& batch, const EffectPtr& effect,
                                 Engine* engine)
{
  if (batch->visibleInstances.find(subMesh->_id) == batch->visibleInstances.end()) {
    return *this;
  }

  auto& visibleInstances = batch->visibleInstances[subMesh->_id];
  if (visibleInstances.empty()) {
    return *this;
  }

  auto& instanceStorage           = _instanceDataStorage;
  auto currentInstancesBufferSize = instanceStorage->instancesBufferSize;
  auto& instancesBuffer           = instanceStorage->instancesBuffer;
  size_t matricesCount            = visibleInstances.size() + 1;
  size_t bufferSize               = matricesCount * 16 * 4;

  while (instanceStorage->instancesBufferSize < bufferSize) {
    instanceStorage->instancesBufferSize *= 2;
  }

  if (instanceStorage->instancesData.empty()
      || currentInstancesBufferSize != instanceStorage->instancesBufferSize) {
    instanceStorage->instancesData = Float32Array(instanceStorage->instancesBufferSize / 4);
  }

  auto offset         = 0u;
  auto instancesCount = 0u;

  const auto& renderSelf = batch->renderSelf[subMesh->_id];

  const auto needUpdateBuffer
    = !instancesBuffer || currentInstancesBufferSize != instanceStorage->instancesBufferSize;

  if (!_instanceDataStorage->manualUpdate && (!instanceStorage->isFrozen || needUpdateBuffer)) {
    auto world = _effectiveMesh()->getWorldMatrix();
    if (renderSelf) {
      world.copyToArray(instanceStorage->instancesData, offset);
      offset += 16;
      instancesCount++;
    }

    if (!visibleInstances.empty()) {
      for (auto instance : visibleInstances) {
        instance->getWorldMatrix().copyToArray(instanceStorage->instancesData, offset);
        offset += 16;
        ++instancesCount;
      }
    }
  }
  else {
    instancesCount = (renderSelf ? 1 : 0) + static_cast<unsigned int>(visibleInstances.size());
  }

  if (needUpdateBuffer) {
    if (instancesBuffer) {
      instancesBuffer->dispose();
    }

    instancesBuffer
      = std::make_shared<Buffer>(engine, instanceStorage->instancesData, true, 16, false, true);

    setVerticesBuffer(instancesBuffer->createVertexBuffer(VertexBuffer::World0Kind, 0, 4));
    setVerticesBuffer(instancesBuffer->createVertexBuffer(VertexBuffer::World1Kind, 4, 4));
    setVerticesBuffer(instancesBuffer->createVertexBuffer(VertexBuffer::World2Kind, 8, 4));
    setVerticesBuffer(instancesBuffer->createVertexBuffer(VertexBuffer::World3Kind, 12, 4));
  }
  else {
    if (!_instanceDataStorage->isFrozen) {
      instancesBuffer->updateDirectly(instanceStorage->instancesData, 0, instancesCount);
    }
  }

  _processInstancedBuffers(visibleInstances, renderSelf);

  // Stats
  getScene()->_activeIndices.addCount(subMesh->indexCount * instancesCount, false);

  // Draw
  _bind(subMesh, effect, fillMode);
  _draw(subMesh, static_cast<int>(fillMode), instancesCount);

  engine->unbindInstanceAttributes();

  return *this;
}

size_t Mesh::thinInstanceAdd(Matrix& matrix, bool refresh)
{
  _thinInstanceUpdateBufferSize("matrix", 1);

  const auto index = _thinInstanceDataStorage->instancesCount;

  thinInstanceSetMatrixAt(_thinInstanceDataStorage->instancesCount++, matrix, refresh);

  return index;
}

size_t Mesh::thinInstanceAdd(std::vector<Matrix>& matrix, bool refresh)
{
  _thinInstanceUpdateBufferSize("matrix", matrix.size());

  const auto index = _thinInstanceDataStorage->instancesCount;

  for (size_t i = 0; i < matrix.size(); ++i) {
    thinInstanceSetMatrixAt(_thinInstanceDataStorage->instancesCount++, matrix[i],
                            (i == matrix.size() - 1) && refresh);
  }

  return index;
}

size_t Mesh::thinInstanceAddSelf(bool refresh)
{
  auto matrix = Matrix::IdentityReadOnly();
  return thinInstanceAdd(matrix, refresh);
}

void Mesh::thinInstanceRegisterAttribute(const std::string& kind, unsigned int stride)
{
  removeVerticesData(kind);

  _thinInstanceInitializeUserStorage();

  _userThinInstanceBuffersStorage->strides[kind] = stride;
  _userThinInstanceBuffersStorage->sizes[kind]
    = stride
      * std::max(32, static_cast<int>(_thinInstanceDataStorage->instancesCount)); // Initial size
  _userThinInstanceBuffersStorage->data[kind]
    = {static_cast<float>(_userThinInstanceBuffersStorage->sizes[kind])};
  _userThinInstanceBuffersStorage->vertexBuffers[kind] = std::make_shared<VertexBuffer>(
    getEngine(), _userThinInstanceBuffersStorage->data[kind], kind, true, false, stride, true);

  setVerticesBuffer(_userThinInstanceBuffersStorage->vertexBuffers[kind]);
}

bool Mesh::thinInstanceSetMatrixAt(size_t index, Matrix& iMatrix, bool refresh)
{
  if (_thinInstanceDataStorage->matrixData.empty()
      || index >= _thinInstanceDataStorage->instancesCount) {
    return false;
  }

  auto& matrixData = _thinInstanceDataStorage->matrixData;

  iMatrix.copyToArray(matrixData, static_cast<unsigned>(index) * 16);

  if (_thinInstanceDataStorage->worldMatrices) {
    if (index >= _thinInstanceDataStorage->worldMatrices->size()) {
      _thinInstanceDataStorage->worldMatrices->resize(index + 1);
    }
    (*_thinInstanceDataStorage->worldMatrices)[index] = iMatrix;
  }

  if (refresh) {
    thinInstanceBufferUpdated("matrix");

    if (!doNotSyncBoundingInfo) {
      thinInstanceRefreshBoundingInfo(false);
    }
  }

  return true;
}

bool Mesh::thinInstanceSetAttributeAt(const std::string& kind, size_t index,
                                      const Float32Array& value, bool refresh)
{
  if (!_userThinInstanceBuffersStorage || _userThinInstanceBuffersStorage->data[kind].empty()
      || index >= _thinInstanceDataStorage->instancesCount) {
    return false;
  }

  // make sure the buffer for the kind attribute is big enough
  _thinInstanceUpdateBufferSize(kind, 0);

  auto offset = index * _userThinInstanceBuffersStorage->strides[kind];
  for (const auto v : value) {
    _userThinInstanceBuffersStorage->data[kind][offset++] = v;
  }

  if (refresh) {
    thinInstanceBufferUpdated(kind);
  }

  return true;
}

void Mesh::thinInstanceSetBuffer(const std::string& kind, const Float32Array& buffer,
                                 unsigned int stride, bool staticBuffer)
{
  stride = (stride == 0) ? 16u : stride;

  if (kind == "matrix") {
    if (_thinInstanceDataStorage->matrixBuffer) {
      _thinInstanceDataStorage->matrixBuffer->dispose();
      _thinInstanceDataStorage->matrixBuffer = nullptr;
    }
    _thinInstanceDataStorage->matrixBufferSize = !buffer.empty() ? buffer.size() : 32 * stride;
    _thinInstanceDataStorage->matrixData       = buffer;
    _thinInstanceDataStorage->worldMatrices    = std::nullopt;

    if (!buffer.empty()) {
      _thinInstanceDataStorage->instancesCount = buffer.size() / stride;

      const auto matrixBuffer
        = std::make_shared<Buffer>(getEngine(), buffer, !staticBuffer, stride, false, true);

      _thinInstanceDataStorage->matrixBuffer = matrixBuffer;

      setVerticesBuffer(matrixBuffer->createVertexBuffer("world0", 0, 4));
      setVerticesBuffer(matrixBuffer->createVertexBuffer("world1", 4, 4));
      setVerticesBuffer(matrixBuffer->createVertexBuffer("world2", 8, 4));
      setVerticesBuffer(matrixBuffer->createVertexBuffer("world3", 12, 4));

      if (!doNotSyncBoundingInfo) {
        thinInstanceRefreshBoundingInfo(false);
      }
    }
    else {
      _thinInstanceDataStorage->instancesCount = 0;
      if (!doNotSyncBoundingInfo) {
        // mesh has no more thin instances, so need to recompute the bounding box because it's the
        // regular mesh that will now be displayed
        refreshBoundingInfo(true);
      }
    }
  }
  else {
    if (buffer.empty()) {
      if (_userThinInstanceBuffersStorage && !_userThinInstanceBuffersStorage->data[kind].empty()) {
        removeVerticesData(kind);
        _userThinInstanceBuffersStorage->data.erase(kind);
        _userThinInstanceBuffersStorage->strides.erase(kind);
        _userThinInstanceBuffersStorage->sizes.erase(kind);
        _userThinInstanceBuffersStorage->vertexBuffers.erase(kind);
      }
    }
    else {
      _thinInstanceInitializeUserStorage();

      _userThinInstanceBuffersStorage->data[kind]          = buffer;
      _userThinInstanceBuffersStorage->strides[kind]       = stride;
      _userThinInstanceBuffersStorage->sizes[kind]         = buffer.size();
      _userThinInstanceBuffersStorage->vertexBuffers[kind] = std::make_shared<VertexBuffer>(
        getEngine(), buffer, kind, !staticBuffer, false, stride, true);

      setVerticesBuffer(_userThinInstanceBuffersStorage->vertexBuffers[kind]);
    }
  }
}

void Mesh::thinInstanceBufferUpdated(const std::string& kind)
{
  if (kind == "matrix") {
    if (_thinInstanceDataStorage->matrixBuffer) {
      _thinInstanceDataStorage->matrixBuffer->updateDirectly(
        _thinInstanceDataStorage->matrixData, 0, _thinInstanceDataStorage->instancesCount);
    }
  }
  else if (_userThinInstanceBuffersStorage
           && stl_util::contains(_userThinInstanceBuffersStorage->vertexBuffers, kind)
           && _userThinInstanceBuffersStorage->vertexBuffers[kind]) {
    _userThinInstanceBuffersStorage->vertexBuffers[kind]->updateDirectly(
      _userThinInstanceBuffersStorage->data[kind], 0);
  }
}

void Mesh::thinInstancePartialBufferUpdate(const std::string& kind, const Float32Array& data,
                                           size_t offset)
{
  if (kind == "matrix") {
    if (_thinInstanceDataStorage->matrixBuffer) {
      _thinInstanceDataStorage->matrixBuffer->updateDirectly(data, offset);
    }
  }
  else if (_userThinInstanceBuffersStorage
           && stl_util::contains(_userThinInstanceBuffersStorage->vertexBuffers, kind)
           && _userThinInstanceBuffersStorage->vertexBuffers[kind]) {
    _userThinInstanceBuffersStorage->vertexBuffers[kind]->updateDirectly(data, offset);
  }
}

std::vector<Matrix> Mesh::thinInstanceGetWorldMatrices()
{
  if (_thinInstanceDataStorage->matrixData.empty() || !_thinInstanceDataStorage->matrixBuffer) {
    return {};
  }
  const auto& matrixData = _thinInstanceDataStorage->matrixData;

  if (!_thinInstanceDataStorage->worldMatrices) {
    _thinInstanceDataStorage->worldMatrices = std::vector<Matrix>();

    for (unsigned int i = 0; i < _thinInstanceDataStorage->instancesCount; ++i) {
      _thinInstanceDataStorage->worldMatrices->emplace_back(Matrix::FromArray(matrixData, i * 16));
    }
  }

  return *_thinInstanceDataStorage->worldMatrices;
}

void Mesh::thinInstanceRefreshBoundingInfo(bool forceRefreshParentInfo)
{
  if (_thinInstanceDataStorage->matrixData.empty() || !_thinInstanceDataStorage->matrixBuffer) {
    return;
  }

  auto& vectors = _thinInstanceDataStorage->boundingVectors;

  if (forceRefreshParentInfo) {
    vectors.clear();
    refreshBoundingInfo(true);
  }

  const auto boundingInfo = getBoundingInfo();
  const auto matrixData   = _thinInstanceDataStorage->matrixData;

  if (vectors.empty()) {
    for (size_t v = 0; v < boundingInfo->boundingBox.vectors.size(); ++v) {
      vectors.emplace_back(boundingInfo->boundingBox.vectors[v]);
    }
  }

  TmpVectors::Vector3Array[0].setAll(std::numeric_limits<float>::max());    // min
  TmpVectors::Vector3Array[1].setAll(std::numeric_limits<float>::lowest()); // max

  for (unsigned int i = 0; i < _thinInstanceDataStorage->instancesCount; ++i) {
    Matrix::FromArrayToRef(matrixData, i * 16, TmpVectors::MatrixArray[0]);

    for (size_t v = 0; v < vectors.size(); ++v) {
      Vector3::TransformCoordinatesToRef(vectors[v], TmpVectors::MatrixArray[0],
                                         TmpVectors::Vector3Array[2]);
      TmpVectors::Vector3Array[0].minimizeInPlace(TmpVectors::Vector3Array[2]);
      TmpVectors::Vector3Array[1].maximizeInPlace(TmpVectors::Vector3Array[2]);
    }
  }

  boundingInfo->reConstruct(TmpVectors::Vector3Array[0], TmpVectors::Vector3Array[1]);

  _updateBoundingInfo();
}

void Mesh::_thinInstanceUpdateBufferSize(const std::string& kind, size_t numInstances)
{
  const auto kindIsMatrix = kind == "matrix";

  if (!kindIsMatrix
      && (!_userThinInstanceBuffersStorage
          || !stl_util::contains(_userThinInstanceBuffersStorage->strides, kind)
          || !_userThinInstanceBuffersStorage->strides[kind])) {
    return;
  }

  const auto stride      = kindIsMatrix ? 16 : _userThinInstanceBuffersStorage->strides[kind];
  const auto currentSize = kindIsMatrix ? _thinInstanceDataStorage->matrixBufferSize :
                                          _userThinInstanceBuffersStorage->sizes[kind];
  auto data              = kindIsMatrix ? _thinInstanceDataStorage->matrixData :
                                          _userThinInstanceBuffersStorage->data[kind];

  const auto bufferSize = (_thinInstanceDataStorage->instancesCount + numInstances) * stride;

  auto newSize = currentSize;

  while (newSize < bufferSize) {
    newSize *= 2;
  }

  if (data.empty() || currentSize != newSize) {
    if (data.empty()) {
      data = Float32Array(newSize);
    }
    else {
      auto newData = Float32Array(newSize);
      for (size_t i = 0; i < data.size(); ++i) {
        newData[i] = data[i];
      }
      data = newData;
    }

    if (kindIsMatrix) {
      _thinInstanceDataStorage->matrixBuffer->dispose();

      const auto matrixBuffer
        = std::make_shared<Buffer>(getEngine(), data, true, stride, false, true);

      _thinInstanceDataStorage->matrixBuffer     = matrixBuffer;
      _thinInstanceDataStorage->matrixData       = data;
      _thinInstanceDataStorage->matrixBufferSize = newSize;

      setVerticesBuffer(matrixBuffer->createVertexBuffer("world0", 0, 4));
      setVerticesBuffer(matrixBuffer->createVertexBuffer("world1", 4, 4));
      setVerticesBuffer(matrixBuffer->createVertexBuffer("world2", 8, 4));
      setVerticesBuffer(matrixBuffer->createVertexBuffer("world3", 12, 4));
    }
    else {
      _userThinInstanceBuffersStorage->vertexBuffers[kind]->dispose();

      _userThinInstanceBuffersStorage->data[kind]  = data;
      _userThinInstanceBuffersStorage->sizes[kind] = newSize;
      _userThinInstanceBuffersStorage->vertexBuffers[kind]
        = std::make_shared<VertexBuffer>(getEngine(), data, kind, true, false, stride, true);

      setVerticesBuffer(_userThinInstanceBuffersStorage->vertexBuffers[kind]);
    }
  }
}

void Mesh::_thinInstanceInitializeUserStorage()
{
  if (!_userThinInstanceBuffersStorage) {
    _userThinInstanceBuffersStorage = std::make_unique<_UserThinInstanceBuffersStorage>();
  }
}

void Mesh::_renderWithThinInstances(SubMesh* subMesh, unsigned int fillMode,
                                    const EffectPtr& effect, Engine* engine)
{
  // Stats
  const auto instancesCount = _thinInstanceDataStorage->instancesCount;

  getScene()->_activeIndices.addCount(subMesh->indexCount * instancesCount, false);

  // Draw
  _bind(subMesh, effect, fillMode);
  _draw(subMesh, fillMode, instancesCount);

  engine->unbindInstanceAttributes();
}

void Mesh::registerInstancedBuffer(const std::string& kind, size_t stride)
{
  // Remove existing one
  if (_userInstancedBuffersStorage
      && stl_util::contains(_userInstancedBuffersStorage->vertexBuffers, kind)
      && _userInstancedBuffersStorage->vertexBuffers[kind]) {
    _userInstancedBuffersStorage->vertexBuffers[kind]->dispose();
    _userInstancedBuffersStorage->vertexBuffers.erase(kind);
  }

  // Creates the instancedBuffer field if not present
  if (!_userInstancedBuffersStorage) {
    _userInstancedBuffersStorage = UserInstancedBuffersStorage{};
    if (getEngine()->getCaps().vertexArrayObject) {
      _userInstancedBuffersStorage->vertexArrayObjects
        = std::unordered_map<std::string, WebGLVertexArrayObjectPtr>();
    }
    else {
      _userInstancedBuffersStorage->vertexArrayObjects = std::nullopt;
    }
  }

  // Creates an empty property for this kind
  instancedBuffers[kind] = {0};

  _userInstancedBuffersStorage->strides[kind] = stride;
  _userInstancedBuffersStorage->sizes[kind]   = stride * 32; // Initial size
  _userInstancedBuffersStorage->data[kind]
    = Float32Array(_userInstancedBuffersStorage->sizes[kind]);
  _userInstancedBuffersStorage->vertexBuffers[kind] = std::make_unique<VertexBuffer>(
    getEngine(), _userInstancedBuffersStorage->data[kind], kind, true, false, stride, true);

  for (const auto& instance : instances) {
    instance->instancedBuffers[kind] = nullptr;
  }

  _invalidateInstanceVertexArrayObject();
}

void Mesh::_processInstancedBuffers(const std::vector<InstancedMesh*>& /*visibleInstances*/,
                                    bool /*renderSelf*/)
{
}

void Mesh::_invalidateInstanceVertexArrayObject()
{
  if (!_userInstancedBuffersStorage
      || !_userInstancedBuffersStorage->vertexArrayObjects.has_value()) {
    return;
  }

  for (const auto& [kind, vaos] : *_userInstancedBuffersStorage->vertexArrayObjects) {
    getEngine()->releaseVertexArrayObject(vaos);
  }

  _userInstancedBuffersStorage->vertexArrayObjects = {};
}

Mesh& Mesh::_processRendering(
  AbstractMesh* renderingMesh, SubMesh* subMesh, const EffectPtr& effect, int fillMode,
  const _InstancesBatchPtr& batch, bool hardwareInstancedRendering,
  std::function<void(bool isInstance, const Matrix& world, Material* effectiveMaterial)>
    iOnBeforeDraw,
  Material* effectiveMaterial)
{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  if (hardwareInstancedRendering && subMesh->getRenderingMesh()->hasThinInstances()) {
    _renderWithThinInstances(subMesh, fillMode, effect, engine);
    return *this;
  }

  if (hardwareInstancedRendering) {
    _renderWithInstances(subMesh, static_cast<unsigned>(fillMode), batch, effect, engine);
  }
  else {
    size_t instanceCount = 0;
    if (batch->renderSelf[subMesh->_id]) {
      // Draw
      if (iOnBeforeDraw) {
        iOnBeforeDraw(false, renderingMesh->_effectiveMesh()->getWorldMatrix(), effectiveMaterial);
      }
      ++instanceCount;

      _draw(subMesh, fillMode, _instanceDataStorage->overridenInstanceCount);
    }

    auto& visibleInstancesForSubMesh = batch->visibleInstances[subMesh->_id];
    if (!visibleInstancesForSubMesh.empty()) {
      const auto visibleInstanceCount = visibleInstancesForSubMesh.size();
      instanceCount += visibleInstanceCount;

      // Stats
      for (size_t instanceIndex = 0; instanceIndex < visibleInstanceCount; ++instanceIndex) {
        const auto& instance = visibleInstancesForSubMesh[instanceIndex];

        // World
        auto world = instance->getWorldMatrix();
        if (iOnBeforeDraw) {
          iOnBeforeDraw(true, world, effectiveMaterial);
        }

        // Draw
        _draw(subMesh, fillMode);
      }
    }

    // Stats
    scene->_activeIndices.addCount(subMesh->indexCount * instanceCount, false);
  }

  return *this;
}

void Mesh::_rebuild()
{
  if (_instanceDataStorage->instancesBuffer) {
    // Dispose instance buffer to be recreated in _renderWithInstances when rendered
    _instanceDataStorage->instancesBuffer->dispose();
    _instanceDataStorage->instancesBuffer = nullptr;
  }
  AbstractMesh::_rebuild();
}

void Mesh::_freeze()
{
  if (subMeshes.empty()) {
    return;
  }

  // Prepare batches
  for (unsigned int index = 0; index < subMeshes.size(); ++index) {
    _getInstancesRenderList(index);
  }

  _effectiveMaterial             = nullptr;
  _instanceDataStorage->isFrozen = true;
}

void Mesh::_unFreeze()
{
  _instanceDataStorage->isFrozen      = false;
  _instanceDataStorage->previousBatch = nullptr;
}

Mesh& Mesh::render(SubMesh* subMesh, bool enableAlphaMode,
                   const AbstractMeshPtr& effectiveMeshReplacement)
{
  auto& scene = *getScene();

  if (_internalAbstractMeshDataInfo._isActiveIntermediate) {
    _internalAbstractMeshDataInfo._isActiveIntermediate = false;
  }
  else {
    _internalAbstractMeshDataInfo._isActive = false;
  }

  if (_checkOcclusionQuery()) {
    return *this;
  }

  // Managing instances
  auto batch = _getInstancesRenderList(subMesh->_id, effectiveMeshReplacement != nullptr);

  if (batch->mustReturn) {
    return *this;
  }

  // Checking geometry state
  if (!_geometry || _geometry->getVertexBuffers().empty()
      || (!_unIndexed && !_geometry->getIndexBuffer())) {
    return *this;
  }

  /* if (_internalMeshDataInfo->_onBeforeRenderObservable) */ {
    _internalMeshDataInfo->_onBeforeRenderObservable.notifyObservers(this);
  }

  auto engine                     = scene.getEngine();
  auto hardwareInstancedRendering = batch->hardwareInstancedRendering[subMesh->_id]
                                    || subMesh->getRenderingMesh()->hasThinInstances();
  auto& instanceDataStorage = *_instanceDataStorage;

  // Material
  auto iMaterial = subMesh->getMaterial();

  if (!iMaterial) {
    return *this;
  }

  // Material
  if (!instanceDataStorage.isFrozen || !_effectiveMaterial || _effectiveMaterial != iMaterial) {
    if (iMaterial->_storeEffectOnSubMeshes) {
      if (!iMaterial->isReadyForSubMesh(this, subMesh, hardwareInstancedRendering)) {
        return *this;
      }
    }
    else if (!iMaterial->isReady(this, hardwareInstancedRendering)) {
      return *this;
    }

    _effectiveMaterial = iMaterial;
  }

  // Alpha mode
  if (enableAlphaMode) {
    engine->setAlphaMode(static_cast<unsigned>(_effectiveMaterial->alphaMode()));
  }

  EffectPtr effect = nullptr;
  if (_effectiveMaterial->_storeEffectOnSubMeshes) {
    effect = subMesh->effect();
  }
  else {
    effect = _effectiveMaterial->getEffect();
  }

  for (const auto& step : scene._beforeRenderingMeshStage) {
    step.action(this, subMesh, batch, effect ? effect.get() : nullptr);
  }

  if (!effect) {
    return *this;
  }

  auto& effectiveMesh = effectiveMeshReplacement ? *effectiveMeshReplacement : *_effectiveMesh();

  std::optional<unsigned int> sideOrientation = std::nullopt;

  if (!instanceDataStorage.isFrozen
      && (_effectiveMaterial->backFaceCulling() || overrideMaterialSideOrientation.has_value())) {
    const auto mainDeterminant = effectiveMesh._getWorldMatrixDeterminant();
    sideOrientation            = overrideMaterialSideOrientation;
    if (!sideOrientation.has_value()) {
      sideOrientation = static_cast<unsigned>(_effectiveMaterial->sideOrientation);
    }
    if (mainDeterminant < 0.f) {
      sideOrientation = (sideOrientation == Material::ClockWiseSideOrientation ?
                           Material::CounterClockWiseSideOrientation :
                           Material::ClockWiseSideOrientation);
    }
    instanceDataStorage.sideOrientation = *sideOrientation;
  }
  else {
    sideOrientation = instanceDataStorage.sideOrientation;
  }

  auto reverse = _effectiveMaterial->_preBind(effect, sideOrientation);

  if (_effectiveMaterial->forceDepthWrite) {
    engine->setDepthWrite(true);
  }

  // Bind
  auto fillMode
    = scene.forcePointsCloud() ?
        Material::PointFillMode :
        (scene.forceWireframe() ? Material::WireFrameFillMode : _effectiveMaterial->fillMode());

  /* if (_internalMeshDataInfo->_onBeforeBindObservable) */ {
    _internalMeshDataInfo->_onBeforeBindObservable.notifyObservers(this);
  }

  // Binding will be done later because we need to add more info to the VB
  if (!hardwareInstancedRendering) {
    _bind(subMesh, effect, fillMode);
  }

  auto _world = effectiveMesh.getWorldMatrix();

  if (_effectiveMaterial->_storeEffectOnSubMeshes) {
    _effectiveMaterial->bindForSubMesh(_world, this, subMesh);
  }
  else {
    _effectiveMaterial->bind(_world, this);
  }

  if (!_effectiveMaterial->backFaceCulling() && _effectiveMaterial->separateCullingPass) {
    engine->setState(true, _effectiveMaterial->zOffset, false, !reverse);
    _processRendering(
      this, subMesh, effect, static_cast<int>(fillMode), batch, hardwareInstancedRendering,
      [&](bool isInstance, Matrix world, Material* effectiveMaterial) {
        _onBeforeDraw(isInstance, world, effectiveMaterial);
      },
      _effectiveMaterial.get());
    engine->setState(true, _effectiveMaterial->zOffset, false, reverse);
  }

  // Draw
  _processRendering(this, subMesh, effect, static_cast<int>(fillMode), batch,
                    hardwareInstancedRendering,
                    [&](bool isInstance, Matrix world, Material* effectiveMaterial) {
                      _onBeforeDraw(isInstance, world, effectiveMaterial);
                    });

  // Unbind
  _effectiveMaterial->unbind();

  for (const auto& step : scene._afterRenderingMeshStage) {
    step.action(this, subMesh, batch, effect ? effect.get() : nullptr);
  }

  /* if (_internalMeshDataInfo->_onAfterRenderObservable) */ {
    _internalMeshDataInfo->_onAfterRenderObservable.notifyObservers(this);
  }

  return *this;
}

Mesh& Mesh::_onBeforeDraw(bool isInstance, Matrix& world, Material* effectiveMaterial)
{
  if (isInstance && effectiveMaterial) {
    effectiveMaterial->bindOnlyWorldMatrix(world);
  }
  return *this;
}

std::vector<IParticleSystemPtr> Mesh::getEmittedParticleSystems()
{
  std::vector<IParticleSystemPtr> results;
  for (const auto& particleSystem : getScene()->particleSystems) {
    const auto& emitter = particleSystem->emitter;
    if (std::holds_alternative<AbstractMeshPtr>(emitter)
        && (std::get<AbstractMeshPtr>(emitter).get() == this)) {
      results.emplace_back(particleSystem);
    }
  }

  return results;
}

std::vector<IParticleSystemPtr> Mesh::getHierarchyEmittedParticleSystems()
{
  std::vector<IParticleSystemPtr> results;
  auto descendants = getDescendants();
  descendants.emplace_back(this);

  for (const auto& particleSystem : getScene()->particleSystems) {
    if (std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)) {
      if (std::find(descendants.begin(), descendants.end(),
                    std::get<AbstractMeshPtr>(particleSystem->emitter))
          != descendants.end()) {
        results.emplace_back(particleSystem);
      }
    }
  }

  return results;
}

void Mesh::cleanMatrixWeights()
{
  if (isVerticesDataPresent(VertexBuffer::MatricesWeightsKind)) {
    if (isVerticesDataPresent(VertexBuffer::MatricesWeightsExtraKind)) {
      normalizeSkinWeightsAndExtra();
    }
    else {
      normalizeSkinFourWeights();
    }
  }
}

std::vector<NodePtr> Mesh::getChildren()
{
  std::vector<NodePtr> results;
  for (const auto& mesh : getScene()->meshes) {
    if (mesh->parent() == this) {
      results.emplace_back(mesh);
    }
  }

  return results;
}

void Mesh::normalizeSkinFourWeights()
{
  auto matricesWeights = getVerticesData(VertexBuffer::MatricesWeightsKind);
  auto numWeights      = matricesWeights.size();

  for (size_t a = 0; a < numWeights; a += 4) {
    // accumulate weights
    auto t = matricesWeights[a] + matricesWeights[a + 1] + matricesWeights[a + 2]
             + matricesWeights[a + 3];
    // check for invalid weight and just set it to 1.
    if (t == 0.f) {
      matricesWeights[a] = 1.f;
    }
    else {
      // renormalize so everything adds to 1 use reciprical
      auto recip = 1.f / t;
      matricesWeights[a] *= recip;
      matricesWeights[a + 1] *= recip;
      matricesWeights[a + 2] *= recip;
      matricesWeights[a + 3] *= recip;
    }
  }
  setVerticesData(VertexBuffer::MatricesWeightsKind, matricesWeights);
}

void Mesh::normalizeSkinWeightsAndExtra()
{
  auto matricesWeightsExtra = getVerticesData(VertexBuffer::MatricesWeightsExtraKind);
  auto matricesWeights      = getVerticesData(VertexBuffer::MatricesWeightsKind);
  auto numWeights           = matricesWeights.size();

  for (size_t a = 0; a < numWeights; a += 4) {
    // accumulate weights
    auto t = matricesWeights[a] + matricesWeights[a + 1] + matricesWeights[a + 2]
             + matricesWeights[a + 3];
    t += matricesWeightsExtra[a] + matricesWeightsExtra[a + 1] + matricesWeightsExtra[a + 2]
         + matricesWeightsExtra[a + 3];
    // check for invalid weight and just set it to 1.
    if (t == 0.f) {
      matricesWeights[a] = 1.f;
    }
    else {
      // renormalize so everything adds to 1 use reciprical
      auto recip = 1.f / t;
      matricesWeights[a] *= recip;
      matricesWeights[a + 1] *= recip;
      matricesWeights[a + 2] *= recip;
      matricesWeights[a + 3] *= recip;
      // same goes for extras
      matricesWeightsExtra[a] *= recip;
      matricesWeightsExtra[a + 1] *= recip;
      matricesWeightsExtra[a + 2] *= recip;
      matricesWeightsExtra[a + 3] *= recip;
    }
  }
  setVerticesData(VertexBuffer::MatricesWeightsKind, matricesWeights);
  setVerticesData(VertexBuffer::MatricesWeightsKind, matricesWeightsExtra);
}

SkinningValidationResult Mesh::validateSkinning()
{
  auto matricesWeightsExtra = getVerticesData(VertexBuffer::MatricesWeightsExtraKind);
  auto matricesWeights      = getVerticesData(VertexBuffer::MatricesWeightsKind);
  if (matricesWeights.empty() || skeleton() == nullptr) {
    return {
      false,        // skinned
      true,         // valid
      "not skinned" // report
    };
  }

  auto numWeights          = matricesWeights.size();
  auto numberNotSorted     = 0u;
  auto missingWeights      = 0u;
  auto maxUsedWeights      = 0u;
  auto numberNotNormalized = 0u;
  auto numInfluences       = matricesWeightsExtra.empty() ? 4u : 8u;
  Float32Array usedWeightCounts;
  for (size_t a = 0; a <= numInfluences; a++) {
    usedWeightCounts[a] = 0;
  }
  const auto toleranceEpsilon = 0.001f;

  for (size_t a = 0; a < numWeights; a += 4) {
    auto lastWeight  = matricesWeights[a];
    auto t           = lastWeight;
    auto usedWeights = (t == 0.f) ? 0u : 1u;

    for (size_t b = 1; b < numInfluences; b++) {
      auto d = b < 4 ? matricesWeights[a + b] : matricesWeightsExtra[a + b - 4];
      if (d > lastWeight) {
        ++numberNotSorted;
      }
      if (d != 0.f) {
        ++usedWeights;
      }
      t += d;
      lastWeight = d;
    }
    // count the buffer weights usage
    usedWeightCounts[usedWeights]++;

    // max influences
    if (usedWeights > maxUsedWeights) {
      maxUsedWeights = usedWeights;
    }

    // check for invalid weight and just set it to 1.
    if (t == 0.f) {
      ++missingWeights;
    }
    else {
      // renormalize so everything adds to 1 use reciprical
      auto recip     = 1.f / t;
      auto tolerance = 0.f;
      for (size_t b = 0; b < numInfluences; ++b) {
        if (b < 4) {
          tolerance += std::abs(matricesWeights[a + b] - (matricesWeights[a + b] * recip));
        }
        else {
          tolerance += std::abs(matricesWeightsExtra[a + b - 4]
                                - (matricesWeightsExtra[a + b - 4] * recip));
        }
      }
      // arbitary epsilon value for dicdating not normalized
      if (tolerance > toleranceEpsilon) {
        numberNotNormalized++;
      }
    }
  }

  // validate bone indices are in range of the skeleton
  auto numBones             = skeleton()->bones.size();
  auto matricesIndices      = getVerticesData(VertexBuffer::MatricesIndicesKind);
  auto matricesIndicesExtra = getVerticesData(VertexBuffer::MatricesIndicesExtraKind);
  auto numBadBoneIndices    = 0u;
  for (size_t a = 0; a < numWeights; a += 4) {
    for (size_t b = 0; b < numInfluences; b++) {
      auto index = b < 4 ? matricesIndices[a + b] : matricesIndicesExtra[a + b - 4];
      if (index >= numBones || index < 0) {
        ++numBadBoneIndices;
      }
    }
  }

  // log mesh stats
  std::ostringstream output;
  output << "Number of Weights = " << (numWeights / 4)
         << "\nMaximum influences = " << maxUsedWeights << "\nMissing Weights = " << missingWeights
         << "\nNot Sorted = " << numberNotSorted << "\nNot Normalized = " << numberNotNormalized
         << "\nWeightCounts = [" << usedWeightCounts.size() << "]"
         << "\nNumber of bones = " << numBones << "\nBad Bone Indices = " << numBadBoneIndices;

  auto isValid = (missingWeights == 0 && numberNotNormalized == 0 && numBadBoneIndices == 0);

  return {
    true,        // skinned
    isValid,     // valid
    output.str() // report
  };
}

Mesh& Mesh::_checkDelayState()
{
  auto scene = getScene();

  if (_geometry) {
    _geometry->load(scene);
  }
  else if (delayLoadState == Constants::DELAYLOADSTATE_NOTLOADED) {
    delayLoadState = Constants::DELAYLOADSTATE_LOADING;

    _queueLoad(scene);
  }

  return *this;
}

Mesh& Mesh::_queueLoad(Scene* scene)
{
  scene->_addPendingData(this);

  const auto getBinaryData = StringTools::contains(delayLoadingFile, ".babylonbinarymeshdata");

  FileTools::LoadFile(
    delayLoadingFile,
    [this, &scene](const std::variant<std::string, ArrayBufferView>& data,
                   const std::string& /*responseURL*/) -> void {
      if (std::holds_alternative<ArrayBufferView>(data)) {
        // _delayLoadingFunction(data, shared_from_base<Mesh>());
      }
      else {
        _delayLoadingFunction(json::parse(std::get<std::string>(data)), shared_from_base<Mesh>());
      }

      for (const auto& instance : instances) {
        instance->refreshBoundingInfo();
        instance->_syncSubMeshes();
      }

      delayLoadState = Constants::DELAYLOADSTATE_LOADED;
      scene->_removePendingData(this);
    },
    nullptr, /* scene->offlineProvider, */ getBinaryData);

  return *this;
}

bool Mesh::isInFrustum(const std::array<Plane, 6>& frustumPlanes, unsigned int /*strategy*/)
{
  if (delayLoadState == Constants::DELAYLOADSTATE_LOADING) {
    return false;
  }

  if (!AbstractMesh::isInFrustum(frustumPlanes)) {
    return false;
  }

  _checkDelayState();

  return true;
}

Mesh& Mesh::setMaterialByID(const std::string& iId)
{
  const auto& materials = getScene()->materials;
  for (std::size_t index = materials.size(); index-- > 0;) {
    if (materials[index]->id == iId) {
      material = materials[index];
      return *this;
    }
  }

  // Multi
  const auto& multiMaterials = getScene()->multiMaterials;
  for (std::size_t index = multiMaterials.size(); index-- > 0;) {
    if (multiMaterials[index]->id == iId) {
      material = multiMaterials[index];
      return *this;
    }
  }

  return *this;
}

std::vector<IAnimatablePtr> Mesh::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (material()) {
    results.emplace_back(material());
  }

  if (skeleton()) {
    results.emplace_back(skeleton());
  }

  return results;
}

// Geometry
Mesh& Mesh::bakeTransformIntoVertices(const Matrix& transform)
{
  // Position
  if (!isVerticesDataPresent(VertexBuffer::PositionKind)) {
    return *this;
  }

  auto _submeshes = std::move(subMeshes);

  _resetPointsArrayCache();

  auto data = getVerticesData(VertexBuffer::PositionKind);
  Float32Array temp;
  unsigned int index;
  for (index = 0; index < data.size(); index += 3) {
    Vector3::TransformCoordinates(Vector3::FromArray(data, index), transform).toArray(temp, index);
  }

  setVerticesData(VertexBuffer::PositionKind, temp,
                  getVertexBuffer(VertexBuffer::PositionKind)->isUpdatable());

  // Normals
  if (isVerticesDataPresent(VertexBuffer::NormalKind)) {
    data = getVerticesData(VertexBuffer::NormalKind);
    temp.clear();
    for (index = 0; index < data.size(); index += 3) {
      Vector3::TransformNormal(Vector3::FromArray(data, index), transform)
        .normalize()
        .toArray(temp, index);
    }
    setVerticesData(VertexBuffer::NormalKind, temp,
                    getVertexBuffer(VertexBuffer::NormalKind)->isUpdatable());
  }

  // flip faces?
  const auto& transformM = transform.m();
  if (transformM[0] * transformM[5] * transformM[10] < 0.f) {
    flipFaces();
  }

  // Restore submeshes
  releaseSubMeshes();
  subMeshes = std::move(_submeshes);

  return *this;
}

// Will apply current transform to mesh and reset world matrix
Mesh& Mesh::bakeCurrentTransformIntoVertices(bool bakeIndependenlyOfChildren)
{
  bakeTransformIntoVertices(computeWorldMatrix(true));
  resetLocalMatrix(bakeIndependenlyOfChildren);
  return *this;
}

// Cache
std::vector<Vector3>& Mesh::get__positions()
{
  if (_geometry) {
    return _geometry->_positions;
  }
  return _emptyPositions;
}

Mesh& Mesh::_resetPointsArrayCache()
{
  if (_geometry) {
    _geometry->_resetPointsArrayCache();
  }
  return *this;
}

bool Mesh::_generatePointsArray()
{
  if (_geometry) {
    return _geometry->_generatePointsArray();
  }

  return false;
}

MeshPtr Mesh::clone(const std::string& iName, Node* newParent, bool doNotCloneChildren,
                    bool clonePhysicsImpostor)
{
  return Mesh::New(iName, getScene(), newParent, this, doNotCloneChildren, clonePhysicsImpostor);
}

void Mesh::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  morphTargetManager = nullptr;

  if (_geometry) {
    _geometry->releaseForMesh(this, true);
  }

  auto& internalDataInfo = *_internalMeshDataInfo;

  internalDataInfo._onBeforeDrawObservable.clear();
  internalDataInfo._onBeforeRenderObservable.clear();
  internalDataInfo._onAfterRenderObservable.clear();

  // Sources
  if (_scene->useClonedMeshMap) {
    if (!internalDataInfo.meshMap.empty()) {
      for (const auto& item : internalDataInfo.meshMap) {
        const auto& mesh = item.second;
        if (mesh) {
          mesh->_internalMeshDataInfo->_source = nullptr;
        }
      }
      internalDataInfo.meshMap.clear();
    }

    if (internalDataInfo._source
        && !internalDataInfo._source->_internalMeshDataInfo->meshMap.empty()) {
      internalDataInfo._source->_internalMeshDataInfo->meshMap.erase(std::to_string(uniqueId));
    }
  }
  else {
    const auto& meshes = getScene()->meshes;
    for (const auto& abstractMesh : meshes) {
      auto mesh = std::static_pointer_cast<Mesh>(abstractMesh);
      if (mesh->_internalMeshDataInfo && mesh->_internalMeshDataInfo->_source
          && mesh->_internalMeshDataInfo->_source == this) {
        mesh->_internalMeshDataInfo->_source = nullptr;
      }
    }
  }

  internalDataInfo._source = nullptr;

  // Instances
  _disposeInstanceSpecificData();

  // Thin instances
  _disposeThinInstanceSpecificData();

  AbstractMesh::dispose(doNotRecurse, disposeMaterialAndTextures);
}

void Mesh::_disposeInstanceSpecificData()
{
  if (_instanceDataStorage->instancesBuffer) {
    _instanceDataStorage->instancesBuffer->dispose();
    _instanceDataStorage->instancesBuffer = nullptr;
  }

  for (const auto& instance : instances) {
    instance->dispose();
  }
  instances.clear();

  for (const auto& item : instancedBuffers) {
    const auto& kind = item.first;
    if (_userInstancedBuffersStorage
        && stl_util::contains(_userInstancedBuffersStorage->vertexBuffers, kind)
        && _userInstancedBuffersStorage->vertexBuffers[kind]) {
      _userInstancedBuffersStorage->vertexBuffers[kind]->dispose();
    }
  }

  _invalidateInstanceVertexArrayObject();

  instancedBuffers = {};
}

void Mesh::_disposeThinInstanceSpecificData()
{
  if (_thinInstanceDataStorage && _thinInstanceDataStorage->matrixBuffer) {
    _thinInstanceDataStorage->matrixBuffer->dispose();
    _thinInstanceDataStorage->matrixBuffer = nullptr;
  }
}

Mesh& Mesh::applyDisplacementMap(const std::string& url, float minHeight, float maxHeight,
                                 std::function<void(Mesh* mesh)> onSuccess,
                                 const std::optional<Vector2>& uvOffset,
                                 const std::optional<Vector2>& uvScale, bool forceUpdate)
{
  FileTools::LoadImageFromUrl(
    url,
    [&](const Image& img) {
      auto heightMapWidth  = static_cast<unsigned>(img.width);
      auto heightMapHeight = static_cast<unsigned>(img.height);
      // Create VertexData from map data
      applyDisplacementMapFromBuffer(img.data, heightMapWidth, heightMapHeight, minHeight,
                                     maxHeight, uvOffset, uvScale, forceUpdate);
      // execute success callback, if set
      if (onSuccess) {
        onSuccess(this);
      }
    },
    nullptr, false);

  return *this;
}

void Mesh::applyDisplacementMapFromBuffer(const Uint8Array& buffer, unsigned int heightMapWidth,
                                          unsigned int heightMapHeight, float minHeight,
                                          float maxHeight, const std::optional<Vector2>& iUvOffset,
                                          const std::optional<Vector2>& iUvScale, bool forceUpdate)
{
  if (!isVerticesDataPresent(VertexBuffer::PositionKind)
      || !isVerticesDataPresent(VertexBuffer::NormalKind)
      || !isVerticesDataPresent(VertexBuffer::UVKind)) {
    BABYLON_LOG_WARN("Mesh",
                     "Cannot call applyDisplacementMap: Given mesh is not "
                     "complete. Position, Normal or UV are missing.")
    return;
  }

  auto positions = getVerticesData(VertexBuffer::PositionKind, true, true);
  auto normals   = getVerticesData(VertexBuffer::NormalKind);
  auto uvs       = getVerticesData(VertexBuffer::UVKind);
  auto iPosition = Vector3::Zero();
  auto normal    = Vector3::Zero();
  auto uv        = Vector2::Zero();

  auto uvOffset = iUvOffset.has_value() ? *iUvOffset : Vector2::Zero();
  auto uvScale  = iUvScale.has_value() ? *iUvScale : Vector2(1.f, 1.f);

  for (unsigned int index = 0; index < positions.size(); index += 3) {
    Vector3::FromArrayToRef(positions, index, iPosition);
    Vector3::FromArrayToRef(normals, index, normal);
    Vector2::FromArrayToRef(uvs, (index / 3) * 2, uv);

    // Compute height
    auto u
      = static_cast<unsigned int>(std::abs(uv.x * uvScale.x + uvOffset.x) * (heightMapWidth - 1.f))
        % heightMapWidth;
    auto v
      = static_cast<unsigned int>(std::abs(uv.y * uvScale.y + uvOffset.y) * (heightMapHeight - 1.f))
        % heightMapHeight;

    auto pos = (u + v * heightMapWidth) * 4;
    auto r   = buffer[pos] / 255.f;
    auto g   = buffer[pos + 1] / 255.f;
    auto b   = buffer[pos + 2] / 255.f;

    auto gradient = r * 0.3f + g * 0.59f + b * 0.11f;

    normal.normalize();
    normal.scaleInPlace(minHeight + (maxHeight - minHeight) * gradient);
    iPosition = iPosition.add(normal);

    iPosition.toArray(positions, index);
  }

  VertexData::ComputeNormals(positions, getIndices(), normals);

  if (forceUpdate) {
    setVerticesData(VertexBuffer::PositionKind, positions);
    setVerticesData(VertexBuffer::NormalKind, normals);
  }
  else {
    updateVerticesData(VertexBuffer::PositionKind, positions);
    updateVerticesData(VertexBuffer::NormalKind, normals);
  }
}

Mesh& Mesh::convertToFlatShadedMesh()
{
  auto kinds = getVerticesDataKinds();
  std::unordered_map<std::string, VertexBufferPtr> vbs;
  std::unordered_map<std::string, Float32Array> data;
  std::unordered_map<std::string, Float32Array> newdata;
  bool updatableNormals  = false;
  unsigned int kindIndex = 0;
  std::string kind;
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind              = kinds[kindIndex];
    auto vertexBuffer = getVertexBuffer(kind);

    if (kind == VertexBuffer::NormalKind) {
      updatableNormals = vertexBuffer->isUpdatable();
      kinds.erase(kinds.begin() + kindIndex);
      --kindIndex;
      continue;
    }

    vbs[kind]     = vertexBuffer;
    data[kind]    = vbs[kind]->getData();
    newdata[kind] = Float32Array();
  }

  // Save previous submeshes
  auto previousSubmeshes = subMeshes; // stl_util::to_raw_ptr_vector(subMeshes);

  auto indices      = getIndices();
  auto totalIndices = getTotalIndices();

  // Generating unique vertices per face
  unsigned int index;
  for (index = 0; index < totalIndices; ++index) {
    auto vertexIndex = static_cast<unsigned int>(indices[index]);

    for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
      kind        = kinds[kindIndex];
      auto stride = static_cast<size_t>(vbs[kind]->getStrideSize());

      for (unsigned int offset = 0; offset < stride; ++offset) {
        newdata[kind].emplace_back(data[kind][vertexIndex * stride + offset]);
      }
    }
  }

  // Updating faces & normal
  Float32Array normals;
  auto& positions = newdata[VertexBuffer::PositionKind];
  for (index = 0; index < totalIndices; index += 3) {
    indices[index]     = static_cast<uint16_t>(index);
    indices[index + 1] = static_cast<uint16_t>(index + 1);
    indices[index + 2] = static_cast<uint16_t>(index + 2);

    auto p1 = Vector3::FromArray(positions, index * 3);
    auto p2 = Vector3::FromArray(positions, (index + 1) * 3);
    auto p3 = Vector3::FromArray(positions, (index + 2) * 3);

    auto p1p2 = p1.subtract(p2);
    auto p3p2 = p3.subtract(p2);

    auto normal = Vector3::Normalize(Vector3::Cross(p1p2, p3p2));

    // Store same normals for every vertex
    for (unsigned int localIndex = 0; localIndex < 3; ++localIndex) {
      normals.emplace_back(normal.x);
      normals.emplace_back(normal.y);
      normals.emplace_back(normal.z);
    }
  }

  setIndices(indices);
  setVerticesData(VertexBuffer::NormalKind, normals, updatableNormals);

  // Updating vertex buffers
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind = kinds[kindIndex];
    setVerticesData(kind, newdata[kind], vbs[kind]->isUpdatable());
  }

  // Updating submeshes
  releaseSubMeshes();
  for (const auto& previousOne : previousSubmeshes) {
    SubMesh::AddToMesh(previousOne->materialIndex, static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount, static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount, shared_from_base<Mesh>());
  }

  synchronizeInstances();
  return *this;
}

Mesh& Mesh::convertToUnIndexedMesh()
{
  auto kinds = getVerticesDataKinds();
  std::map<std::string, VertexBufferPtr> vbs;
  std::map<std::string, Float32Array> data;
  std::map<std::string, Float32Array> newdata;
  unsigned int kindIndex = 0;
  std::string kind;
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind          = kinds[kindIndex];
    vbs[kind]     = getVertexBuffer(kind);
    data[kind]    = vbs[kind]->getData();
    newdata[kind] = Float32Array();
  }

  // Save previous submeshes
  auto previousSubmeshes = stl_util::to_raw_ptr_vector(subMeshes);

  auto indices      = getIndices();
  auto totalIndices = getTotalIndices();

  // Generating unique vertices per face
  unsigned int index;
  for (index = 0; index < totalIndices; ++index) {
    auto vertexIndex = static_cast<unsigned int>(indices[index]);

    for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
      kind        = kinds[kindIndex];
      auto stride = static_cast<size_t>(vbs[kind]->getStrideSize());

      for (unsigned int offset = 0; offset < stride; ++offset) {
        newdata[kind].emplace_back(data[kind][vertexIndex * stride + offset]);
      }
    }
  }

  // Updating indices
  for (index = 0; index < totalIndices; index += 3) {
    indices[index + 0] = static_cast<uint16_t>(index + 0);
    indices[index + 1] = static_cast<uint16_t>(index + 1);
    indices[index + 2] = static_cast<uint16_t>(index + 2);
  }

  setIndices(indices);

  // Updating vertex buffers
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind = kinds[kindIndex];
    setVerticesData(kind, newdata[kind], vbs[kind]->isUpdatable());
  }

  // Updating vertex buffers
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind = kinds[kindIndex];
    setVerticesData(kind, newdata[kind], vbs[kind]->isUpdatable());
  }

  // Updating submeshes
  releaseSubMeshes();
  for (const auto& previousOne : previousSubmeshes) {
    SubMesh::AddToMesh(previousOne->materialIndex, static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount, static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount, shared_from_base<Mesh>());
  }

  _unIndexed = true;

  synchronizeInstances();
  return *this;
}

Mesh& Mesh::flipFaces(bool flipNormals)
{
  auto vertex_data = VertexData::ExtractFromMesh(this);
  unsigned int i;
  if (flipNormals && isVerticesDataPresent(VertexBuffer::NormalKind)
      && !vertex_data->normals.empty()) {
    for (i = 0; i < vertex_data->normals.size(); ++i) {
      vertex_data->normals[i] *= -1;
    }
  }

  if (!vertex_data->indices.empty()) {
    for (i = 0; i < vertex_data->indices.size(); i += 3) {
      // reassign indices
      std::swap(vertex_data->indices[i + 1], vertex_data->indices[i + 2]);
    }
  }

  vertex_data->applyToMesh(*this, isVertexBufferUpdatable(VertexBuffer::PositionKind));
  return *this;
}

void Mesh::increaseVertices(size_t numberPerEdge)
{
  auto vertex_data     = VertexData::ExtractFromMesh(this);
  auto& uvs            = vertex_data->uvs;
  auto& currentIndices = vertex_data->indices;
  auto& positions      = vertex_data->positions;
  auto& normals        = vertex_data->normals;

  if (currentIndices.empty() || positions.empty() || normals.empty() || uvs.empty()) {
    BABYLON_LOG_WARN("Mesh", "VertexData contains null entries")
  }
  else {
    auto segments
      = numberPerEdge + 1; // segments per current facet edge, become sides of new facets
    std::vector<Uint32Array> tempIndices;
    for (size_t i = 0; i < segments + 1; ++i) {
      tempIndices[i] = {};
    }
    auto a             = 0u; // vertex index of one end of a side
    auto b             = 0u; // vertex index of other end of the side
    auto deltaPosition = Vector3(0.f, 0.f, 0.f);
    auto deltaNormal   = Vector3(0.f, 0.f, 0.f);
    auto deltaUV       = Vector2(0.f, 0.f);
    Uint32Array indices;
    Uint32Array vertexIndex;
    std::vector<std::vector<Uint32Array>> side;
    auto len         = 0ull;
    auto positionPtr = positions.size();
    auto uvPtr       = uvs.size();

    for (size_t i = 0; i < currentIndices.size(); i += 3) {
      vertexIndex[0] = currentIndices[i];
      vertexIndex[1] = currentIndices[i + 1];
      vertexIndex[2] = currentIndices[i + 2];
      for (unsigned j = 0; j < 3; j++) {
        a = vertexIndex[j];
        b = vertexIndex[(j + 1) % 3];
        if (a >= side.size() && b >= side.size()) {
          side.resize(std::max(a + 1, b + 1));
          side[a] = {};
          side[b] = {};
        }
        else {
          if (a >= side.size()) {
            side.resize(a + 1);
            side[a] = {};
          }
          if (b >= side.size()) {
            side.resize(b + 1);
            side[b] = {};
          }
        }
        if (b >= side[a].size() && a >= side[b].size()) {
          side[a].resize(b + 1);
          deltaPosition.x = (positions[3 * b] - positions[3 * a]) / segments;
          deltaPosition.y = (positions[3 * b + 1] - positions[3 * a + 1]) / segments;
          deltaPosition.z = (positions[3 * b + 2] - positions[3 * a + 2]) / segments;
          deltaNormal.x   = (normals[3 * b] - normals[3 * a]) / segments;
          deltaNormal.y   = (normals[3 * b + 1] - normals[3 * a + 1]) / segments;
          deltaNormal.z   = (normals[3 * b + 2] - normals[3 * a + 2]) / segments;
          deltaUV.x       = (uvs[2 * b] - uvs[2 * a]) / segments;
          deltaUV.y       = (uvs[2 * b + 1] - uvs[2 * a + 1]) / segments;
          side[a][b].emplace_back(a);
          for (size_t k = 1; k < segments; k++) {
            side[a][b].emplace_back(static_cast<uint32_t>(positions.size() / 3));
            positions[positionPtr] = positions[3 * a] + k * deltaPosition.x;
            normals[positionPtr++] = normals[3 * a] + k * deltaNormal.x;
            positions[positionPtr] = positions[3 * a + 1] + k * deltaPosition.y;
            normals[positionPtr++] = normals[3 * a + 1] + k * deltaNormal.y;
            positions[positionPtr] = positions[3 * a + 2] + k * deltaPosition.z;
            normals[positionPtr++] = normals[3 * a + 2] + k * deltaNormal.z;
            uvs[uvPtr++]           = uvs[2 * a] + k * deltaUV.x;
            uvs[uvPtr++]           = uvs[2 * a + 1] + k * deltaUV.y;
          }
          side[a][b].emplace_back(b);
          side[b][a] = {};
          len        = side[a][b].size();
          for (size_t idx = 0; idx < len; idx++) {
            side[b][a][idx] = side[a][b][len - 1 - idx];
          }
        }
      }
      // Calculate positions, normals and uvs of new internal vertices
      tempIndices[0][0] = currentIndices[i];
      tempIndices[1][0] = side[currentIndices[i]][currentIndices[i + 1]][1];
      tempIndices[1][1] = side[currentIndices[i]][currentIndices[i + 2]][1];
      for (size_t k = 2; k < segments; k++) {
        tempIndices[k][0] = side[currentIndices[i]][currentIndices[i + 1]][k];
        tempIndices[k][k] = side[currentIndices[i]][currentIndices[i + 2]][k];
        deltaPosition.x = (positions[3 * tempIndices[k][k]] - positions[3 * tempIndices[k][0]]) / k;
        deltaPosition.y
          = (positions[3 * tempIndices[k][k] + 1] - positions[3 * tempIndices[k][0] + 1]) / k;
        deltaPosition.z
          = (positions[3 * tempIndices[k][k] + 2] - positions[3 * tempIndices[k][0] + 2]) / k;
        deltaNormal.x = (normals[3 * tempIndices[k][k]] - normals[3 * tempIndices[k][0]]) / k;
        deltaNormal.y
          = (normals[3 * tempIndices[k][k] + 1] - normals[3 * tempIndices[k][0] + 1]) / k;
        deltaNormal.z
          = (normals[3 * tempIndices[k][k] + 2] - normals[3 * tempIndices[k][0] + 2]) / k;
        deltaUV.x = (uvs[2 * tempIndices[k][k]] - uvs[2 * tempIndices[k][0]]) / k;
        deltaUV.y = (uvs[2 * tempIndices[k][k] + 1] - uvs[2 * tempIndices[k][0] + 1]) / k;
        for (size_t j = 1; j < k; j++) {
          tempIndices[k][j]      = static_cast<unsigned>(positions.size() / 3);
          positions[positionPtr] = positions[3 * tempIndices[k][0]] + j * deltaPosition.x;
          normals[positionPtr++] = normals[3 * tempIndices[k][0]] + j * deltaNormal.x;
          positions[positionPtr] = positions[3 * tempIndices[k][0] + 1] + j * deltaPosition.y;
          normals[positionPtr++] = normals[3 * tempIndices[k][0] + 1] + j * deltaNormal.y;
          positions[positionPtr] = positions[3 * tempIndices[k][0] + 2] + j * deltaPosition.z;
          normals[positionPtr++] = normals[3 * tempIndices[k][0] + 2] + j * deltaNormal.z;
          uvs[uvPtr++]           = uvs[2 * tempIndices[k][0]] + j * deltaUV.x;
          uvs[uvPtr++]           = uvs[2 * tempIndices[k][0] + 1] + j * deltaUV.y;
        }
      }
      tempIndices[segments] = side[currentIndices[i + 1]][currentIndices[i + 2]];

      // reform indices
      stl_util::concat(indices, {tempIndices[0][0], tempIndices[1][0], tempIndices[1][1]});
      for (size_t k = 1; k < segments; k++) {
        size_t j = 0;
        for (j = 0; j < k; j++) {
          stl_util::concat(indices,
                           {tempIndices[k][j], tempIndices[k + 1][j], tempIndices[k + 1][j + 1]});
          stl_util::concat(indices,
                           {tempIndices[k][j], tempIndices[k + 1][j + 1], tempIndices[k][j + 1]});
        }
        stl_util::concat(indices,
                         {tempIndices[k][j], tempIndices[k + 1][j], tempIndices[k + 1][j + 1]});
      }
    }

    vertex_data->indices = std::move(indices);
    vertex_data->applyToMesh(*this, isVertexBufferUpdatable(VertexBuffer::PositionKind));
  }
}

void Mesh::forceSharedVertices()
{
  auto vertex_data       = VertexData::ExtractFromMesh(this);
  auto& currentUVs       = vertex_data->uvs;
  auto& currentIndices   = vertex_data->indices;
  auto& currentPositions = vertex_data->positions;
  auto& currentColors    = vertex_data->colors;

  if (currentIndices.empty() || currentPositions.empty()) {
    BABYLON_LOG_WARN("Mesh", "VertexData contains empty entries")
  }
  else {
    Float32Array positions;
    Uint32Array indices;
    Float32Array uvs;
    Float32Array colors;
    // lists facet vertex positions (a,b,c) as string "a|b|c"
    std::vector<std::string> pstring;

    auto indexPtr = 0ull;                                 // pointer to next available index value
    std::unordered_map<std::string, int> uniquePositions; // unique vertex positions
    auto ptr = 0;                                         // pointer to element in uniquePositions
    IndicesArray facet;

    for (size_t i = 0; i < currentIndices.size(); i += 3) {
      facet
        = {currentIndices[i], currentIndices[i + 1], currentIndices[i + 2]}; // facet vertex indices
      pstring.clear();
      for (unsigned int j = 0; j < 3; j++) {
        pstring[j] = "";
        for (unsigned int k = 0; k < 3; k++) {
          // small values make 0
          if (std::abs(currentPositions[3 * facet[j] + k]) < 0.00000001f) {
            currentPositions[3 * facet[j] + k] = 0;
          }
          pstring[j] += std::to_string(currentPositions[3 * facet[j] + k]) + "|";
        }
      }
      // check facet vertices to see that none are repeated
      // do not process any facet that has a repeated vertex, ie is a line
      if (!(pstring[0] == pstring[1] || pstring[0] == pstring[2] || pstring[1] == pstring[2])) {
        // for each facet position check if already listed in uniquePositions
        // if not listed add to uniquePositions and set index pointer
        // if listed use its index in uniquePositions and new index pointer
        for (size_t j = 0; j < 3; j++) {
          ptr = stl_util::contains(uniquePositions, pstring[j]) ? uniquePositions[pstring[j]] : -1;
          if (ptr == -1) {
            uniquePositions[pstring[j]] = static_cast<int>(indexPtr);
            ptr                         = static_cast<int>(indexPtr++);
            // not listed so add individual x, y, z coordinates to positions
            for (unsigned k = 0; k < 3; k++) {
              positions.emplace_back(currentPositions[3 * facet[j] + k]);
            }
            if (!currentColors.empty()) {
              for (unsigned k = 0; k < 4; k++) {
                colors.emplace_back(currentColors[4 * facet[j] + k]);
              }
            }
            if (!currentUVs.empty()) {
              for (unsigned k = 0; k < 2; k++) {
                uvs.emplace_back(currentUVs[2 * facet[j] + k]);
              }
            }
          }
          // add new index pointer to indices array
          indices.emplace_back(ptr);
        }
      }
    }

    Float32Array normals;
    VertexData::ComputeNormals(positions, indices, normals);

    // create new vertex data object and update
    vertex_data->positions = std::move(positions);
    vertex_data->indices   = std::move(indices);
    vertex_data->normals   = std::move(normals);
    if (!currentUVs.empty()) {
      vertex_data->uvs = std::move(uvs);
    }
    if (!currentColors.empty()) {
      vertex_data->colors = std::move(colors);
    }

    vertex_data->applyToMesh(*this, isVertexBufferUpdatable(VertexBuffer::PositionKind));
  }
}

InstancedMeshPtr Mesh::createInstance(const std::string& iName)
{
  const auto& iGeometry = geometry();

  if (iGeometry && iGeometry->meshes().size() > 1) {
    for (const auto& other : iGeometry->meshes()) {
      if (other == this) {
        continue;
      }
      other->makeGeometryUnique();
    }
  }

  return InstancedMesh::New(iName, shared_from_base<Mesh>());
}

Mesh& Mesh::synchronizeInstances()
{
  if (_geometry && _geometry->meshes().size() != 1 && instances.size()) {
    makeGeometryUnique();
  }

  for (const auto& instance : instances) {
    instance->_syncSubMeshes();
  }
  return *this;
}

void Mesh::optimizeIndices(const std::function<void(Mesh* mesh)>& successCallback)
{
  successCallback(nullptr);
}

void Mesh::minimizeVertices()
{
  auto _pdata = getVerticesData(VertexBuffer::PositionKind);
  auto _ndata = getVerticesData(VertexBuffer::NormalKind);
  auto _idata = getIndices();

  Float32Array _newPdata; // new positions array
  IndicesArray _newIdata; // new indices array

  auto _mapPtr = 0;                          // new index;
  std::vector<std::string> _uniquePositions; // unique vertex positions
  for (size_t _i = 0; _i < _idata.size(); _i += 3) {
    // facet vertex indices
    IndicesArray _facet{_idata[_i], _idata[_i + 1], _idata[_i + 2]};
    // lists facet vertex positions (x,y,z) as string "xyz""
    std::vector<std::string> _pstring(3);
    for (unsigned int _j = 0; _j < 3; ++_j) { //
      _pstring[_j] = "";
      for (size_t _k = 0; _k < 3; ++_k) {
        // small values make 0
        if (std::abs(_pdata[3 * _facet[_j] + _k]) < 0.0001f) {
          _pdata[3 * _facet[_j] + _k] = 0;
        }
        _pstring[_j] += std::to_string(_pdata[3 * _facet[_j] + _k]) + "|";
      }
      _pstring[_j] = _pstring[_j].substr(0, _pstring[_j].size() - 1);
    }
    // check facet vertices to see that none are repeated
    // do not process any facet that has a repeated vertex, ie is a line
    if (!(_pstring[0] == _pstring[1] || _pstring[0] == _pstring[2] || _pstring[1] == _pstring[2])) {
      // for each facet position check if already listed in uniquePositions
      // if not listed add to uniquePositions and set index pointer
      // if listed use its index in uniquePositions and new index pointer
      for (unsigned int _j = 0; _j < 3; ++_j) {
        int _ptr = stl_util::index_of(_uniquePositions, _pstring[_j]);
        if (_ptr < 0) {
          _uniquePositions.emplace_back(_pstring[_j]);
          _ptr = _mapPtr++;
          // not listed so add individual x, y, z coordinates to new positions
          // array newPdata and add matching normal data to new normals array
          // newNdata
          for (unsigned int _k = 0; _k < 3; ++_k) {
            _newPdata.emplace_back(_pdata[3 * _facet[_j] + _k]);
          }
        }
        // add new index pointer to new indices array newIdata
        _newIdata.emplace_back(_ptr);
      }
    }
  }

  Float32Array _newNdata; // new normal data

  VertexData::ComputeNormals(_newPdata, _newIdata, _newNdata);

  // create new vertex data object and update
  auto _vertexData       = std::make_unique<VertexData>();
  _vertexData->positions = std::move(_newPdata);
  _vertexData->indices   = std::move(_newIdata);
  _vertexData->normals   = std::move(_newNdata);

  _vertexData->applyToMesh(*this);
}

void Mesh::serialize(json& /*serializationObject*/) const
{
}

void Mesh::_syncGeometryWithMorphTargetManager()
{
  auto iGeometry = geometry();
  if (!iGeometry) {
    return;
  }

  _markSubMeshesAsAttributesDirty();

  auto iMorphTargetManager = _internalMeshDataInfo->_morphTargetManager;
  if (iMorphTargetManager && iMorphTargetManager->vertexCount()) {
    if (iMorphTargetManager->vertexCount() != getTotalVertices()) {
      BABYLON_LOG_ERROR("Mesh",
                        "Mesh is incompatible with morph targets. Targets and "
                        "mesh must all have the same vertices count.")
      iMorphTargetManager = nullptr;
      return;
    }

    std::string indexStr;
    for (unsigned int index = 0; index < iMorphTargetManager->numInfluencers(); ++index) {
      indexStr         = std::to_string(index);
      auto morphTarget = iMorphTargetManager->getActiveTarget(index);

      const auto& positions = morphTarget->getPositions();
      if (positions.empty()) {
        BABYLON_LOG_ERROR("Mesh", "Invalid morph target. Target must have positions.")
        return;
      }

      iGeometry->setVerticesData(VertexBuffer::PositionKind + indexStr, positions, false, 3);

      const auto& normals = morphTarget->getNormals();
      if (!normals.empty()) {
        iGeometry->setVerticesData(VertexBuffer::NormalKind + indexStr, normals, false, 3);
      }

      const auto& tangents = morphTarget->getTangents();
      if (!tangents.empty()) {
        iGeometry->setVerticesData(VertexBuffer::TangentKind + indexStr, tangents, false, 3);
      }

      const auto& uvs = morphTarget->getUVs();
      if (!uvs.empty()) {
        iGeometry->setVerticesData(VertexBuffer::UVKind + std::string("_") + indexStr, uvs, false,
                                   2);
      }
    }
  }
  else {
    unsigned int index = 0;
    auto indexStr      = std::to_string(index);

    // Positions
    while (iGeometry->isVerticesDataPresent(VertexBuffer::PositionKind + indexStr)) {
      iGeometry->removeVerticesData(VertexBuffer::PositionKind + indexStr);

      if (iGeometry->isVerticesDataPresent(VertexBuffer::NormalKind + indexStr)) {
        iGeometry->removeVerticesData(VertexBuffer::NormalKind + indexStr);
      }
      if (iGeometry->isVerticesDataPresent(VertexBuffer::TangentKind + indexStr)) {
        iGeometry->removeVerticesData(VertexBuffer::TangentKind + indexStr);
      }
      if (iGeometry->isVerticesDataPresent(VertexBuffer::UVKind + indexStr)) {
        iGeometry->removeVerticesData(VertexBuffer::UVKind + std::string("_") + indexStr);
      }
      ++index;
      indexStr = std::to_string(index);
    }
  }
}

std::vector<Vector3> Mesh::createInnerPoints(size_t pointsNb)
{
  const auto& boundInfo = getBoundingInfo();
  const auto diameter   = 2.f * boundInfo->boundingSphere.radius;
  updateFacetData();

  auto positions = getVerticesData(VertexBuffer::PositionKind);
  auto indices   = getIndices();

  auto point = Vector3::Zero();
  std::vector<Vector3> points;
  std::vector<Vector3> directions;

  auto index   = 0u;
  auto id0     = 0u;
  auto id1     = 0u;
  auto id2     = 0u;
  auto v0X     = 0.f;
  auto v0Y     = 0.f;
  auto v0Z     = 0.f;
  auto v1X     = 0.f;
  auto v1Y     = 0.f;
  auto v1Z     = 0.f;
  auto v2X     = 0.f;
  auto v2Y     = 0.f;
  auto v2Z     = 0.f;
  auto vertex0 = Vector3::Zero();
  auto vertex1 = Vector3::Zero();
  auto vertex2 = Vector3::Zero();
  auto vec0    = Vector3::Zero();
  auto vec1    = Vector3::Zero();

  auto lamda         = 0.f;
  auto mu            = 0.f;
  auto norm          = Vector3::Zero();
  auto tang          = Vector3::Zero();
  auto biNorm        = Vector3::Zero();
  auto angle         = 0.f;
  auto facetPlaneVec = Vector3::Zero();

  Ray ray(Vector3::Zero(), Axis::X());
  PickingInfo pickInfo;
  auto facetPoint        = Vector3::Zero();
  auto direction         = Vector3::Zero();
  auto particleDirection = Vector3::Zero();
  auto testPoint         = Vector3::Zero();

  for (size_t p = 0; p < pointsNb; ++p) {
    index = static_cast<unsigned>(std::floor(Scalar::RandomRange(0.f, indices.size() / 3.f)));
    id0   = indices[3 * index];
    id1   = indices[3 * index + 1];
    id2   = indices[3 * index + 2];
    v0X   = positions[3 * id0];
    v0Y   = positions[3 * id0 + 1];
    v0Z   = positions[3 * id0 + 2];
    v1X   = positions[3 * id1];
    v1Y   = positions[3 * id1 + 1];
    v1Z   = positions[3 * id1 + 2];
    v2X   = positions[3 * id2];
    v2Y   = positions[3 * id2 + 1];
    v2Z   = positions[3 * id2 + 2];
    vertex0.set(v0X, v0Y, v0Z);
    vertex1.set(v1X, v1Y, v1Z);
    vertex2.set(v2X, v2Y, v2Z);
    vertex1.subtractToRef(vertex0, vec0);
    vertex2.subtractToRef(vertex1, vec1);

    norm          = getFacetNormal(index).normalize().scale(-1.f);
    tang          = vec0.copy().normalize();
    biNorm        = Vector3::Cross(norm, tang);
    angle         = Scalar::RandomRange(0.f, Math::PI2);
    facetPlaneVec = tang.scale(std::cos(angle)).add(biNorm.scale(std::sin(angle)));
    angle         = Scalar::RandomRange(0.1f, Math::PI);
    direction     = facetPlaneVec.scale(std::cos(angle)).add(norm.scale(std::sin(angle)));

    // form a point inside the facet v0, v1, v2;
    lamda      = Scalar::RandomRange(0.f, 1.f);
    mu         = Scalar::RandomRange(0.f, 1.f);
    facetPoint = vertex0.add(vec0.scale(lamda)).add(vec1.scale(lamda * mu));

    ray.origin    = facetPoint;
    ray.direction = direction;
    ray.length    = diameter;
    pickInfo      = ray.intersectsMesh(this);
    if (pickInfo.hit) {
      float distance = pickInfo.pickedPoint->subtract(facetPoint).length();
      float gap      = Scalar::RandomRange(0.f, 1.f) * distance;
      point          = facetPoint.add(direction.scale(gap));
    }
    else {
      point.set(0.f, 0.f, 0.f);
    }
    points.emplace_back(point);
  }

  return points;
}

bool Mesh::pointIsInside(Vector3 point)
{
  auto boundInfo = getBoundingInfo();
  auto max       = boundInfo->maximum();
  auto min       = boundInfo->minimum();
  auto diameter  = 2.f * boundInfo->boundingSphere.radius;
  if (point.x < min.x || point.x > max.x) {
    return false;
  }
  if (point.y < min.y || point.y > max.y) {
    return false;
  }
  if (point.z < min.z || point.z > max.z) {
    return false;
  }

  auto pointFound = false;
  auto d          = 0.f;
  auto hitCount   = 0;
  Ray ray(Vector3::Zero(), Axis::X(), diameter);
  PickingInfo pickInfo;
  auto direction = Vector3::Zero();

  while (d < 2.f && !pointFound) {
    hitCount      = 0;
    direction     = Axis::X().scale(2.f * (0.5f - d));
    ray.origin    = point;
    ray.direction = direction;
    pickInfo      = ray.intersectsMesh(this);
    while (pickInfo.hit) {
      ++hitCount;
      pickInfo.pickedPoint->addToRef(direction.scale(0.00000001f), point);
      ray.origin = point;
      pickInfo   = ray.intersectsMesh(this);
    }
    if ((hitCount % 2) == 1) {
      pointFound = true;
    }
    else if ((hitCount % 2) == 0 && hitCount > 0) {
      pointFound = true;
    }
    ++d;
  }

  return pointFound;
}

MeshPtr Mesh::Parse(const json& parsedMesh, Scene* scene, const std::string& rootUrl)
{
  MeshPtr mesh = nullptr;
  if (json_util::get_string(parsedMesh, "type") == "GroundMesh") {
    mesh = GroundMesh::Parse(parsedMesh, scene);
  }
  else {
    mesh = Mesh::New(json_util::get_string(parsedMesh, "name"), scene);
  }
  mesh->id = json_util::get_string(parsedMesh, "id");

  // Tags.AddTagsTo(mesh, parsedMesh.tags);

  if (json_util::has_valid_key_value(parsedMesh, "position")) {
    mesh->position = Vector3::FromArray(json_util::get_array<float>(parsedMesh, "position"));
  }

  if (json_util::has_valid_key_value(parsedMesh, "metadata")) {
    mesh->metadata = parsedMesh["metadata"];
  }

  if (json_util::has_valid_key_value(parsedMesh, "rotationQuaternion")) {
    mesh->rotationQuaternion
      = Quaternion::FromArray(json_util::get_array<float>(parsedMesh, "rotationQuaternion"));
  }
  else if (json_util::has_valid_key_value(parsedMesh, "rotation")) {
    mesh->rotation = Vector3::FromArray(json_util::get_array<float>(parsedMesh, "rotation"));
  }

  if (json_util::has_valid_key_value(parsedMesh, "scaling")) {
    mesh->scaling = Vector3::FromArray(json_util::get_array<float>(parsedMesh, "scaling"));
  }

  if (json_util::has_valid_key_value(parsedMesh, "localMatrix")) {
    auto tmpMatrix = Matrix::FromArray(json_util::get_array<float>(parsedMesh, "localMatrix"));
    mesh->setPreTransformMatrix(tmpMatrix);
  }
  else if (json_util::has_valid_key_value(parsedMesh, "pivotMatrix")) {
    auto tmpMatrix = Matrix::FromArray(json_util::get_array<float>(parsedMesh, "pivotMatrix"));
    mesh->setPivotMatrix(tmpMatrix);
  }

  mesh->setEnabled(json_util::get_bool(parsedMesh, "isEnabled", true));
  mesh->isVisible        = json_util::get_bool(parsedMesh, "isVisible", true);
  mesh->infiniteDistance = json_util::get_bool(parsedMesh, "infiniteDistance");

  mesh->showBoundingBox          = json_util::get_bool(parsedMesh, "showBoundingBox");
  mesh->showSubMeshesBoundingBox = json_util::get_bool(parsedMesh, "showSubMeshesBoundingBox");

  if (json_util::has_valid_key_value(parsedMesh, "applyFog")) {
    mesh->applyFog = json_util::get_bool(parsedMesh, "applyFog", true);
  }

  if (json_util::has_valid_key_value(parsedMesh, "isPickable")) {
    mesh->isPickable = json_util::get_bool(parsedMesh, "isPickable", true);
  }

  if (json_util::has_valid_key_value(parsedMesh, "alphaIndex")) {
    mesh->alphaIndex
      = json_util::get_number(parsedMesh, "alphaIndex", std::numeric_limits<int>::max());
  }

  mesh->receiveShadows = json_util::get_bool(parsedMesh, "receiveShadows", false);
  mesh->billboardMode
    = json_util::get_number(parsedMesh, "billboardMode", AbstractMesh::BILLBOARDMODE_NONE);

  if (json_util::has_valid_key_value(parsedMesh, "visibility")) {
    mesh->visibility = json_util::get_number(parsedMesh, "visibility", 1.f);
  }

  mesh->checkCollisions = json_util::get_bool(parsedMesh, "checkCollisions");
  if (json_util::has_valid_key_value(parsedMesh, "overrideMaterialSideOrientation")) {
    mesh->overrideMaterialSideOrientation
      = json_util::get_bool(parsedMesh, "overrideMaterialSideOrientation");
  }

  if (json_util::has_valid_key_value(parsedMesh, "isBlocker")) {
    mesh->isBlocker = json_util::get_bool(parsedMesh, "isBlocker");
  }

  mesh->_shouldGenerateFlatShading = json_util::get_bool(parsedMesh, "useFlatShading");

  // freezeWorldMatrix
  if (json_util::has_valid_key_value(parsedMesh, "freezeWorldMatrix")) {
    mesh->_waitingData.freezeWorldMatrix = json_util::get_bool(parsedMesh, "freezeWorldMatrix");
  }

  // Parent
  if (json_util::has_valid_key_value(parsedMesh, "parentId")) {
    mesh->_waitingParentId = json_util::get_string(parsedMesh, "parentId");
  }

  // Actions
  if (json_util::has_valid_key_value(parsedMesh, "actions")) {
    mesh->_waitingData.actions = json_util::get_array<json>(parsedMesh, "actions");
  }

  // Overlay
  if (json_util::has_valid_key_value(parsedMesh, "overlayAlpha")) {
    mesh->overlayAlpha = json_util::get_number(parsedMesh, "overlayAlpha", 0.5f);
  }

  if (json_util::has_valid_key_value(parsedMesh, "overlayColor")) {
    mesh->overlayColor = Color3::FromArray(json_util::get_array<float>(parsedMesh, "overlayColor"));
  }

  if (json_util::has_valid_key_value(parsedMesh, "renderOverlay")) {
    mesh->renderOverlay = json_util::get_bool(parsedMesh, "renderOverlay");
  }

  // Geometry
  mesh->isUnIndexed    = json_util::get_bool(parsedMesh, "isUnIndexed", false);
  mesh->hasVertexAlpha = json_util::get_bool(parsedMesh, "hasVertexAlpha", false);

  if (json_util::has_valid_key_value(parsedMesh, "delayLoadingFile")) {
    mesh->delayLoadState   = Constants::DELAYLOADSTATE_NOTLOADED;
    mesh->delayLoadingFile = rootUrl + json_util::get_string(parsedMesh, "delayLoadingFile");
    mesh->_boundingInfo    = std::make_unique<BoundingInfo>(
      Vector3::FromArray(json_util::get_array<float>(parsedMesh, "boundingBoxMinimum")),
      Vector3::FromArray(json_util::get_array<float>(parsedMesh, "boundingBoxMaximum")));

    if (json_util::has_valid_key_value(parsedMesh, "_binaryInfo")) {
      mesh->_binaryInfo = json_util::get_string(parsedMesh, "_binaryInfo");
    }

    mesh->_delayInfoKinds.clear();
    if (json_util::has_valid_key_value(parsedMesh, "hasUVs")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UVKind);
    }

    if (json_util::has_valid_key_value(parsedMesh, "hasUVs2")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV2Kind);
    }

    if (json_util::has_valid_key_value(parsedMesh, "hasUVs3")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV3Kind);
    }

    if (json_util::has_valid_key_value(parsedMesh, "hasUVs4")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV4Kind);
    }

    if (json_util::has_valid_key_value(parsedMesh, "hasUVs5")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV5Kind);
    }

    if (json_util::has_valid_key_value(parsedMesh, "hasUVs6")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV6Kind);
    }

    if (json_util::has_valid_key_value(parsedMesh, "hasColors")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::ColorKind);
    }

    if (json_util::has_valid_key_value(parsedMesh, "hasMatricesIndices")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::MatricesIndicesKind);
    }

    if (json_util::has_valid_key_value(parsedMesh, "hasMatricesWeights")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::MatricesWeightsKind);
    }

    mesh->_delayLoadingFunction = Geometry::_ImportGeometry;

    if (SceneLoaderFlags::ForceFullSceneLoadingForIncremental()
        && !json_util::is_null(parsedMesh["localMatrix"])) {
      mesh->_checkDelayState();
    }
  }
  else {
    Geometry::_ImportGeometry(parsedMesh, mesh);
  }

  // Material
  if (json_util::has_valid_key_value(parsedMesh, "materialId")) {
    mesh->setMaterialByID(json_util::get_string(parsedMesh, "materialId"));
  }
  else {
    mesh->material = nullptr;
  }

  // Morph targets
  const auto morphTargetManagerId = json_util::get_number(parsedMesh, "morphTargetManagerId", -1);
  if (morphTargetManagerId > -1) {
    mesh->morphTargetManager
      = scene->getMorphTargetManagerById(static_cast<unsigned>(morphTargetManagerId));
  }

  // Skeleton
  const auto parsedSkeletonId = json_util::get_number(parsedMesh, "skeletonId", -1);
  if (parsedSkeletonId > -1) {
    mesh->skeleton = scene->getLastSkeletonByID(std::to_string(parsedSkeletonId));
    if (json_util::has_valid_key_value(parsedMesh, "numBoneInfluencers")) {
      mesh->numBoneInfluencers = json_util::get_number(parsedMesh, "numBoneInfluencers", 0u);
    }
  }

  // Animations
  if (json_util::has_valid_key_value(parsedMesh, "animations")) {
    for (const auto& parsedAnimation : json_util::get_array<json>(parsedMesh, "animations")) {
      mesh->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
    Node::ParseAnimationRanges(*mesh, parsedMesh, scene);
  }

  if (json_util::has_valid_key_value(parsedMesh, "autoAnimate")
      && json_util::get_bool(parsedMesh, "autoAnimate")) {
    scene->beginAnimation(mesh, json_util::get_number(parsedMesh, "autoAnimateFrom", 0.f),
                          json_util::get_number(parsedMesh, "autoAnimateTo", 0.f),
                          json_util::get_bool(parsedMesh, "autoAnimateLoop", false),
                          json_util::get_number(parsedMesh, "autoAnimateSpeed", 1.f));
  }

  // Layer Mask
  if (json_util::has_valid_key_value(parsedMesh, "layerMask")) {
    auto layerMask = json_util::get_string(parsedMesh, "layerMask");
    if (!layerMask.empty()) {
      mesh->layerMask = static_cast<unsigned>(std::stoi(layerMask));
    }
  }
  else {
    mesh->layerMask
      = static_cast<unsigned>(json_util::get_number(parsedMesh, "layerMask", 0x0FFFFFFF));
  }

  // Physics
  if (json_util::has_valid_key_value(parsedMesh, "physicsImpostor")) {
  }

  // Levels
  if (json_util::has_valid_key_value(parsedMesh, "lodMeshIds")) {
    auto lodMeshIds = json_util::get_array<std::string>(parsedMesh, "lodMeshIds");
    if (!lodMeshIds.empty()) {
      mesh->_waitingData.lods = {
        lodMeshIds,                                              // ids
        json_util::get_array<float>(parsedMesh, "lodDistances"), // distances
        json_util::get_array<float>(parsedMesh, "lodCoverages"), // coverages
      };
    }
  }

  // Instances
  if (json_util::has_valid_key_value(parsedMesh, "instances")) {
    for (const auto& parsedInstance : json_util::get_array<json>(parsedMesh, "instances")) {
      auto instance = mesh->createInstance(json_util::get_string(parsedInstance, "name"));

      if (json_util::has_valid_key_value(parsedInstance, "id")
          && !json_util::is_null(parsedMesh["localMatrix"])) {
        instance->id = json_util::get_string(parsedInstance, "id");
      }

      // Tags.AddTagsTo(instance, parsedInstance.tags);

      mesh->position = Vector3::FromArray(json_util::get_array<float>(parsedInstance, "position"));

      if (json_util::has_valid_key_value(parsedInstance, "parentId")) {
        instance->_waitingParentId = json_util::get_string(parsedInstance, "parentId");
      }

      if (json_util::has_valid_key_value(parsedInstance, "isEnabled")) {
        instance->setEnabled(json_util::get_bool(parsedInstance, "isEnabled"));
      }

      if (json_util::has_valid_key_value(parsedInstance, "isVisible")) {
        instance->isVisible = json_util::get_bool(parsedInstance, "isVisible");
      }

      if (json_util::has_valid_key_value(parsedInstance, "isPickable")) {
        instance->isPickable = json_util::get_bool(parsedInstance, "isPickable");
      }

      if (json_util::has_valid_key_value(parsedInstance, "rotationQuaternion")) {
        instance->rotationQuaternion = Quaternion::FromArray(
          json_util::get_array<float>(parsedInstance, "rotationQuaternion"));
      }
      else if (json_util::has_valid_key_value(parsedInstance, "rotation")) {
        instance->rotation
          = Vector3::FromArray(json_util::get_array<float>(parsedInstance, "rotation"));
      }

      if (json_util::has_valid_key_value(parsedMesh, "scaling")) {
        instance->position
          = Vector3::FromArray(json_util::get_array<float>(parsedInstance, "scaling"));
      }

      instance->checkCollisions = json_util::get_bool(parsedInstance, "checkCollisions");

      if (json_util::has_valid_key_value(parsedMesh, "animations")) {
        for (const auto& parsedAnimation : json_util::get_array<json>(parsedMesh, "animations")) {
          instance->animations.emplace_back(Animation::Parse(parsedAnimation));
        }
        Node::ParseAnimationRanges(*instance, parsedMesh, scene);

        if (json_util::has_valid_key_value(parsedMesh, "autoAnimate")) {
          scene->beginAnimation(instance, json_util::get_number(parsedMesh, "autoAnimateFrom", 0.f),
                                json_util::get_number(parsedMesh, "autoAnimateTo", 0.f),
                                json_util::get_bool(parsedMesh, "autoAnimateLoop"),
                                json_util::get_number<float>(parsedMesh, "autoAnimateSpeed", 1.0));
        }
      }
    }
  }

  // Thin instances
  if (json_util::has_valid_key_value(parsedMesh, "thinInstances")) {
    const auto thinInstances = parsedMesh["thinInstances"];

    if (json_util::has_valid_key_value(thinInstances, "matrixData")) {
#if 0
      mesh->thinInstanceSetBuffer(
        "matrix", json_util::get_array<float>(thinInstances, "matrixData"), 16, false);
#endif

      mesh->_thinInstanceDataStorage->matrixBufferSize
        = json_util::get_number<size_t>(thinInstances, "matrixBufferSize");
      mesh->_thinInstanceDataStorage->instancesCount
        = json_util::get_number<size_t>(thinInstances, "instancesCount");
    }
    else {
      mesh->_thinInstanceDataStorage->matrixBufferSize
        = json_util::get_number<size_t>(thinInstances, "matrixBufferSize");
    }
#if 0
    if (json_util::has_valid_key_value(thinInstances, "userThinInstance")) {
      const auto userThinInstance = thinInstances["userThinInstance"];

      for (const auto& item : userThinInstance["data"].items()) {
        const auto kind = item.key();
        mesh.thinInstanceSetBuffer(kind, new Float32Array(userThinInstance.data[kind]),
                                   userThinInstance.strides[kind], false);
        mesh._userThinInstanceBuffersStorage.sizes[kind] = userThinInstance.sizes[kind];
      }
    }
#endif
  }

  return mesh;
}

// Statics
MeshPtr Mesh::CreateRibbon(const std::string& iName,
                           const std::vector<std::vector<Vector3>>& pathArray, bool closeArray,
                           bool closePath, int offset, Scene* scene,
                           const std::optional<bool>& updatable,
                           const std::optional<unsigned int>& sideOrientation,
                           const MeshPtr& instance)
{
  RibbonOptions options;
  options.pathArray       = pathArray;
  options.closeArray      = closeArray;
  options.closePath       = closePath;
  options.offset          = offset;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;

  return RibbonBuilder::CreateRibbon(iName, options, scene);
}

MeshPtr Mesh::CreateDisc(const std::string& iName, float radius, unsigned int tessellation,
                         Scene* scene, const std::optional<bool>& updatable,
                         const std::optional<unsigned int>& sideOrientation)
{
  DiscOptions options;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return DiscBuilder::CreateDisc(iName, options, scene);
}

MeshPtr Mesh::CreateBox(const std::string& iName, float size, Scene* scene,
                        const std::optional<bool>& updatable,
                        const std::optional<unsigned int>& sideOrientation)
{
  BoxOptions options;
  options.size            = size;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return BoxBuilder::CreateBox(iName, options, scene);
}

MeshPtr Mesh::CreateSphere(const std::string& iName, unsigned int segments, float diameter,
                           Scene* scene, const std::optional<bool>& updatable,
                           const std::optional<unsigned int>& sideOrientation)
{
  SphereOptions options;
  options.segments        = segments;
  options.diameterX       = diameter;
  options.diameterY       = diameter;
  options.diameterZ       = diameter;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return SphereBuilder::CreateSphere(iName, options, scene);
}

MeshPtr Mesh::CreateHemisphere(const std::string& iName, unsigned int segments, float diameter,
                               Scene* scene)
{
  HemisphereOptions options;
  options.segments = segments;
  options.diameter = diameter;

  return HemisphereBuilder::CreateHemisphere(iName, options, scene);
}

// Cylinder and cone
MeshPtr Mesh::CreateCylinder(const std::string& iName, float height, float diameterTop,
                             float diameterBottom, unsigned int tessellation,
                             unsigned int subdivisions, Scene* scene,
                             const std::optional<bool>& updatable,
                             const std::optional<unsigned int>& sideOrientation)
{
  CylinderOptions options;
  options.height          = height;
  options.diameterTop     = diameterTop;
  options.diameterBottom  = diameterBottom;
  options.tessellation    = tessellation;
  options.subdivisions    = subdivisions;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return CylinderBuilder::CreateCylinder(iName, options, scene);
}

// Torus
MeshPtr Mesh::CreateTorus(const std::string& iName, float diameter, float thickness,
                          unsigned int tessellation, Scene* scene,
                          const std::optional<bool>& updatable,
                          const std::optional<unsigned int>& sideOrientation)
{
  TorusOptions options;
  options.diameter        = diameter;
  options.thickness       = thickness;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return TorusBuilder::CreateTorus(iName, options, scene);
}

MeshPtr Mesh::CreateTorusKnot(const std::string& iName, float radius, float tube,
                              unsigned int radialSegments, unsigned int tubularSegments, float p,
                              float q, Scene* scene, const std::optional<bool>& updatable,
                              const std::optional<unsigned int>& sideOrientation)
{
  TorusKnotOptions options;
  options.radius          = radius;
  options.tube            = tube;
  options.radialSegments  = radialSegments;
  options.tubularSegments = tubularSegments;
  options.p               = p;
  options.q               = q;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return TorusKnotBuilder::CreateTorusKnot(iName, options, scene);
}

LinesMeshPtr Mesh::CreateLines(const std::string& iName, const std::vector<Vector3>& points,
                               Scene* scene, bool updatable, const LinesMeshPtr& instance)
{
  LinesOptions options;
  options.points    = points;
  options.updatable = updatable;
  options.instance  = instance;

  return LinesBuilder::CreateLines(iName, options, scene);
}

LinesMeshPtr Mesh::CreateDashedLines(const std::string& iName, std::vector<Vector3>& points,
                                     float dashSize, float gapSize, unsigned int dashNb,
                                     Scene* scene, bool updatable, const LinesMeshPtr& instance)
{
  DashedLinesOptions options;
  options.points    = points;
  options.dashSize  = dashSize;
  options.gapSize   = gapSize;
  options.dashNb    = dashNb;
  options.updatable = updatable;
  options.instance  = instance;

  return LinesBuilder::CreateDashedLines(iName, options, scene);
}

MeshPtr Mesh::CreatePolygon(const std::string& iName, const std::vector<Vector3>& shape,
                            Scene* scene, const std::vector<std::vector<Vector3>>& holes,
                            bool updatable, unsigned int sideOrientation)
{
  PolygonOptions options;
  options.shape           = shape;
  options.holes           = holes;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;

  return PolygonBuilder::CreatePolygon(iName, options, scene);
}

MeshPtr Mesh::ExtrudePolygon(const std::string& iName, const std::vector<Vector3>& shape,
                             float depth, Scene* scene,
                             const std::vector<std::vector<Vector3>>& holes, bool updatable,
                             unsigned int sideOrientation)
{
  PolygonOptions options;
  options.shape           = shape;
  options.holes           = holes;
  options.depth           = depth;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;

  return PolygonBuilder::ExtrudePolygon(iName, options, scene);
}

MeshPtr Mesh::ExtrudeShape(const std::string& iName, const std::vector<Vector3>& shape,
                           const std::vector<Vector3>& path, float scale, float iRotation,
                           unsigned int cap, Scene* scene, const std::optional<bool>& updatable,
                           const std::optional<unsigned int>& sideOrientation,
                           const MeshPtr& instance)
{
  ExtrudeShapeOptions options;
  options.shape           = shape;
  options.path            = path;
  options.scale           = scale;
  options.rotation        = iRotation;
  options.cap             = (cap == 0) ? 0 : Mesh::NO_CAP;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;
  options.updatable       = updatable;

  return ShapeBuilder::ExtrudeShape(iName, options, scene);
}

MeshPtr Mesh::ExtrudeShapeCustom(
  const std::string& iName, const std::vector<Vector3>& shape, const std::vector<Vector3>& path,
  const std::function<float(float i, float distance)>& scaleFunction,
  const std::function<float(float i, float distance)>& rotationFunction, bool ribbonCloseArray,
  bool ribbonClosePath, unsigned int cap, Scene* scene, const std::optional<bool>& updatable,
  const std::optional<unsigned int>& sideOrientation, const MeshPtr& instance)
{
  ExtrudeShapeCustomOptions options;
  options.shape            = shape;
  options.path             = path;
  options.scaleFunction    = scaleFunction;
  options.rotationFunction = rotationFunction;
  options.ribbonCloseArray = ribbonCloseArray;
  options.ribbonClosePath  = ribbonClosePath;
  options.cap              = (cap == 0) ? 0 : Mesh::NO_CAP;
  options.sideOrientation  = sideOrientation;
  options.instance         = instance;
  options.updatable        = updatable;

  return ShapeBuilder::ExtrudeShapeCustom(iName, options, scene);
}

MeshPtr Mesh::CreateLathe(const std::string& iName, const std::vector<Vector3>& shape, float radius,
                          unsigned int tessellation, Scene* scene,
                          const std::optional<bool>& updatable,
                          const std::optional<unsigned int>& sideOrientation)
{
  LatheOptions options;
  options.shape           = shape;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return LatheBuilder::CreateLathe(iName, options, scene);
}

MeshPtr Mesh::CreatePlane(const std::string& iName, float size, Scene* scene,
                          const std::optional<bool>& updatable,
                          const std::optional<unsigned int>& sideOrientation)
{
  PlaneOptions options;
  options.size            = size;
  options.width           = size;
  options.height          = size;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return PlaneBuilder::CreatePlane(iName, options, scene);
}

MeshPtr Mesh::CreateGround(const std::string& iName, unsigned int width, unsigned int height,
                           unsigned int subdivisions, Scene* scene,
                           const std::optional<bool>& updatable)
{
  GroundOptions options;
  options.subdivisions = subdivisions;
  options.width        = width;
  options.height       = height;
  options.updatable    = updatable;

  return GroundBuilder::CreateGround(iName, options, scene);
}

MeshPtr Mesh::CreateTiledGround(const std::string& iName, float xmin, float zmin, float xmax,
                                float zmax, const ISize& subdivisions, const ISize& precision,
                                Scene* scene, const std::optional<bool>& updatable)
{
  TiledGroundOptions options;
  options.xmin         = xmin;
  options.zmin         = zmin;
  options.xmax         = xmax;
  options.zmax         = zmax;
  options.subdivisions = subdivisions;
  options.precision    = precision;
  options.updatable    = updatable;

  return GroundBuilder::CreateTiledGround(iName, options, scene);
}

GroundMeshPtr Mesh::CreateGroundFromHeightMap(
  const std::string& iName, const std::string& url, unsigned int width, unsigned int height,
  unsigned int subdivisions, unsigned int minHeight, unsigned int maxHeight, Scene* scene,
  bool updatable, const std::function<void(const GroundMeshPtr& mesh)>& iOnReady,
  std::optional<float> alphaFilter)
{
  GroundFromHeightMapOptions options;
  options.width        = static_cast<float>(width);
  options.height       = static_cast<float>(height);
  options.subdivisions = subdivisions;
  options.minHeight    = static_cast<float>(minHeight);
  options.maxHeight    = static_cast<float>(maxHeight);
  options.updatable    = updatable;
  options.onReady      = iOnReady;
  options.alphaFilter  = alphaFilter;

  return GroundBuilder::CreateGroundFromHeightMap(iName, url, options, scene);
}

MeshPtr Mesh::CreateTube(const std::string& iName, const std::vector<Vector3>& path, float radius,
                         unsigned int tessellation,
                         const std::function<float(unsigned int i, float distance)>& radiusFunction,
                         unsigned int cap, Scene* scene, const std::optional<bool>& updatable,
                         const std::optional<unsigned int>& sideOrientation,
                         const MeshPtr& instance)
{
  TubeOptions options;
  options.path            = path;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.radiusFunction  = radiusFunction;
  options.arc             = 1.f;
  options.cap             = cap;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;

  return TubeBuilder::CreateTube(iName, options, scene);
}

MeshPtr Mesh::CreatePolyhedron(const std::string& iName, PolyhedronOptions& options, Scene* scene)
{
  return PolyhedronBuilder::CreatePolyhedron(iName, options, scene);
}

MeshPtr Mesh::CreateCapsule(const std::string& name, ICreateCapsuleOptions& options, Scene* scene)
{
  return CapsuleBuilder::CreateCapsule(name, options, scene);
}

MeshPtr Mesh::CreateIcoSphere(const std::string& iName, IcoSphereOptions& options, Scene* scene)
{
  return IcoSphereBuilder::CreateIcoSphere(iName, options, scene);
}

MeshPtr Mesh::CreateDecal(const std::string& iName, const AbstractMeshPtr& sourceMesh,
                          const Vector3& iPosition, const Vector3& normal, const Vector3& size,
                          float angle)
{
  DecalOptions options;
  options.position = iPosition;
  options.normal   = normal;
  options.size     = size;
  options.angle    = angle;

  return DecalBuilder::CreateDecal(iName, sourceMesh, options);
}

Float32Array& Mesh::setPositionsForCPUSkinning()
{
  auto& internalDataInfo = *_internalMeshDataInfo;
  if (internalDataInfo._sourcePositions.empty()) {
    auto iSource = getVerticesData(VertexBuffer::PositionKind);
    if (iSource.empty()) {
      return internalDataInfo._sourceNormals;
    }

    internalDataInfo._sourcePositions = iSource;

    if (!isVertexBufferUpdatable(VertexBuffer::PositionKind)) {
      setVerticesData(VertexBuffer::PositionKind, iSource, true);
    }
  }
  return internalDataInfo._sourcePositions;
}

Float32Array& Mesh::setNormalsForCPUSkinning()
{
  auto& internalDataInfo = *_internalMeshDataInfo;
  if (internalDataInfo._sourceNormals.empty()) {
    auto iSource = getVerticesData(VertexBuffer::NormalKind);
    if (iSource.empty()) {
      return internalDataInfo._sourceNormals;
    }

    internalDataInfo._sourceNormals = iSource;

    if (!isVertexBufferUpdatable(VertexBuffer::NormalKind)) {
      setVerticesData(VertexBuffer::NormalKind, iSource, true);
    }
  }
  return internalDataInfo._sourceNormals;
}

Mesh* Mesh::applySkeleton(const SkeletonPtr& iSkeleton)
{
  if (!_geometry) {
    return this;
  }

  if (_geometry->_softwareSkinningFrameId == getScene()->getRenderId()) {
    return this;
  }

  _geometry->_softwareSkinningFrameId = getScene()->getRenderId();

  if (!isVerticesDataPresent(VertexBuffer::PositionKind)) {
    return this;
  }
  if (!isVerticesDataPresent(VertexBuffer::MatricesIndicesKind)) {
    return this;
  }
  if (!isVerticesDataPresent(VertexBuffer::MatricesWeightsKind)) {
    return this;
  }

  const auto hasNormals = isVerticesDataPresent(VertexBuffer::NormalKind);

  auto& internalDataInfo = *_internalMeshDataInfo;

  if (internalDataInfo._sourcePositions.empty()) {
    auto _submeshes = std::move(subMeshes);
    setPositionsForCPUSkinning();
    subMeshes = std::move(_submeshes);
  }

  if (hasNormals && internalDataInfo._sourceNormals.empty()) {
    setNormalsForCPUSkinning();
  }

  // positionsData checks for not being Float32Array will only pass at most
  // once
  auto positionsData = getVerticesData(VertexBuffer::PositionKind);

  if (positionsData.empty()) {
    return this;
  }

  // normalsData checks for not being Float32Array will only pass at most once
  auto normalsData = getVerticesData(VertexBuffer::NormalKind);

  if (hasNormals) {
    if (normalsData.empty()) {
      return this;
    }
  }

  auto matricesIndicesData = getVerticesData(VertexBuffer::MatricesIndicesKind);
  auto matricesWeightsData = getVerticesData(VertexBuffer::MatricesWeightsKind);

  if (matricesWeightsData.empty() || matricesIndicesData.empty()) {
    return this;
  }

  bool needExtras = numBoneInfluencers() > 4;
  auto matricesIndicesExtraData
    = needExtras ? getVerticesData(VertexBuffer::MatricesIndicesExtraKind) : Float32Array();
  auto matricesWeightsExtraData
    = needExtras ? getVerticesData(VertexBuffer::MatricesWeightsExtraKind) : Float32Array();

  auto skeletonMatrices = iSkeleton->getTransformMatrices(this);

  auto tempVector3 = Vector3::Zero();
  Matrix finalMatrix;
  Matrix tempMatrix;

  unsigned int matWeightIdx = 0;
  unsigned int inf;
  float weight;
  for (unsigned int index = 0; index < positionsData.size(); index += 3, matWeightIdx += 4) {
    for (inf = 0; inf < 4; ++inf) {
      weight = matricesWeightsData[matWeightIdx + inf];
      if (weight > 0.f) {
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
        if (weight > 0.f) {
          Matrix::FromFloat32ArrayToRefScaled(skeletonMatrices,
                                              static_cast<unsigned int>(std::floor(
                                                matricesIndicesExtraData[matWeightIdx + inf] * 16)),
                                              weight, tempMatrix);
          finalMatrix.addToSelf(tempMatrix);
        }
      }
    }

    Vector3::TransformCoordinatesFromFloatsToRef(
      internalDataInfo._sourcePositions[index], internalDataInfo._sourcePositions[index + 1],
      internalDataInfo._sourcePositions[index + 2], finalMatrix, tempVector3);
    tempVector3.toArray(positionsData, index);

    if (hasNormals) {
      Vector3::TransformNormalFromFloatsToRef(
        internalDataInfo._sourceNormals[index], internalDataInfo._sourceNormals[index + 1],
        internalDataInfo._sourceNormals[index + 2], finalMatrix, tempVector3);
      tempVector3.toArray(normalsData, index);
    }

    finalMatrix.reset();
  }

  updateVerticesData(VertexBuffer::PositionKind, positionsData);
  if (hasNormals) {
    updateVerticesData(VertexBuffer::NormalKind, normalsData);
  }

  return this;
}

MinMax Mesh::GetMinMax(const std::vector<AbstractMeshPtr>& meshes)
{
  bool minVectorSet = false;
  Vector3 minVector;
  Vector3 maxVector;
  for (const auto& mesh : meshes) {
    const BoundingBox& boundingBox = mesh->getBoundingInfo()->boundingBox;
    if (!minVectorSet) {
      minVector    = boundingBox.minimumWorld;
      maxVector    = boundingBox.maximumWorld;
      minVectorSet = true;
    }
    else {
      minVector.minimizeInPlace(boundingBox.minimumWorld);
      maxVector.maximizeInPlace(boundingBox.maximumWorld);
    }
  }

  MinMax minMax;
  if (!minVectorSet) {
    minMax.min = minVector;
    minMax.max = maxVector;
  }
  else {
    minMax.min = Vector3::Zero();
    minMax.max = Vector3::Zero();
  }

  return minMax;
}

Vector3 Mesh::Center(const MinMaxDistance& MinMaxVectorAndDistance)
{
  return Vector3::Center(MinMaxVectorAndDistance.min, MinMaxVectorAndDistance.max);
}

Vector3 Mesh::Center(const MinMax& minMaxVector)
{
  return Vector3::Center(minMaxVector.min, minMaxVector.max);
}

Vector3 Mesh::Center(const std::vector<AbstractMeshPtr>& meshes)
{
  MinMax minMaxVector = Mesh::GetMinMax(meshes);
  return Vector3::Center(minMaxVector.min, minMaxVector.max);
}

MeshPtr Mesh::MergeMeshes(const std::vector<MeshPtr>& meshes, bool disposeSource,
                          bool allow32BitsIndices, MeshPtr meshSubclass,
                          bool subdivideWithSubMeshes, bool multiMultiMaterials)
{
  unsigned int index = 0;
  if (!allow32BitsIndices) {
    size_t totalVertices = 0;

    // Counting vertices
    for (index = 0; index < meshes.size(); ++index) {
      if (meshes[index]) {
        totalVertices += meshes[index]->getTotalVertices();

        if (totalVertices >= 65536) {
          BABYLON_LOG_WARN("Mesh",
                           "Cannot merge meshes because resulting mesh will "
                           "have more than 65536 vertices. Please use "
                           "allow32BitsIndices = true to use 32 bits indices.")
          return nullptr;
        }
      }
    }
  }

  MultiMaterialPtr newMultiMaterial = nullptr;
  auto subIndex                     = 0ull;
  auto matIndex                     = 0ull;
  if (multiMultiMaterials) {
    subdivideWithSubMeshes = false;
  }
  std::vector<MaterialPtr> materialArray;
  Uint32Array materialIndexArray;

  // Merge
  std::unique_ptr<VertexData> vertexData      = nullptr;
  std::unique_ptr<VertexData> otherVertexData = nullptr;
  IndicesArray indiceArray;
  MeshPtr source = nullptr;
  for (const auto& mesh : meshes) {
    if (mesh) {
      if (mesh->isAnInstance()) {
        BABYLON_LOG_WARN("Mesh", "Cannot merge instance meshes.")
        return nullptr;
      }
      const auto& wm  = mesh->computeWorldMatrix(true);
      otherVertexData = VertexData::ExtractFromMesh(mesh.get(), true, true);

      otherVertexData->transform(wm);

      if (vertexData) {
        vertexData->merge(*otherVertexData, allow32BitsIndices);
      }
      else {
        vertexData = std::move(otherVertexData);
        source     = mesh;
      }

      if (subdivideWithSubMeshes) {
        indiceArray.emplace_back(static_cast<uint32_t>(mesh->getTotalIndices()));
      }
      if (multiMultiMaterials) {
        if (mesh->material()) {
          auto material = std::static_pointer_cast<MultiMaterial>(mesh->material());
          if (material) {
            for (matIndex = 0; matIndex < material->subMaterials().size(); matIndex++) {
              if (stl_util::index_of(materialArray, material->subMaterials()[matIndex]) < 0) {
                materialArray.emplace_back(material->subMaterials()[matIndex]);
              }
            }
            for (subIndex = 0; subIndex < mesh->subMeshes.size(); ++subIndex) {
              const auto materialIndex = stl_util::index_of(
                materialArray, material->subMaterials()[mesh->subMeshes[subIndex]->materialIndex]);
              materialIndexArray.emplace_back(materialIndex);
              indiceArray.emplace_back(
                static_cast<uint32_t>(mesh->subMeshes[subIndex]->indexCount));
            }
          }
          else {
            if (stl_util::index_of(materialArray, material) < 0) {
              materialArray.emplace_back(material);
            }
            for (subIndex = 0; subIndex < mesh->subMeshes.size(); ++subIndex) {
              const auto materialIndex = stl_util::index_of(materialArray, material);
              materialIndexArray.emplace_back(materialIndex);
              indiceArray.emplace_back(
                static_cast<uint32_t>(mesh->subMeshes[subIndex]->indexCount));
            }
          }
        }
        else {
          for (subIndex = 0; subIndex < mesh->subMeshes.size(); ++subIndex) {
            materialIndexArray.emplace_back(0);
            indiceArray.emplace_back(static_cast<uint32_t>(mesh->subMeshes[subIndex]->indexCount));
          }
        }
      }
    }
  }

  if (!meshSubclass) {
    meshSubclass = Mesh::New(source->name + "_merged", source->getScene());
  }

  if ((!vertexData) || (!source))
    return meshSubclass;

  vertexData->applyToMesh(*meshSubclass);

  // Setting properties
  meshSubclass->checkCollisions                 = source->checkCollisions();
  meshSubclass->overrideMaterialSideOrientation = source->overrideMaterialSideOrientation;

  // Cleaning
  if (disposeSource) {
    for (const auto& mesh : meshes) {
      if (mesh) {
        mesh->dispose();
      }
    }
  }

  // Subdivide
  if (subdivideWithSubMeshes || multiMultiMaterials) {
    //-- removal of global submesh
    meshSubclass->releaseSubMeshes();
    index               = 0;
    unsigned int offset = 0;
    //-- apply subdivision according to index table
    while (index < indiceArray.size()) {
      SubMesh::CreateFromIndices(0, offset, indiceArray[index], meshSubclass);
      offset += indiceArray[index];
      ++index;
    }
  }

  if (multiMultiMaterials) {
    newMultiMaterial = MultiMaterial::New(source->name + "_merged", source->getScene());
    newMultiMaterial->subMaterials = materialArray;
    for (subIndex = 0; subIndex < meshSubclass->subMeshes.size(); subIndex++) {
      meshSubclass->subMeshes[subIndex]->materialIndex = materialIndexArray[subIndex];
    }
    meshSubclass->material = newMultiMaterial;
  }
  else {
    meshSubclass->material = source->material();
  }

  return meshSubclass;
}

void Mesh::addInstance(const InstancedMeshPtr& instance)
{
  instance->_indexInSourceMeshInstanceArray = static_cast<int>(instances.size());
  instances.emplace_back(instance);
}

void Mesh::removeInstance(InstancedMesh* instance)
{
  // Remove from mesh
  const auto index = instance->_indexInSourceMeshInstanceArray;
  if (index != -1) {
    if (index != static_cast<int>(instances.size()) - 1) {
      const auto& last                      = instances.back();
      instances[static_cast<size_t>(index)] = last;
      last->_indexInSourceMeshInstanceArray = index;
    }

    instance->_indexInSourceMeshInstanceArray = -1;
    instances.pop_back();
  }
}

} // end of namespace BABYLON
