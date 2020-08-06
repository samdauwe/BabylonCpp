#include <babylon/rendering/edges_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/ishader_material_options.h>
#include <babylon/materials/shader_material.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/_instance_data_storage.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/rendering/face_adjacencies.h>

namespace BABYLON {

EdgesRenderer::EdgesRenderer(const AbstractMeshPtr& source, float epsilon,
                             bool checkVerticesInsteadOfIndices, bool generateEdgesLines,
                             const std::optional<IEdgesRendererOptions>& options)
    : edgesWidthScalerForOrthographic{1000.f}
    , edgesWidthScalerForPerspective{50.f}
    , _lineShader{nullptr}
    , _ib{nullptr}
    , _options{std::nullopt}
{
  _source                        = source;
  _checkVerticesInsteadOfIndices = checkVerticesInsteadOfIndices;
  if (options) {
    _options = *options;
  }

  _epsilon = epsilon;

  isEnabled = true;
  _prepareResources();
  if (generateEdgesLines) {
    if (options && options->useAlternateEdgeFinder.value_or(true)) {
      _generateEdgesLinesAlternate();
    }
    else {
      _generateEdgesLines();
    }
  }

  _meshRebuildObserver = _source->onRebuildObservable.add(
    [this](AbstractMesh* /*mesh*/, EventState& /*es*/) { _rebuild(); });

  _meshDisposeObserver
    = _source->onDisposeObservable.add([this](Node* /*node*/, EventState& /*es*/) { dispose(); });
}

EdgesRenderer::~EdgesRenderer() = default;

ShaderMaterialPtr EdgesRenderer::GetShader(Scene* scene)
{
  if (!scene->_edgeRenderLineShader) {
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "normal"};
    shaderMaterialOptions.uniforms   = {"world", "viewProjection", "color", "width", "aspectRatio"};

    const auto shader = ShaderMaterial::New("lineShader", scene, "line", shaderMaterialOptions);

    shader->disableDepthWrite = true;
    shader->backFaceCulling   = false;

    scene->_edgeRenderLineShader = std::move(shader);
  }

  return scene->_edgeRenderLineShader;
}

void EdgesRenderer::_prepareResources()
{
  if (_lineShader) {
    return;
  }

  _lineShader = EdgesRenderer::GetShader(_source->getScene());
}

void EdgesRenderer::_rebuild()
{
  for (const auto bufferKind : {VertexBuffer::PositionKind, VertexBuffer::NormalKind}) {
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

void EdgesRenderer::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  _source->onRebuildObservable.remove(_meshRebuildObserver);
  _source->onDisposeObservable.remove(_meshDisposeObserver);

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

  if (_ib) {
    _source->getScene()->getEngine()->_releaseBuffer(_ib);
  }
  _lineShader->dispose();
}

int EdgesRenderer::_processEdgeForAdjacencies(unsigned int pa, unsigned int pb, unsigned int p0,
                                              unsigned int p1, unsigned int p2)
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

int EdgesRenderer::_processEdgeForAdjacenciesWithVertices(const Vector3& pa, const Vector3& pb,
                                                          const Vector3& p0, const Vector3& p1,
                                                          const Vector3& p2)
{
  const auto eps = 1e-10f;
  if ((pa.equalsWithEpsilon(p0, eps) && pb.equalsWithEpsilon(p1, eps))
      || (pa.equalsWithEpsilon(p1, eps) && pb.equalsWithEpsilon(p0, eps))) {
    return 0;
  }

  if ((pa.equalsWithEpsilon(p1, eps) && pb.equalsWithEpsilon(p2, eps))
      || (pa.equalsWithEpsilon(p2, eps) && pb.equalsWithEpsilon(p1, eps))) {
    return 1;
  }

  if ((pa.equalsWithEpsilon(p2, eps) && pb.equalsWithEpsilon(p0, eps))
      || (pa.equalsWithEpsilon(p0, eps) && pb.equalsWithEpsilon(p2, eps))) {
    return 2;
  }

  return -1;
}

void EdgesRenderer::_checkEdge(size_t faceIndex, int edge, const std::vector<Vector3>& faceNormals,
                               const Vector3& p0, const Vector3& p1)
{
  auto needToCreateLine = false;

  if (edge == -1) {
    needToCreateLine = true;
  }
  else {
    auto dotProduct = Vector3::Dot(faceNormals[faceIndex], faceNormals[static_cast<size_t>(edge)]);

    needToCreateLine = dotProduct < _epsilon;
  }

  if (needToCreateLine) {
    const auto offset = static_cast<uint32_t>(_linesPositions.size() / 3);
    createLine(p0, p1, offset);
  }
}

void EdgesRenderer::createLine(const Vector3& p0, const Vector3& p1, uint32_t offset)
{
  // Positions
  stl_util::concat(_linesPositions, {
                                      p0.x, p0.y, p0.z, //
                                      p0.x, p0.y, p0.z, //
                                      p1.x, p1.y, p1.z, //
                                      p1.x, p1.y, p1.z  //
                                    });

  // Normals
  stl_util::concat(_linesNormals, {
                                    p1.x, p1.y, p1.z, -1.f, //
                                    p1.x, p1.y, p1.z, 1.f,  //
                                    p0.x, p0.y, p0.z, -1.f, //
                                    p0.x, p0.y, p0.z, 1.f   //
                                  });

  // Indices
  stl_util::concat(_linesIndices, {
                                    offset, offset + 1, offset + 2, //
                                    offset, offset + 2, offset + 3  //
                                  });
}

void EdgesRenderer::_tessellateTriangle(
  const std::vector<std::vector<std::array<float, 2>>>& edgePoints, size_t indexTriangle,
  IndicesArray& indices, const IndicesArray& remapVertexIndices)
{
  const auto makePointList = [](const std::vector<std::array<float, 2>>& edgePoints,
                                IndicesArray& pointIndices, int firstIndex) -> void {
    if (firstIndex >= 0) {
      pointIndices.emplace_back(static_cast<uint32_t>(firstIndex));
    }

    for (const auto& edgePoint : edgePoints) {
      pointIndices.emplace_back(edgePoint[0]);
    }
  };

  auto startEdge = 0ull;

  if (edgePoints[1].size() >= edgePoints[0].size()
      && edgePoints[1].size() >= edgePoints[2].size()) {
    startEdge = 1;
  }
  else if (edgePoints[2].size() >= edgePoints[0].size()
           && edgePoints[2].size() >= edgePoints[1].size()) {
    startEdge = 2;
  }

  for (auto e = 0ull; e < 3; ++e) {
    if (e == startEdge) {
      stl_util::sort_js_style(
        edgePoints[e], [](const std::array<float, 2>& a, const std::array<float, 2>& b) -> int {
          return a[1] < b[1] ? -1 : a[1] > b[1] ? 1 : 0;
        });
    }
    else {
      stl_util::sort_js_style(
        edgePoints[e], [](const std::array<float, 2>& a, const std::array<float, 2>& b) -> int {
          return a[1] > b[1] ? -1 : a[1] < b[1] ? 1 : 0;
        });
    }
  }

  IndicesArray mainPointIndices, otherPointIndices;

  makePointList(edgePoints[startEdge], mainPointIndices, -1);

  auto numMainPoints = mainPointIndices.size();

  for (auto i = startEdge + 2; i >= startEdge + 1; --i) {
    makePointList(edgePoints[i % 3], otherPointIndices,
                  i != startEdge + 2 ? remapVertexIndices[indices[indexTriangle + ((i + 1) % 3)]] :
                                       -1);
  }

  const auto numOtherPoints = otherPointIndices.size();

  auto idxMain  = 0ull;
  auto idxOther = 0ull;

  stl_util::concat(indices, {remapVertexIndices[indices[indexTriangle + startEdge]],
                             mainPointIndices[0], otherPointIndices[0]});
  stl_util::concat(indices,
                   {remapVertexIndices[indices[indexTriangle + ((startEdge + 1) % 3)]],
                    otherPointIndices[numOtherPoints - 1], mainPointIndices[numMainPoints - 1]});

  const auto bucketIsMain = numMainPoints <= numOtherPoints;

  const auto bucketStep     = bucketIsMain ? numMainPoints : numOtherPoints;
  const auto bucketLimit    = bucketIsMain ? numOtherPoints : numMainPoints;
  const auto bucketIdxLimit = bucketIsMain ? numMainPoints - 1 : numOtherPoints - 1;
  const auto winding        = bucketIsMain ? 0 : 1;

  auto numTris = static_cast<int>(numMainPoints) + static_cast<int>(numOtherPoints) - 2;

  auto bucketIdx     = bucketIsMain ? idxMain : idxOther;
  auto nbucketIdx    = bucketIsMain ? idxOther : idxMain;
  auto bucketPoints  = bucketIsMain ? mainPointIndices : otherPointIndices;
  auto nbucketPoints = bucketIsMain ? otherPointIndices : mainPointIndices;

  auto bucket = 0ull;

  while (numTris-- > 0) {
    if (winding) {
      stl_util::concat(indices, {bucketPoints[bucketIdx], nbucketPoints[nbucketIdx]});
    }
    else {
      stl_util::concat(indices, {nbucketPoints[nbucketIdx], bucketPoints[bucketIdx]});
    }

    bucket += bucketStep;

    auto lastIdx = 0u;

    if (bucket >= bucketLimit && bucketIdx < bucketIdxLimit) {
      lastIdx = bucketPoints[++bucketIdx];
      bucket -= bucketLimit;
    }
    else {
      lastIdx = nbucketPoints[++nbucketIdx];
    }

    indices.emplace_back(lastIdx);
  }

  indices[indexTriangle + 0] = indices[indices.size() - 3];
  indices[indexTriangle + 1] = indices[indices.size() - 2];
  indices[indexTriangle + 2] = indices[indices.size() - 1];

  for (auto i = 0; i < 3; ++i) {
    indices.pop_back();
  }
}

void EdgesRenderer::_generateEdgesLinesAlternate()
{
  auto positions = _source->getVerticesData(VertexBuffer::PositionKind);
  auto indices   = _source->getIndices();

  if (indices.empty() || positions.empty()) {
    return;
  }

  /**
   * Find all vertices that are at the same location (with an epsilon) and remapp them on the same
   * vertex
   */
  const auto useFastVertexMerger = _options ? _options->useFastVertexMerger.value_or(true) : true;
  const auto epsilonVertexMerge  = _options ? _options->epsilonVertexMerge.value_or(1e-6) : 1e-6;
  const auto epsVertexMerge      = useFastVertexMerger ?
                                std::round(-std::log(epsilonVertexMerge) / std::log(10)) :
                                epsilonVertexMerge;
  IndicesArray remapVertexIndices;
  IndicesArray uniquePositions; // list of unique index of vertices - needed for tessellation

  const auto toFixed = [](float var, float epsilon) -> float {
    const auto epsilonI = static_cast<int>(std::min(std::max(0.f, epsilon), 20.f));
    float value         = static_cast<int>(var * epsilonI + .5);
    return value / epsilonI;
  };

  if (useFastVertexMerger) {
    std::unordered_map<std::string, uint32_t> mapVertices;
    for (auto v1 = 0ull; v1 < positions.size(); v1 += 3) {
      const auto x1 = positions[v1 + 0], y1 = positions[v1 + 1], z1 = positions[v1 + 2];

      const auto key
        = StringTools::printf("%f|%f|", toFixed(x1, epsVertexMerge), toFixed(y1, epsVertexMerge),
                              toFixed(z1, epsVertexMerge));

      if (stl_util::contains(mapVertices, key)) {
        remapVertexIndices.emplace_back(mapVertices[key]);
      }
      else {
        const auto idx   = v1 / 3u;
        mapVertices[key] = idx;
        remapVertexIndices.emplace_back(idx);
        uniquePositions.emplace_back(idx);
      }
    }
  }
  else {
    for (auto v1 = 0ull; v1 < positions.size(); v1 += 3) {
      const auto x1 = positions[v1 + 0], y1 = positions[v1 + 1], z1 = positions[v1 + 2];
      auto found = false;
      for (auto v2 = 0ull; v2 < v1 && !found; v2 += 3) {
        const auto x2 = positions[v2 + 0], y2 = positions[v2 + 1], z2 = positions[v2 + 2];

        if (std::abs(x1 - x2) < epsVertexMerge && std::abs(y1 - y2) < epsVertexMerge
            && std::abs(z1 - z2) < epsVertexMerge) {
          remapVertexIndices.emplace_back(v2 / 3u);
          found = true;
          break;
        }
      }

      if (!found) {
        remapVertexIndices.emplace_back(v1 / 3u);
        uniquePositions.emplace_back(v1 / 3u);
      }
    }
  }

  if (_options ? _options->applyTessellation.value_or(false) : false) {
    /**
     * Tessellate triangles if necessary:
     *
     *               A
     *               +
     *               |\
     *               | \
     *               |  \
     *             E +   \
     *              /|    \
     *             / |     \
     *            /  |      \
     *           +---+-------+ B
     *           D   C
     *
     * For the edges to be rendered correctly, the ABC triangle has to be split into ABE and BCE,
     * else AC is considered to be an edge, whereas only AE should be.
     *
     * The tessellation process looks for the vertices like E that are in-between two other vertices
     * making of an edge and create new triangles as necessary
     */

    // First step: collect the triangles to tessellate
    const auto epsVertexAligned = _options ? _options->epsilonVertexAligned.value_or(1e-6) : 1e-6;
    struct TesselateItem {
      size_t index = 0;
      std::vector<std::vector<std::array<float, 2>>> edgesPoints;
    };                                        // ens of struct TesselateItem
    std::vector<TesselateItem> mustTesselate; // liste of triangles that must be tessellated

    for (auto index = 0ull; index < indices.size(); index += 3) { // loop over all triangles
      std::optional<TesselateItem> triangleToTessellate = std::nullopt;

      for (auto i = 0u; i < 3; ++i) { // loop over the 3 edges of the triangle
        auto p0Index = remapVertexIndices[indices[index + i]];
        auto p1Index = remapVertexIndices[indices[index + (i + 1) % 3]];
        auto p2Index = remapVertexIndices[indices[index + (i + 2) % 3]];

        if (p0Index == p1Index) {
          continue;
        } // degenerated triangle - don't process

        const auto p0x = positions[p0Index * 3 + 0], p0y = positions[p0Index * 3 + 1],
                   p0z = positions[p0Index * 3 + 2];
        const auto p1x = positions[p1Index * 3 + 0], p1y = positions[p1Index * 3 + 1],
                   p1z = positions[p1Index * 3 + 2];

        const auto p0p1 = std::sqrt((p1x - p0x) * (p1x - p0x) + (p1y - p0y) * (p1y - p0y)
                                    + (p1z - p0z) * (p1z - p0z));

        for (auto v = 0ull; v < uniquePositions.size() - 1;
             ++v) { // loop over all (unique) vertices and look for the ones that would be
                    // in-between p0 and p1
          const auto vIndex = uniquePositions[v];

          if (vIndex == p0Index || vIndex == p1Index || vIndex == p2Index) {
            continue;
          } // don't handle the vertex if it is a vertex of the current triangle

          const auto x = positions[vIndex * 3 + 0], y = positions[vIndex * 3 + 1],
                     z = positions[vIndex * 3 + 2];

          const auto p0p
            = std::sqrt((x - p0x) * (x - p0x) + (y - p0y) * (y - p0y) + (z - p0z) * (z - p0z));
          const auto pp1
            = std::sqrt((x - p1x) * (x - p1x) + (y - p1y) * (y - p1y) + (z - p1z) * (z - p1z));

          if (std::abs(p0p + pp1 - p0p1)
              < epsVertexAligned) { // vertices are aligned and p in-between p0 and p1 if
                                    // distance(p0, p) + distance (p, p1) ~ distance(p0, p1)
            if (!triangleToTessellate) {
              triangleToTessellate = TesselateItem{
                index,       // index,
                {{}, {}, {}} // edgesPoints
              };
              mustTesselate.emplace_back(*triangleToTessellate);
            }
            triangleToTessellate->edgesPoints[i].emplace_back(
              std::array<float, 2>{static_cast<float>(vIndex), p0p});
          }
        }
      }
    }

    // Second step: tesselate the triangles
    for (const auto& triangle : mustTesselate) {
      _tessellateTriangle(triangle.edgesPoints, triangle.index, indices, remapVertexIndices);
    }

    mustTesselate.clear();
  }

  /**
   * Collect the edges to render
   */
  struct EdgeToRenderItem {
    Vector3 normal;
    bool done    = false;
    size_t index = 0;
    size_t i     = 0;
  }; // ens of struct EdgeToRender
  std::unordered_map<std::string, EdgeToRenderItem> edges;

  for (size_t index = 0; index < indices.size(); index += 3) {
    std::optional<Vector3> faceNormal = std::nullopt;
    for (auto i = 0u; i < 3; ++i) {
      auto p0Index = remapVertexIndices[indices[index + i]];
      auto p1Index = remapVertexIndices[indices[index + (i + 1) % 3]];
      auto p2Index = remapVertexIndices[indices[index + (i + 2) % 3]];

      if (p0Index == p1Index) {
        continue;
      }

      TmpVectors::Vector3Array[0].copyFromFloats(
        positions[p0Index * 3 + 0], positions[p0Index * 3 + 1], positions[p0Index * 3 + 2]);
      TmpVectors::Vector3Array[1].copyFromFloats(
        positions[p1Index * 3 + 0], positions[p1Index * 3 + 1], positions[p1Index * 3 + 2]);
      TmpVectors::Vector3Array[2].copyFromFloats(
        positions[p2Index * 3 + 0], positions[p2Index * 3 + 1], positions[p2Index * 3 + 2]);

      if (!faceNormal) {
        TmpVectors::Vector3Array[1].subtractToRef(TmpVectors::Vector3Array[0],
                                                  TmpVectors::Vector3Array[3]);
        TmpVectors::Vector3Array[2].subtractToRef(TmpVectors::Vector3Array[1],
                                                  TmpVectors::Vector3Array[4]);
        faceNormal = Vector3::Cross(TmpVectors::Vector3Array[3], TmpVectors::Vector3Array[4]);
        faceNormal->normalize();
      }

      if (p0Index > p1Index) {
        const auto tmp = p0Index;
        p0Index        = p1Index;
        p1Index        = tmp;
      }

      const auto key = StringTools::printf("%u_%u", p0Index, p1Index);

      if (stl_util::contains(edges, key)) {
        auto& ei = edges[key];
        if (!ei.done) {
          const auto dotProduct = Vector3::Dot(*faceNormal, ei.normal);

          if (dotProduct < _epsilon) {
            createLine(TmpVectors::Vector3Array[0], TmpVectors::Vector3Array[1],
                       _linesPositions.size() / 3);
          }

          ei.done = true;
        }
      }
      else {
        edges[key] = EdgeToRenderItem{
          *faceNormal, // normal
          false,       // done
          index,       // index
          i            // i
        };
      }
    }
  }

  for (const auto& item : edges) {
    const auto& ei = edges[item.first];
    if (!ei.done) {
      // Orphaned edge - we must display it
      const auto p0Index = remapVertexIndices[indices[ei.index + ei.i]];
      const auto p1Index = remapVertexIndices[indices[ei.index + (ei.i + 1) % 3]];

      TmpVectors::Vector3Array[0].copyFromFloats(
        positions[p0Index * 3 + 0], positions[p0Index * 3 + 1], positions[p0Index * 3 + 2]);
      TmpVectors::Vector3Array[1].copyFromFloats(
        positions[p1Index * 3 + 0], positions[p1Index * 3 + 1], positions[p1Index * 3 + 2]);

      createLine(TmpVectors::Vector3Array[0], TmpVectors::Vector3Array[1],
                 _linesPositions.size() / 3);
    }
  }

  /**
   * Merge into a single mesh
   */
  auto engine = _source->getScene()->getEngine();

  _buffers[VertexBuffer::PositionKind]
    = std::make_shared<VertexBuffer>(engine, _linesPositions, VertexBuffer::PositionKind, false);
  _buffers[VertexBuffer::NormalKind] = std::make_shared<VertexBuffer>(
    engine, _linesNormals, VertexBuffer::NormalKind, false, false, 4);

  _buffersForInstances[VertexBuffer::PositionKind] = _buffers[VertexBuffer::PositionKind];
  _buffersForInstances[VertexBuffer::NormalKind]   = _buffers[VertexBuffer::NormalKind];

  _ib = engine->createIndexBuffer(_linesIndices);

  _indicesCount = _linesIndices.size();
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
      = Vector3(positions[p0Index * 3 + 0], positions[p0Index * 3 + 1], positions[p0Index * 3 + 2]);
    _faceAdjacencies.p1
      = Vector3(positions[p1Index * 3 + 0], positions[p1Index * 3 + 1], positions[p1Index * 3 + 2]);
    _faceAdjacencies.p2
      = Vector3(positions[p2Index * 3 + 0], positions[p2Index * 3 + 1], positions[p2Index * 3 + 2]);
    auto faceNormal = Vector3::Cross(_faceAdjacencies.p1.subtract(_faceAdjacencies.p0),
                                     _faceAdjacencies.p2.subtract(_faceAdjacencies.p1));

    faceNormal.normalize();

    faceNormals.emplace_back(faceNormal);
    adjacencies.emplace_back(_faceAdjacencies);
  }

  // Scan
  for (unsigned int index = 0; index < adjacencies.size(); ++index) {
    auto& faceAdjacencies = adjacencies[index];

    for (unsigned int otherIndex = index + 1; otherIndex < adjacencies.size(); ++otherIndex) {
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
                indices[index * 3], indices[index * 3 + 1], otherP0, otherP1, otherP2);
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
                indices[index * 3 + 1], indices[index * 3 + 2], otherP0, otherP1, otherP2);
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
                indices[index * 3 + 2], indices[index * 3], otherP0, otherP1, otherP2);
            }
        }

        if (otherEdgeIndex == -1) {
          continue;
        }

        faceAdjacencies.edges[edgeIndex] = static_cast<int>(otherIndex);
        otherFaceAdjacencies.edges[static_cast<size_t>(otherEdgeIndex)] = static_cast<int>(index);

        ++faceAdjacencies.edgesConnectedCount;
        ++otherFaceAdjacencies.edgesConnectedCount;

        if (faceAdjacencies.edgesConnectedCount == 3) {
          break;
        }
      }
    }
  }

  // Create lines
  for (auto index = 0ull; index < adjacencies.size(); ++index) {
    // We need a line when a face has no adjacency on a specific edge or if all
    // the adjacencies has an angle greater than epsilon
    const auto& current = adjacencies[index];

    _checkEdge(index, current.edges[0], faceNormals, current.p0, current.p1);
    _checkEdge(index, current.edges[1], faceNormals, current.p1, current.p2);
    _checkEdge(index, current.edges[2], faceNormals, current.p2, current.p0);
  }

  // Merge into a single mesh
  auto engine = _source->getScene()->getEngine();

  _buffers[VertexBuffer::PositionKind]
    = std::make_shared<VertexBuffer>(engine, _linesPositions, VertexBuffer::PositionKind, false);
  _buffers[VertexBuffer::NormalKind] = std::make_shared<VertexBuffer>(
    engine, _linesNormals, VertexBuffer::NormalKind, false, false, 4);

  _buffersForInstances[VertexBuffer::PositionKind] = _buffers[VertexBuffer::PositionKind];
  _buffersForInstances[VertexBuffer::NormalKind]   = _buffers[VertexBuffer::NormalKind];

  _ib = engine->createIndexBuffer(_linesIndices);

  _indicesCount = _linesIndices.size();
}

bool EdgesRenderer::isReady()
{
  return _lineShader->isReady(_source.get(), (_source->hasInstances() && customInstances.size() > 0)
                                               || _source->hasThinInstances());
}

void EdgesRenderer::render()
{
  auto scene = _source->getScene();

  if (!isReady() || !scene->activeCamera()) {
    return;
  }

  auto engine = scene->getEngine();
  _lineShader->_preBind();

  if (_source->edgesColor.a != 1.f) {
    engine->setAlphaMode(Constants::ALPHA_COMBINE);
  }
  else {
    engine->setAlphaMode(Constants::ALPHA_DISABLE);
  }

  const auto hasInstances            = _source->hasInstances() && customInstances.size() > 0;
  const auto useBuffersWithInstances = hasInstances || _source->hasThinInstances();

  auto instanceCount = 0ull;

  if (useBuffersWithInstances) {
    auto _sourceMesh               = std::static_pointer_cast<Mesh>(_source);
    _buffersForInstances["world0"] = _sourceMesh->getVertexBuffer("world0");
    _buffersForInstances["world1"] = _sourceMesh->getVertexBuffer("world1");
    _buffersForInstances["world2"] = _sourceMesh->getVertexBuffer("world2");
    _buffersForInstances["world3"] = _sourceMesh->getVertexBuffer("world3");

    if (hasInstances) {
      auto& instanceStorage = _sourceMesh->_instanceDataStorage;

      instanceCount = customInstances.size();

      if (!instanceStorage->isFrozen) {
        auto offset = 0ull;

        for (auto i = 0ull; i < instanceCount; ++i) {
          customInstances.data()[i].copyToArray(instanceStorage->instancesData, offset);
          offset += 16;
        }

        if (instanceStorage->instancesBuffer) {
          instanceStorage->instancesBuffer->updateDirectly(instanceStorage->instancesData, 0,
                                                           instanceCount);
        }
      }
    }
    else {
      instanceCount = _sourceMesh->thinInstanceCount();
    }
  }

  // VBOs
  engine->bindBuffers(useBuffersWithInstances ? _buffersForInstances : _buffers, _ib,
                      _lineShader->getEffect());

  scene->resetCachedMaterial();
  _lineShader->setColor4("color", _source->edgesColor);

  if (scene->activeCamera()->mode == Camera::ORTHOGRAPHIC_CAMERA) {
    _lineShader->setFloat("width", _source->edgesWidth / edgesWidthScalerForOrthographic);
  }
  else {
    _lineShader->setFloat("width", _source->edgesWidth / edgesWidthScalerForPerspective);
  }

  _lineShader->setFloat("aspectRatio", engine->getAspectRatio(*scene->activeCamera()));
  _lineShader->bind(_source->getWorldMatrix());

  // Draw order
  engine->drawElementsType(Material::TriangleFillMode, 0, static_cast<int>(_indicesCount),
                           static_cast<int>(instanceCount));
  _lineShader->unbind();

  customInstances.clear();
}

} // end of namespace BABYLON
