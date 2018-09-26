#include <babylon/rendering/line_edges_renderer.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/rendering/face_adjacencies.h>

namespace BABYLON {

LineEdgesRenderer::LineEdgesRenderer(const AbstractMeshPtr& source,
                                     float epsilon,
                                     bool checkVerticesInsteadOfIndices)
    : EdgesRenderer{source, epsilon, checkVerticesInsteadOfIndices, false}
{
  _generateEdgesLines();
}

LineEdgesRenderer::~LineEdgesRenderer()
{
}
void LineEdgesRenderer::_checkEdge(size_t /*faceIndex*/, int /*edge*/,
                                   const std::vector<Vector3>& /*faceNormals*/,
                                   const Vector3& p0, const Vector3& p1)
{
  auto offset = _linesPositions.size() / 3;
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
  _linesNormals.emplace_back(1);

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

void LineEdgesRenderer::_generateEdgesLines()
{
  auto positions = _source->getVerticesData(VertexBuffer::PositionKind);
  auto indices   = _source->getIndices();

  if (indices.empty() || positions.empty()) {
    return;
  }

  // First let's find adjacencies
  std::vector<FaceAdjacencies> adjacencies;
  std::vector<Vector3> faceNormals;
  for (size_t i = 0; i < (positions.size() / 3) - 1; ++i) {
    FaceAdjacencies currentAdjecancy;
    currentAdjecancy.p0
      = Vector3(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
    currentAdjecancy.p1
      = Vector3(positions[(i + 1) * 3], positions[(i + 1) * 3 + 1],
                positions[(i + 1) * 3 + 2]);
    adjacencies.emplace_back(currentAdjecancy);
  }
  // Create lines
  for (size_t index = 0; index < adjacencies.size(); index++) {
    // We need a line when a face has no adjacency on a specific edge or if all
    // the adjacencies has an angle greater than epsilon
    const auto& current = adjacencies[index];
    _checkEdge(index, current.edges[0], faceNormals, current.p0, current.p1);
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

} // end of namespace BABYLON
