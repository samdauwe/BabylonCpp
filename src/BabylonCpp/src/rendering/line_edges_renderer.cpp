#include <babylon/rendering/line_edges_renderer.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/math/tmp_vectors.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

LineEdgesRenderer::LineEdgesRenderer(const AbstractMeshPtr& source,
                                     float epsilon,
                                     bool checkVerticesInsteadOfIndices)
    : EdgesRenderer{source, epsilon, checkVerticesInsteadOfIndices, false}
{
  _generateEdgesLines();
}

LineEdgesRenderer::~LineEdgesRenderer() = default;

void LineEdgesRenderer::_generateEdgesLines()
{
  auto positions = _source->getVerticesData(VertexBuffer::PositionKind);
  auto indices   = _source->getIndices();

  if (indices.empty() || positions.empty()) {
    return;
  }

  auto& p0 = TmpVectors::Vector3Array[0];
  auto& p1 = TmpVectors::Vector3Array[1];
  auto len = indices.size() - 1;
  for (uint32_t i = 0, offset = 0; i < len; i += 2, offset += 4) {
    Vector3::FromArrayToRef(positions, 3 * indices[i], p0);
    Vector3::FromArrayToRef(positions, 3 * indices[i + 1], p1);
    createLine(p0, p1, offset);
  }

  // Merge into a single mesh
  auto engine = _source->getScene()->getEngine();

  _buffers[VertexBuffer::PositionKind] = std::make_shared<VertexBuffer>(
    engine, _linesPositions, VertexBuffer::PositionKind, false);
  _buffers[VertexBuffer::NormalKind] = std::make_shared<VertexBuffer>(
    engine, _linesNormals, VertexBuffer::NormalKind, false, false, 4);

  _ib = engine->createIndexBuffer(_linesIndices);

  _indicesCount = _linesIndices.size();
}

} // end of namespace BABYLON
