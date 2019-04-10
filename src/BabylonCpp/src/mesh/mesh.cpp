#include <babylon/mesh/sub_mesh.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/layer/highlight_layer.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/materials/multi_material.h>
#include <babylon/math/matrix.h>
#include <babylon/math/scalar.h>
#include <babylon/math/tmp.h>
#include <babylon/math/vector2.h>
#include <babylon/mesh/_creation_data_storage.h>
#include <babylon/mesh/_instance_data_storage.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/_visible_instances.h>
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/ground_mesh.h>
#include <babylon/mesh/instanced_mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/mesh_lod_level.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/morph/morph_target.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/particles/particle_system.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/rendering/outline_renderer.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Mesh::Mesh(const std::string& iName, Scene* scene, Node* iParent, Mesh* source,
           bool doNotCloneChildren, bool clonePhysicsImpostor)
    : AbstractMesh{iName, scene}
    , onBeforeRenderObservable{this, &Mesh::get_onBeforeDrawObservable}
    , onAfterRenderObservable{this, &Mesh::get_onAfterRenderObservable}
    , onBeforeDrawObservable{this, &Mesh::get_onBeforeDrawObservable}
    , onBeforeDraw{this, &Mesh::set_onBeforeDraw}
    , delayLoadState{EngineConstants::DELAYLOADSTATE_NONE}
    , onLODLevelSelection{nullptr}
    , morphTargetManager{this, &Mesh::get_morphTargetManager,
                         &Mesh::set_morphTargetManager}
    , _geometry{nullptr}
    , _shouldGenerateFlatShading{false}
    , _originalBuilderSideOrientation{Mesh::DEFAULTSIDE}
    , overrideMaterialSideOrientation{std::nullopt}
    , source{this, &Mesh::get_source}
    , isUnIndexed{this, &Mesh::get_isUnIndexed, &Mesh::set_isUnIndexed}
    , hasLODLevels{this, &Mesh::get_hasLODLevels}
    , geometry{this, &Mesh::get_geometry}
    , areNormalsFrozen{this, &Mesh::get_areNormalsFrozen}
    , overridenInstanceCount{this, &Mesh::set_overridenInstanceCount}
    , _onBeforeDrawObserver{nullptr}
    , _morphTargetManager{nullptr}
    , _creationDataStorage{std::make_shared<_CreationDataStorage>()}
    , _instanceDataStorage{std::make_unique<_InstanceDataStorage>()}
    , _effectiveMaterial{nullptr}
    , _preActivateId{-1}
    , _areNormalsFrozen{false}
    , _source{nullptr}
    , _tessellation{0}
    , _arc{1.f}
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
    _source = source;

    // Construction Params
    // Clone parameters allowing mesh to be updated in case of parametric
    // shapes.
    _originalBuilderSideOrientation = source->_originalBuilderSideOrientation;
    _creationDataStorage            = source->_creationDataStorage;

    // Parent
    Node::set_parent(source->parent());

    // Pivot
    setPivotMatrix(source->getPivotMatrix());

    id = name + "." + source->id;

    // Material
    material = source->getMaterial();
    if (!doNotCloneChildren) {
      // Children
      for (auto& mesh : scene->meshes) {
        if (mesh->parent() == source) {
          // doNotCloneChildren is always going to be False
          mesh->clone(name + "." + mesh->name, this, doNotCloneChildren);
        }
      }
    }

    // Physics clone
    auto physicsEngine = getScene()->getPhysicsEngine();
    if (clonePhysicsImpostor && physicsEngine) {
      // auto impostor = physicsEngine->getImpostorForPhysicsObject(source);
      // if (impostor) {
      //  physicsImpostor = impostor->clone(this);
      //}
    }

    // Particles
    for (auto& system : scene->particleSystems) {
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
}

Mesh::~Mesh()
{
}

MorphTargetManagerPtr& Mesh::get_morphTargetManager()
{
  return _morphTargetManager;
}

void Mesh::set_morphTargetManager(const MorphTargetManagerPtr& value)
{
  if (_morphTargetManager == value) {
    return;
  }
  _morphTargetManager = value;
  _syncGeometryWithMorphTargetManager();
}

Mesh*& Mesh::get_source()
{
  return _source;
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

const std::string Mesh::getClassName() const
{
  return "Mesh";
}

Type Mesh::type() const
{
  return Type::MESH;
}

Observable<Mesh>& Mesh::get_onBeforeRenderObservable()
{
  return _onBeforeRenderObservable;
}

Observable<Mesh>& Mesh::get_onAfterRenderObservable()
{
  return _onAfterRenderObservable;
}

Observable<Mesh>& Mesh::get_onBeforeDrawObservable()
{
  return _onBeforeDrawObservable;
}

void Mesh::set_onBeforeDraw(
  const std::function<void(Mesh*, EventState&)>& callback)
{
  if (_onBeforeDrawObserver) {
    onBeforeDrawObservable().remove(_onBeforeDrawObserver);
  }
  _onBeforeDrawObserver = onBeforeDrawObservable().add(callback);
}

std::string Mesh::toString(bool fullDetails)
{
  std::ostringstream oss;
  oss << AbstractMesh::toString(fullDetails);
  oss << ", n vertices: " << getTotalVertices();
  oss << ", parent: "
      << ((!_waitingParentId.empty()) ? _waitingParentId :
                                        (parent() ? parent()->name : "NONE"));

  if (!animations.empty()) {
    for (auto& animation : animations) {
      oss << ", animation[0]: " << animation->toString(fullDetails);
    }
  }

  if (fullDetails) {
    if (_geometry) {
      auto ib = getIndices();
      auto vb = getVerticesData(VertexBuffer::PositionKind);

      if (!vb.empty() && !ib.empty()) {
        oss << ", flat shading: "
            << (vb.size() / 3 == ib.size() ? "YES" : "NO");
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

  for (auto& instance : instances) {
    instance->_unBindEffect();
  }
}

bool Mesh::get_hasLODLevels() const
{
  return _LODLevels.size() > 0;
}

std::vector<MeshLODLevel*> Mesh::getLODLevels()
{
  return stl_util::to_raw_ptr_vector(_LODLevels);
}

void Mesh::_sortLODLevels()
{
  std::sort(_LODLevels.begin(), _LODLevels.end(),
            [](const std::unique_ptr<MeshLODLevel>& a,
               const std::unique_ptr<MeshLODLevel>& b) {
              if (a->distance < b->distance) {
                return 1;
              }
              if (a->distance > b->distance) {
                return -1;
              }

              return 0;
            });
}

Mesh& Mesh::addLODLevel(float distance, Mesh* mesh)
{
  if (mesh && mesh->_masterMesh) {
    BABYLON_LOG_WARN("Mesh", "You cannot use a mesh as LOD level twice");
    return *this;
  }

  auto level = std::make_unique<MeshLODLevel>(distance, mesh);
  _LODLevels.emplace_back(std::move(level));

  if (mesh) {
    mesh->_masterMesh = this;
  }

  _sortLODLevels();

  return *this;
}

Mesh* Mesh::getLODLevelAtDistance(float distance)
{
  for (const auto& level : _LODLevels) {
    if (stl_util::almost_equal(level->distance, distance)) {
      return level->mesh;
    }
  }
  return nullptr;
}

Mesh& Mesh::removeLODLevel(Mesh* mesh)
{
  for (auto& level : _LODLevels) {
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

AbstractMesh* Mesh::getLOD(const CameraPtr& camera,
                           BoundingSphere* boundingSphere)
{
  if (_LODLevels.empty()) {
    return this;
  }

  BoundingSphere* bSphere = nullptr;

  if (boundingSphere) {
    bSphere = boundingSphere;
  }
  else {
    auto boundingInfo = getBoundingInfo();

    bSphere = &boundingInfo.boundingSphere;
  }

  auto distanceToCamera
    = bSphere->centerWorld.subtract(camera->globalPosition()).length();

  if (_LODLevels.back()->distance > distanceToCamera) {
    if (onLODLevelSelection) {
      onLODLevelSelection(distanceToCamera, this, _LODLevels.back()->mesh);
    }
    return this;
  }

  for (auto& level : _LODLevels) {
    if (level->distance < distanceToCamera) {
      if (level->mesh) {
        level->mesh->_preActivate();
        level->mesh->_updateSubMeshesBoundingInfo(worldMatrixFromCache());
      }

      if (onLODLevelSelection) {
        onLODLevelSelection(distanceToCamera, this, level->mesh);
      }
      return level->mesh;
    }
  }

  if (onLODLevelSelection) {
    onLODLevelSelection(distanceToCamera, this, this);
  }
  return this;
}

Geometry*& Mesh::get_geometry()
{
  return _geometry;
}

void Mesh::setGeometry(Geometry* iGeometry)
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

Float32Array Mesh::getVerticesData(const std::string& kind, bool copyWhenShared,
                                   bool forceCopy)
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
      return std::find_if(
               _delayInfo.begin(), _delayInfo.end(),
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
      return std::find_if(
               _delayInfo.begin(), _delayInfo.end(),
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
      for (auto& vertexBuffer : _delayInfo) {
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
  if (delayLoadState == EngineConstants::DELAYLOADSTATE_LOADING) {
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
      || (engine->getCaps().instancedArrays && instances.size() > 0);

  computeWorldMatrix();

  auto mat = material() ? material() : scene->defaultMaterial();
  if (mat) {
    if (mat->storeEffectOnSubMeshes) {
      for (auto& subMesh : subMeshes) {
        auto effectiveMaterial = subMesh->getMaterial();
        if (effectiveMaterial) {
          if (effectiveMaterial->storeEffectOnSubMeshes) {
            if (!effectiveMaterial->isReadyForSubMesh(
                  this, subMesh.get(), hardwareInstancedRendering)) {
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
  for (auto& light : _lightSources) {
    auto generator = light->getShadowGenerator();

    if (generator) {
      for (auto& subMesh : subMeshes) {
        if (!generator->isReady(subMesh.get(), hardwareInstancedRendering)) {
          return false;
        }
      }
    }
  }

  // LOD
  for (auto& lod : _LODLevels) {
    if (lod->mesh && !lod->mesh->isReady(hardwareInstancedRendering)) {
      return false;
    }
  }

  return true;
}

bool Mesh::get_areNormalsFrozen() const
{
  return _areNormalsFrozen;
}

Mesh& Mesh::freezeNormals()
{
  _areNormalsFrozen = true;
  return *this;
}

Mesh& Mesh::unfreezeNormals()
{
  _areNormalsFrozen = false;
  return *this;
}

void Mesh::set_overridenInstanceCount(size_t count)
{
  _instanceDataStorage->overridenInstanceCount = count;
}

void Mesh::_preActivate()
{
  int sceneRenderId = getScene()->getRenderId();
  if (_preActivateId == sceneRenderId) {
    return;
  }

  _preActivateId                         = sceneRenderId;
  _instanceDataStorage->visibleInstances = nullptr;
}

void Mesh::_preActivateForIntermediateRendering(int renderId)
{
  if (_instanceDataStorage->visibleInstances) {
    _instanceDataStorage->visibleInstances->intermediateDefaultRenderId
      = renderId;
  }
}

Mesh& Mesh::_registerInstanceForRenderId(InstancedMesh* instance, int renderId)
{
  if (!_instanceDataStorage->visibleInstances) {
    _instanceDataStorage->visibleInstances
      = std::make_shared<_VisibleInstances>();
    _instanceDataStorage->visibleInstances->defaultRenderId     = renderId;
    _instanceDataStorage->visibleInstances->selfDefaultRenderId = _renderId;
  }

  if (_instanceDataStorage->visibleInstances->meshes.find(renderId)
      == _instanceDataStorage->visibleInstances->meshes.end()) {
    _instanceDataStorage->visibleInstances->meshes[renderId]
      = std::vector<InstancedMesh*>();
  }

  _instanceDataStorage->visibleInstances->meshes[renderId].emplace_back(
    instance);

  return *this;
}

Mesh& Mesh::refreshBoundingInfo()
{
  return _refreshBoundingInfo(false);
}

Mesh& Mesh::_refreshBoundingInfo(bool applySkeleton)
{
  if (_boundingInfo && _boundingInfo->isLocked()) {
    return *this;
  }

  auto data = _getPositionData(applySkeleton);
  if (!data.empty()) {
    std::optional<Vector2> bias = std::nullopt;
    if (geometry) {
      bias = geometry()->boundingBias;
    }
    auto extend   = Tools::ExtractMinAndMax(data, 0, getTotalVertices(), bias);
    _boundingInfo = std::make_unique<BoundingInfo>(extend.min, extend.max);
  }

  if (!subMeshes.empty()) {
    for (auto& subMesh : subMeshes) {
      subMesh->refreshBoundingInfo();
    }
  }

  _updateBoundingInfo();

  return *this;
}

Float32Array Mesh::_getPositionData(bool applySkeleton)
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
    bool needToRecreate     = false;

    if (force) {
      needToRecreate = true;
    }
    else {
      for (auto& submesh : subMeshes) {
        if (submesh->indexStart + submesh->indexCount >= totalIndices) {
          needToRecreate = true;
          break;
        }

        if (submesh->verticesStart + submesh->verticesCount >= totalVertices) {
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
  return SubMesh::New(0, 0, totalVertices, 0, getTotalIndices(),
                      shared_from_base<Mesh>());
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
                               std::min(subdivisionSize, totalIndices - offset),
                               shared_from_base<Mesh>());

    offset += subdivisionSize;
  }

  synchronizeInstances();
}

Mesh* Mesh::setVerticesData(const std::string& kind, const Float32Array& data,
                            bool updatable, const std::optional<size_t>& stride)
{
  if (!_geometry) {
    auto vertexData = std::make_unique<VertexData>();
    vertexData->set(data, kind);

    auto scene = getScene();

    Geometry::New(Geometry::RandomId(), scene, vertexData.get(), updatable,
                  this);
  }
  else {
    _geometry->setVerticesData(kind, data, updatable, stride);
  }

  return this;
}

void Mesh::markVerticesDataAsUpdatable(const std::string& kind, bool updatable)
{
  auto vb = getVertexBuffer(kind);

  if (!vb || vb->isUpdatable() == updatable) {
    return;
  }

  setVerticesData(kind, getVerticesData(kind), updatable);
}

Mesh& Mesh::setVerticesBuffer(std::unique_ptr<VertexBuffer>&& buffer)
{
  if (!_geometry) {
    _geometry = Geometry::CreateGeometryForMesh(this).get();
  }

  _geometry->setVerticesBuffer(std::move(buffer));

  return *this;
}

Mesh* Mesh::updateVerticesData(const std::string& kind,
                               const Float32Array& data, bool updateExtends,
                               bool makeItUnique)
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

Mesh& Mesh::updateMeshPositions(
  std::function<void(Float32Array& positions)> positionFunction,
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
  auto oldGeometry = _geometry;
  auto geometry_   = _geometry->copy(Geometry::RandomId());
  oldGeometry->releaseForMesh(this, true);
  geometry_->applyToMesh(this);

  return *this;
}

Mesh* Mesh::setIndices(const IndicesArray& indices, size_t totalVertices,
                       bool updatable)
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

Mesh& Mesh::updateIndices(const IndicesArray& indices, int offset)
{
  if (!_geometry) {
    return *this;
  }

  _geometry->updateIndices(indices, offset);
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

void Mesh::_bind(SubMesh* subMesh, const EffectPtr& effect,
                 unsigned int fillMode)
{
  if (!_geometry) {
    return;
  }

  auto engine = getScene()->getEngine();

  // Wireframe
  GL::IGLBuffer* indexToBind;

  if (_unIndexed) {
    indexToBind = nullptr;
  }
  else {
    switch (fillMode) {
      case Material::PointFillMode():
        indexToBind = nullptr;
        break;
      case Material::WireFrameFillMode():
        indexToBind = subMesh->_getLinesIndexBuffer(getIndices(), engine);
        break;
      default:
      case Material::TriangleFillMode():
        indexToBind = _unIndexed ? nullptr : _geometry->getIndexBuffer();
        break;
    }
  }

  // VBOs
  _geometry->_bind(effect, indexToBind);
}

void Mesh::_draw(SubMesh* subMesh, int fillMode, size_t instancesCount,
                 bool alternate)
{
  if (!_geometry || !_geometry->getVertexBuffers().size()
      || (!_unIndexed && !_geometry->getIndexBuffer())) {
    return;
  }

  onBeforeDrawObservable().notifyObservers(this);

  auto scene  = getScene();
  auto engine = scene->getEngine();

  const auto _fillMode = static_cast<unsigned int>(fillMode);
  if (_unIndexed || _fillMode == Material::PointFillMode()) {
    // or triangles as points
    engine->drawArraysType(_fillMode, static_cast<int>(subMesh->verticesStart),
                           static_cast<int>(subMesh->verticesCount),
                           static_cast<int>(instancesCount));
  }
  else if (_fillMode == Material::WireFrameFillMode()) {
    // Triangles as wireframe
    engine->drawElementsType(_fillMode, 0,
                             static_cast<int>(subMesh->_linesIndexCount),
                             static_cast<int>(instancesCount));
  }
  else {
    engine->drawElementsType(_fillMode, static_cast<int>(subMesh->indexStart),
                             static_cast<int>(subMesh->indexCount),
                             static_cast<int>(instancesCount));
  }

  if (scene->_isAlternateRenderingEnabled() && !alternate) {
    auto effect
      = subMesh->effect() ? subMesh->effect() : _effectiveMaterial->getEffect();
    if (!effect || !scene->activeCamera) {
      return;
    }
    scene->_switchToAlternateCameraConfiguration(true);
    _effectiveMaterial->bindView(effect.get());
    _effectiveMaterial->bindViewProjection(effect);

    engine->setViewport(scene->activeCamera->_alternateCamera->viewport);
    _draw(subMesh, fillMode, instancesCount, true);
    engine->setViewport(scene->activeCamera->viewport);

    scene->_switchToAlternateCameraConfiguration(false);
    _effectiveMaterial->bindView(effect.get());
    _effectiveMaterial->bindViewProjection(effect);
  }
}

Mesh& Mesh::registerBeforeRender(
  const std::function<void(Mesh* mesh, EventState&)>& func)
{
  onBeforeRenderObservable().add(func);
  return *this;
}

Mesh& Mesh::unregisterBeforeRender(
  const std::function<void(Mesh* mesh, EventState&)>& func)
{
  onBeforeRenderObservable().removeCallback(func);
  return *this;
}

Mesh& Mesh::registerAfterRender(
  const std::function<void(Mesh* mesh, EventState&)>& func)
{
  onAfterRenderObservable().add(func);
  return *this;
}

Mesh& Mesh::unregisterAfterRender(
  const std::function<void(Mesh* mesh, EventState&)>& func)
{
  onAfterRenderObservable().removeCallback(func);
  return *this;
}

_InstancesBatchPtr Mesh::_getInstancesRenderList(size_t subMeshId)
{
  auto scene                              = getScene();
  auto& batchCache                        = _instanceDataStorage->batchCache;
  batchCache->mustReturn                  = false;
  batchCache->renderSelf[subMeshId]       = isEnabled() && isVisible;
  batchCache->visibleInstances[subMeshId] = std::vector<InstancedMesh*>();

  if (_instanceDataStorage->visibleInstances) {
    auto& visibleInstances = _instanceDataStorage->visibleInstances;
    auto currentRenderId   = scene->getRenderId();
    auto defaultRenderId   = (scene->_isInIntermediateRendering() ?
                              visibleInstances->intermediateDefaultRenderId :
                              visibleInstances->defaultRenderId);
    batchCache->visibleInstances[subMeshId]
      = visibleInstances->meshes[currentRenderId];
    auto selfRenderId = _renderId;

    if ((batchCache->visibleInstances.find(subMeshId)
         == batchCache->visibleInstances.end())
        && batchCache->visibleInstances[subMeshId].empty() && defaultRenderId) {
      batchCache->visibleInstances[subMeshId]
        = visibleInstances->meshes[defaultRenderId];
      currentRenderId = std::max(defaultRenderId, currentRenderId);
      selfRenderId
        = std::max(visibleInstances->selfDefaultRenderId, currentRenderId);
    }

    auto visibleInstancesForSubMesh = batchCache->visibleInstances[subMeshId];
    if ((batchCache->visibleInstances.find(subMeshId)
         != batchCache->visibleInstances.end())
        && batchCache->visibleInstances[subMeshId].size() > 0) {
      if (subMeshId < _instanceDataStorage->renderIdForInstances.size()
          && _instanceDataStorage->renderIdForInstances[subMeshId]
               == currentRenderId) {
        batchCache->mustReturn = true;
        return batchCache;
      }

      if (currentRenderId != selfRenderId) {
        batchCache->renderSelf[subMeshId] = false;
      }
    }
    if (subMeshId >= _instanceDataStorage->renderIdForInstances.size()) {
      _instanceDataStorage->renderIdForInstances.resize(subMeshId + 1);
    }
    _instanceDataStorage->renderIdForInstances[subMeshId] = currentRenderId;
  }

  return batchCache;
}

Mesh& Mesh::_renderWithInstances(SubMesh* subMesh, unsigned int fillMode,
                                 const _InstancesBatchPtr& batch,
                                 const EffectPtr& effect, Engine* engine)
{
  if (batch->visibleInstances.find(subMesh->_id)
      == batch->visibleInstances.end()) {
    return *this;
  }

  auto& visibleInstances = batch->visibleInstances[subMesh->_id];
  if (visibleInstances.empty()) {
    return *this;
  }

  size_t matricesCount = visibleInstances.size() + 1;
  size_t bufferSize    = matricesCount * 16 * 4;

  auto& instanceStorage           = _instanceDataStorage;
  auto currentInstancesBufferSize = instanceStorage->instancesBufferSize;
  auto& instancesBuffer           = instanceStorage->instancesBuffer;

  while (instanceStorage->instancesBufferSize < bufferSize) {
    instanceStorage->instancesBufferSize *= 2;
  }

  if (instanceStorage->instancesData.empty()
      || currentInstancesBufferSize != instanceStorage->instancesBufferSize) {
    instanceStorage->instancesData
      = Float32Array(instanceStorage->instancesBufferSize / 4);
  }

  unsigned int offset         = 0;
  unsigned int instancesCount = 0;

  auto world = getWorldMatrix();
  if (batch->renderSelf[subMesh->_id]) {
    world.copyToArray(instanceStorage->instancesData, offset);
    offset += 16;
    instancesCount++;
  }

  if (!visibleInstances.empty()) {
    for (size_t instanceIndex = 0; instanceIndex < visibleInstances.size();
         ++instanceIndex) {
      auto instance = visibleInstances[instanceIndex];
      instance->getWorldMatrix().copyToArray(instanceStorage->instancesData,
                                             offset);
      offset += 16;
      ++instancesCount;
    }
  }

  if (!instancesBuffer
      || currentInstancesBufferSize != instanceStorage->instancesBufferSize) {
    if (instancesBuffer) {
      instancesBuffer->dispose();
    }

    instancesBuffer = std::make_shared<Buffer>(
      engine, instanceStorage->instancesData, true, 16, false, true);

    setVerticesBuffer(
      instancesBuffer->createVertexBuffer(VertexBuffer::World0Kind, 0, 4));
    setVerticesBuffer(
      instancesBuffer->createVertexBuffer(VertexBuffer::World1Kind, 4, 4));
    setVerticesBuffer(
      instancesBuffer->createVertexBuffer(VertexBuffer::World2Kind, 8, 4));
    setVerticesBuffer(
      instancesBuffer->createVertexBuffer(VertexBuffer::World3Kind, 12, 4));
  }
  else {
    instancesBuffer->updateDirectly(instanceStorage->instancesData, 0,
                                    instancesCount);
  }

  _bind(subMesh, effect, fillMode);

  _draw(subMesh, static_cast<int>(fillMode), instancesCount);

  engine->unbindInstanceAttributes();

  return *this;
}

Mesh& Mesh::_processRendering(
  SubMesh* subMesh, const EffectPtr& effect, int fillMode,
  const _InstancesBatchPtr& batch, bool hardwareInstancedRendering,
  std::function<void(bool isInstance, const Matrix& world,
                     Material* effectiveMaterial)>
    iOnBeforeDraw,
  Material* effectiveMaterial)
{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  if (hardwareInstancedRendering) {
    _renderWithInstances(subMesh, static_cast<unsigned>(fillMode), batch,
                         effect, engine);
  }
  else {
    if (batch->renderSelf[subMesh->_id]) {
      // Draw
      if (iOnBeforeDraw) {
        iOnBeforeDraw(false, getWorldMatrix(), effectiveMaterial);
      }

      _draw(subMesh, fillMode, _instanceDataStorage->overridenInstanceCount);
    }

    auto& visibleInstancesForSubMesh = batch->visibleInstances[subMesh->_id];
    if (!visibleInstancesForSubMesh.empty()) {
      for (auto& instance : visibleInstancesForSubMesh) {
        // World
        auto world = instance->getWorldMatrix();
        if (iOnBeforeDraw) {
          iOnBeforeDraw(true, world, effectiveMaterial);
        }

        // Draw
        _draw(subMesh, fillMode);
      }
    }
  }

  return *this;
}

Mesh& Mesh::render(SubMesh* subMesh, bool enableAlphaMode)
{
  if (_checkOcclusionQuery()) {
    return *this;
  }

  auto scene = getScene();

  // Managing instances
  auto batch = _getInstancesRenderList(subMesh->_id);

  if (batch->mustReturn) {
    return *this;
  }

  // Checking geometry state
  if (!_geometry || _geometry->getVertexBuffers().empty()
      || (!_unIndexed && !_geometry->getIndexBuffer())) {
    return *this;
  }

  onBeforeRenderObservable().notifyObservers(this);

  auto engine = scene->getEngine();
  auto hardwareInstancedRendering
    = (engine->getCaps().instancedArrays != false)
      && (batch->visibleInstances.find(subMesh->_id)
          != batch->visibleInstances.end())
      && (!batch->visibleInstances[subMesh->_id].empty());

  // Material
  auto iMaterial = subMesh->getMaterial();

  if (!iMaterial) {
    return *this;
  }

  _effectiveMaterial = iMaterial;

  if (_effectiveMaterial->storeEffectOnSubMeshes) {
    if (!_effectiveMaterial->isReadyForSubMesh(this, subMesh,
                                               hardwareInstancedRendering)) {
      return *this;
    }
  }
  else if (!_effectiveMaterial->isReady(this, hardwareInstancedRendering)) {
    return *this;
  }

  // Alpha mode
  if (enableAlphaMode) {
    engine->setAlphaMode(
      static_cast<unsigned>(_effectiveMaterial->alphaMode()));
  }

  for (auto& step : scene->_beforeRenderingMeshStage) {
    step.action(this, subMesh, batch);
  }

  EffectPtr effect = nullptr;
  if (_effectiveMaterial->storeEffectOnSubMeshes) {
    effect = subMesh->effect();
  }
  else {
    effect = _effectiveMaterial->getEffect();
  }

  if (!effect) {
    return *this;
  }

  auto sideOrientation = overrideMaterialSideOrientation;
  if (!sideOrientation.has_value()) {
    sideOrientation
      = static_cast<unsigned>(_effectiveMaterial->sideOrientation);
    if (_getWorldMatrixDeterminant() < 0) {
      sideOrientation
        = (sideOrientation == Material::ClockWiseSideOrientation() ?
             Material::CounterClockWiseSideOrientation() :
             Material::ClockWiseSideOrientation());
    }
  }

  auto reverse = _effectiveMaterial->_preBind(effect, *sideOrientation);

  if (_effectiveMaterial->forceDepthWrite) {
    engine->setDepthWrite(true);
  }

  // Bind
  auto fillMode = scene->forcePointsCloud() ?
                    Material::PointFillMode() :
                    (scene->forceWireframe() ? Material::WireFrameFillMode() :
                                               _effectiveMaterial->fillMode());

  // Binding will be done later because we need to add more info to the VB
  if (!hardwareInstancedRendering) {
    _bind(subMesh, effect, fillMode);
  }

  auto _world = getWorldMatrix();

  if (_effectiveMaterial->storeEffectOnSubMeshes) {
    _effectiveMaterial->bindForSubMesh(_world, this, subMesh);
  }
  else {
    _effectiveMaterial->bind(_world, this);
  }

  if (!_effectiveMaterial->backFaceCulling()
      && _effectiveMaterial->separateCullingPass) {
    engine->setState(true, _effectiveMaterial->zOffset, false, !reverse);
    _processRendering(
      subMesh, effect, static_cast<int>(fillMode), batch,
      hardwareInstancedRendering,
      [&](bool isInstance, Matrix world, Material* _effectiveMaterial) {
        _onBeforeDraw(isInstance, world, _effectiveMaterial);
      },
      _effectiveMaterial.get());
    engine->setState(true, _effectiveMaterial->zOffset, false, reverse);
  }

  // Draw
  _processRendering(
    subMesh, effect, static_cast<int>(fillMode), batch,
    hardwareInstancedRendering,
    [&](bool isInstance, Matrix world, Material* _effectiveMaterial) {
      _onBeforeDraw(isInstance, world, _effectiveMaterial);
    });

  // Unbind
  _effectiveMaterial->unbind();

  for (auto& step : scene->_afterRenderingMeshStage) {
    step.action(this, subMesh, batch);
  }

  onAfterRenderObservable().notifyObservers(this);

  return *this;
}

Mesh& Mesh::_onBeforeDraw(bool isInstance, Matrix& world,
                          Material* effectiveMaterial)
{
  if (isInstance && effectiveMaterial) {
    effectiveMaterial->bindOnlyWorldMatrix(world);
  }
  return *this;
}

std::vector<IParticleSystemPtr> Mesh::getEmittedParticleSystems()
{
  std::vector<IParticleSystemPtr> results;
  for (auto& particleSystem : getScene()->particleSystems) {
    auto& emitter = particleSystem->emitter;
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

  for (auto& particleSystem : getScene()->particleSystems) {
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
  for (auto& mesh : getScene()->meshes) {
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
    auto t = matricesWeights[a] + matricesWeights[a + 1]
             + matricesWeights[a + 2] + matricesWeights[a + 3];
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
  auto matricesWeightsExtra
    = getVerticesData(VertexBuffer::MatricesWeightsExtraKind);
  auto matricesWeights = getVerticesData(VertexBuffer::MatricesWeightsKind);
  auto numWeights      = matricesWeights.size();

  for (size_t a = 0; a < numWeights; a += 4) {
    // accumulate weights
    auto t = matricesWeights[a] + matricesWeights[a + 1]
             + matricesWeights[a + 2] + matricesWeights[a + 3];
    t += matricesWeightsExtra[a] + matricesWeightsExtra[a + 1]
         + matricesWeightsExtra[a + 2] + matricesWeightsExtra[a + 3];
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
  auto matricesWeightsExtra
    = getVerticesData(VertexBuffer::MatricesWeightsExtraKind);
  auto matricesWeights = getVerticesData(VertexBuffer::MatricesWeightsKind);
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
  const float toleranceEpsilon = 0.001f;

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
          tolerance += std::abs(matricesWeights[a + b]
                                - (matricesWeights[a + b] * recip));
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
  auto numBones        = skeleton()->bones.size();
  auto matricesIndices = getVerticesData(VertexBuffer::MatricesIndicesKind);
  auto matricesIndicesExtra
    = getVerticesData(VertexBuffer::MatricesIndicesExtraKind);
  auto numBadBoneIndices = 0u;
  for (size_t a = 0; a < numWeights; a++) {
    for (size_t b = 0; b < numInfluences; b++) {
      auto index = b < 4 ? matricesIndices[b] : matricesIndicesExtra[b - 4];
      if (index >= numBones || index < 0) {
        numBadBoneIndices++;
      }
    }
  }

  // log mesh stats
  std::ostringstream output;
  output << "Number of Weights = " << (numWeights / 4)
         << "\nMaximum influences = " << maxUsedWeights
         << "\nMissing Weights = " << missingWeights
         << "\nNot Sorted = " << numberNotSorted
         << "\nNot Normalized = " << numberNotNormalized << "\nWeightCounts = ["
         << usedWeightCounts << "]"
         << "\nNumber of bones = " << numBones
         << "\nBad Bone Indices = " << numBadBoneIndices;

  bool isValid = (missingWeights == 0 && numberNotNormalized == 0
                  && numBadBoneIndices == 0);

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
  else if (delayLoadState == EngineConstants::DELAYLOADSTATE_NOTLOADED) {
    delayLoadState = EngineConstants::DELAYLOADSTATE_LOADING;

    _queueLoad(scene);
  }

  return *this;
}

Mesh& Mesh::_queueLoad(Scene* scene)
{
  scene->_addPendingData(this);
  return *this;
}

bool Mesh::isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                       unsigned int /*strategy*/)
{
  if (delayLoadState == EngineConstants::DELAYLOADSTATE_LOADING) {
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
    Vector3::TransformCoordinates(Vector3::FromArray(data, index), transform)
      .toArray(temp, index);
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
  if (transform.m[0] * transform.m[5] * transform.m[10] < 0) {
    flipFaces();
  }

  // Restore submeshes
  releaseSubMeshes();
  subMeshes = std::move(_submeshes);

  return *this;
}

// Will apply current transform to mesh and reset world matrix
Mesh& Mesh::bakeCurrentTransformIntoVertices()
{
  bakeTransformIntoVertices(computeWorldMatrix(true));
  scaling().copyFromFloats(1.f, 1.f, 1.f);
  position().copyFromFloats(0.f, 0.f, 0.f);
  rotation().copyFromFloats(0.f, 0.f, 0.f);
  // only if quaternion is already set
  if (rotationQuaternion()) {
    rotationQuaternion = Quaternion::Identity();
  }
  _worldMatrix = Matrix::Identity();
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

MeshPtr Mesh::clone(const std::string& iName, Node* newParent,
                    bool doNotCloneChildren, bool clonePhysicsImpostor)
{
  return Mesh::New(iName, getScene(), newParent, this, doNotCloneChildren,
                   clonePhysicsImpostor);
}

void Mesh::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  morphTargetManager = nullptr;

  if (_geometry) {
    _geometry->releaseForMesh(this, true);
  }

  onBeforeDrawObservable().clear();
  onBeforeRenderObservable().clear();
  onAfterRenderObservable().clear();

  // Sources
  for (auto& mesh : getScene()->meshes) {
    if (mesh->type() == Type::MESH) {
      auto _mesh = static_cast<Mesh*>(mesh.get());
      if (_mesh->_source && _mesh->_source == this) {
        _mesh->_source = nullptr;
      }
    }
  }
  _source = nullptr;

  // Instances
  if (_instanceDataStorage->instancesBuffer) {
    _instanceDataStorage->instancesBuffer->dispose();
    _instanceDataStorage->instancesBuffer = nullptr;
  }

  for (auto& instance : instances) {
    instance->dispose();
  }

  AbstractMesh::dispose(doNotRecurse, disposeMaterialAndTextures);
}

Mesh& Mesh::applyDisplacementMap(
  const std::string& url, float minHeight, float maxHeight,
  const std::function<void(Mesh* mesh)> onSuccess,
  const std::optional<Vector2>& uvOffset, const std::optional<Vector2>& uvScale,
  bool forceUpdate)
{
  Tools::LoadImageFromUrl(
    url,
    [&](const Image& img) {
      auto heightMapWidth  = static_cast<unsigned>(img.width);
      auto heightMapHeight = static_cast<unsigned>(img.height);
      // Create VertexData from map data
      applyDisplacementMapFromBuffer(img.data, heightMapWidth, heightMapHeight,
                                     minHeight, maxHeight, uvOffset, uvScale,
                                     forceUpdate);
      // execute success callback, if set
      if (onSuccess) {
        onSuccess(this);
      }
    },
    nullptr, false);

  return *this;
}

void Mesh::applyDisplacementMapFromBuffer(
  const Uint8Array& buffer, unsigned int heightMapWidth,
  unsigned int heightMapHeight, float minHeight, float maxHeight,
  const std::optional<Vector2>& iUvOffset,
  const std::optional<Vector2>& iUvScale, bool forceUpdate)
{
  if (!isVerticesDataPresent(VertexBuffer::PositionKind)
      || !isVerticesDataPresent(VertexBuffer::NormalKind)
      || !isVerticesDataPresent(VertexBuffer::UVKind)) {
    BABYLON_LOG_WARN("Mesh",
                     "Cannot call applyDisplacementMap: Given mesh is not "
                     "complete. Position, Normal or UV are missing.");
    return;
  }

  auto positions = getVerticesData(VertexBuffer::PositionKind, true, true);
  auto normals   = getVerticesData(VertexBuffer::NormalKind);
  auto uvs       = getVerticesData(VertexBuffer::UVKind);
  auto iPosition = Vector3::Zero();
  auto normal    = Vector3::Zero();
  auto uv        = Vector2::Zero();

  Vector2 uvOffset = iUvOffset.has_value() ? *iUvOffset : Vector2::Zero();
  Vector2 uvScale  = iUvScale.has_value() ? *iUvScale : Vector2(1.f, 1.f);

  for (unsigned int index = 0; index < positions.size(); index += 3) {
    Vector3::FromArrayToRef(positions, index, iPosition);
    Vector3::FromArrayToRef(normals, index, normal);
    Vector2::FromArrayToRef(uvs, (index / 3) * 2, uv);

    // Compute height
    auto u = static_cast<unsigned int>(std::abs(uv.x * uvScale.x + uvOffset.x)
                                       * heightMapWidth)
             % heightMapWidth;
    auto v = static_cast<unsigned int>(std::abs(uv.y * uvScale.y + uvOffset.y)
                                       * heightMapHeight)
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
  std::string kind       = "";
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
  auto previousSubmeshes = stl_util::to_raw_ptr_vector(subMeshes);

  auto indices      = getIndices();
  auto totalIndices = getTotalIndices();

  // Generating unique vertices per face
  unsigned int index;
  for (index = 0; index < totalIndices; ++index) {
    auto vertexIndex = static_cast<unsigned int>(indices[index]);

    for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
      kind          = kinds[kindIndex];
      size_t stride = static_cast<size_t>(vbs[kind]->getStrideSize());

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
    SubMesh::AddToMesh(previousOne->materialIndex,
                       static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount,
                       static_cast<unsigned>(previousOne->indexStart),
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
  std::string kind       = "";
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
      kind          = kinds[kindIndex];
      size_t stride = static_cast<size_t>(vbs[kind]->getStrideSize());

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
    SubMesh::AddToMesh(previousOne->materialIndex,
                       static_cast<unsigned>(previousOne->indexStart),
                       previousOne->indexCount,
                       static_cast<unsigned>(previousOne->indexStart),
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

  vertex_data->applyToMesh(*this);
  return *this;
}

InstancedMeshPtr Mesh::createInstance(const std::string& iName)
{
  return InstancedMesh::New(iName, this);
}

Mesh& Mesh::synchronizeInstances()
{
  for (auto& instance : instances) {
    instance->_syncSubMeshes();
  }
  return *this;
}

void Mesh::optimizeIndices(
  const std::function<void(Mesh* mesh)>& successCallback)
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
    if (!(_pstring[0] == _pstring[1] || _pstring[0] == _pstring[2]
          || _pstring[1] == _pstring[2])) {
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

void Mesh::serialize(json& /*serializationObject*/)
{
}

void Mesh::_syncGeometryWithMorphTargetManager()
{
  auto iGeometry = geometry();
  if (!iGeometry) {
    return;
  }

  _markSubMeshesAsAttributesDirty();

  auto iMorphTargetManager = _morphTargetManager;
  if (iMorphTargetManager && iMorphTargetManager->vertexCount()) {
    if (iMorphTargetManager->vertexCount() != getTotalVertices()) {
      BABYLON_LOG_ERROR("Mesh",
                        "Mesh is incompatible with morph targets. Targets and "
                        "mesh must all have the same vertices count.");
      iMorphTargetManager = nullptr;
      return;
    }

    std::string indexStr;
    for (unsigned int index = 0; index < iMorphTargetManager->numInfluencers();
         ++index) {
      indexStr         = std::to_string(index);
      auto morphTarget = iMorphTargetManager->getActiveTarget(index);

      const auto positions = morphTarget->getPositions();
      if (positions.empty()) {
        BABYLON_LOG_ERROR("Mesh",
                          "Invalid morph target. Target must have positions.");
        return;
      }

      iGeometry->setVerticesData(VertexBuffer::PositionKind + indexStr,
                                 positions, false, 3);

      const auto normals = morphTarget->getNormals();
      if (!normals.empty()) {
        iGeometry->setVerticesData(VertexBuffer::NormalKind + indexStr, normals,
                                   false, 3);
      }

      const auto tangents = morphTarget->getTangents();
      if (!tangents.empty()) {
        iGeometry->setVerticesData(VertexBuffer::TangentKind + indexStr,
                                   tangents, false, 3);
      }
    }
  }
  else {
    unsigned int index = 0;
    auto indexStr      = std::to_string(index);

    // Positions
    while (
      iGeometry->isVerticesDataPresent(VertexBuffer::PositionKind + indexStr)) {
      iGeometry->removeVerticesData(VertexBuffer::PositionKind + indexStr);

      if (iGeometry->isVerticesDataPresent(VertexBuffer::NormalKind
                                           + indexStr)) {
        iGeometry->removeVerticesData(VertexBuffer::NormalKind + index);
      }
      if (iGeometry->isVerticesDataPresent(VertexBuffer::TangentKind
                                           + indexStr)) {
        iGeometry->removeVerticesData(VertexBuffer::TangentKind + indexStr);
      }
      ++index;
      indexStr = std::to_string(index);
    }
  }
}

std::vector<Vector3> Mesh::createInnerPoints(size_t pointsNb)
{
  const auto& boundInfo = getBoundingInfo();
  const auto diameter   = 2.f * boundInfo.boundingSphere.radius;
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

  auto gap      = 0.f;
  auto distance = 0.f;
  Ray ray(Vector3::Zero(), Axis::X());
  PickingInfo pickInfo;
  auto facetPoint        = Vector3::Zero();
  auto direction         = Vector3::Zero();
  auto particleDirection = Vector3::Zero();
  auto testPoint         = Vector3::Zero();

  for (size_t p = 0; p < pointsNb; ++p) {
    index = static_cast<unsigned>(
      std::floor(Scalar::RandomRange(0.f, indices.size() / 3.f)));
    id0 = indices[3 * index];
    id1 = indices[3 * index + 1];
    id2 = indices[3 * index + 2];
    v0X = positions[3 * id0];
    v0Y = positions[3 * id0 + 1];
    v0Z = positions[3 * id0 + 2];
    v1X = positions[3 * id1];
    v1Y = positions[3 * id1 + 1];
    v1Z = positions[3 * id1 + 2];
    v2X = positions[3 * id2];
    v2Y = positions[3 * id2 + 1];
    v2Z = positions[3 * id2 + 2];
    vertex0.set(v0X, v0Y, v0Z);
    vertex1.set(v1X, v1Y, v1Z);
    vertex2.set(v2X, v2Y, v2Z);
    vertex1.subtractToRef(vertex0, vec0);
    vertex2.subtractToRef(vertex1, vec1);

    norm   = getFacetNormal(index).normalize().scale(-1.f);
    tang   = vec0.copy().normalize();
    biNorm = Vector3::Cross(norm, tang);
    angle  = Scalar::RandomRange(0.f, Math::PI2);
    facetPlaneVec
      = tang.scale(std::cos(angle)).add(biNorm.scale(std::sin(angle)));
    angle = Scalar::RandomRange(0.1f, Math::PI);
    direction
      = facetPlaneVec.scale(std::cos(angle)).add(norm.scale(std::sin(angle)));

    // form a point inside the facet v0, v1, v2;
    lamda      = Scalar::RandomRange(0.f, 1.f);
    mu         = Scalar::RandomRange(0.f, 1.f);
    facetPoint = vertex0.add(vec0.scale(lamda)).add(vec1.scale(lamda * mu));

    gap           = 0.f;
    distance      = 0.f;
    ray.origin    = facetPoint;
    ray.direction = direction;
    ray.length    = diameter;
    pickInfo      = ray.intersectsMesh(this);
    if (pickInfo.hit) {
      distance = pickInfo.pickedPoint->subtract(facetPoint).length();
      gap      = Scalar::RandomRange(0.f, 1.f) * distance;
      point    = facetPoint.add(direction.scale(gap));
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
  auto max       = boundInfo.maximum();
  auto min       = boundInfo.minimum();
  auto diameter  = 2.f * boundInfo.boundingSphere.radius;
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

MeshPtr Mesh::Parse(const json& parsedMesh, Scene* scene,
                    const std::string& rootUrl)
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
    mesh->position
      = Vector3::FromArray(json_util::get_array<float>(parsedMesh, "position"));
  }

  if (json_util::has_valid_key_value(parsedMesh, "metadata")) {
    mesh->metadata = parsedMesh["metadata"];
  }

  if (json_util::has_valid_key_value(parsedMesh, "rotationQuaternion")) {
    mesh->rotationQuaternion = Quaternion::FromArray(
      json_util::get_array<float>(parsedMesh, "rotationQuaternion"));
  }
  else if (json_util::has_valid_key_value(parsedMesh, "rotation")) {
    mesh->rotation
      = Vector3::FromArray(json_util::get_array<float>(parsedMesh, "rotation"));
  }

  if (json_util::has_valid_key_value(parsedMesh, "scaling")) {
    mesh->scaling
      = Vector3::FromArray(json_util::get_array<float>(parsedMesh, "scaling"));
  }

  if (json_util::has_valid_key_value(parsedMesh, "localMatrix")) {
    auto tmpMatrix = Matrix::FromArray(
      json_util::get_array<float>(parsedMesh, "localMatrix"));
    mesh->setPreTransformMatrix(tmpMatrix);
  }
  else if (json_util::has_valid_key_value(parsedMesh, "pivotMatrix")) {
    auto tmpMatrix = Matrix::FromArray(
      json_util::get_array<float>(parsedMesh, "pivotMatrix"));
    mesh->setPivotMatrix(tmpMatrix);
  }

  mesh->setEnabled(json_util::get_bool(parsedMesh, "isEnabled", true));
  mesh->isVisible        = json_util::get_bool(parsedMesh, "isVisible", true);
  mesh->infiniteDistance = json_util::get_bool(parsedMesh, "infiniteDistance");

  mesh->showBoundingBox = json_util::get_bool(parsedMesh, "showBoundingBox");
  mesh->showSubMeshesBoundingBox
    = json_util::get_bool(parsedMesh, "showSubMeshesBoundingBox");

  if (json_util::has_valid_key_value(parsedMesh, "applyFog")) {
    mesh->applyFog = json_util::get_bool(parsedMesh, "applyFog", true);
  }

  if (json_util::has_valid_key_value(parsedMesh, "isPickable")) {
    mesh->isPickable = json_util::get_bool(parsedMesh, "isPickable", true);
  }

  if (json_util::has_valid_key_value(parsedMesh, "alphaIndex")) {
    mesh->alphaIndex = json_util::get_number(parsedMesh, "alphaIndex",
                                             std::numeric_limits<int>::max());
  }

  mesh->receiveShadows
    = json_util::get_bool(parsedMesh, "receiveShadows", false);
  mesh->billboardMode = json_util::get_number(parsedMesh, "billboardMode",
                                              AbstractMesh::BILLBOARDMODE_NONE);

  if (json_util::has_valid_key_value(parsedMesh, "visibility")) {
    mesh->visibility = json_util::get_number(parsedMesh, "visibility", 1.f);
  }

  mesh->checkCollisions = json_util::get_bool(parsedMesh, "checkCollisions");

  if (json_util::has_valid_key_value(parsedMesh, "isBlocker")) {
    mesh->isBlocker = json_util::get_bool(parsedMesh, "isBlocker");
  }

  mesh->_shouldGenerateFlatShading
    = json_util::get_bool(parsedMesh, "useFlatShading");

  // freezeWorldMatrix
  if (json_util::has_valid_key_value(parsedMesh, "freezeWorldMatrix")) {
    mesh->_waitingFreezeWorldMatrix
      = json_util::get_bool(parsedMesh, "freezeWorldMatrix");
  }

  // Parent
  if (json_util::has_valid_key_value(parsedMesh, "parentId")) {
    mesh->_waitingParentId = json_util::get_string(parsedMesh, "parentId");
  }

  // Actions
  if (json_util::has_valid_key_value(parsedMesh, "actions")) {
    mesh->_waitingActions = json_util::get_array<json>(parsedMesh, "actions");
  }

  // Overlay
  if (json_util::has_valid_key_value(parsedMesh, "overlayAlpha")) {
    mesh->overlayAlpha
      = json_util::get_number(parsedMesh, "overlayAlpha", 0.5f);
  }

  if (json_util::has_valid_key_value(parsedMesh, "overlayColor")) {
    mesh->overlayColor = Color3::FromArray(
      json_util::get_array<float>(parsedMesh, "overlayColor"));
  }

  if (json_util::has_valid_key_value(parsedMesh, "renderOverlay")) {
    mesh->renderOverlay = json_util::get_bool(parsedMesh, "renderOverlay");
  }

  // Geometry
  mesh->isUnIndexed = json_util::get_bool(parsedMesh, "isUnIndexed", false);
  mesh->hasVertexAlpha
    = json_util::get_bool(parsedMesh, "hasVertexAlpha", false);

  if (json_util::has_valid_key_value(parsedMesh, "delayLoadingFile")) {
    mesh->delayLoadState = EngineConstants::DELAYLOADSTATE_NOTLOADED;
    mesh->delayLoadingFile
      = rootUrl + json_util::get_string(parsedMesh, "delayLoadingFile");
    mesh->_boundingInfo = std::make_unique<BoundingInfo>(
      Vector3::FromArray(
        json_util::get_array<float>(parsedMesh, "boundingBoxMinimum")),
      Vector3::FromArray(
        json_util::get_array<float>(parsedMesh, "boundingBoxMaximum")));

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

    if (SceneLoader::ForceFullSceneLoadingForIncremental()
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
  const auto morphTargetManagerId
    = json_util::get_number(parsedMesh, "morphTargetManagerId", -1);
  if (morphTargetManagerId > -1) {
    mesh->morphTargetManager = scene->getMorphTargetManagerById(
      static_cast<unsigned>(morphTargetManagerId));
  }

  // Skeleton
  const auto parsedSkeletonId
    = json_util::get_number(parsedMesh, "skeletonId", -1);
  if (parsedSkeletonId > -1) {
    mesh->skeleton
      = scene->getLastSkeletonByID(std::to_string(parsedSkeletonId));
    if (json_util::has_valid_key_value(parsedMesh, "numBoneInfluencers")) {
      mesh->numBoneInfluencers
        = json_util::get_number(parsedMesh, "numBoneInfluencers", 0u);
    }
  }

  // Animations
  if (json_util::has_valid_key_value(parsedMesh, "animations")) {
    for (auto& parsedAnimation :
         json_util::get_array<json>(parsedMesh, "animations")) {
      mesh->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
    Node::ParseAnimationRanges(*mesh, parsedMesh, scene);
  }

  if (json_util::has_valid_key_value(parsedMesh, "autoAnimate")
      && json_util::get_bool(parsedMesh, "autoAnimate")) {
    scene->beginAnimation(
      mesh, json_util::get_number(parsedMesh, "autoAnimateFrom", 0.f),
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
    mesh->layerMask = static_cast<unsigned>(
      json_util::get_number(parsedMesh, "layerMask", 0x0FFFFFFF));
  }

  // Instances
  if (json_util::has_valid_key_value(parsedMesh, "instances")) {
    for (auto& parsedInstance :
         json_util::get_array<json>(parsedMesh, "instances")) {
      auto instance
        = mesh->createInstance(json_util::get_string(parsedInstance, "name"));

      if (json_util::has_valid_key_value(parsedInstance, "id")
          && !json_util::is_null(parsedMesh["localMatrix"])) {
        instance->id = json_util::get_string(parsedInstance, "id");
      }

      // Tags.AddTagsTo(instance, parsedInstance.tags);

      mesh->position = Vector3::FromArray(
        json_util::get_array<float>(parsedInstance, "position"));

      if (json_util::has_valid_key_value(parsedInstance, "parentId")) {
        instance->_waitingParentId
          = json_util::get_string(parsedInstance, "parentId");
      }

      if (json_util::has_valid_key_value(parsedInstance,
                                         "rotationQuaternion")) {
        instance->rotationQuaternion = Quaternion::FromArray(
          json_util::get_array<float>(parsedInstance, "rotationQuaternion"));
      }
      else if (json_util::has_valid_key_value(parsedInstance, "rotation")) {
        instance->rotation = Vector3::FromArray(
          json_util::get_array<float>(parsedInstance, "rotation"));
      }

      if (json_util::has_valid_key_value(parsedMesh, "scaling")) {
        instance->position = Vector3::FromArray(
          json_util::get_array<float>(parsedInstance, "scaling"));
      }

      instance->checkCollisions
        = json_util::get_bool(parsedInstance, "checkCollisions");

      if (json_util::has_valid_key_value(parsedMesh, "animations")) {
        for (auto& parsedAnimation :
             json_util::get_array<json>(parsedMesh, "animations")) {
          instance->animations.emplace_back(Animation::Parse(parsedAnimation));
        }
        Node::ParseAnimationRanges(*instance, parsedMesh, scene);

        if (json_util::has_valid_key_value(parsedMesh, "autoAnimate")) {
          scene->beginAnimation(
            instance, json_util::get_number(parsedMesh, "autoAnimateFrom", 0.f),
            json_util::get_number(parsedMesh, "autoAnimateTo", 0.f),
            json_util::get_bool(parsedMesh, "autoAnimateLoop"),
            json_util::get_number<float>(parsedMesh, "autoAnimateSpeed", 1.0));
        }
      }
    }
  }

  return mesh;
}

// Statics
MeshPtr Mesh::CreateRibbon(const std::string& name,
                           const std::vector<std::vector<Vector3>>& pathArray,
                           bool closeArray, bool closePath, int offset,
                           Scene* scene, bool updatable,
                           unsigned int sideOrientation,
                           const MeshPtr& instance)
{
  RibbonOptions options(pathArray, offset);
  options.closeArray      = closeArray;
  options.closePath       = closePath;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;

  return MeshBuilder::CreateRibbon(name, options, scene);
}

MeshPtr Mesh::CreateDisc(const std::string& name, float radius,
                         unsigned int tessellation, Scene* scene,
                         bool updatable, unsigned int sideOrientation)
{
  DiscOptions options;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateDisc(name, options, scene);
}

MeshPtr Mesh::CreateBox(const std::string& name, float size, Scene* scene,
                        bool updatable, unsigned int sideOrientation)
{
  BoxOptions options(size);
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateBox(name, options, scene);
}

MeshPtr Mesh::CreateSphere(const std::string& name, unsigned int segments,
                           float diameter, Scene* scene, bool updatable,
                           unsigned int sideOrientation)
{
  SphereOptions options;
  options.segments        = segments;
  options.diameterX       = diameter;
  options.diameterY       = diameter;
  options.diameterZ       = diameter;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateSphere(name, options, scene);
}

// Cylinder and cone
MeshPtr Mesh::CreateCylinder(const std::string& name, float height,
                             float diameterTop, float diameterBottom,
                             unsigned int tessellation,
                             unsigned int subdivisions, Scene* scene,
                             bool updatable, unsigned int sideOrientation)
{
  CylinderOptions options;
  options.height          = height;
  options.diameterTop     = diameterTop;
  options.diameterBottom  = diameterBottom;
  options.tessellation    = tessellation;
  options.subdivisions    = subdivisions;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateCylinder(name, options, scene);
}

// Torus
MeshPtr Mesh::CreateTorus(const std::string& name, float diameter,
                          float thickness, unsigned int tessellation,
                          Scene* scene, bool updatable,
                          unsigned int sideOrientation)
{
  TorusOptions options;
  options.diameter        = diameter;
  options.thickness       = thickness;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateTorus(name, options, scene);
}

MeshPtr Mesh::CreateTorusKnot(const std::string& name, float radius, float tube,
                              unsigned int radialSegments,
                              unsigned int tubularSegments, float p, float q,
                              Scene* scene, bool updatable,
                              unsigned int sideOrientation)
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

  return MeshBuilder::CreateTorusKnot(name, options, scene);
}

LinesMeshPtr Mesh::CreateLines(const std::string& name,
                               const std::vector<Vector3>& points, Scene* scene,
                               bool updatable, const LinesMeshPtr& instance)
{
  LinesOptions options;
  options.points    = points;
  options.updatable = updatable;
  options.instance  = instance;

  return MeshBuilder::CreateLines(name, options, scene);
}

LinesMeshPtr Mesh::CreateDashedLines(const std::string& name,
                                     std::vector<Vector3>& points,
                                     float dashSize, float gapSize,
                                     unsigned int dashNb, Scene* scene,
                                     bool updatable,
                                     const LinesMeshPtr& instance)
{
  DashedLinesOptions options;
  options.points    = points;
  options.dashSize  = dashSize;
  options.gapSize   = gapSize;
  options.dashNb    = dashNb;
  options.updatable = updatable;
  options.instance  = instance;

  return MeshBuilder::CreateDashedLines(name, options, scene);
}

MeshPtr Mesh::CreatePolygon(const std::string& name,
                            const std::vector<Vector3>& shape, Scene* scene,
                            const std::vector<std::vector<Vector3>>& holes,
                            bool updatable, unsigned int sideOrientation)
{
  PolygonOptions options;
  options.shape           = shape;
  options.holes           = holes;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;

  return MeshBuilder::CreatePolygon(name, options, scene);
}

MeshPtr Mesh::ExtrudePolygon(const std::string& name,
                             const std::vector<Vector3>& shape, float depth,
                             Scene* scene,
                             const std::vector<std::vector<Vector3>>& holes,
                             bool updatable, unsigned int sideOrientation)
{
  PolygonOptions options;
  options.shape           = shape;
  options.holes           = holes;
  options.depth           = depth;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;

  return MeshBuilder::ExtrudePolygon(name, options, scene);
}

MeshPtr Mesh::ExtrudeShape(const std::string& name,
                           const std::vector<Vector3>& shape,
                           const std::vector<Vector3>& path, float scale,
                           float rotation, unsigned int cap, Scene* scene,
                           bool updatable, unsigned int sideOrientation,
                           const MeshPtr& instance)
{
  ExtrudeShapeOptions options;
  options.shape           = shape;
  options.path            = path;
  options.scale           = scale;
  options.rotation        = rotation;
  options.cap             = cap;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;
  options.updatable       = updatable;

  return MeshBuilder::ExtrudeShape(name, options, scene);
}

MeshPtr Mesh::ExtrudeShapeCustom(
  const std::string& name, const std::vector<Vector3>& shape,
  const std::vector<Vector3>& path,
  const std::function<float(float i, float distance)>& scaleFunction,
  const std::function<float(float i, float distance)>& rotationFunction,
  bool ribbonCloseArray, bool ribbonClosePath, unsigned int cap, Scene* scene,
  bool updatable, unsigned int sideOrientation, const MeshPtr& instance)
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

  return MeshBuilder::ExtrudeShapeCustom(name, options, scene);
}

MeshPtr Mesh::CreateLathe(const std::string& name,
                          const std::vector<Vector3>& shape, float radius,
                          unsigned int tessellation, Scene* scene,
                          bool updatable, unsigned int sideOrientation)
{
  LatheOptions options;
  options.shape           = shape;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateLathe(name, options, scene);
}

MeshPtr Mesh::CreatePlane(const std::string& name, float size, Scene* scene,
                          bool updatable, unsigned int sideOrientation)
{
  PlaneOptions options(size);
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreatePlane(name, options, scene);
}

MeshPtr Mesh::CreateGround(const std::string& name, unsigned int width,
                           unsigned int height, unsigned int subdivisions,
                           Scene* scene, bool updatable)
{
  GroundOptions options(subdivisions);
  options.width     = width;
  options.height    = height;
  options.updatable = updatable;

  return MeshBuilder::CreateGround(name, options, scene);
}

MeshPtr Mesh::CreateTiledGround(const std::string& name, float xmin, float zmin,
                                float xmax, float zmax,
                                const ISize& subdivisions,
                                const ISize& precision, Scene* scene,
                                bool updatable)
{
  TiledGroundOptions options;
  options.xmin         = xmin;
  options.zmin         = zmin;
  options.xmax         = xmax;
  options.zmax         = zmax;
  options.subdivisions = subdivisions;
  options.precision    = precision;
  options.updatable    = updatable;

  return MeshBuilder::CreateTiledGround(name, options, scene);
}

GroundMeshPtr Mesh::CreateGroundFromHeightMap(
  const std::string& name, const std::string& url, unsigned int width,
  unsigned int height, unsigned int subdivisions, unsigned int minHeight,
  unsigned int maxHeight, Scene* scene, bool updatable,
  const std::function<void(GroundMesh* mesh)>& onReady,
  std::optional<float> alphaFilter)
{
  GroundFromHeightMapOptions options;
  options.width        = static_cast<float>(width);
  options.height       = static_cast<float>(height);
  options.subdivisions = subdivisions;
  options.minHeight    = static_cast<float>(minHeight);
  options.maxHeight    = static_cast<float>(maxHeight);
  options.updatable    = updatable;
  options.onReady      = onReady;
  options.alphaFilter  = alphaFilter;

  return MeshBuilder::CreateGroundFromHeightMap(name, url, options, scene);
}

MeshPtr Mesh::CreateTube(
  const std::string& name, const std::vector<Vector3>& path, float radius,
  unsigned int tessellation,
  const std::function<float(unsigned int i, float distance)>& radiusFunction,
  unsigned int cap, Scene* scene, bool updatable, unsigned int sideOrientation,
  const MeshPtr& instance)
{
  TubeOptions options;
  options.path           = path;
  options.radius         = radius;
  options.tessellation   = tessellation;
  options.radiusFunction = radiusFunction;
  options.setArc(1.f);
  options.cap             = cap;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;

  return MeshBuilder::CreateTube(name, options, scene);
}

MeshPtr Mesh::CreatePolyhedron(const std::string& name,
                               PolyhedronOptions& options, Scene* scene)
{
  return MeshBuilder::CreatePolyhedron(name, options, scene);
}

MeshPtr Mesh::CreateIcoSphere(const std::string& name,
                              IcoSphereOptions& options, Scene* scene)
{
  return MeshBuilder::CreateIcoSphere(name, options, scene);
}

MeshPtr Mesh::CreateDecal(const std::string& name,
                          const AbstractMeshPtr& sourceMesh,
                          const Vector3& position, const Vector3& normal,
                          const Vector3& size, float angle)
{
  DecalOptions options;
  options.position = position;
  options.normal   = normal;
  options.size     = size;
  options.angle    = angle;

  return MeshBuilder::CreateDecal(name, sourceMesh, options);
}

Float32Array& Mesh::setPositionsForCPUSkinning()
{
  Float32Array iSource;
  if (_sourcePositions.empty()) {
    iSource = getVerticesData(VertexBuffer::PositionKind);
    if (iSource.empty()) {
      return _sourceNormals;
    }

    _sourcePositions = iSource;

    if (!isVertexBufferUpdatable(VertexBuffer::PositionKind)) {
      setVerticesData(VertexBuffer::PositionKind, iSource, true);
    }
  }
  return _sourcePositions;
}

Float32Array& Mesh::setNormalsForCPUSkinning()
{
  Float32Array iSource;
  if (_sourceNormals.empty()) {
    iSource = getVerticesData(VertexBuffer::NormalKind);
    if (iSource.empty()) {
      return _sourceNormals;
    }

    _sourceNormals = iSource;

    if (!isVertexBufferUpdatable(VertexBuffer::NormalKind)) {
      setVerticesData(VertexBuffer::NormalKind, iSource, true);
    }
  }
  return _sourceNormals;
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
  if (!isVerticesDataPresent(VertexBuffer::NormalKind)) {
    return this;
  }
  if (!isVerticesDataPresent(VertexBuffer::MatricesIndicesKind)) {
    return this;
  }
  if (!isVerticesDataPresent(VertexBuffer::MatricesWeightsKind)) {
    return this;
  }

  if (_sourcePositions.empty()) {
    auto _submeshes = std::move(subMeshes);
    setPositionsForCPUSkinning();
    subMeshes = std::move(_submeshes);
  }

  if (_sourceNormals.empty()) {
    setNormalsForCPUSkinning();
  }

  // positionsData checks for not being Float32Array will only pass at most once
  auto positionsData = getVerticesData(VertexBuffer::PositionKind);

  if (positionsData.empty()) {
    return this;
  }

  // normalsData checks for not being Float32Array will only pass at most once
  auto normalsData = getVerticesData(VertexBuffer::NormalKind);

  if (normalsData.empty()) {
    return this;
  }

  auto matricesIndicesData = getVerticesData(VertexBuffer::MatricesIndicesKind);
  auto matricesWeightsData = getVerticesData(VertexBuffer::MatricesWeightsKind);

  if (matricesWeightsData.empty() || matricesIndicesData.empty()) {
    return this;
  }

  bool needExtras = numBoneInfluencers() > 4;
  auto matricesIndicesExtraData
    = needExtras ? getVerticesData(VertexBuffer::MatricesIndicesExtraKind) :
                   Float32Array();
  auto matricesWeightsExtraData
    = needExtras ? getVerticesData(VertexBuffer::MatricesWeightsExtraKind) :
                   Float32Array();

  auto skeletonMatrices = iSkeleton->getTransformMatrices(this);

  auto tempVector3 = Vector3::Zero();
  Matrix finalMatrix;
  Matrix tempMatrix;

  unsigned int matWeightIdx = 0;
  unsigned int inf;
  float weight;
  for (unsigned int index = 0; index < positionsData.size();
       index += 3, matWeightIdx += 4) {
    for (inf = 0; inf < 4; ++inf) {
      weight = matricesWeightsData[matWeightIdx + inf];
      if (weight > 0.f) {
        Matrix::FromFloat32ArrayToRefScaled(
          skeletonMatrices,
          static_cast<unsigned int>(
            std::floor(matricesIndicesData[matWeightIdx + inf] * 16)),
          weight, tempMatrix);
        finalMatrix.addToSelf(tempMatrix);
      }
    }
    if (needExtras) {
      for (inf = 0; inf < 4; ++inf) {
        weight = matricesWeightsExtraData[matWeightIdx + inf];
        if (weight > 0.f) {
          Matrix::FromFloat32ArrayToRefScaled(
            skeletonMatrices,
            static_cast<unsigned int>(
              std::floor(matricesIndicesExtraData[matWeightIdx + inf] * 16)),
            weight, tempMatrix);
          finalMatrix.addToSelf(tempMatrix);
        }
      }
    }

    Vector3::TransformCoordinatesFromFloatsToRef(
      _sourcePositions[index], _sourcePositions[index + 1],
      _sourcePositions[index + 2], finalMatrix, tempVector3);
    tempVector3.toArray(positionsData, index);

    Vector3::TransformNormalFromFloatsToRef(
      _sourceNormals[index], _sourceNormals[index + 1],
      _sourceNormals[index + 2], finalMatrix, tempVector3);
    tempVector3.toArray(normalsData, index);

    finalMatrix.reset();
  }

  updateVerticesData(VertexBuffer::PositionKind, positionsData);
  updateVerticesData(VertexBuffer::NormalKind, normalsData);

  return this;
}

MinMax Mesh::GetMinMax(const std::vector<AbstractMeshPtr>& meshes)
{
  bool minVectorSet = false;
  Vector3 minVector;
  Vector3 maxVector;
  for (auto& mesh : meshes) {
    const BoundingBox& boundingBox = mesh->getBoundingInfo().boundingBox;
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
  return Vector3::Center(MinMaxVectorAndDistance.min,
                         MinMaxVectorAndDistance.max);
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

MeshPtr Mesh::MergeMeshes(const std::vector<MeshPtr>& meshes,
                          bool disposeSource, bool allow32BitsIndices,
                          MeshPtr meshSubclass, bool subdivideWithSubMeshes)
{
  unsigned int index = 0;
  if (!allow32BitsIndices) {
    size_t totalVertices = 0;

    // Counting vertices
    for (index = 0; index < meshes.size(); ++index) {
      if (meshes[index]) {
        totalVertices += meshes[index]->getTotalVertices();

        if (totalVertices > 65536) {
          BABYLON_LOG_WARN("Mesh",
                           "Cannot merge meshes because resulting mesh will "
                           "have more than 65536 vertices. Please use "
                           "allow32BitsIndices = true to use 32 bits indices.");
          return nullptr;
        }
      }
    }
  }

  // Merge
  std::unique_ptr<VertexData> vertexData      = nullptr;
  std::unique_ptr<VertexData> otherVertexData = nullptr;
  IndicesArray indiceArray;
  MeshPtr source = nullptr;
  for (auto& mesh : meshes) {
    if (mesh) {
      const auto& wm  = mesh->computeWorldMatrix(true);
      otherVertexData = VertexData::ExtractFromMesh(mesh.get(), true, true);

      otherVertexData->transform(wm);

      if (vertexData) {
        vertexData->merge(*otherVertexData.get(), allow32BitsIndices);
      }
      else {
        vertexData = std::move(otherVertexData);
        source     = meshes[index];
      }

      if (subdivideWithSubMeshes) {
        indiceArray.emplace_back(
          static_cast<uint32_t>(meshes[index]->getTotalIndices()));
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
  meshSubclass->material        = source->getMaterial();
  meshSubclass->checkCollisions = source->checkCollisions();

  // Cleaning
  if (disposeSource) {
    for (auto& mesh : meshes) {
      if (mesh) {
        mesh->dispose();
      }
    }
  }

  // Subdivide
  if (subdivideWithSubMeshes) {
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

  return meshSubclass;
}

} // end of namespace BABYLON
