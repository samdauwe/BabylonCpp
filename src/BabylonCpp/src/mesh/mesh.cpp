#include <babylon/mesh/sub_mesh.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/engine/engine.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/layer/highlight_layer.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/materials/multi_material.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
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
#include <babylon/particles/particle_system.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/rendering/outline_renderer.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Mesh::Mesh(const std::string& iName, Scene* scene, Node* iParent, Mesh* source,
           bool doNotCloneChildren, bool clonePhysicsImpostor)
    : AbstractMesh{iName, scene}
    , delayLoadState{Engine::DELAYLOADSTATE_NONE}
    , _geometry{nullptr}
    , _onBeforeDrawObserver{nullptr}
    , _batchCache{std_util::make_unique<_InstancesBatch>()}
    , _instancesBufferSize{32 * 16 * 4} // maximum of 32 instances
    , _overridenInstanceCount{0}
    , _preActivateId{-1}
    , _sideOrientation{Mesh::DEFAULTSIDE}
    , _areNormalsFrozen{false}
    , _source{nullptr}
    , _tessellation{0}
    , _cap{Mesh::NO_CAP}
    , _arc{1.f}
    , _closePath{false}
    , _closeArray{false}
{
  _boundingInfo = nullptr;

  if (source) {
    // Source mesh
    _source = source;

    // Geometry
    if (source->_geometry) {
      source->_geometry->applyToMesh(this);
    }

    // Deep copy
    // Tools.DeepCopy(source, this, ["name", "material", "skeleton",
    // "instances"], []);

    // Parent
    Node::setParent(source->parent());

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
      auto impostor = physicsEngine->getImpostorForPhysicsObject(source);
      if (impostor) {
        physicsImpostor = impostor->clone(this);
      }
    }

    // Particles
    for (auto& system : scene->particleSystems) {
      if (system->emitter == source) {
        system->clone(system->name, this);
      }
    }
    computeWorldMatrix(true);
  }

  // Parent
  if (iParent != nullptr) {
    Node::setParent(iParent);
  }
}

Mesh::~Mesh()
{
}

Mesh* Mesh::source()
{
  return _source;
}

const char* Mesh::getClassName() const
{
  return "Mesh";
}

IReflect::Type Mesh::type() const
{
  return IReflect::Type::MESH;
}

void Mesh::setOnBeforeDraw(const std::function<void()>& callback)
{
  if (_onBeforeDrawObserver) {
    onBeforeDrawObservable.remove(_onBeforeDrawObserver);
  }
  _onBeforeDrawObserver = onBeforeDrawObservable.add(callback);
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
    oss << ", flat shading: "
        << (_geometry ? (getVerticesData(VertexBuffer::PositionKind).size() / 3
                             == getIndices().size() ?
                           "YES" :
                           "NO") :
                        "UNKNOWN");
  }
  return oss.str();
}

void Mesh::addSubMesh(SubMesh* subMesh)
{
  subMeshes.emplace_back(subMesh);
}

bool Mesh::hasLODLevels() const
{
  return _LODLevels.size() > 0;
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

  _LODLevels.emplace_back(std_util::make_unique<MeshLODLevel>(distance, mesh));

  if (mesh) {
    mesh->_masterMesh = this;
  }

  _sortLODLevels();

  return *this;
}

Mesh* Mesh::getLODLevelAtDistance(float distance)
{
  for (const auto& level : _LODLevels) {
    if (std_util::almost_equal(level->distance, distance)) {
      return level->mesh;
    }
  }
  return nullptr;
}

Mesh& Mesh::removeLODLevel(Mesh* mesh)
{
  for (auto& level : _LODLevels) {
    if (level->mesh == mesh) {
      std_util::erase(_LODLevels, level);
      if (mesh) {
        mesh->_masterMesh = nullptr;
      }
    }
  }

  _sortLODLevels();
  return *this;
}

AbstractMesh* Mesh::getLOD(Camera* camera, BoundingSphere* boundingSphere)
{
  if (_LODLevels.empty()) {
    return this;
  }

  float distanceToCamera = 0.f;
  if (boundingSphere) {
    boundingSphere->centerWorld.subtract(camera->globalPosition()).length();
  }
  else {
    getBoundingInfo()
      ->boundingSphere.centerWorld.subtract(camera->globalPosition())
      .length();
  }

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

Geometry* Mesh::geometry() const
{
  return _geometry;
}

void Mesh::setGeometry(Geometry* geometry)
{
  if (_geometry) {
    _geometry = nullptr;
  }

  _geometry = geometry;
}

size_t Mesh::getTotalVertices() const
{
  if (!_geometry) {
    return 0;
  }
  return _geometry->getTotalVertices();
}

Float32Array Mesh::getVerticesData(unsigned int kind, bool copyWhenShared)
{
  if (!_geometry) {
    return Float32Array();
  }
  return _geometry->getVerticesData(kind, copyWhenShared);
}

VertexBuffer* Mesh::getVertexBuffer(unsigned int kind)
{
  if (!_geometry) {
    return nullptr;
  }
  return _geometry->getVertexBuffer(kind);
}

bool Mesh::isVerticesDataPresent(unsigned int kind)
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

Uint32Array Mesh::getVerticesDataKinds()
{
  if (!_geometry) {
    Uint32Array result;
    if (!_delayInfo.empty()) {
      for (auto& vertexBuffer : _delayInfo) {
        result.emplace_back(vertexBuffer->getKind());
      }
    }
    return result;
  }
  return _geometry->getVerticesDataKinds();
}

size_t Mesh::getTotalIndices()
{
  if (!_geometry) {
    return 0;
  }
  return _geometry->getTotalIndices();
}

IndicesArray Mesh::getIndices(bool copyWhenShared)
{
  if (!_geometry) {
    return Uint32Array();
  }
  return _geometry->getIndices(copyWhenShared);
}

bool Mesh::isBlocked()
{
  return _masterMesh != nullptr;
}

bool Mesh::isReady() const
{
  if (delayLoadState == Engine::DELAYLOADSTATE_LOADING) {
    return false;
  }

  return AbstractMesh::isReady();
}

bool Mesh::isDisposed() const
{
  return _isDisposed;
}

unsigned int Mesh::sideOrientation() const
{
  return _sideOrientation;
}

void Mesh::setSideOrientation(unsigned int sideO)
{
  _sideOrientation = sideO;
}

bool Mesh::areNormalsFrozen() const
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

void Mesh::setOverridenInstanceCount(size_t count)
{
  _overridenInstanceCount = count;
}

void Mesh::_preActivate()
{
  int sceneRenderId = getScene()->getRenderId();
  if (_preActivateId == sceneRenderId) {
    return;
  }

  _preActivateId    = sceneRenderId;
  _visibleInstances = nullptr;
}

void Mesh::_preActivateForIntermediateRendering(int renderId)
{
  if (_visibleInstances) {
    _visibleInstances->intermediateDefaultRenderId = renderId;
  }
}

Mesh& Mesh::_registerInstanceForRenderId(InstancedMesh* instance, int renderId)
{
  if (!_visibleInstances) {
    _visibleInstances = std_util::make_unique<_VisibleInstances>();
    _visibleInstances->defaultRenderId     = renderId;
    _visibleInstances->selfDefaultRenderId = _renderId;
  }

  if (_visibleInstances->meshes.find(renderId)
      == _visibleInstances->meshes.end()) {
    _visibleInstances->meshes[renderId] = std::vector<InstancedMesh*>();
  }

  _visibleInstances->meshes[renderId].emplace_back(instance);

  return *this;
}

Mesh& Mesh::refreshBoundingInfo()
{
  if (_boundingInfo->isLocked()) {
    return *this;
  }

  auto data = getVerticesData(VertexBuffer::PositionKind);

  if (!data.empty()) {
    auto extend   = Tools::ExtractMinAndMax(data, 0, getTotalVertices());
    _boundingInfo = std_util::make_unique<BoundingInfo>(extend.min, extend.max);
  }

  if (!subMeshes.empty()) {
    for (auto& subMesh : subMeshes) {
      subMesh->refreshBoundingInfo();
    }
  }

  _updateBoundingInfo();

  return *this;
}

SubMesh* Mesh::_createGlobalSubMesh()
{
  size_t totalVertices = getTotalVertices();
  if (!totalVertices) {
    return nullptr;
  }

  releaseSubMeshes();
  return SubMesh::New(0, 0, totalVertices, 0, getTotalIndices(),
                      dynamic_cast<AbstractMesh*>(this));
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
                               this);

    offset += subdivisionSize;
  }

  synchronizeInstances();
}

void Mesh::setVerticesData(unsigned int kind, const Float32Array& data,
                           bool updatable, int stride)
{
  if (!_geometry) {
    auto vertexData = std_util::make_unique<VertexData>();
    vertexData->set(data, kind);

    auto scene = getScene();

    Geometry::New(Geometry::RandomId(), scene, vertexData.get(), updatable,
                  this);
  }
  else {
    _geometry->setVerticesData(kind, data, updatable, stride);
  }
}

void Mesh::markVerticesDataAsUpdatable(unsigned int kind, bool updatable)
{
  if (getVertexBuffer(kind)->isUpdatable() == updatable) {
    return;
  }

  setVerticesData(kind, getVerticesData(kind), updatable);
}

Mesh& Mesh::setVerticesBuffer(std::unique_ptr<VertexBuffer>&& buffer)
{
  if (!_geometry) {
    auto scene = getScene();

    Geometry::New(Geometry::RandomId(), scene)->applyToMesh(this);
  }

  _geometry->setVerticesBuffer(std::move(buffer));

  return *this;
}

void Mesh::updateVerticesData(unsigned int kind, const Float32Array& data,
                              bool updateExtends, bool makeItUnique)
{
  if (!_geometry) {
    return;
  }
  if (!makeItUnique) {
    _geometry->updateVerticesData(kind, data, updateExtends);
  }
  else {
    makeGeometryUnique();
    updateVerticesData(kind, data, updateExtends, false);
  }
}

Mesh& Mesh::updateMeshPositions(
  std::function<void(Float32Array& positions)> positionFunction,
  bool computeNormals)
{
  auto positions = getVerticesData(VertexBuffer::PositionKind);
  positionFunction(positions);
  updateVerticesData(VertexBuffer::PositionKind, positions, false, false);
  if (computeNormals) {
    auto indices = getIndices();
    auto normals = getVerticesData(VertexBuffer::NormalKind);
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

void Mesh::setIndices(const IndicesArray& indices, int totalVertices)
{
  if (!_geometry) {
    auto vertexData     = std_util::make_unique<VertexData>();
    vertexData->indices = indices;

    auto scene = getScene();

    Geometry(Geometry::RandomId(), scene, vertexData.get(), false, this);
  }
  else {
    _geometry->setIndices(indices, totalVertices);
  }
}

Mesh& Mesh::toLeftHanded()
{
  if (!_geometry) {
    return *this;
  }

  _geometry->toLeftHanded();

  return *this;
}

void Mesh::_bind(SubMesh* subMesh, Effect* effect, unsigned int fillMode)
{
  auto engine = getScene()->getEngine();

  // Wireframe
  GL::IGLBuffer* indexToBind;

  if (_unIndexed) {
    indexToBind = nullptr;
  }
  else {
    switch (fillMode) {
      case Material::PointFillMode:
        indexToBind = nullptr;
        break;
      case Material::WireFrameFillMode:
        indexToBind = subMesh->getLinesIndexBuffer(getIndices(), engine);
        break;
      default:
      case Material::TriangleFillMode:
        indexToBind = _unIndexed ? nullptr : _geometry->getIndexBuffer();
        break;
    }
  }

  // VBOs
  _geometry->_bind(effect, indexToBind);
}

void Mesh::_draw(SubMesh* subMesh, int fillMode, size_t instancesCount)
{
  if (!_geometry || !_geometry->getVertexBuffers().size()
      || !_geometry->getIndexBuffer()) {
    return;
  }

  onBeforeDrawObservable.notifyObservers(this);

  auto engine               = getScene()->getEngine();
  auto subMeshVerticesStart = static_cast<int>(subMesh->verticesStart);
  auto subMeshVerticesCount = static_cast<size_t>(subMesh->verticesCount);

  // Draw order
  switch (fillMode) {
    case Material::PointFillMode:
      engine->drawPointClouds(subMeshVerticesStart, subMeshVerticesCount,
                              instancesCount);
      break;
    case Material::WireFrameFillMode:
      if (_unIndexed) {
        engine->drawUnIndexed(false, subMeshVerticesStart, subMeshVerticesCount,
                              instancesCount);
      }
      else {
        engine->draw(false, 0,
                     instancesCount > 0 ? subMesh->linesIndexCount / 2 :
                                          subMesh->linesIndexCount,
                     instancesCount);
      }
      break;
    default:
      if (_unIndexed) {
        engine->drawUnIndexed(true, subMeshVerticesStart, subMeshVerticesCount,
                              instancesCount);
      }
      else {
        engine->draw(true, static_cast<unsigned>(subMesh->indexStart),
                     subMesh->indexCount, instancesCount);
      }
  }
}

Mesh& Mesh::registerBeforeRender(
  const std::function<void(AbstractMesh* mesh)>& func)
{
  onBeforeRenderObservable.add(func);
  return *this;
}

Mesh& Mesh::unregisterBeforeRender(
  const std::function<void(AbstractMesh* mesh)>& func)
{
  onBeforeRenderObservable.removeCallback(func);
  return *this;
}

Mesh& Mesh::registerAfterRender(
  const std::function<void(AbstractMesh* mesh)>& func)
{
  onAfterRenderObservable.add(func);
  return *this;
}

Mesh& Mesh::unregisterAfterRender(
  const std::function<void(AbstractMesh* mesh)>& func)
{
  onAfterRenderObservable.removeCallback(func);
  return *this;
}

_InstancesBatch* Mesh::_getInstancesRenderList(size_t subMeshId)
{
  auto scene                               = getScene();
  _batchCache->mustReturn                  = false;
  _batchCache->renderSelf[subMeshId]       = isEnabled() && isVisible;
  _batchCache->visibleInstances[subMeshId] = std::vector<InstancedMesh*>();

  if (_visibleInstances) {
    auto currentRenderId_ = scene->getRenderId();
    auto defaultRenderId  = scene->_isInIntermediateRendering() ?
                             _visibleInstances->intermediateDefaultRenderId :
                             _visibleInstances->defaultRenderId;
    _batchCache->visibleInstances[subMeshId]
      = _visibleInstances->meshes[currentRenderId_];
    int selfRenderId = _renderId;

    if ((_batchCache->visibleInstances.find(subMeshId)
         == _batchCache->visibleInstances.end())
        && defaultRenderId) {
      _batchCache->visibleInstances[subMeshId]
        = _visibleInstances->meshes[defaultRenderId];
      currentRenderId_ = std::max(defaultRenderId, currentRenderId_);
      selfRenderId
        = std::max(_visibleInstances->selfDefaultRenderId, currentRenderId_);
    }

    if ((_batchCache->visibleInstances.find(subMeshId)
         != _batchCache->visibleInstances.end())
        && _batchCache->visibleInstances[subMeshId].size() > 0) {
      if (_renderIdForInstances[subMeshId] == currentRenderId_) {
        _batchCache->mustReturn = true;
        return _batchCache.get();
      }

      if (currentRenderId_ != selfRenderId) {
        _batchCache->renderSelf[subMeshId] = false;
      }
    }
    _renderIdForInstances[subMeshId] = currentRenderId_;
  }

  return _batchCache.get();
}

Mesh& Mesh::_renderWithInstances(SubMesh* subMesh, int fillMode,
                                 _InstancesBatch* batch, Effect* effect,
                                 Engine* engine)
{
  if (batch->visibleInstances.find(subMesh->_id)
      == batch->visibleInstances.end()) {
    return *this;
  }

  std::vector<InstancedMesh*> visibleInstances
    = batch->visibleInstances[subMesh->_id];
  size_t matricesCount = visibleInstances.size() + 1;
  size_t bufferSize    = matricesCount * 16 * 4;

  auto currentInstancesBufferSize = _instancesBufferSize;

  while (_instancesBufferSize < bufferSize) {
    _instancesBufferSize *= 2;
  }

  if (_instancesData.empty()
      || currentInstancesBufferSize != _instancesBufferSize) {
    _instancesData = Float32Array(_instancesBufferSize / 4);
  }

  unsigned int offset         = 0;
  unsigned int instancesCount = 0;

  auto world = getWorldMatrix();
  if (batch->renderSelf[subMesh->_id]) {
    world->copyToArray(_instancesData, offset);
    offset += 16;
    instancesCount++;
  }

  if (!visibleInstances.empty()) {
    for (size_t instanceIndex = 0; instanceIndex < visibleInstances.size();
         ++instanceIndex) {
      InstancedMesh* instance = visibleInstances[instanceIndex];
      instance->getWorldMatrix()->copyToArray(_instancesData, offset);
      offset += 16;
      ++instancesCount;
    }
  }

  if (!_instancesBuffer || currentInstancesBufferSize != _instancesBufferSize) {
    if (_instancesBuffer) {
      _instancesBuffer->dispose();
    }

    _instancesBuffer = std_util::make_unique<Buffer>(engine, _instancesData,
                                                     true, 16, false, true);

    setVerticesBuffer(
      _instancesBuffer->createVertexBuffer(VertexBuffer::World0Kind, 0, 4));
    setVerticesBuffer(
      _instancesBuffer->createVertexBuffer(VertexBuffer::World1Kind, 4, 4));
    setVerticesBuffer(
      _instancesBuffer->createVertexBuffer(VertexBuffer::World2Kind, 8, 4));
    setVerticesBuffer(
      _instancesBuffer->createVertexBuffer(VertexBuffer::World3Kind, 12, 4));
  }
  else {
    _instancesBuffer->updateDirectly(_instancesData, 0, instancesCount);
  }

  geometry()->_bind(effect);

  _draw(subMesh, fillMode, instancesCount);

  engine->unbindInstanceAttributes();

  return *this;
}

Mesh& Mesh::_processRendering(SubMesh* subMesh, Effect* effect, int fillMode,
                              _InstancesBatch* batch,
                              bool hardwareInstancedRendering,
                              std::function<void(bool isInstance, Matrix world,
                                                 Material* effectiveMaterial)>
                                onBeforeDraw,
                              Material* effectiveMaterial)
{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  if (hardwareInstancedRendering) {
    _renderWithInstances(subMesh, fillMode, batch, effect, engine);
  }
  else {
    if (batch->renderSelf[subMesh->_id]) {
      // Draw
      if (onBeforeDraw) {
        onBeforeDraw(false, *getWorldMatrix(), effectiveMaterial);
      }

      _draw(subMesh, fillMode, _overridenInstanceCount);
    }

    if (!batch->visibleInstances[subMesh->_id].empty()) {
      for (auto& instance : batch->visibleInstances[subMesh->_id]) {
        // World
        Matrix* world = instance->getWorldMatrix();
        if (onBeforeDraw) {
          onBeforeDraw(true, *world, effectiveMaterial);
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
  auto scene = getScene();

  // Managing instances
  auto batch = _getInstancesRenderList(subMesh->_id);

  if (batch->mustReturn) {
    return *this;
  }

  // Checking geometry state
  if (!_geometry || _geometry->getVertexBuffers().empty()
      || !_geometry->getIndexBuffer()) {
    return *this;
  }

  onBeforeRenderObservable.notifyObservers(this);

  auto engine = scene->getEngine();
  auto hardwareInstancedRendering
    = (engine->getCaps().instancedArrays != false)
      && (batch->visibleInstances.find(subMesh->_id)
          != batch->visibleInstances.end())
      && (!batch->visibleInstances[subMesh->_id].empty());

  // Material
  auto effectiveMaterial = subMesh->getMaterial();

  if (!effectiveMaterial
      || !effectiveMaterial->isReady(this, hardwareInstancedRendering)) {
    return *this;
  }

  // Outline - step 1
  auto savedDepthWrite = engine->getDepthWrite();
  if (renderOutline) {
    engine->setDepthWrite(false);
    scene->getOutlineRenderer()->render(subMesh, batch);
    engine->setDepthWrite(savedDepthWrite);
  }

  effectiveMaterial->_preBind();
  auto effect = effectiveMaterial->getEffect();

  // Bind
  auto fillMode = scene->forcePointsCloud ?
                    Material::PointFillMode :
                    (scene->forceWireframe ? Material::WireFrameFillMode :
                                             effectiveMaterial->fillMode());
  _bind(subMesh, effect, fillMode);

  auto _world = getWorldMatrix();

  effectiveMaterial->bind(_world, this);

  // Alpha mode
  if (enableAlphaMode) {
    engine->setAlphaMode(effectiveMaterial->alphaMode);
  }

  // Draw
  _processRendering(
    subMesh, effect, static_cast<int>(fillMode), batch,
    hardwareInstancedRendering,
    [&](bool isInstance, Matrix world, Material* _effectiveMaterial) {
      _onBeforeDraw(isInstance, world, _effectiveMaterial);
    });

  // Unbind
  effectiveMaterial->unbind();

  // Outline - step 2
  if (renderOutline && savedDepthWrite) {
    engine->setDepthWrite(true);
    engine->setColorWrite(false);
    scene->getOutlineRenderer()->render(subMesh, batch);
    engine->setColorWrite(true);
  }

  // Overlay
  if (renderOverlay) {
    int currentMode = engine->getAlphaMode();
    engine->setAlphaMode(Engine::ALPHA_COMBINE);
    scene->getOutlineRenderer()->render(subMesh, batch, true);
    engine->setAlphaMode(currentMode);
  }

  onAfterRenderObservable.notifyObservers(this);

  return *this;
}

Mesh& Mesh::_onBeforeDraw(bool isInstance, Matrix& world,
                          Material* effectiveMaterial)
{
  if (isInstance) {
    effectiveMaterial->bindOnlyWorldMatrix(world);
  }
  return *this;
}

std::vector<ParticleSystem*> Mesh::getEmittedParticleSystems()
{
  std::vector<ParticleSystem*> results;
  for (auto& particleSystem : getScene()->particleSystems) {
    if (particleSystem->emitter == this) {
      results.emplace_back(particleSystem.get());
    }
  }

  return results;
}

std::vector<ParticleSystem*> Mesh::getHierarchyEmittedParticleSystems()
{
  std::vector<ParticleSystem*> results;
  std::vector<Node*> descendants = getDescendants();
  descendants.emplace_back(this);

  for (auto& particleSystem : getScene()->particleSystems) {
    if (std::find(std::begin(descendants), std::end(descendants),
                  particleSystem->emitter)
        != std::end(descendants)) {
      results.emplace_back(particleSystem.get());
    }
  }

  return results;
}

std::vector<BABYLON::Node*> Mesh::getChildren()
{
  std::vector<Node*> results;
  for (auto& mesh : getScene()->meshes) {
    if (mesh->parent() == this) {
      results.emplace_back(mesh.get());
    }
  }

  return results;
}

Mesh& Mesh::_checkDelayState()
{
  auto scene = getScene();

  if (_geometry) {
    _geometry->load(scene);
  }
  else if (delayLoadState == Engine::DELAYLOADSTATE_NOTLOADED) {
    delayLoadState = Engine::DELAYLOADSTATE_LOADING;

    _queueLoad(this, scene);
  }

  return *this;
}

Mesh& Mesh::_queueLoad(Mesh* mesh, Scene* scene)
{
  scene->_addPendingData(mesh);
  return *this;
}

bool Mesh::isInFrustum(const std::array<Plane, 6>& frustumPlanes)
{
  if (delayLoadState == Engine::DELAYLOADSTATE_LOADING) {
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
  for (auto& _material : materials) {
    if (_material->id == iId) {
      material = _material.get();
      return *this;
    }
  }

  // Multi
  const auto& multiMaterials = getScene()->multiMaterials;
  for (auto& multiMaterial : multiMaterials) {
    if (multiMaterial->id == iId) {
      material = multiMaterial.get();
      return *this;
    }
  }

  return *this;
}

std::vector<IAnimatable*> Mesh::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (material) {
    results.emplace_back(material);
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
  if (!isVerticesDataPresent(VertexBuffer::NormalKind)) {
    return *this;
  }
  data = getVerticesData(VertexBuffer::NormalKind);
  temp.clear();
  for (index = 0; index < data.size(); index += 3) {
    Vector3::TransformNormal(Vector3::FromArray(data, index), transform)
      .normalize()
      .toArray(temp, index);
  }
  setVerticesData(VertexBuffer::NormalKind, temp,
                  getVertexBuffer(VertexBuffer::NormalKind)->isUpdatable());

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
  if (rotationQuaternionSet()) {
    setRotationQuaternion(Quaternion::Identity());
  }
  _worldMatrix = std_util::make_unique<Matrix>(Matrix::Identity());
  return *this;
}

Mesh& Mesh::_resetPointsArrayCache()
{
  _positions.clear();
  return *this;
}

bool Mesh::_generatePointsArray()
{
  if (!_positions.empty()) {
    return true;
  }

  _positions.clear();

  auto data = getVerticesData(VertexBuffer::PositionKind);

  if (data.empty()) {
    return false;
  }

  _positions.resize(data.size());
  for (unsigned int index = 0; index < data.size(); index += 3) {
    _positions.emplace_back(Vector3::FromArray(data, index));
  }

  return true;
}

Mesh* Mesh::clone(const std::string& iName, Node* newParent,
                  bool doNotCloneChildren, bool clonePhysicsImpostor)
{
  return Mesh::New(iName, getScene(), newParent, this, doNotCloneChildren,
                   clonePhysicsImpostor);
}

void Mesh::dispose(bool /*doNotRecurse*/)
{
  if (_geometry) {
    _geometry->releaseForMesh(this, true);
  }

  // Sources
  for (auto& mesh : getScene()->meshes) {
    if (mesh->type() == IReflect::Type::MESH) {
      auto _mesh = static_cast<Mesh*>(mesh.get());
      if (_mesh->_source && _mesh->_source == this) {
        _mesh->_source = nullptr;
      }
    }
  }
  _source = nullptr;

  // Instances
  if (_instancesBuffer) {
    _instancesBuffer->dispose();
    _instancesBuffer.reset(nullptr);
  }

  for (auto& instance : instances) {
    instance->dispose();
  }

  // Highlight layers.
  auto& highlightLayers = getScene()->highlightLayers;
  for (auto& highlightLayer : highlightLayers) {
    if (highlightLayer) {
      highlightLayer->removeMesh(this);
      highlightLayer->removeExcludedMesh(this);
    }
  }

  // Events
  onBeforeRenderObservable.clear();
  onAfterRenderObservable.clear();
  onBeforeDrawObservable.clear();

  AbstractMesh::dispose();
}

void Mesh::applyDisplacementMap(const std::string& url, int minHeight,
                                int maxHeight,
                                const std::function<void(Mesh* mesh)> onSuccess)
{
  std::cout << url << minHeight << maxHeight;
  onSuccess(nullptr);
}

void Mesh::applyDisplacementMapFromBuffer(const Uint8Array& buffer,
                                          unsigned int heightMapWidth,
                                          unsigned int heightMapHeight,
                                          int minHeight, int maxHeight)
{
  if (!isVerticesDataPresent(VertexBuffer::PositionKind)
      || !isVerticesDataPresent(VertexBuffer::NormalKind)
      || !isVerticesDataPresent(VertexBuffer::UVKind)) {
    BABYLON_LOG_WARN("Mesh",
                     "Cannot call applyDisplacementMap: Given mesh is not "
                     "complete. Position, Normal or UV are missing.");
    return;
  }

  auto positions = getVerticesData(VertexBuffer::PositionKind);
  auto normals   = getVerticesData(VertexBuffer::NormalKind);
  auto uvs       = getVerticesData(VertexBuffer::UVKind);
  auto position  = Vector3::Zero();
  auto normal    = Vector3::Zero();
  auto uv        = Vector2::Zero();

  for (unsigned int index = 0; index < positions.size(); index += 3) {
    Vector3::FromArrayToRef(positions, index, position);
    Vector3::FromArrayToRef(normals, index, normal);
    Vector2::FromArrayToRef(uvs, (index / 3) * 2, uv);

    // Compute height
    auto u = (static_cast<unsigned int>(std::abs(uv.x)) * heightMapWidth)
             % heightMapWidth;
    auto v = (static_cast<unsigned int>(std::abs(uv.y)) * heightMapHeight)
             % heightMapHeight;

    auto pos = (u + v * heightMapWidth) * 4;
    auto r   = buffer[pos] / 255.f;
    auto g   = buffer[pos + 1] / 255.f;
    auto b   = buffer[pos + 2] / 255.f;

    auto gradient = r * 0.3f + g * 0.59f + b * 0.11f;

    normal.normalize();
    auto scale = static_cast<float>(minHeight)
                 + static_cast<float>(maxHeight - minHeight) * gradient;
    normal.scaleInPlace(scale);
    position = position.add(normal);

    position.toArray(positions, index);
  }

  VertexData::ComputeNormals(positions, getIndices(), normals);

  updateVerticesData(VertexBuffer::PositionKind, positions);
  updateVerticesData(VertexBuffer::NormalKind, normals);
}

Mesh& Mesh::convertToFlatShadedMesh()
{
  auto kinds = getVerticesDataKinds();
  std::map<unsigned int, VertexBuffer*> vbs;
  std::map<unsigned int, Float32Array> data;
  std::map<unsigned int, Float32Array> newdata;
  bool updatableNormals = false;
  unsigned int kindIndex;
  unsigned int kind;
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind                       = kinds[kindIndex];
    VertexBuffer* vertexBuffer = getVertexBuffer(kind);

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
  std::vector<SubMesh*> previousSubmeshes;
  std::for_each(subMeshes.begin(), subMeshes.end(),
                [&previousSubmeshes](const std::unique_ptr<SubMesh>& subMesh) {
                  previousSubmeshes.emplace_back(subMesh.get());
                });

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
    indices[index + 0] = static_cast<uint16_t>(index + 0);
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
    SubMesh::New(previousOne->materialIndex,
                 static_cast<unsigned>(previousOne->indexStart),
                 previousOne->indexCount,
                 static_cast<unsigned>(previousOne->indexStart),
                 previousOne->indexCount, dynamic_cast<AbstractMesh*>(this));
  }

  synchronizeInstances();
  return *this;
}

Mesh& Mesh::convertToUnIndexedMesh()
{
  auto kinds = getVerticesDataKinds();
  std::map<unsigned int, VertexBuffer*> vbs;
  std::map<unsigned int, Float32Array> data;
  std::map<unsigned int, Float32Array> newdata;
  unsigned int kindIndex;
  unsigned int kind;
  for (kindIndex = 0; kindIndex < kinds.size(); ++kindIndex) {
    kind          = kinds[kindIndex];
    vbs[kind]     = getVertexBuffer(kind);
    data[kind]    = vbs[kind]->getData();
    newdata[kind] = Float32Array();
  }

  // Save previous submeshes
  std::vector<SubMesh*> previousSubmeshes(subMeshes.size());
  std::for_each(subMeshes.begin(), subMeshes.end(),
                [&previousSubmeshes](const std::unique_ptr<SubMesh>& subMesh) {
                  previousSubmeshes.emplace_back(subMesh.get());
                });

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
    SubMesh::New(previousOne->materialIndex,
                 static_cast<unsigned>(previousOne->indexStart),
                 previousOne->indexCount,
                 static_cast<unsigned>(previousOne->indexStart),
                 previousOne->indexCount, dynamic_cast<AbstractMesh*>(this));
  }

  _unIndexed = true;

  synchronizeInstances();
  return *this;
}

Mesh& Mesh::flipFaces(bool flipNormals)
{
  auto vertex_data = VertexData::ExtractFromMesh(this);
  unsigned int i;
  if (flipNormals && isVerticesDataPresent(VertexBuffer::NormalKind)) {
    for (i = 0; i < vertex_data->normals.size(); ++i) {
      vertex_data->normals[i] *= -1;
    }
  }

  for (i = 0; i < vertex_data->indices.size(); i += 3) {
    // reassign indices
    std::swap(vertex_data->indices[i + 1], vertex_data->indices[i + 2]);
  }

  vertex_data->applyToMesh(this);
  return *this;
}

InstancedMesh* Mesh::createInstance(const std::string& iName)
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

/*void Mesh::simplify(
  const std::vector<ISimplificationSettings*>& settings,
  bool parallelProcessing, SimplificationType simplificationType,
  std::function<void(Mesh* mesh, int submeshIndex)>& successCallback)
{
  settings(nullptr);
  std::cout << parallelProcessing << simplificationType;
  successCallback(nullptr, 0);
}*/

void Mesh::optimizeIndices(
  const std::function<void(Mesh* mesh)>& successCallback)
{
  successCallback(nullptr);
}

Mesh* Mesh::Parse(const Json::value& parsedMesh, Scene* scene,
                  const std::string& rootUrl)
{
  Mesh* mesh = nullptr;
  if (Json::GetString(parsedMesh, "type") == "GroundMesh") {
    mesh = GroundMesh::Parse(parsedMesh, scene);
  }
  else {
    mesh = Mesh::New(Json::GetString(parsedMesh, "name"), scene);
  }
  mesh->id = Json::GetString(parsedMesh, "id");

  // Tags.AddTagsTo(mesh, parsedMesh.tags);

  if (parsedMesh.contains("position")) {
    mesh->setPosition(
      Vector3::FromArray(Json::ToArray<float>(parsedMesh, "position")));
  }

  if (parsedMesh.contains("metadata")) {
    // mesh.metadata = parsedMesh.metadata;
  }

  if (parsedMesh.contains("rotationQuaternion")) {
    mesh->setRotationQuaternion(Quaternion::FromArray(
      Json::ToArray<float>(parsedMesh, "rotationQuaternion")));
  }
  else if (parsedMesh.contains("rotation")) {
    mesh->setRotation(
      Vector3::FromArray(Json::ToArray<float>(parsedMesh, "rotation")));
  }

  if (parsedMesh.contains("scaling")) {
    mesh->setPosition(
      Vector3::FromArray(Json::ToArray<float>(parsedMesh, "scaling")));
  }

  if (parsedMesh.contains("localMatrix")) {
    mesh->setPivotMatrix(
      Matrix::FromArray(Json::ToArray<float>(parsedMesh, "localMatrix")));
  }
  else if (parsedMesh.contains("pivotMatrix")) {
    mesh->setPivotMatrix(
      Matrix::FromArray(Json::ToArray<float>(parsedMesh, "pivotMatrix")));
  }

  mesh->setEnabled(Json::GetBool(parsedMesh, "isEnabled", true));
  mesh->isVisible        = Json::GetBool(parsedMesh, "isVisible", true);
  mesh->infiniteDistance = Json::GetBool(parsedMesh, "infiniteDistance");

  mesh->showBoundingBox = Json::GetBool(parsedMesh, "showBoundingBox");
  mesh->showSubMeshesBoundingBox
    = Json::GetBool(parsedMesh, "showSubMeshesBoundingBox");

  if (parsedMesh.contains("applyFog")) {
    mesh->applyFog = Json::GetBool(parsedMesh, "applyFog", true);
  }

  if (parsedMesh.contains("isPickable")) {
    mesh->isPickable = Json::GetBool(parsedMesh, "isPickable", true);
  }

  if (parsedMesh.contains("alphaIndex")) {
    mesh->alphaIndex = Json::GetNumber(parsedMesh, "alphaIndex",
                                       std::numeric_limits<int>::max());
  }

  mesh->receiveShadows = Json::GetBool(parsedMesh, "receiveShadows", false);
  mesh->billboardMode  = Json::GetNumber(parsedMesh, "billboardMode",
                                        AbstractMesh::BILLBOARDMODE_NONE);

  if (parsedMesh.contains("visibility")) {
    mesh->visibility = Json::GetNumber(parsedMesh, "visibility", 1.f);
  }

  mesh->setCheckCollisions(Json::GetBool(parsedMesh, "checkCollisions"));

  if (parsedMesh.contains("isBlocker")) {
    mesh->isBlocker = Json::GetBool(parsedMesh, "isBlocker");
  }

  mesh->_shouldGenerateFlatShading
    = Json::GetBool(parsedMesh, "useFlatShading");

  // freezeWorldMatrix
  if (parsedMesh.contains("freezeWorldMatrix")) {
    mesh->_waitingFreezeWorldMatrix
      = Json::GetBool(parsedMesh, "freezeWorldMatrix");
  }

  // Parent
  if (parsedMesh.contains("parentId")) {
    mesh->_waitingParentId = Json::GetString(parsedMesh, "parentId");
  }

  // Actions
  if (parsedMesh.contains("actions")) {
    mesh->_waitingActions = Json::GetArray(parsedMesh, "actions");
  }

  // Overlay
  if (parsedMesh.contains("overlayAlpha")) {
    mesh->overlayAlpha = Json::GetNumber(parsedMesh, "overlayAlpha", 0.5f);
  }

  if (parsedMesh.contains("overlayColor")) {
    mesh->overlayColor
      = Color3::FromArray(Json::ToArray<float>(parsedMesh, "overlayColor"));
  }

  if (parsedMesh.contains("renderOverlay")) {
    mesh->renderOverlay = Json::GetBool(parsedMesh, "renderOverlay");
  }

  // Geometry
  mesh->hasVertexAlpha = Json::GetBool(parsedMesh, "hasVertexAlpha", false);

  if (parsedMesh.contains("delayLoadingFile")) {
    mesh->delayLoadState = Engine::DELAYLOADSTATE_NOTLOADED;
    mesh->delayLoadingFile
      = rootUrl + Json::GetString(parsedMesh, "delayLoadingFile");
    mesh->_boundingInfo = std_util::make_unique<BoundingInfo>(
      Vector3::FromArray(
        Json::ToArray<float>(parsedMesh, "boundingBoxMinimum")),
      Vector3::FromArray(
        Json::ToArray<float>(parsedMesh, "boundingBoxMaximum")));

    if (parsedMesh.contains("_binaryInfo")) {
      mesh->_binaryInfo = Json::GetString(parsedMesh, "_binaryInfo");
    }

    mesh->_delayInfoKinds.clear();
    if (parsedMesh.contains("hasUVs")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UVKind);
    }

    if (parsedMesh.contains("hasUVs2")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV2Kind);
    }

    if (parsedMesh.contains("hasUVs3")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV3Kind);
    }

    if (parsedMesh.contains("hasUVs4")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV4Kind);
    }

    if (parsedMesh.contains("hasUVs5")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV5Kind);
    }

    if (parsedMesh.contains("hasUVs6")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::UV6Kind);
    }

    if (parsedMesh.contains("hasColors")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::ColorKind);
    }

    if (parsedMesh.contains("hasMatricesIndices")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::MatricesIndicesKind);
    }

    if (parsedMesh.contains("hasMatricesWeights")) {
      mesh->_delayInfoKinds.emplace_back(VertexBuffer::MatricesWeightsKind);
    }

    mesh->_delayLoadingFunction = Geometry::ImportGeometry;

    if (SceneLoader::ForceFullSceneLoadingForIncremental) {
      mesh->_checkDelayState();
    }
  }
  else {
    Geometry::ImportGeometry(parsedMesh, mesh);
  }

  // Material
  if (parsedMesh.contains("materialId")) {
    mesh->setMaterialByID(Json::GetString(parsedMesh, "materialId"));
  }
  else {
    mesh->material = nullptr;
  }

  // Skeleton
  if (parsedMesh.contains("skeletonId")) {
    std::string parsedSkeletonId = Json::GetString(parsedMesh, "skeletonId");
    if (!parsedSkeletonId.empty()) {
      mesh->setSkeleton(scene->getLastSkeletonByID(parsedSkeletonId));
      if (parsedMesh.contains("numBoneInfluencers")) {
        mesh->numBoneInfluencers
          = Json::GetNumber(parsedMesh, "numBoneInfluencers", 0u);
      }
    }
  }

  // Animations
  if (parsedMesh.contains("animations")) {
    for (auto& parsedAnimation : Json::GetArray(parsedMesh, "animations")) {
      mesh->animations.emplace_back(Animation::Parse(parsedAnimation));
    }
    Node::ParseAnimationRanges(mesh, parsedMesh, scene);
  }

  if (parsedMesh.contains("autoAnimate")) {
    scene->beginAnimation(mesh,
                          Json::GetNumber(parsedMesh, "autoAnimateFrom", 0),
                          Json::GetNumber(parsedMesh, "autoAnimateTo", 0),
                          Json::GetBool(parsedMesh, "autoAnimateLoop", false),
                          Json::GetNumber(parsedMesh, "autoAnimateSpeed", 1.f));
  }

  // Layer Mask
  if (parsedMesh.contains("layerMask")) {
    auto layerMask = Json::GetString(parsedMesh, "layerMask");
    if (!layerMask.empty()) {
      mesh->layerMask = static_cast<unsigned>(std::stoi(layerMask));
    }
  }
  else {
    mesh->layerMask = static_cast<unsigned>(
      Json::GetNumber(parsedMesh, "layerMask", 0x0FFFFFFF));
  }

  // Instances
  if (parsedMesh.contains("instances")) {
    for (auto& parsedInstance : Json::GetArray(parsedMesh, "instances")) {
      auto instance
        = mesh->createInstance(Json::GetString(parsedInstance, "name"));

      // Tags.AddTagsTo(instance, parsedInstance.tags);

      mesh->setPosition(
        Vector3::FromArray(Json::ToArray<float>(parsedInstance, "position")));

      if (parsedInstance.contains("parentId")) {
        instance->_waitingParentId
          = Json::GetString(parsedInstance, "parentId");
      }

      if (parsedInstance.contains("rotationQuaternion")) {
        instance->setRotationQuaternion(Quaternion::FromArray(
          Json::ToArray<float>(parsedInstance, "rotationQuaternion")));
      }
      else if (parsedInstance.contains("rotation")) {
        instance->setRotation(
          Vector3::FromArray(Json::ToArray<float>(parsedInstance, "rotation")));
      }

      if (parsedMesh.contains("scaling")) {
        instance->setPosition(
          Vector3::FromArray(Json::ToArray<float>(parsedInstance, "scaling")));
      }

      instance->setCheckCollisions(
        Json::GetBool(parsedInstance, "checkCollisions"));

      if (parsedMesh.contains("animations")) {
        for (auto& parsedAnimation : Json::GetArray(parsedMesh, "animations")) {
          instance->animations.emplace_back(Animation::Parse(parsedAnimation));
        }
        Node::ParseAnimationRanges(instance, parsedMesh, scene);
      }
    }
  }

  return mesh;
}

// Statics
Mesh* Mesh::CreateRibbon(const std::string& name,
                         const std::vector<std::vector<Vector3>>& pathArray,
                         bool closeArray, bool closePath, int offset,
                         Scene* scene, bool updatable,
                         unsigned int sideOrientation, Mesh* instance)
{
  RibbonOptions options(pathArray, offset);
  options.closeArray      = closeArray;
  options.closePath       = closePath;
  options.updatable       = updatable;
  options.sideOrientation = sideOrientation;
  options.instance        = instance;

  return MeshBuilder::CreateRibbon(name, options, scene);
}

Mesh* Mesh::CreateDisc(const std::string& name, float radius,
                       unsigned int tessellation, Scene* scene, bool updatable,
                       unsigned int sideOrientation)
{
  DiscOptions options;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateDisc(name, options, scene);
}

Mesh* Mesh::CreateBox(const std::string& name, float size, Scene* scene,
                      bool updatable, unsigned int sideOrientation)
{
  BoxOptions options(size);
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateBox(name, options, scene);
}

Mesh* Mesh::CreateSphere(const std::string& name, unsigned int segments,
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
Mesh* Mesh::CreateCylinder(const std::string& name, float height,
                           float diameterTop, float diameterBottom,
                           unsigned int tessellation, unsigned int subdivisions,
                           Scene* scene, bool updatable,
                           unsigned int sideOrientation)
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
Mesh* Mesh::CreateTorus(const std::string& name, float diameter,
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

Mesh* Mesh::CreateTorusKnot(const std::string& name, float radius, float tube,
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

LinesMesh* Mesh::CreateLines(const std::string& name,
                             const std::vector<Vector3>& points, Scene* scene,
                             bool updatable, LinesMesh* instance)
{
  LinesOptions options;
  options.points    = points;
  options.updatable = updatable;
  options.instance  = instance;

  return MeshBuilder::CreateLines(name, options, scene);
}

LinesMesh* Mesh::CreateDashedLines(const std::string& name,
                                   std::vector<Vector3>& points, float dashSize,
                                   float gapSize, unsigned int dashNb,
                                   Scene* scene, bool updatable,
                                   LinesMesh* instance)
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

Mesh* Mesh::ExtrudeShape(const std::string& name,
                         const std::vector<Vector3>& shape,
                         const std::vector<Vector3>& path, float scale,
                         float rotation, unsigned int cap, Scene* scene,
                         bool updatable, unsigned int sideOrientation,
                         Mesh* instance)
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

Mesh* Mesh::ExtrudeShapeCustom(
  const std::string& name, const std::vector<Vector3>& shape,
  const std::vector<Vector3>& path,
  const std::function<float(float i, float distance)>& scaleFunction,
  const std::function<float(float i, float distance)>& rotationFunction,
  bool ribbonCloseArray, bool ribbonClosePath, unsigned int cap, Scene* scene,
  bool updatable, unsigned int sideOrientation, Mesh* instance)
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

Mesh* Mesh::CreateLathe(const std::string& name,
                        const std::vector<Vector3>& shape, float radius,
                        unsigned int tessellation, Scene* scene, bool updatable,
                        unsigned int sideOrientation)
{
  LatheOptions options;
  options.shape           = shape;
  options.radius          = radius;
  options.tessellation    = tessellation;
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreateLathe(name, options, scene);
}

Mesh* Mesh::CreatePlane(const std::string& name, float size, Scene* scene,
                        bool updatable, unsigned int sideOrientation)
{
  PlaneOptions options(size);
  options.sideOrientation = sideOrientation;
  options.updatable       = updatable;

  return MeshBuilder::CreatePlane(name, options, scene);
}

Mesh* Mesh::CreateGround(const std::string& name, unsigned int width,
                         unsigned int height, unsigned int subdivisions,
                         Scene* scene, bool updatable)
{
  GroundOptions options(subdivisions);
  options.width     = width;
  options.height    = height;
  options.updatable = updatable;

  return MeshBuilder::CreateGround(name, options, scene);
}

Mesh* Mesh::CreateTiledGround(const std::string& name, float xmin, float zmin,
                              float xmax, float zmax, const ISize& subdivisions,
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

GroundMesh* Mesh::CreateGroundFromHeightMap(
  const std::string& name, const std::string& url, unsigned int width,
  unsigned int height, unsigned int subdivisions, unsigned int minHeight,
  unsigned int maxHeight, Scene* scene, bool updatable,
  const std::function<void(GroundMesh* mesh)>& onReady)
{
  GroundFromHeightMapOptions options;
  options.width        = static_cast<float>(width);
  options.height       = static_cast<float>(height);
  options.subdivisions = subdivisions;
  options.minHeight    = static_cast<float>(minHeight);
  options.maxHeight    = static_cast<float>(maxHeight);
  options.updatable    = updatable;
  options.onReady      = onReady;

  return MeshBuilder::CreateGroundFromHeightMap(name, url, options, scene);
}

Mesh* Mesh::CreateTube(
  const std::string& name, const std::vector<Vector3>& path, float radius,
  unsigned int tessellation,
  const std::function<float(unsigned int i, unsigned int distance)>&
    radiusFunction,
  unsigned int cap, Scene* scene, bool updatable, unsigned int sideOrientation,
  Mesh* instance)
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

Mesh* Mesh::CreatePolyhedron(const std::string& name,
                             PolyhedronOptions& options, Scene* scene)
{
  return MeshBuilder::CreatePolyhedron(name, options, scene);
}

Mesh* Mesh::CreateIcoSphere(const std::string& name, IcoSphereOptions& options,
                            Scene* scene)
{
  return MeshBuilder::CreateIcoSphere(name, options, scene);
}

Mesh* Mesh::CreateDecal(const std::string& name, AbstractMesh* sourceMesh,
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
  Float32Array source;
  if (_sourcePositions.empty()) {
    source = getVerticesData(VertexBuffer::PositionKind);

    _sourcePositions = source;

    if (!getVertexBuffer(VertexBuffer::PositionKind)->isUpdatable()) {
      setVerticesData(VertexBuffer::PositionKind, source, true);
    }
  }
  return _sourcePositions;
}

Float32Array& Mesh::setNormalsForCPUSkinning()
{
  Float32Array source;
  if (_sourceNormals.empty()) {
    source = getVerticesData(VertexBuffer::NormalKind);

    _sourceNormals = source;

    if (!getVertexBuffer(VertexBuffer::NormalKind)->isUpdatable()) {
      setVerticesData(VertexBuffer::NormalKind, source, true);
    }
  }
  return _sourceNormals;
}

Mesh* Mesh::applySkeleton(Skeleton* skeleton)
{
  if (!_geometry) {
    return this;
  }

  if (_geometry->_softwareSkinningRenderId == getScene()->getRenderId()) {
    return this;
  }

  _geometry->_softwareSkinningRenderId = getScene()->getRenderId();

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

  // normalsData checks for not being Float32Array will only pass at most once
  auto normalsData = getVerticesData(VertexBuffer::NormalKind);

  auto matricesIndicesData = getVerticesData(VertexBuffer::MatricesIndicesKind);
  auto matricesWeightsData = getVerticesData(VertexBuffer::MatricesWeightsKind);

  bool needExtras = numBoneInfluencers > 4;
  auto matricesIndicesExtraData
    = needExtras ? getVerticesData(VertexBuffer::MatricesIndicesExtraKind) :
                   Float32Array();
  auto matricesWeightsExtraData
    = needExtras ? getVerticesData(VertexBuffer::MatricesWeightsExtraKind) :
                   Float32Array();

  auto skeletonMatrices = skeleton->getTransformMatrices(this);

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
          static_cast<unsigned>(matricesIndicesData[matWeightIdx + inf]) * 16,
          weight, tempMatrix);
        finalMatrix.addToSelf(tempMatrix);
      }
      else {
        break;
      }
    }
    if (needExtras) {
      for (inf = 0; inf < 4; ++inf) {
        weight = matricesWeightsExtraData[matWeightIdx + inf];
        if (weight > 0.f) {
          Matrix::FromFloat32ArrayToRefScaled(
            skeletonMatrices,
            static_cast<unsigned>(matricesIndicesExtraData[matWeightIdx + inf])
              * 16,
            weight, tempMatrix);
          finalMatrix.addToSelf(tempMatrix);
        }
        else {
          break;
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

MinMax Mesh::GetMinMax(const std::vector<AbstractMesh*>& meshes)
{
  bool minVectorSet = false;
  Vector3 minVector;
  Vector3 maxVector;
  for (auto& mesh : meshes) {
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
  minMax.min = minVector;
  minMax.max = maxVector;

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

Vector3 Mesh::Center(const std::vector<AbstractMesh*>& meshes)
{
  MinMax minMaxVector = Mesh::GetMinMax(meshes);
  return Vector3::Center(minMaxVector.min, minMaxVector.max);
}

Mesh* Mesh::MergeMeshes(std::vector<Mesh*>& meshes, bool disposeSource,
                        bool allow32BitsIndices, Mesh* meshSubclass,
                        bool subdivideWithSubMeshes)
{
  unsigned int index;
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
  Mesh* source = nullptr;
  for (index = 0; index < meshes.size(); ++index) {
    if (meshes[index]) {
      meshes[index]->computeWorldMatrix(true);
      otherVertexData = VertexData::ExtractFromMesh(meshes[index], true);
      otherVertexData->transform(*meshes[index]->getWorldMatrix());

      if (vertexData) {
        vertexData->merge(otherVertexData.get());
      }
      else {
        vertexData = std::move(otherVertexData);
        source     = meshes[index];
      }

      if (subdivideWithSubMeshes) {
        indiceArray.emplace_back(meshes[index]->getTotalIndices());
      }
    }
  }

  if (!meshSubclass) {
    meshSubclass = Mesh::New(source->name + "_merged", source->getScene());
  }

  if ((!vertexData) || (!source))
    return meshSubclass;

  vertexData->applyToMesh(meshSubclass);

  // Setting properties
  meshSubclass->material = source->getMaterial();
  meshSubclass->setCheckCollisions(source->checkCollisions());

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
    //-- Suppresions du submesh global
    meshSubclass->releaseSubMeshes();
    index               = 0;
    unsigned int offset = 0;
    //-- aplique la subdivision en fonction du tableau d'indices
    while (index < indiceArray.size()) {
      SubMesh::CreateFromIndices(0, offset, indiceArray[index], meshSubclass);
      offset += indiceArray[index];
      ++index;
    }
  }

  return meshSubclass;
}

} // end of namespace BABYLON
