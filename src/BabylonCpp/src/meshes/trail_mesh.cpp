#include <babylon/meshes/trail_mesh.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

TrailMesh::TrailMesh(const std::string& iName, const AbstractMeshPtr& generator,
                     Scene* scene, float diameter, float length, bool autoStart)
    : Mesh{iName, scene}
    , _sectionPolygonPointsCount{4}
    , _beforeRenderObserver{nullptr}
{
  _running   = false;
  _autoStart = autoStart;
  _generator = generator;
  _diameter  = diameter;
  _length    = length;
  _sectionVectors.resize(_sectionPolygonPointsCount);
  _sectionNormalVectors.resize(_sectionPolygonPointsCount);
  for (uint32_t i = 0; i < _sectionPolygonPointsCount; ++i) {
    _sectionVectors[i]       = Vector3::Zero();
    _sectionNormalVectors[i] = Vector3::Zero();
  }
  _createMesh();
}

TrailMesh::~TrailMesh() = default;

const std::string TrailMesh::getClassName() const
{
  return "TrailMesh";
}

void TrailMesh::_createMesh()
{
  auto data = std::make_unique<VertexData>();
  Float32Array positions;
  Float32Array normals;
  IndicesArray indices;
  auto meshCenter = Vector3::Zero();
  if (_generator->_boundingInfo) {
    meshCenter = _generator->_boundingInfo->boundingBox.centerWorld;
  }
  auto alpha = 2.f * Math::PI / _sectionPolygonPointsCount;
  for (uint32_t i = 0; i < _sectionPolygonPointsCount; ++i) {
    stl_util::concat(positions, {meshCenter.x + std::cos(i * alpha) * _diameter,
                                 meshCenter.y + std::sin(i * alpha) * _diameter,
                                 meshCenter.z});
  }
  for (float i = 1.f; i <= _length; ++i) {
    for (uint32_t j = 0; j < _sectionPolygonPointsCount; ++j) {
      stl_util::concat(positions,
                       {meshCenter.x + std::cos(j * alpha) * _diameter,
                        meshCenter.y + std::sin(j * alpha) * _diameter,
                        meshCenter.z});
    }
    auto l = static_cast<uint32_t>(positions.size() / 3
                                   - 2 * _sectionPolygonPointsCount);
    for (uint32_t j = 0; j < _sectionPolygonPointsCount - 1; ++j) {
      stl_util::concat(indices, {
                                  l + j,
                                  l + j + _sectionPolygonPointsCount,
                                  l + j + _sectionPolygonPointsCount + 1,
                                });
      stl_util::concat(
        indices, {l + j, l + j + _sectionPolygonPointsCount + 1, l + j + 1});
    }
    stl_util::concat(indices, {
                                l + _sectionPolygonPointsCount - 1,
                                l + _sectionPolygonPointsCount - 1
                                  + _sectionPolygonPointsCount,
                                l + _sectionPolygonPointsCount,
                              });
    stl_util::concat(indices, {l + _sectionPolygonPointsCount - 1,
                               l + _sectionPolygonPointsCount, l});
  }
  VertexData::ComputeNormals(positions, indices, normals);
  data->positions = std::move(positions);
  data->normals   = std::move(normals);
  data->indices   = std::move(indices);
  data->applyToMesh(*this, true);
  if (_autoStart) {
    start();
  }
}

void TrailMesh::start()
{
  if (!_running) {
    _running              = true;
    _beforeRenderObserver = getScene()->onBeforeRenderObservable.add(
      [this](Scene* /*scene*/, EventState & /*s*/) -> void { update(); });
  }
}

void TrailMesh::stop()
{
  if (_beforeRenderObserver && _running) {
    _running = false;
    getScene()->onBeforeRenderObservable.remove(_beforeRenderObserver);
  }
}

void TrailMesh::update()
{
  auto positions = getVerticesData(VertexBuffer::PositionKind);
  auto normals   = getVerticesData(VertexBuffer::NormalKind);
  auto& wm       = _generator->getWorldMatrix();
  if (!positions.empty() && !normals.empty()) {
    for (uint32_t i = 3 * _sectionPolygonPointsCount; i < positions.size();
         ++i) {
      positions[i - 3 * _sectionPolygonPointsCount]
        = positions[i] - normals[i] / _length * _diameter;
    }
    for (uint32_t i = 3 * _sectionPolygonPointsCount; i < normals.size(); ++i) {
      normals[i - 3 * _sectionPolygonPointsCount] = normals[i];
    }
    auto l     = positions.size() - 3 * _sectionPolygonPointsCount;
    auto alpha = 2 * Math::PI / _sectionPolygonPointsCount;
    for (uint32_t i = 0; i < _sectionPolygonPointsCount; ++i) {
      _sectionVectors[i].copyFromFloats(std::cos(i * alpha) * _diameter,
                                        std::sin(i * alpha) * _diameter, 0);
      _sectionNormalVectors[i].copyFromFloats(std::cos(i * alpha),
                                              std::sin(i * alpha), 0);
      Vector3::TransformCoordinatesToRef(_sectionVectors[i], wm,
                                         _sectionVectors[i]);
      Vector3::TransformNormalToRef(_sectionNormalVectors[i], wm,
                                    _sectionNormalVectors[i]);
    }
    for (uint32_t i = 0; i < _sectionPolygonPointsCount; ++i) {
      positions[l + 3 * i]     = _sectionVectors[i].x;
      positions[l + 3 * i + 1] = _sectionVectors[i].y;
      positions[l + 3 * i + 2] = _sectionVectors[i].z;
      normals[l + 3 * i]       = _sectionNormalVectors[i].x;
      normals[l + 3 * i + 1]   = _sectionNormalVectors[i].y;
      normals[l + 3 * i + 2]   = _sectionNormalVectors[i].z;
    }
    updateVerticesData(VertexBuffer::PositionKind, positions, true, false);
    updateVerticesData(VertexBuffer::NormalKind, normals, true, false);
  }
}

TrailMeshPtr TrailMesh::clone(const std::string& iName,
                              const AbstractMeshPtr& newGenerator)
{
  return TrailMesh::New(iName,
                        (newGenerator == nullptr ? _generator : newGenerator),
                        getScene(), _diameter, _length, _autoStart);
}

void TrailMesh::serialize(json& serializationObject) const
{
  Mesh::serialize(serializationObject);
}

TrailMeshPtr TrailMesh::Parse(const json& /*parsedMesh*/, Scene* /*scene*/)
{
  return nullptr;
}

} // end of namespace BABYLON
