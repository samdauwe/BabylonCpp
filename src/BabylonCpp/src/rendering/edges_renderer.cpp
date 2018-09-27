#include <babylon/rendering/edges_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/shader_material_options.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/rendering/face_adjacencies.h>

namespace BABYLON {

EdgesRenderer::EdgesRenderer(const AbstractMeshPtr& source, float epsilon,
                             bool checkVerticesInsteadOfIndices,
                             bool generateEdgesLines)
    : edgesWidthScalerForOrthographic{1000.f}
    , edgesWidthScalerForPerspective{50.f}
    , isEnabled{true}
    , _source{source}
    , _epsilon{epsilon}
    , _lineShader{nullptr}
    , _ib{nullptr}
    , _checkVerticesInsteadOfIndices{checkVerticesInsteadOfIndices}
{
  _prepareResources();
  if (generateEdgesLines) {
    _generateEdgesLines();
  }
}

EdgesRenderer::~EdgesRenderer()
{
}

void EdgesRenderer::_prepareResources()
{
  if (_lineShader) {
    return;
  }

  ShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes = {"position", "normal"};
  shaderMaterialOptions.uniforms
    = {"worldViewProjection", "color", "width", "aspectRatio"};

  _lineShader = ShaderMaterial::New("lineShader", _source->getScene(), "line",
                                    shaderMaterialOptions);

  _lineShader->disableDepthWrite = true;
  _lineShader->backFaceCulling   = false;
}

void EdgesRenderer::_rebuild()
{
  for (auto bufferKind :
       {VertexBuffer::PositionKind, VertexBuffer::NormalKind}) {
    if (stl_util::contains(_buffers, bufferKind)) {
      auto& buffer = _buffers[bufferKind];
      if (buffer) {
        buffer->_rebuild();
      }
    }
  }

  auto scene  = _source->getScene();
  auto engine = scene->getEngine();
  _ib         = engine->createIndexBuffer(_linesIndices);
}

void EdgesRenderer::dispose()
{
  if (_buffers.find(VertexBuffer::PositionKind) != _buffers.end()) {
    auto& buffer = _buffers[VertexBuffer::PositionKind];
    if (buffer) {
      buffer->dispose();
      _buffers.erase(VertexBuffer::PositionKind);
    }
  }

  if (_buffers.find(VertexBuffer::NormalKind) != _buffers.end()) {
    auto& buffer = _buffers[VertexBuffer::NormalKind];
    if (buffer) {
      buffer->dispose();
      _buffers.erase(VertexBuffer::NormalKind);
    }
  }

  _buffers.clear();
  _bufferPtrs.clear();

  _source->getScene()->getEngine()->_releaseBuffer(_ib.get());
  _lineShader->dispose();
}

int EdgesRenderer::_processEdgeForAdjacencies(unsigned int pa, unsigned int pb,
                                              unsigned int p0, unsigned int p1,
                                              unsigned int p2)
{
  if ((pa == p0 && pb == p1) || (pa == p1 && pb == p0)) {
    return 0;
  }

  if ((pa == p1 && pb == p2) || (pa == p2 && pb == p1)) {
    return 1;
  }

  if ((pa == p2 && pb == p0) || (pa == p0 && pb == p2)) {
    return 2;
  }

  return -1;
}

int EdgesRenderer::_processEdgeForAdjacenciesWithVertices(const Vector3& pa,
                                                          const Vector3& pb,
                                                          const Vector3& p0,
                                                          const Vector3& p1,
                                                          const Vector3& p2)
{
  if ((pa.equalsWithEpsilon(p0) && pb.equalsWithEpsilon(p1))
      || (pa.equalsWithEpsilon(p1) && pb.equalsWithEpsilon(p0))) {
    return 0;
  }

  if ((pa.equalsWithEpsilon(p1) && pb.equalsWithEpsilon(p2))
      || (pa.equalsWithEpsilon(p2) && pb.equalsWithEpsilon(p1))) {
    return 1;
  }

  if ((pa.equalsWithEpsilon(p2) && pb.equalsWithEpsilon(p0))
      || (pa.equalsWithEpsilon(p0) && pb.equalsWithEpsilon(p2))) {
    return 2;
  }

  return -1;
}

void EdgesRenderer::_checkEdge(size_t faceIndex, int edge,
                               const std::vector<Vector3>& faceNormals,
                               const Vector3& p0, const Vector3& p1)
{
  auto needToCreateLine = false;

  if (edge == -1) {
    needToCreateLine = true;
  }
  else {
    auto dotProduct  = Vector3::Dot(faceNormals[faceIndex],
                                   faceNormals[static_cast<size_t>(edge)]);
    needToCreateLine = dotProduct < _epsilon;
  }

  if (needToCreateLine) {
    auto offset = static_cast<int32_t>(_linesPositions.size() / 3);
    auto normal = p0.subtract(p1);
    normal.normalize();

    // Positions
    _linesPositions.emplace_back(p0.x);
    _linesPositions.emplace_back(p0.y);
    _linesPositions.emplace_back(p0.z);

    _linesPositions.emplace_back(p0.x);
    _linesPositions.emplace_back(p0.y);
    _linesPositions.emplace_back(p0.z);

    _linesPositions.emplace_back(p1.x);
    _linesPositions.emplace_back(p1.y);
    _linesPositions.emplace_back(p1.z);

    _linesPositions.emplace_back(p1.x);
    _linesPositions.emplace_back(p1.y);
    _linesPositions.emplace_back(p1.z);

    // Normals
    _linesNormals.emplace_back(p1.x);
    _linesNormals.emplace_back(p1.y);
    _linesNormals.emplace_back(p1.z);
    _linesNormals.emplace_back(-1.f);

    _linesNormals.emplace_back(p1.x);
    _linesNormals.emplace_back(p1.y);
    _linesNormals.emplace_back(p1.z);
    _linesNormals.emplace_back(1.f);

    _linesNormals.emplace_back(p0.x);
    _linesNormals.emplace_back(p0.y);
    _linesNormals.emplace_back(p0.z);
    _linesNormals.emplace_back(-1.f);

    _linesNormals.emplace_back(p0.x);
    _linesNormals.emplace_back(p0.y);
    _linesNormals.emplace_back(p0.z);
    _linesNormals.emplace_back(1.f);

    // Indices
    _linesIndices.emplace_back(offset);
    _linesIndices.emplace_back(offset + 1);
    _linesIndices.emplace_back(offset + 2);
    _linesIndices.emplace_back(offset);
    _linesIndices.emplace_back(offset + 2);
    _linesIndices.emplace_back(offset + 3);
  }
}

void EdgesRenderer::_generateEdgesLines()
{
  auto positions = _source->getVerticesData(VertexBuffer::PositionKind);
  auto indices   = _source->getIndices();

  if (indices.empty() || positions.empty()) {
    return;
  }

  // First let's find adjacencies
  std::vector<FaceAdjacencies> adjacencies;
  std::vector<Vector3> faceNormals;

  // Prepare faces
  for (unsigned int index = 0; index < indices.size(); index += 3) {
    FaceAdjacencies _faceAdjacencies;
    unsigned int p0Index = indices[index + 0];
    unsigned int p1Index = indices[index + 1];
    unsigned int p2Index = indices[index + 2];

    _faceAdjacencies.edges = {0, 0, 0};

    _faceAdjacencies.p0
      = Vector3(positions[p0Index * 3 + 0], positions[p0Index * 3 + 1],
                positions[p0Index * 3 + 2]);
    _faceAdjacencies.p1
      = Vector3(positions[p1Index * 3 + 0], positions[p1Index * 3 + 1],
                positions[p1Index * 3 + 2]);
    _faceAdjacencies.p2
      = Vector3(positions[p2Index * 3 + 0], positions[p2Index * 3 + 1],
                positions[p2Index * 3 + 2]);
    auto faceNormal
      = Vector3::Cross(_faceAdjacencies.p1.subtract(_faceAdjacencies.p0),
                       _faceAdjacencies.p2.subtract(_faceAdjacencies.p1));

    faceNormal.normalize();

    faceNormals.emplace_back(faceNormal);
    adjacencies.emplace_back(_faceAdjacencies);
  }

  // Scan
  for (unsigned int index = 0; index < adjacencies.size(); ++index) {
    auto& faceAdjacencies = adjacencies[index];

    for (unsigned int otherIndex = index + 1; otherIndex < adjacencies.size();
         ++otherIndex) {
      auto& otherFaceAdjacencies = adjacencies[otherIndex];

      if (faceAdjacencies.edgesConnectedCount == 3) { // Full
        break;
      }

      if (otherFaceAdjacencies.edgesConnectedCount == 3) { // Full
        continue;
      }

      const auto& otherP0 = indices[otherIndex * 3 + 0];
      const auto& otherP1 = indices[otherIndex * 3 + 1];
      const auto& otherP2 = indices[otherIndex * 3 + 2];

      for (unsigned int edgeIndex = 0; edgeIndex < 3; ++edgeIndex) {
        int otherEdgeIndex = 0;

        if (edgeIndex >= faceAdjacencies.edges.size()) {
          continue;
        }

        switch (edgeIndex) {
          case 0:
            if (_checkVerticesInsteadOfIndices) {
              otherEdgeIndex = _processEdgeForAdjacenciesWithVertices(
                faceAdjacencies.p0, faceAdjacencies.p1, otherFaceAdjacencies.p0,
                otherFaceAdjacencies.p1, otherFaceAdjacencies.p2);
            }
            else {
              otherEdgeIndex = _processEdgeForAdjacencies(
                indices[index * 3], indices[index * 3 + 1], otherP0, otherP1,
                otherP2);
            }
            break;
          case 1:
            if (_checkVerticesInsteadOfIndices) {
              otherEdgeIndex = _processEdgeForAdjacenciesWithVertices(
                faceAdjacencies.p1, faceAdjacencies.p2, otherFaceAdjacencies.p0,
                otherFaceAdjacencies.p1, otherFaceAdjacencies.p2);
            }
            else {
              otherEdgeIndex = _processEdgeForAdjacencies(
                indices[index * 3 + 1], indices[index * 3 + 2], otherP0,
                otherP1, otherP2);
            }
            break;
          case 2:
          default:
            if (_checkVerticesInsteadOfIndices) {
              otherEdgeIndex = _processEdgeForAdjacenciesWithVertices(
                faceAdjacencies.p2, faceAdjacencies.p0, otherFaceAdjacencies.p0,
                otherFaceAdjacencies.p1, otherFaceAdjacencies.p2);
            }
            else {
              otherEdgeIndex = _processEdgeForAdjacencies(
                indices[index * 3 + 2], indices[index * 3], otherP0, otherP1,
                otherP2);
            }
        }

        if (otherEdgeIndex == -1) {
          continue;
        }

        faceAdjacencies.edges[edgeIndex] = static_cast<int>(otherIndex);
        otherFaceAdjacencies.edges[static_cast<size_t>(otherEdgeIndex)]
          = static_cast<int>(index);

        ++faceAdjacencies.edgesConnectedCount;
        ++otherFaceAdjacencies.edgesConnectedCount;

        if (faceAdjacencies.edgesConnectedCount == 3) {
          break;
        }
      }
    }
  }

  // Create lines
  for (unsigned index = 0; index < adjacencies.size(); ++index) {
    // We need a line when a face has no adjacency on a specific edge or if all
    // the adjacencies has an angle greater than epsilon
    const auto& current = adjacencies[index];

    _checkEdge(index, current.edges[0], faceNormals, current.p0, current.p1);
    _checkEdge(index, current.edges[1], faceNormals, current.p1, current.p2);
    _checkEdge(index, current.edges[2], faceNormals, current.p2, current.p0);
  }

  // Merge into a single mesh
  auto engine = _source->getScene()->getEngine();

  _buffers[VertexBuffer::PositionKind] = std::make_unique<VertexBuffer>(
    engine, ToVariant<Float32Array, Buffer*>(_linesPositions),
    VertexBuffer::PositionKind, false);
  _buffers[VertexBuffer::NormalKind] = std::make_unique<VertexBuffer>(
    engine, ToVariant<Float32Array, Buffer*>(_linesNormals),
    VertexBuffer::NormalKind, false, false, 4);
  _bufferPtrs[VertexBuffer::PositionKindChars]
    = _buffers[VertexBuffer::PositionKind].get();
  _bufferPtrs[VertexBuffer::NormalKindChars]
    = _buffers[VertexBuffer::NormalKind].get();

  _ib = engine->createIndexBuffer(_linesIndices);

  _indicesCount = _linesIndices.size();
}

void EdgesRenderer::render()
{
  auto scene = _source->getScene();

  if (!_lineShader->isReady() || !scene->activeCamera) {
    return;
  }

  auto engine = scene->getEngine();
  _lineShader->_preBind();

  if (_source->edgesColor.a != 1.f) {
    engine->setAlphaMode(EngineConstants::ALPHA_COMBINE);
  }
  else {
    engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);
  }

  // VBOs
  engine->bindBuffers(_bufferPtrs, _ib.get(), _lineShader->getEffect());

  scene->resetCachedMaterial();
  _lineShader->setColor4("color", _source->edgesColor);

  if (scene->activeCamera->mode == Camera::ORTHOGRAPHIC_CAMERA()) {
    _lineShader->setFloat("width", _source->edgesWidth
                                     / edgesWidthScalerForOrthographic);
  }
  else {
    _lineShader->setFloat("width",
                          _source->edgesWidth / edgesWidthScalerForPerspective);
  }

  _lineShader->setFloat("aspectRatio",
                        engine->getAspectRatio(*scene->activeCamera));
  _lineShader->bind(_source->getWorldMatrix());

  // Draw order
  engine->drawElementsType(Material::TriangleFillMode(), 0,
                           static_cast<int>(_indicesCount));
  _lineShader->unbind();
}

} // end of namespace BABYLON
