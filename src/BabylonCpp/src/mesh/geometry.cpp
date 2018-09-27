#include <babylon/mesh/geometry.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/json.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/effect.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Geometry::Geometry(const std::string& iId, Scene* scene, VertexData* vertexData,
                   bool updatable, Mesh* mesh)
    : id{iId}
    , delayLoadState{EngineConstants::DELAYLOADSTATE_NONE}
    , boundingBias(this, &Geometry::get_boundingBias,
                   &Geometry::set_boundingBias)
    , _updatable{updatable}
    , extend(this, &Geometry::get_extend)
    , doNotSerialize(this, &Geometry::get_doNotSerialize)
    , _scene{scene}
    , _engine{scene->getEngine()}
    , _totalVertices{0}
    , _isDisposed{false}
    , _indexBuffer{nullptr}
    , _indexBufferIsUpdatable{false}
{
  _meshes.clear();
  // Init vertex buffer cache
  _vertexBuffers.clear();
  _indices.clear();

  // vertexData
  if (vertexData) {
    setAllVerticesData(vertexData, updatable);
  }
  else {
    _totalVertices = 0;
    _indices.clear();
  }

  if (_engine->getCaps().vertexArrayObject) {
    _vertexArrayObjects.clear();
  }

  // applyToMesh
  if (mesh) {
    if (mesh->type() == IReflect::Type::LINESMESH) {
      auto linesMesh = static_cast<LinesMesh*>(mesh);
      boundingBias   = Vector2(0, linesMesh->intersectionThreshold());
      _updateExtend(Float32Array());
    }

    applyToMesh(mesh);
    mesh->computeWorldMatrix(true);
  }
}

Geometry::~Geometry()
{
}

void Geometry::addToScene(const GeometryPtr& newGeometry)
{
  _scene->pushGeometry(newGeometry, true);
}

std::optional<Vector2>& Geometry::get_boundingBias()
{
  return _boundingBias;
}

void Geometry::set_boundingBias(const std::optional<Vector2>& value)
{
  if (_boundingBias && (*_boundingBias).equals(*value)) {
    return;
  }

  _boundingBias = value;

  _updateBoundingInfo(true, Float32Array());
}

GeometryPtr Geometry::CreateGeometryForMesh(Mesh* mesh)
{
  auto geometry = Geometry::New(Geometry::RandomId(), mesh->getScene());

  geometry->applyToMesh(mesh);

  return geometry;
}

MinMax& Geometry::get_extend()
{
  if (!_extend) {
    _updateExtend(Float32Array());
  }
  return *_extend;
}

Scene* Geometry::getScene()
{
  return _scene;
}

Engine* Geometry::getEngine()
{
  return _engine;
}

bool Geometry::isReady() const
{
  return delayLoadState == EngineConstants::DELAYLOADSTATE_LOADED
         || delayLoadState == EngineConstants::DELAYLOADSTATE_NONE;
}

bool Geometry::get_doNotSerialize() const
{
  for (const auto& mesh : _meshes) {
    if (!mesh->doNotSerialize) {
      return false;
    }
  }

  return true;
}

void Geometry::_rebuild()
{
  if (!_vertexArrayObjects.empty()) {
    _vertexArrayObjects.clear();
  }

  // Index buffer
  if (!_meshes.empty() && !_indices.empty()) {
    _indexBuffer = _engine->createIndexBuffer(_indices);
  }

  // Vertex buffers
  for (auto& item : _vertexBuffers) {
    item.second->_rebuild();
  }
}

void Geometry::setAllVerticesData(VertexData* vertexData, bool updatable)
{
  vertexData->applyToGeometry(*this, updatable);
  notifyUpdate();
}

AbstractMesh* Geometry::setVerticesData(unsigned int kind,
                                        const Float32Array& data,
                                        bool updatable,
                                        const std::optional<size_t>& stride)
{
  auto buffer = std::make_unique<VertexBuffer>(
    _engine, ToVariant<Float32Array, Buffer*>(data), kind, updatable,
    _meshes.empty(), stride);

  setVerticesBuffer(std::move(buffer));

  return nullptr;
}

void Geometry::removeVerticesData(unsigned int kind)
{
  if (stl_util::contains(_vertexBuffers, kind)) {
    _vertexBuffers[kind]->dispose();
    _vertexBuffers[kind].reset(nullptr);
  }
}

void Geometry::setVerticesBuffer(std::unique_ptr<VertexBuffer>&& buffer,
                                 const std::optional<size_t>& totalVertices)
{
  auto kind = buffer->getKind();
  if (stl_util::contains(_vertexBuffers, kind)) {
    _vertexBuffers[kind]->dispose();
  }

  _vertexBuffers[kind] = std::move(buffer);
  auto _buffer         = _vertexBuffers[kind].get();

  if (kind == VertexBuffer::PositionKind) {
    auto& data = _buffer->getData();

    if (totalVertices.has_value()) {
      _totalVertices = *totalVertices;
    }
    else {
      if (!data.empty()) {
        _totalVertices = data.size() / (_buffer->byteStride / 4);
      }
    }

    _updateExtend(data);
    _resetPointsArrayCache();

    for (auto& mesh : _meshes) {
      mesh->_boundingInfo
        = std::make_unique<BoundingInfo>(extend().min, extend().max);
      mesh->_createGlobalSubMesh(false);
      mesh->computeWorldMatrix(true);
    }
  }

  notifyUpdate(kind);

  if (!_vertexArrayObjects.empty()) {
    _disposeVertexArrayObjects();
    // Will trigger a rebuild of the VAO if supported
    _vertexArrayObjects.clear();
  }
}

void Geometry::updateVerticesDataDirectly(unsigned int kind,
                                          const Float32Array& data,
                                          size_t offset, bool useBytes)
{
  auto vertexBuffer = getVertexBuffer(kind);

  if (!vertexBuffer) {
    return;
  }

  vertexBuffer->updateDirectly(data, offset, useBytes);
  notifyUpdate(kind);
}

AbstractMesh* Geometry::updateVerticesData(unsigned int kind,
                                           const Float32Array& data,
                                           bool updateExtends,
                                           bool /*makeItUnique*/)
{
  auto vertexBuffer = getVertexBuffer(kind);

  if (!vertexBuffer) {
    return nullptr;
  }

  vertexBuffer->update(data);

  if (kind == VertexBuffer::PositionKind) {
    _updateBoundingInfo(updateExtends, data);
  }
  notifyUpdate(kind);

  return nullptr;
}

void Geometry::_updateBoundingInfo(bool updateExtends, const Float32Array& data)
{
  if (updateExtends) {
    _updateExtend(data);
  }

  _resetPointsArrayCache();

  for (auto& mesh : _meshes) {
    if (updateExtends) {
      mesh->_boundingInfo
        = std::make_unique<BoundingInfo>(extend().min, extend().max);

      for (auto& subMesh : mesh->subMeshes) {
        subMesh->refreshBoundingInfo();
      }
    }
  }
}

void Geometry::_bind(Effect* effect, GL::IGLBuffer* indexToBind)
{
  if (!effect) {
    return;
  }

  if (indexToBind == nullptr) {
    indexToBind = _indexBuffer.get();
  }

  auto vbs = getVertexBuffers();

  if (vbs.empty()) {
    return;
  }

  if (indexToBind != _indexBuffer.get() /*|| _vertexArrayObjects.empty()*/) {
    _engine->bindBuffers(vbs, indexToBind, effect);
    return;
  }

  // Using VAO
  if (!stl_util::contains(_vertexArrayObjects, effect->key())) {
    _vertexArrayObjects[effect->key()]
      = _engine->recordVertexArrayObject(vbs, indexToBind, effect);
  }

  _engine->bindVertexArrayObject(_vertexArrayObjects[effect->key()].get(),
                                 indexToBind);
}

size_t Geometry::getTotalVertices() const
{
  if (!isReady()) {
    return 0;
  }

  return _totalVertices;
}

Float32Array Geometry::getVerticesData(unsigned int kind, bool copyWhenShared,
                                       bool forceCopy)
{
  auto vertexBuffer = getVertexBuffer(kind);
  if (!vertexBuffer) {
    return Float32Array();
  }

  auto data = vertexBuffer->getData();
  if (data.empty()) {
    return Float32Array();
  }

  const auto tightlyPackedByteStride
    = vertexBuffer->getSize()
      * VertexBuffer::GetTypeByteLength(vertexBuffer->type);
  const auto count = _totalVertices * vertexBuffer->getSize();

  if (vertexBuffer->type != VertexBuffer::FLOAT
      || vertexBuffer->byteStride != tightlyPackedByteStride) {
    Float32Array copy(count);
    vertexBuffer->forEach(
      count, [&](float value, size_t index) { copy[index] = value; });
    return copy;
  }

  if (forceCopy || (copyWhenShared && _meshes.size() != 1)) {
    return data;
  }

  return data;
}

bool Geometry::isVertexBufferUpdatable(unsigned int kind) const
{
  auto it = _vertexBuffers.find(kind);

  if (it == _vertexBuffers.end()) {
    return false;
  }

  return it->second->isUpdatable();
}

VertexBuffer* Geometry::getVertexBuffer(unsigned int kind) const
{
  if (!isReady() || _vertexBuffers.empty()) {
    return nullptr;
  }

  auto it = _vertexBuffers.find(kind);
  if (it != _vertexBuffers.end()) {
    return it->second.get();
  }
  else {
    return nullptr;
  }
}

std::unordered_map<std::string, VertexBuffer*> Geometry::getVertexBuffers()
{
  std::unordered_map<std::string, VertexBuffer*> vertexBuffers;

  if (!isReady()) {
    return vertexBuffers;
  }

  for (const auto& item : _vertexBuffers) {
    const std::string kind = VertexBuffer::KindAsString(item.first);
    vertexBuffers[kind] = item.second.get();
  }

  return vertexBuffers;
}

bool Geometry::isVerticesDataPresent(unsigned int kind) const
{
  if (_vertexBuffers.empty()) {
    if (!_delayInfo.empty()) {
      return (std::find(_delayInfo.begin(), _delayInfo.end(), kind)
              != _delayInfo.end());
    }
    return false;
  }
  return (_vertexBuffers.find(kind) != _vertexBuffers.end());
}

Uint32Array Geometry::getVerticesDataKinds()
{
  Uint32Array result;
  if (_vertexBuffers.empty() && !_delayInfo.empty()) {
    for (auto& kind : _delayInfo) {
      result.emplace_back(kind);
    }
  }
  else {
    for (auto& item : _vertexBuffers) {
      result.emplace_back(item.first);
    }
  }

  return result;
}

void Geometry::updateIndices(const IndicesArray& indices, int offset)
{
  if (!_indexBuffer) {
    return;
  }

  if (!_indexBufferIsUpdatable) {
    setIndices(indices, 0, true);
  }
  else {
    _engine->updateDynamicIndexBuffer(_indexBuffer, indices, offset);
  }
}

AbstractMesh* Geometry::setIndices(const IndicesArray& indices,
                                   size_t totalVertices, bool updatable)
{
  if (_indexBuffer) {
    _engine->_releaseBuffer(_indexBuffer.get());
  }

  _disposeVertexArrayObjects();

  _indices                = indices;
  _indexBufferIsUpdatable = updatable;
  if (!_meshes.empty() && !_indices.empty()) {
    _indexBuffer = std::unique_ptr<GL::IGLBuffer>(
      _engine->createIndexBuffer(_indices, updatable));
  }

  if (totalVertices != 0) { // including null and undefined
    _totalVertices = static_cast<size_t>(totalVertices);
  }

  for (auto& mesh : _meshes) {
    mesh->_createGlobalSubMesh(true);
  }

  notifyUpdate();

  return nullptr;
}

size_t Geometry::getTotalIndices()
{
  if (!isReady()) {
    return 0;
  }
  return _indices.size();
}

IndicesArray Geometry::getIndices(bool copyWhenShared, bool forceCopy)
{
  if (!isReady()) {
    return IndicesArray();
  }
  if (!forceCopy && (!copyWhenShared || _meshes.size() == 1)) {
    return _indices;
  }
  else {
    IndicesArray _copy(_indices.size());
    std::copy(_indices.begin(), _indices.end(), _copy.begin());
    return _copy;
  }
}

GL::IGLBuffer* Geometry::getIndexBuffer()
{
  if (!isReady()) {
    return nullptr;
  }
  return _indexBuffer.get();
}

void Geometry::_releaseVertexArrayObject(Effect* effect)
{
  if (!effect || _vertexArrayObjects.empty()) {
    return;
  }

  if (stl_util::contains(_vertexArrayObjects, effect->key())) {
    _engine->releaseVertexArrayObject(_vertexArrayObjects[effect->key()].get());
    _vertexArrayObjects[effect->key()].reset(nullptr);
  }
}

void Geometry::releaseForMesh(Mesh* mesh, bool shouldDispose)
{
  auto it = std::find(_meshes.begin(), _meshes.end(), mesh);
  if (it == _meshes.end()) {
    return;
  }

  _meshes.erase(it);

  mesh->_geometry = nullptr;

  if (_meshes.empty() && shouldDispose) {
    dispose();
  }
}

void Geometry::applyToMesh(Mesh* mesh)
{
  if (mesh->geometry() == this) {
    return;
  }

  auto previousGeometry = mesh->geometry();
  if (previousGeometry) {
    previousGeometry->releaseForMesh(mesh);
  }

  // must be done before setting vertexBuffers because of
  // mesh->_createGlobalSubMesh()
  mesh->setGeometry(this);

  // Geometry is already in scene when constructed
  // _scene->pushGeometry(this);

  _meshes.emplace_back(mesh);

  if (isReady()) {
    _applyToMesh(mesh);
  }
  else {
    mesh->_boundingInfo = std::unique_ptr<BoundingInfo>(_boundingInfo.get());
  }
}

void Geometry::_updateExtend(Float32Array data)
{
  if (data.empty()) {
    data = getVerticesData(VertexBuffer::PositionKind);
  }

  _extend
    = Tools::ExtractMinAndMax(data, 0, _totalVertices, *boundingBias(), 3);
}

void Geometry::_applyToMesh(Mesh* mesh)
{
  auto numOfMeshes = _meshes.size();

  // vertexBuffers
  for (auto& item : _vertexBuffers) {
    const unsigned int kind = item.first;
    if (numOfMeshes == 1) {
      _vertexBuffers[kind]->create();
    }

    auto buffer = _vertexBuffers[kind]->getBuffer();
    if (buffer) {
      buffer->references = numOfMeshes;
    }

    if (kind == VertexBuffer::PositionKind) {
      mesh->_resetPointsArrayCache();

      if (!_extend) {
        _updateExtend(Float32Array());
      }
      mesh->_boundingInfo
        = std::make_unique<BoundingInfo>(extend().min, extend().max);

      mesh->_createGlobalSubMesh(false);

      // bounding info was just created again, world matrix should be applied
      // again.
      mesh->_updateBoundingInfo();
    }
  }

  // indexBuffer
  if (numOfMeshes == 1 && _indices.size() > 0) {
    _indexBuffer
      = std::unique_ptr<GL::IGLBuffer>(_engine->createIndexBuffer(_indices));
  }
  if (_indexBuffer) {
    _indexBuffer->references = numOfMeshes;
  }
}

void Geometry::notifyUpdate(unsigned int kind)
{
  if (onGeometryUpdated) {
    onGeometryUpdated(this, kind);
  }

  for (auto& mesh : _meshes) {
    mesh->_markSubMeshesAsAttributesDirty();
  }
}

void Geometry::load(Scene* scene, const std::function<void()>& onLoaded)
{
  if (delayLoadState == EngineConstants::DELAYLOADSTATE_LOADING) {
    return;
  }

  if (isReady()) {
    if (onLoaded) {
      onLoaded();
    }
    return;
  }

  delayLoadState = EngineConstants::DELAYLOADSTATE_LOADING;

  _queueLoad(scene, onLoaded);
}

void Geometry::_queueLoad(Scene* /*scene*/,
                          const std::function<void()>& /*onLoaded*/)
{
}

void Geometry::toLeftHanded()
{
  // Flip faces
  auto tIndices = getIndices(false);
  if (!tIndices.empty()) {
    for (unsigned int i = 0; i < tIndices.size(); i += 3) {
      std::swap(tIndices[i + 0], tIndices[i + 2]);
    }
    setIndices(tIndices);
  }

  // Negate position.z
  auto tPositions = getVerticesData(VertexBuffer::PositionKind, false);
  if (!tPositions.empty()) {
    for (unsigned int i = 0; i < tPositions.size(); i += 3) {
      tPositions[i + 2] = -tPositions[i + 2];
    }
    setVerticesData(VertexBuffer::PositionKind, tPositions, false);
  }

  // Negate normal.z
  auto tNormals = getVerticesData(VertexBuffer::NormalKind, false);
  if (!tNormals.empty()) {
    for (unsigned int i = 0; i < tNormals.size(); i += 3) {
      tNormals[i + 2] = -tNormals[i + 2];
    }
    setVerticesData(VertexBuffer::NormalKind, tNormals, false);
  }
}

void Geometry::_resetPointsArrayCache()
{
  _positions.clear();
}

bool Geometry::_generatePointsArray()
{
  if (!_positions.empty()) {
    return true;
  }

  auto data = getVerticesData(VertexBuffer::PositionKind);

  if (data.empty()) {
    return false;
  }

  _positions.clear();

  for (unsigned int index = 0; index < data.size(); index += 3) {
    _positions.emplace_back(Vector3::FromArray(data, index));
  }

  return true;
}

bool Geometry::isDisposed() const
{
  return _isDisposed;
}

void Geometry::_disposeVertexArrayObjects()
{
  if (!_vertexArrayObjects.empty()) {
    for (auto& item : _vertexArrayObjects) {
      _engine->releaseVertexArrayObject(item.second.get());
    }
    _vertexArrayObjects.clear();
  }
}

void Geometry::dispose()
{
  for (const auto& mesh : _meshes) {
    releaseForMesh(mesh);
  }
  _meshes.clear();

  _disposeVertexArrayObjects();

  for (auto& item : _vertexBuffers) {
    _vertexBuffers[item.first]->dispose();
    _vertexBuffers[item.first].reset(nullptr);
  }
  _vertexBuffers.clear();
  _totalVertices = 0;

  if (_indexBuffer) {
    _engine->_releaseBuffer(_indexBuffer.get());
  }
  _indexBuffer = nullptr;
  _indices.clear();

  delayLoadState = EngineConstants::DELAYLOADSTATE_NONE;
  delayLoadingFile.clear();
  _delayLoadingFunction = nullptr;
  _delayInfo.clear();

  _boundingInfo = nullptr;

  _scene->removeGeometry(this);
  _isDisposed = true;
}

GeometryPtr Geometry::copy(const std::string& iId)
{
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices.clear();

  auto indices = getIndices();
  if (!indices.empty()) {
    vertexData->indices.reserve(indices.size());
    for (auto& index : indices) {
      vertexData->indices.emplace_back(index);
    }
  }

  bool updatable    = false;
  bool stopChecking = false;
  unsigned int kind = 0;
  for (auto& vertexBuffer : _vertexBuffers) {
    kind      = vertexBuffer.first;
    auto data = getVerticesData(kind);
    vertexData->set(data, kind);
    if (!stopChecking) {
      auto vb = getVertexBuffer(kind);
      if (vb) {
        updatable    = getVertexBuffer(kind)->isUpdatable();
        stopChecking = !updatable;
      }
    }
  }

  auto geometry = Geometry::New(iId, _scene, vertexData.get(), updatable);

  geometry->delayLoadState        = delayLoadState;
  geometry->delayLoadingFile      = delayLoadingFile;
  geometry->_delayLoadingFunction = _delayLoadingFunction;
  geometry->_delayInfo            = _delayInfo;

  // Bounding info
  geometry->_boundingInfo
    = std::make_unique<BoundingInfo>(extend().min, extend().max);

  return geometry;
}

Json::object Geometry::serialize() const
{
  return Json::object();
}

Json::object Geometry::serializeVerticeData() const
{
  return Json::object();
}

GeometryPtr Geometry::ExtractFromMesh(Mesh* mesh, const std::string& id)
{
  auto geometry = mesh->geometry();

  if (!geometry) {
    return nullptr;
  }

  return geometry->copy(id);
}

std::string Geometry::RandomId()
{
  return Tools::RandomId();
}

void Geometry::_ImportGeometry(const Json::value& parsedGeometry,
                               const MeshPtr& mesh)
{
  auto scene = mesh->getScene();

  if (parsedGeometry.contains("geometryId")) {
    std::string geometryId = Json::GetString(parsedGeometry, "geometryId", "");
    auto geometry       = scene->getGeometryByID(geometryId);
    if (geometry) {
      geometry->applyToMesh(mesh.get());
    }
  }
  else if (parsedGeometry.contains("positions")
           && parsedGeometry.contains("normals")
           && parsedGeometry.contains("indices")) {
    auto parsedPositions = Json::ToArray<float>(parsedGeometry, "positions");
    mesh->setVerticesData(VertexBuffer::PositionKind, parsedPositions, false);

    mesh->setVerticesData(VertexBuffer::NormalKind,
                          Json::ToArray<float>(parsedGeometry, "normals"),
                          false);

    if (parsedGeometry.contains("tangents")) {
      mesh->setVerticesData(VertexBuffer::TangentKind,
                            Json::ToArray<float>(parsedGeometry, "tangents"),
                            false);
    }

    if (parsedGeometry.contains("uvs")) {
      mesh->setVerticesData(VertexBuffer::UVKind,
                            Json::ToArray<float>(parsedGeometry, "uvs"), false);
    }

    if (parsedGeometry.contains("uvs2")) {
      mesh->setVerticesData(VertexBuffer::UV2Kind,
                            Json::ToArray<float>(parsedGeometry, "uvs2"),
                            false);
    }

    if (parsedGeometry.contains("uvs3")) {
      mesh->setVerticesData(VertexBuffer::UV3Kind,
                            Json::ToArray<float>(parsedGeometry, "uvs3"),
                            false);
    }

    if (parsedGeometry.contains("uvs4")) {
      mesh->setVerticesData(VertexBuffer::UV4Kind,
                            Json::ToArray<float>(parsedGeometry, "uvs4"),
                            false);
    }

    if (parsedGeometry.contains("uvs5")) {
      mesh->setVerticesData(VertexBuffer::UV5Kind,
                            Json::ToArray<float>(parsedGeometry, "uvs5"),
                            false);
    }

    if (parsedGeometry.contains("uvs6")) {
      mesh->setVerticesData(VertexBuffer::UV6Kind,
                            Json::ToArray<float>(parsedGeometry, "uvs6"),
                            false);
    }

    if (parsedGeometry.contains("colors")) {
      const Float32Array parsedColors
        = Json::ToArray<float>(parsedGeometry, "colors");
      mesh->setVerticesData(
        VertexBuffer::ColorKind,
        Color4::CheckColors4(parsedColors, parsedPositions.size() / 3), false);
    }

    if (parsedGeometry.contains("matricesIndicesExtra")) {
      // TODO
    }

    if (parsedGeometry.contains("matricesWeights")) {
      auto matricesWeights
        = Json::ToArray<float>(parsedGeometry, "matricesWeights");
      Geometry::_CleanMatricesWeights(parsedGeometry, mesh);
      mesh->setVerticesData(VertexBuffer::MatricesWeightsKind, matricesWeights,
                            false);
    }

    if (parsedGeometry.contains("matricesWeightsExtra")) {
      auto matricesWeightsExtra
        = Json::ToArray<float>(parsedGeometry, "matricesWeightsExtra");
      mesh->setVerticesData(VertexBuffer::MatricesWeightsExtraKind,
                            matricesWeightsExtra, false);
    }

    if (parsedGeometry.contains("indices")) {
      mesh->setIndices(Json::ToArray<uint32_t>(parsedGeometry, "indices"), 0);
    }
  }

  // SubMeshes
  if (parsedGeometry.contains("subMeshes")
      && parsedGeometry.get("subMeshes").is<Json::array>()) {
    mesh->subMeshes.clear();
    for (auto& parsedSubMesh :
         parsedGeometry.get("subMeshes").get<Json::array>()) {
      SubMesh::AddToMesh(Json::GetNumber(parsedSubMesh, "materialIndex", 0u),
                         Json::GetNumber(parsedSubMesh, "verticesStart", 0u),
                         Json::GetNumber(parsedSubMesh, "verticesCount", 0ul),
                         Json::GetNumber(parsedSubMesh, "indexStart", 0u),
                         Json::GetNumber(parsedSubMesh, "indexCount", 0ul),
                         mesh);
    }
  }

  // Flat shading
  if (mesh->_shouldGenerateFlatShading) {
    mesh->convertToFlatShadedMesh();
  }

  // Update
  mesh->computeWorldMatrix(true);

  // Octree
  auto sceneOctree = scene->selectionOctree();
  if (sceneOctree) {
    // sceneOctree->addMesh(mesh);
  }
}

void Geometry::_CleanMatricesWeights(const Json::value& parsedGeometry,
                                     const MeshPtr& mesh)
{
  const auto epsilon = 1e-3f;
  if (!SceneLoader::CleanBoneMatrixWeights()) {
    return;
  }
  size_t noInfluenceBoneIndex = 0;
  auto parsedGeometrySkeletonId
    = Json::GetNumber<int>(parsedGeometry, "skeletonId", -1);
  if (parsedGeometrySkeletonId > -1) {
    auto skeleton = mesh->getScene()->getLastSkeletonByID(
      std::to_string(parsedGeometrySkeletonId));

    if (!skeleton) {
      return;
    }
    noInfluenceBoneIndex = skeleton->bones.size();
  }
  else {
    return;
  }
  auto matricesIndices
    = mesh->getVerticesData(VertexBuffer::MatricesIndicesKind);
  auto matricesIndicesExtra
    = mesh->getVerticesData(VertexBuffer::MatricesIndicesExtraKind);
  auto matricesWeights
    = Json::ToArray<float>(parsedGeometry, "matricesWeights");
  auto matricesWeightsExtra
    = Json::ToArray<float>(parsedGeometry, "matricesWeightsExtra");
  auto influencers = Json::GetNumber(parsedGeometry, "numBoneInfluencers", 0u);
  auto size        = matricesWeights.size();

  for (size_t i = 0; i < size; i += 4) {
    float weight        = 0.f;
    int firstZeroWeight = -1;
    for (unsigned int j = 0; j < 4; ++j) {
      auto w = matricesWeights[i + j];
      weight += w;
      if (w < epsilon && firstZeroWeight < 0) {
        firstZeroWeight = static_cast<int>(j);
      }
    }
    if (!matricesWeightsExtra.empty()) {
      for (unsigned int j = 0; j < 4; ++j) {
        auto w = matricesWeightsExtra[i + j];
        weight += w;
        if (w < epsilon && firstZeroWeight < 0) {
          firstZeroWeight = static_cast<int>(j + 4);
        }
      }
    }
    auto _firstZeroWeight
      = (firstZeroWeight < 0) ? 0 : static_cast<unsigned>(firstZeroWeight);
    if (firstZeroWeight < 0 || _firstZeroWeight > (influencers - 1)) {
      _firstZeroWeight = influencers - 1;
    }
    if (weight > epsilon) {
      auto mweight = 1.f / weight;
      for (unsigned int j = 0; j < 4; ++j) {
        matricesWeights[i + j] *= mweight;
      }
      if (!matricesWeightsExtra.empty()) {
        for (unsigned int j = 0; j < 4; ++j) {
          matricesWeightsExtra[i + j] *= mweight;
        }
      }
    }
    else {
      if (_firstZeroWeight >= 4) {
        matricesWeightsExtra[i + _firstZeroWeight - 4] = 1.f - weight;
        matricesIndicesExtra[i + _firstZeroWeight - 4] = noInfluenceBoneIndex;
      }
      else {
        matricesWeights[i + _firstZeroWeight] = 1.f - weight;
        matricesIndices[i + _firstZeroWeight] = noInfluenceBoneIndex;
      }
    }
  }

  mesh->setVerticesData(VertexBuffer::MatricesIndicesKind, matricesIndices);
  if (!matricesWeightsExtra.empty()) {
    mesh->setVerticesData(VertexBuffer::MatricesIndicesExtraKind,
                          matricesIndicesExtra);
  }
}

GeometryPtr Geometry::Parse(const Json::value& parsedVertexData, Scene* scene,
                            const std::string& rootUrl)
{
  const auto parsedVertexDataId = Json::GetString(parsedVertexData, "id");
  if (parsedVertexDataId.empty()
      || scene->getGeometryByID(parsedVertexDataId)) {
    return nullptr; // nullptr since geometry could be something else than a
                    // box...
  }

  auto geometry = Geometry::New(parsedVertexDataId, scene, nullptr,
                                Json::GetBool(parsedVertexData, "updatable"));

  // Tags.AddTagsTo(geometry, parsedVertexData.tags);

  if (parsedVertexData.contains("delayLoadingFile")) {
    geometry->delayLoadState = EngineConstants::DELAYLOADSTATE_NOTLOADED;
    geometry->delayLoadingFile
      = rootUrl + Json::GetString(parsedVertexData, "delayLoadingFile", "");
    geometry->_boundingInfo = std::make_unique<BoundingInfo>(
      Vector3::FromArray(
        Json::ToArray<float>(parsedVertexData, "boundingBoxMinimum")),
      Vector3::FromArray(
        Json::ToArray<float>(parsedVertexData, "boundingBoxMaximum")));

    geometry->_delayInfoKinds.clear();
    if (parsedVertexData.contains("hasUVs")) {
      geometry->_delayInfoKinds.emplace_back(VertexBuffer::UVKind);
    }

    if (parsedVertexData.contains("hasUVs2")) {
      geometry->_delayInfoKinds.emplace_back(VertexBuffer::UV2Kind);
    }

    if (parsedVertexData.contains("hasUVs3")) {
      geometry->_delayInfoKinds.emplace_back(VertexBuffer::UV3Kind);
    }

    if (parsedVertexData.contains("hasUVs4")) {
      geometry->_delayInfoKinds.emplace_back(VertexBuffer::UV4Kind);
    }

    if (parsedVertexData.contains("hasUVs5")) {
      geometry->_delayInfoKinds.emplace_back(VertexBuffer::UV5Kind);
    }

    if (parsedVertexData.contains("hasUVs6")) {
      geometry->_delayInfoKinds.emplace_back(VertexBuffer::UV6Kind);
    }

    if (parsedVertexData.contains("hasColors")) {
      geometry->_delayInfoKinds.emplace_back(VertexBuffer::ColorKind);
    }

    if (parsedVertexData.contains("hasMatricesIndices")) {
      geometry->_delayInfoKinds.emplace_back(VertexBuffer::MatricesIndicesKind);
    }

    if (parsedVertexData.contains("hasMatricesWeights")) {
      geometry->_delayInfoKinds.emplace_back(VertexBuffer::MatricesWeightsKind);
    }

    geometry->_delayLoadingFunction = VertexData::ImportVertexData;
  }
  else {
    VertexData::ImportVertexData(parsedVertexData, *geometry);
  }

  return geometry;
}

} // end of namespace BABYLON
