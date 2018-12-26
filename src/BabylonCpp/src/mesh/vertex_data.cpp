#include <babylon/mesh/vertex_data.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engine/engine.h>
#include <babylon/math/axis.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/facet_parameters.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

VertexData::VertexData()
{
}

VertexData::~VertexData()
{
}

void VertexData::set(const Float32Array& data, unsigned int kind)
{
  switch (kind) {
    case VertexBuffer::PositionKind:
      positions = data;
      break;
    case VertexBuffer::NormalKind:
      normals = data;
      break;
    case VertexBuffer::TangentKind:
      tangents = data;
      break;
    case VertexBuffer::UVKind:
      uvs = data;
      break;
    case VertexBuffer::UV2Kind:
      uvs2 = data;
      break;
    case VertexBuffer::UV3Kind:
      uvs3 = data;
      break;
    case VertexBuffer::UV4Kind:
      uvs4 = data;
      break;
    case VertexBuffer::UV5Kind:
      uvs5 = data;
      break;
    case VertexBuffer::UV6Kind:
      uvs6 = data;
      break;
    case VertexBuffer::ColorKind:
      colors = data;
      break;
    case VertexBuffer::MatricesIndicesKind:
      matricesIndices = data;
      break;
    case VertexBuffer::MatricesWeightsKind:
      matricesWeights = data;
      break;
    case VertexBuffer::MatricesIndicesExtraKind:
      matricesIndicesExtra = data;
      break;
    case VertexBuffer::MatricesWeightsExtraKind:
      matricesWeightsExtra = data;
      break;
    default:
      break;
  }
}

VertexData& VertexData::applyToMesh(Mesh& mesh, bool updatable)
{
  _applyTo(mesh, updatable);
  return *this;
}

VertexData& VertexData::applyToGeometry(Geometry& geometry, bool updatable)
{
  _applyTo(geometry, updatable);
  return *this;
}

VertexData& VertexData::updateMesh(Mesh* mesh, bool updateExtends,
                                   bool makeItUnique)
{
  _update(mesh, updateExtends, makeItUnique);
  return *this;
}

VertexData& VertexData::updateGeometry(Geometry* geometry, bool updateExtends,
                                       bool makeItUnique)
{
  _update(geometry, updateExtends, makeItUnique);
  return *this;
}

VertexData& VertexData::_applyTo(IGetSetVerticesData& meshOrGeometry,
                                 bool updatable)
{
  if (!positions.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::PositionKind, positions,
                                   updatable);
  }

  if (!normals.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::NormalKind, normals,
                                   updatable);
  }

  if (!tangents.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::TangentKind, tangents,
                                   updatable);
  }

  if (!uvs.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::UVKind, uvs, updatable);
  }

  if (!uvs2.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::UV2Kind, uvs2, updatable);
  }

  if (!uvs3.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::UV3Kind, uvs3, updatable);
  }

  if (!uvs4.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::UV4Kind, uvs4, updatable);
  }

  if (!uvs5.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::UV5Kind, uvs5, updatable);
  }

  if (!uvs6.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::UV6Kind, uvs6, updatable);
  }

  if (!colors.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::ColorKind, colors, updatable);
  }

  if (!matricesIndices.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::MatricesIndicesKind,
                                   matricesIndices, updatable);
  }

  if (!matricesWeights.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::MatricesWeightsKind,
                                   matricesWeights, updatable);
  }

  if (!matricesIndicesExtra.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::MatricesIndicesExtraKind,
                                   matricesIndicesExtra, updatable);
  }

  if (!matricesWeightsExtra.empty()) {
    meshOrGeometry.setVerticesData(VertexBuffer::MatricesWeightsExtraKind,
                                   matricesWeightsExtra, updatable);
  }

  if (!indices.empty()) {
    meshOrGeometry.setIndices(indices, 0, updatable);
  }
  else {
    meshOrGeometry.setIndices({}, 0);
  }

  return *this;
}

VertexData& VertexData::_update(IGetSetVerticesData* meshOrGeometry,
                                bool updateExtends, bool makeItUnique)
{
  if (!positions.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::PositionKind, positions,
                                       updateExtends, makeItUnique);
  }

  if (!normals.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::NormalKind, normals,
                                       updateExtends, makeItUnique);
  }

  if (!tangents.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::TangentKind, tangents,
                                       updateExtends, makeItUnique);
  }

  if (!uvs.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::UVKind, uvs, updateExtends,
                                       makeItUnique);
  }

  if (!uvs2.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::UV2Kind, uvs2,
                                       updateExtends, makeItUnique);
  }

  if (!uvs3.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::UV3Kind, uvs3,
                                       updateExtends, makeItUnique);
  }

  if (!uvs4.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::UV4Kind, uvs4,
                                       updateExtends, makeItUnique);
  }

  if (!uvs5.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::UV5Kind, uvs5,
                                       updateExtends, makeItUnique);
  }

  if (!uvs6.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::UV6Kind, uvs6,
                                       updateExtends, makeItUnique);
  }

  if (!colors.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::ColorKind, colors,
                                       updateExtends, makeItUnique);
  }

  if (!matricesIndices.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::MatricesIndicesKind,
                                       matricesIndices, updateExtends,
                                       makeItUnique);
  }

  if (!matricesWeights.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::MatricesWeightsKind,
                                       matricesWeights, updateExtends,
                                       makeItUnique);
  }

  if (!matricesIndicesExtra.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::MatricesIndicesExtraKind,
                                       matricesIndicesExtra, updateExtends,
                                       makeItUnique);
  }

  if (!matricesWeightsExtra.empty()) {
    meshOrGeometry->updateVerticesData(VertexBuffer::MatricesWeightsExtraKind,
                                       matricesWeightsExtra, updateExtends,
                                       makeItUnique);
  }

  if (!indices.empty()) {
    meshOrGeometry->setIndices(indices, 0);
  }

  return *this;
}

VertexData& VertexData::transform(const Matrix& matrix)
{
  bool flip        = matrix.m[0] * matrix.m[5] * matrix.m[10] < 0.f;
  auto transformed = Vector3::Zero();
  if (!positions.empty()) {
    auto position = Vector3::Zero();

    for (unsigned int index = 0; index < positions.size(); index += 3) {
      Vector3::FromArrayToRef(positions, index, position);

      Vector3::TransformCoordinatesToRef(position, matrix, transformed);
      positions[index + 0] = transformed.x;
      positions[index + 1] = transformed.y;
      positions[index + 2] = transformed.z;
    }
  }

  if (!normals.empty()) {
    auto normal = Vector3::Zero();

    for (unsigned int index = 0; index < normals.size(); index += 3) {
      Vector3::FromArrayToRef(normals, index, normal);

      Vector3::TransformNormalToRef(normal, matrix, transformed);
      normals[index + 0] = transformed.x;
      normals[index + 1] = transformed.y;
      normals[index + 2] = transformed.z;
    }
  }

  if (!tangents.empty()) {
    auto tangent            = Vector4::Zero();
    auto tangentTransformed = Vector4::Zero();

    for (unsigned int index = 0; index < tangents.size(); index += 4) {
      Vector4::FromArrayToRef(tangents, index, tangent);

      Vector4::TransformNormalToRef(tangent, matrix, tangentTransformed);
      tangents[index]     = tangentTransformed.x;
      tangents[index + 1] = tangentTransformed.y;
      tangents[index + 2] = tangentTransformed.z;
      tangents[index + 3] = tangentTransformed.w;
    }
  }

  if (flip && !indices.empty()) {
    for (size_t index = 0; index < indices.size(); index += 3) {
      std::swap(indices[index + 1], indices[index + 2]);
    }
  }

  return *this;
}

VertexData& VertexData::merge(VertexData& other, bool /*use32BitsIndices*/)
{
  _validate();
  other._validate();

  if (!other.indices.empty()) {
    unsigned int offset
      = (!positions.empty()) ? static_cast<unsigned>(positions.size()) / 3 : 0;
    for (auto& index : other.indices) {
      indices.emplace_back(static_cast<int32_t>(index + offset));
    }
  }

  positions = _mergeElement(positions, other.positions);

  normals         = _mergeElement(normals, other.normals);
  tangents        = _mergeElement(tangents, other.tangents);
  uvs             = _mergeElement(uvs, other.uvs);
  uvs2            = _mergeElement(uvs2, other.uvs2);
  uvs3            = _mergeElement(uvs3, other.uvs3);
  uvs4            = _mergeElement(uvs4, other.uvs4);
  uvs5            = _mergeElement(uvs5, other.uvs5);
  uvs6            = _mergeElement(uvs6, other.uvs6);
  colors          = _mergeElement(colors, other.colors);
  matricesIndices = _mergeElement(matricesIndices, other.matricesIndices);
  matricesWeights = _mergeElement(matricesWeights, other.matricesWeights);
  matricesIndicesExtra
    = _mergeElement(matricesIndicesExtra, other.matricesIndicesExtra);
  matricesWeightsExtra
    = _mergeElement(matricesWeightsExtra, other.matricesWeightsExtra);

  return *this;
}

Float32Array VertexData::_mergeElement(const Float32Array& source,
                                       const Float32Array& other) const
{
  if (source.empty()) {
    return other;
  }

  if (other.empty()) {
    return source;
  }

  // use non-loop method when the source is Float32Array
  Float32Array ret32 = source;
  stl_util::concat(ret32, other);
  return ret32;
}

void VertexData::_validate()
{
  if (positions.empty()) {
    throw new std::runtime_error("Positions are required");
  }

  const auto getElementCount
    = [](unsigned int kind, const Float32Array& values) -> size_t {
    const auto stride = VertexBuffer::DeduceStride(kind);
    if ((values.size() % stride) != 0) {
      throw new std::runtime_error("The " + VertexBuffer::KindAsString(kind)
                                   + "s array count must be a multiple of "
                                   + std::to_string(stride));
    }

    return values.size() / stride;
  };

  const auto positionsElementCount
    = getElementCount(VertexBuffer::PositionKind, positions);

  const auto validateElementCount
    = [&](unsigned int kind, const Float32Array& values) {
        const auto elementCount = getElementCount(kind, values);
        if (elementCount != positionsElementCount) {
          throw new std::runtime_error(
            "The " + VertexBuffer::KindAsString(kind) + "s element count ("
            + std::to_string(elementCount)
            + ") does not match the positions count ("
            + std::to_string(positionsElementCount) + ")");
        }
      };

  if (!normals.empty()) {
    validateElementCount(VertexBuffer::NormalKind, normals);
  }
  if (!tangents.empty()) {
    validateElementCount(VertexBuffer::TangentKind, tangents);
  }
  if (!uvs.empty()) {
    validateElementCount(VertexBuffer::UVKind, uvs);
  }
  if (!uvs2.empty()) {
    validateElementCount(VertexBuffer::UV2Kind, uvs2);
  }
  if (!uvs3.empty()) {
    validateElementCount(VertexBuffer::UV3Kind, uvs3);
  }
  if (!uvs4.empty()) {
    validateElementCount(VertexBuffer::UV4Kind, uvs4);
  }
  if (!uvs5.empty()) {
    validateElementCount(VertexBuffer::UV5Kind, uvs5);
  }
  if (!uvs6.empty()) {
    validateElementCount(VertexBuffer::UV6Kind, uvs6);
  }
  if (!colors.empty()) {
    validateElementCount(VertexBuffer::ColorKind, colors);
  }
  if (!matricesIndices.empty()) {
    validateElementCount(VertexBuffer::MatricesIndicesKind, matricesIndices);
  }
  if (!matricesWeights.empty()) {
    validateElementCount(VertexBuffer::MatricesWeightsKind, matricesWeights);
  }
  if (!matricesIndicesExtra.empty()) {
    validateElementCount(VertexBuffer::MatricesIndicesExtraKind,
                         matricesIndicesExtra);
  }
  if (!matricesWeightsExtra.empty()) {
    validateElementCount(VertexBuffer::MatricesWeightsExtraKind,
                         matricesWeightsExtra);
  }
}

json VertexData::serialize() const
{
  return nullptr;
}

std::unique_ptr<VertexData>
VertexData::ExtractFromMesh(Mesh* mesh, bool copyWhenShared, bool forceCopy)
{
  return VertexData::_ExtractFrom(mesh, copyWhenShared, forceCopy);
}

std::unique_ptr<VertexData> VertexData::ExtractFromGeometry(Geometry* geometry,
                                                            bool copyWhenShared,
                                                            bool forceCopy)
{
  return VertexData::_ExtractFrom(geometry, copyWhenShared, forceCopy);
}

std::unique_ptr<VertexData>
VertexData::_ExtractFrom(IGetSetVerticesData* meshOrGeometry,
                         bool copyWhenShared, bool forceCopy)
{
  auto result = std::make_unique<VertexData>();

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::PositionKind)) {
    result->positions = meshOrGeometry->getVerticesData(
      VertexBuffer::PositionKind, copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::NormalKind)) {
    result->normals = meshOrGeometry->getVerticesData(
      VertexBuffer::NormalKind, copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::TangentKind)) {
    result->tangents = meshOrGeometry->getVerticesData(
      VertexBuffer::TangentKind, copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::UVKind)) {
    result->uvs = meshOrGeometry->getVerticesData(VertexBuffer::UVKind,
                                                  copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
    result->uvs2 = meshOrGeometry->getVerticesData(VertexBuffer::UV2Kind,
                                                   copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::UV3Kind)) {
    result->uvs3 = meshOrGeometry->getVerticesData(VertexBuffer::UV3Kind,
                                                   copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::UV4Kind)) {
    result->uvs4 = meshOrGeometry->getVerticesData(VertexBuffer::UV4Kind,
                                                   copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::UV5Kind)) {
    result->uvs5 = meshOrGeometry->getVerticesData(VertexBuffer::UV5Kind,
                                                   copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::UV6Kind)) {
    result->uvs6 = meshOrGeometry->getVerticesData(VertexBuffer::UV6Kind,
                                                   copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(VertexBuffer::ColorKind)) {
    result->colors = meshOrGeometry->getVerticesData(VertexBuffer::ColorKind,
                                                     copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(
        VertexBuffer::MatricesIndicesKind)) {
    result->matricesIndices = meshOrGeometry->getVerticesData(
      VertexBuffer::MatricesIndicesKind, copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(
        VertexBuffer::MatricesWeightsKind)) {
    result->matricesWeights = meshOrGeometry->getVerticesData(
      VertexBuffer::MatricesWeightsKind, copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(
        VertexBuffer::MatricesIndicesExtraKind)) {
    result->matricesIndicesExtra = meshOrGeometry->getVerticesData(
      VertexBuffer::MatricesIndicesExtraKind, copyWhenShared, forceCopy);
  }

  if (meshOrGeometry->isVerticesDataPresent(
        VertexBuffer::MatricesWeightsExtraKind)) {
    result->matricesWeightsExtra = meshOrGeometry->getVerticesData(
      VertexBuffer::MatricesWeightsExtraKind, copyWhenShared, forceCopy);
  }

  result->indices = meshOrGeometry->getIndices(copyWhenShared, forceCopy);

  return result;
}

std::unique_ptr<VertexData> VertexData::CreateRibbon(RibbonOptions& options)
{
  auto& pathArray             = options.pathArray();
  const auto offset           = options.offset();
  const auto& closeArray      = options.closeArray;
  const auto& closePath       = options.closePath;
  const auto& invertUV        = options.invertUV;
  const auto& sideOrientation = options.sideOrientation;
  const auto& customUV        = options.uvs;
  const auto& customColors    = options.colors;

  Float32Array positions;
  Uint32Array indices;
  Float32Array normals;
  Float32Array uvs;

  // us[path_id] = [uDist1, uDist2, uDist3 ... ] distances between points on
  // path path_id
  std::vector<Float32Array> us;
  // vs[i] = [vDist1, vDist2, vDist3, ... ] distances between points i of
  // consecutives paths from pathArray
  std::vector<Float32Array> vs;
  // uTotalDistance[p] : total distance of path p
  Float32Array uTotalDistance;
  //  vTotalDistance[i] : total distance between points i of first and last path
  //  from pathArray
  Float32Array vTotalDistance;
  // minimal length among all paths from pathArray
  size_t minlg;
  // array of path lengths : nb of vertex per path
  Uint32Array lg;
  lg.resize(pathArray.size());
  // array of path indexes : index of each path (first vertex) in the total
  // vertex number
  Uint32Array idx;
  idx.resize(pathArray.size());
  size_t ul;
  size_t p; // path iterator
  size_t i; // point iterator
  size_t j; // point iterator

  // if single path in pathArray
  if (pathArray.size() < 2) {
    std::vector<Vector3> ar1;
    std::vector<Vector3> ar2;
    for (i = 0; i < pathArray[0].size() - offset; ++i) {
      ar1.emplace_back(pathArray[0][i]);
      ar2.emplace_back(pathArray[0][i + offset]);
    }
    pathArray = {ar1, ar2};
  }

  // positions and horizontal distances (u)
  size_t idc = 0;
  // the final index will be +1 if closePath
  size_t closePathCorr = (closePath) ? 1 : 0;
  std::vector<Vector3> path;
  size_t l;
  minlg = pathArray[0].size();
  float vectlg;
  float dist;
  for (p = 0; p < pathArray.size(); ++p) {
    uTotalDistance.emplace_back(0.f);
    us.emplace_back(0);
    path  = pathArray[p];
    l     = path.size();
    minlg = (minlg < l) ? minlg : l;

    j = 0;
    while (j < l) {
      stl_util::concat(positions, {path[j].x, path[j].y, path[j].z});
      if (j > 0) {
        vectlg = path[j].subtract(path[j - 1]).length();
        dist   = vectlg + uTotalDistance[p];
        us[p].emplace_back(dist);
        uTotalDistance[p] = dist;
      }
      ++j;
    }

    if (closePath) { // an extra hidden vertex is added in the "positions" array
      --j;
      stl_util::concat(positions, {path[0].x, path[0].y, path[0].z});
      vectlg = path[j].subtract(path[0]).length();
      dist   = vectlg + uTotalDistance[p];
      us[p].emplace_back(dist);
      uTotalDistance[p] = dist;
    }

    lg[p]  = static_cast<unsigned int>(l + closePathCorr);
    idx[p] = static_cast<uint32_t>(idc);
    idc += (l + closePathCorr);
  }

  // vertical distances (v)
  std::vector<Vector3> path1;
  std::vector<Vector3> path2;
  std::optional<Vector3> vertex1 = std::nullopt;
  std::optional<Vector3> vertex2 = std::nullopt;
  for (i = 0, ul = minlg + closePathCorr; i < ul; ++i) {
    vTotalDistance.emplace_back(0.f);
    vs.emplace_back(0);
    for (p = 0; p < pathArray.size() - 1; ++p) {
      path1 = pathArray[p];
      path2 = pathArray[p + 1];
      if (i == minlg) { // closePath
        vertex1 = path1[0];
        vertex2 = path2[0];
      }
      else {
        vertex1 = path1[i];
        vertex2 = path2[i];
      }
      vectlg = (*vertex2).subtract(*vertex1).length();
      dist   = vectlg + vTotalDistance[i];
      vs[i].emplace_back(dist);
      vTotalDistance[i] = dist;
    }
    if (closeArray && vertex2 && vertex1) {
      path1 = pathArray[p];
      path2 = pathArray[0];
      if (i == minlg) { // closePath
        vertex2 = path2[0];
      }
      vectlg            = (*vertex2).subtract(*vertex1).length();
      dist              = vectlg + vTotalDistance[i];
      vTotalDistance[i] = dist;
    }
  }

  // uvs
  float u;
  float v;
  if (!customUV.empty()) {
    for (const auto& customUVi : customUV) {
      stl_util::concat(uvs, {customUVi.x, customUVi.y});
    }
  }
  else {
    for (p = 0; p < pathArray.size(); ++p) {
      for (i = 0; i < minlg + closePathCorr; ++i) {
        u = (uTotalDistance[p] != 0.f) ? us[p][i] / uTotalDistance[p] : 0.f;
        v = (vTotalDistance[i] != 0.f) ? vs[i][p] / vTotalDistance[i] : 0.f;
        if (invertUV) {
          stl_util::concat(uvs, {v, u});
        }
        else {
          stl_util::concat(uvs, {u, v});
        }
      }
    }
  }

  // indices
  p                 = 0;
  unsigned int pi   = 0;
  unsigned int l1   = lg[p] - 1;           // path1 length
  unsigned int l2   = lg[p + 1] - 1;       // path2 length
  unsigned int min  = (l1 < l2) ? l1 : l2; // current path stop index
  unsigned int shft = idx[1] - idx[0];     // shift
  size_t path1nb
    = closeArray ? lg.size() : lg.size() - 1; // number of path1 to iterate on

  while (pi <= min && p < path1nb) {
    //  stay under min and don't go over next to last path
    // draw two triangles between path1 (p1) and path2 (p2) : (p1.pi, p2.pi,
    // p1.pi+1) and (p2.pi+1, p1.pi+1, p2.pi) clockwise
    stl_util::concat(indices, {pi, pi + shft, pi + 1});
    stl_util::concat(indices, {pi + shft + 1, pi + 1, pi + shft});
    pi += 1;
    if (pi == min) {
      // if end of one of two consecutive paths reached, go to next existing
      // path
      ++p;
      if (p == lg.size() - 1) {
        // last path of pathArray reached <=> closeArray == true
        shft = idx[0] - idx[p];
        l1   = lg[p] - 1;
        l2   = lg[0] - 1;
      }
      else {
        shft = idx[p + 1] - idx[p];
        l1   = lg[p] - 1;
        l2   = lg[p + 1] - 1;
      }
      pi  = idx[p];
      min = (l1 < l2) ? l1 + pi : l2 + pi;
    }
  }

  // normals
  VertexData::ComputeNormals(positions, indices, normals);

  // update both the first and last vertex normals to their average value
  if (closePath) {
    size_t indexFirst = 0;
    size_t indexLast  = 0;
    for (p = 0; p < pathArray.size(); ++p) {
      indexFirst = static_cast<size_t>(idx[p] * 3);
      if (p + 1 < pathArray.size()) {
        indexLast = static_cast<size_t>((idx[p + 1] - 1) * 3);
      }
      else {
        indexLast = normals.size() - 3;
      }
      normals[indexFirst] = (normals[indexFirst] + normals[indexLast]) * 0.5f;
      normals[indexFirst + 1]
        = (normals[indexFirst + 1] + normals[indexLast + 1]) * 0.5f;
      normals[indexFirst + 2]
        = (normals[indexFirst + 2] + normals[indexLast + 2]) * 0.5f;
      normals[indexLast]     = normals[indexFirst];
      normals[indexLast + 1] = normals[indexFirst + 1];
      normals[indexLast + 2] = normals[indexFirst + 2];
    }
  }

  // sides
  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            options.frontUVs, options.backUVs);

  // Colors
  Float32Array colors;
  if (!customColors.empty()) {
    colors.resize(customColors.size() * 4);
    for (unsigned int c = 0; c < customColors.size(); ++c) {
      colors[c * 4]     = customColors[c].r;
      colors[c * 4 + 1] = customColors[c].g;
      colors[c * 4 + 2] = customColors[c].b;
      colors[c * 4 + 3] = customColors[c].a;
    }
  }

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  if (!colors.empty()) {
    vertexData->set(colors, VertexBuffer::ColorKind);
  }

  if (closePath) {
    vertexData->_idx = std::move(idx);
  }

  return vertexData;
}

std::unique_ptr<VertexData> VertexData::CreateBox(BoxOptions& options)
{
  std::vector<Vector3> normalsSource = {
    Vector3(0.f, 0.f, 1.f),  //
    Vector3(0.f, 0.f, -1.f), //
    Vector3(1.f, 0.f, 0.f),  //
    Vector3(-1.f, 0.f, 0.f), //
    Vector3(0.f, 1.f, 0.f),  //
    Vector3(0.f, -1.f, 0.f)  //
  };

  Uint32Array indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;

  const auto& width           = options.width;
  const auto& height          = options.height;
  const auto& depth           = options.depth;
  const auto& sideOrientation = options.sideOrientation;
  const auto& faceUV          = options.faceUV;
  const auto& faceColors      = options.faceColors;

  Float32Array colors;

  Vector3 scaleVector(width / 2.f, height / 2.f, depth / 2.f);

  // Create each face in turn.
  for (unsigned int index = 0; index < normalsSource.size(); ++index) {
    auto& normal = normalsSource[index];

    // Get two vectors perpendicular to the face normal and to each other.
    Vector3 side1(normal.y, normal.z, normal.x);
    auto side2 = Vector3::Cross(normal, side1);

    // Six indices (two triangles) per face.
    int32_t verticesLength = static_cast<int32_t>(positions.size() / 3);
    indices.emplace_back(verticesLength);
    indices.emplace_back(verticesLength + 1);
    indices.emplace_back(verticesLength + 2);

    indices.emplace_back(verticesLength);
    indices.emplace_back(verticesLength + 2);
    indices.emplace_back(verticesLength + 3);

    // Four vertices per face.
    auto vertex = normal.subtract(side1).subtract(side2).multiply(scaleVector);
    stl_util::concat(positions, {vertex.x, vertex.y, vertex.z});
    stl_util::concat(normals, {normal.x, normal.y, normal.z});
    stl_util::concat(uvs, {faceUV[index].z, faceUV[index].w});
    if (!faceColors.empty()) {
      stl_util::concat(colors, {faceColors[index].r, faceColors[index].g,
                                faceColors[index].b, faceColors[index].a});
    }

    vertex = normal.subtract(side1).add(side2).multiply(scaleVector);
    stl_util::concat(positions, {vertex.x, vertex.y, vertex.z});
    stl_util::concat(normals, {normal.x, normal.y, normal.z});
    stl_util::concat(uvs, {faceUV[index].x, faceUV[index].w});
    if (!faceColors.empty()) {
      stl_util::concat(colors, {faceColors[index].r, faceColors[index].g,
                                faceColors[index].b, faceColors[index].a});
    }

    vertex = normal.add(side1).add(side2).multiply(scaleVector);
    stl_util::concat(positions, {vertex.x, vertex.y, vertex.z});
    stl_util::concat(normals, {normal.x, normal.y, normal.z});
    stl_util::concat(uvs, {faceUV[index].x, faceUV[index].y});
    if (!faceColors.empty()) {
      stl_util::concat(colors, {faceColors[index].r, faceColors[index].g,
                                faceColors[index].b, faceColors[index].a});
    }

    vertex = normal.add(side1).subtract(side2).multiply(scaleVector);
    stl_util::concat(positions, {vertex.x, vertex.y, vertex.z});
    stl_util::concat(normals, {normal.x, normal.y, normal.z});
    stl_util::concat(uvs, {faceUV[index].z, faceUV[index].y});
    if (!faceColors.empty()) {
      stl_util::concat(colors, {faceColors[index].r, faceColors[index].g,
                                faceColors[index].b, faceColors[index].a});
    }
  }

  // sides
  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            options.frontUVs, options.backUVs);

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  if (!faceColors.empty()) {
    if (sideOrientation == Mesh::DOUBLESIDE) {
      colors.insert(colors.end(), colors.begin(), colors.end());
    }
    vertexData->colors = std::move(colors);
  }

  return vertexData;
}

std::unique_ptr<VertexData> VertexData::CreateSphere(SphereOptions& options)
{
  const auto& segments        = options.segments;
  const auto& diameterX       = options.diameterX;
  const auto& diameterY       = options.diameterY;
  const auto& diameterZ       = options.diameterZ;
  const auto arc              = options.arc();
  const auto slice            = options.slice();
  const auto& sideOrientation = options.sideOrientation;

  Vector3 radius(diameterX / 2.f, diameterY / 2.f, diameterZ / 2.f);

  const unsigned int totalZRotationSteps = 2 + segments;
  const unsigned int totalYRotationSteps = 2 * totalZRotationSteps;

  IndicesArray indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;

  for (unsigned int zRotationStep = 0; zRotationStep <= totalZRotationSteps;
       ++zRotationStep) {
    float normalizedZ = static_cast<float>(zRotationStep)
                        / static_cast<float>(totalZRotationSteps);
    float angleZ = normalizedZ * Math::PI * slice;

    for (unsigned int yRotationStep = 0; yRotationStep <= totalYRotationSteps;
         ++yRotationStep) {
      float normalizedY = static_cast<float>(yRotationStep)
                          / static_cast<float>(totalYRotationSteps);

      float angleY = normalizedY * Math::PI * 2.f * arc;

      auto rotationZ = Matrix::RotationZ(-angleZ);
      auto rotationY = Matrix::RotationY(angleY);
      auto afterRotZ = Vector3::TransformCoordinates(Vector3::Up(), rotationZ);
      auto complete  = Vector3::TransformCoordinates(afterRotZ, rotationY);

      auto vertex = complete.multiply(radius);
      auto normal = complete.divide(radius).normalize();

      stl_util::concat(positions, {vertex.x, vertex.y, vertex.z});
      stl_util::concat(normals, {normal.x, normal.y, normal.z});
      stl_util::concat(uvs, {normalizedY, normalizedZ});
    }

    if (zRotationStep > 0) {
      auto verticesCount        = static_cast<uint32_t>(positions.size() / 3);
      auto _totalYRotationSteps = static_cast<uint32_t>(totalYRotationSteps);
      for (uint32_t firstIndex = verticesCount - 2 * (_totalYRotationSteps + 1);
           (firstIndex + _totalYRotationSteps + 2) < verticesCount;
           ++firstIndex) {
        indices.emplace_back((firstIndex));
        indices.emplace_back((firstIndex + 1));
        indices.emplace_back(firstIndex + _totalYRotationSteps + 1);

        indices.emplace_back((firstIndex + _totalYRotationSteps + 1));
        indices.emplace_back((firstIndex + 1));
        indices.emplace_back((firstIndex + _totalYRotationSteps + 2));
      }
    }
  }

  // Sides
  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            options.frontUVs, options.backUVs);

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  return vertexData;
}

// Cylinder and cone
std::unique_ptr<VertexData> VertexData::CreateCylinder(CylinderOptions& options)
{
  const auto& height          = options.height;
  const auto& diameterTop     = options.diameterTop;
  const auto& diameterBottom  = options.diameterBottom;
  const auto& tessellation    = options.tessellation;
  const auto& subdivisions    = options.subdivisions;
  const auto& hasRings        = options.hasRings;
  const auto& enclose         = options.enclose;
  const auto arc              = options.arc();
  const auto& sideOrientation = options.sideOrientation;
  auto& faceUV                = options.faceUV;
  auto& faceColors            = options.faceColors;

  // default face colors and UV if undefined
  unsigned int quadNb    = (arc != 1.f && enclose) ? 2 : 0;
  unsigned int ringNb    = (hasRings) ? subdivisions : 1;
  unsigned int surfaceNb = 2 + (1 + quadNb) * ringNb;
  unsigned int f;
  for (f = 0; f < surfaceNb; ++f) {
    if (!faceColors.empty() && f >= faceColors.size()) {
      faceColors.emplace_back(Color4(1.f, 1.f, 1.f, 1.f));
    }
  }
  for (f = 0; f < surfaceNb; ++f) {
    if (f >= faceUV.size()) {
      faceUV.emplace_back(Vector4(0.f, 0.f, 1.f, 1.f));
    }
  }

  Uint32Array indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;
  Float32Array colors;

  float angle_step = Math::PI * 2.f * arc / static_cast<float>(tessellation);
  float angle;
  float h;
  float radius;
  float _tan           = (diameterBottom - diameterTop) / 2.f / height;
  auto ringVertex      = Vector3::Zero();
  auto ringNormal      = Vector3::Zero();
  auto ringFirstVertex = Vector3::Zero();
  auto ringFirstNormal = Vector3::Zero();
  auto quadNormal      = Vector3::Zero();
  auto Y               = Axis::Y();

  // positions, normals, uvs
  unsigned int i;
  unsigned int j;
  unsigned int r;
  unsigned int ringIdx = 1;
  unsigned int s       = 1; // surface index
  unsigned int cs      = 0;
  float v              = 0.f;

  for (i = 0; i <= subdivisions; ++i) {
    h       = static_cast<float>(i) / static_cast<float>(subdivisions);
    radius  = (h * (diameterTop - diameterBottom) + diameterBottom) / 2.f;
    ringIdx = (hasRings && i != 0 && i != subdivisions) ? 2 : 1;
    for (r = 0; r < ringIdx; ++r) {
      if (hasRings) {
        s += r;
      }
      if (enclose) {
        s += 2 * r;
      }
      for (j = 0; j <= tessellation; ++j) {
        angle = static_cast<float>(j) * angle_step;

        // position
        ringVertex.x = std::cos(-angle) * radius;
        ringVertex.y = -height / 2.f + h * height;
        ringVertex.z = std::sin(-angle) * radius;

        if (stl_util::almost_equal(diameterTop, 0.f) && i == subdivisions) {
          // if no top cap, reuse former normals
          ringNormal.x = normals[normals.size() - (tessellation + 1) * 3 + 0];
          ringNormal.y = normals[normals.size() - (tessellation + 1) * 3 + 1];
          ringNormal.z = normals[normals.size() - (tessellation + 1) * 3 + 2];
        }
        else {
          ringNormal.x = ringVertex.x;
          ringNormal.z = ringVertex.z;
          ringNormal.y = std::sqrt(ringNormal.x * ringNormal.x
                                   + ringNormal.z * ringNormal.z)
                         * _tan;
          ringNormal.normalize();
        }

        // keep first ring vertex values for enclose
        if (j == 0) {
          ringFirstVertex.copyFrom(ringVertex);
          ringFirstNormal.copyFrom(ringNormal);
        }

        stl_util::concat(positions, {ringVertex.x, ringVertex.y, ringVertex.z});
        stl_util::concat(normals, {ringNormal.x, ringNormal.y, ringNormal.z});
        if (hasRings) {
          v = (cs != s) ? faceUV[s].y : faceUV[s].w;
        }
        else {
          v = faceUV[s].y + (faceUV[s].w - faceUV[s].y) * h;
        }
        stl_util::concat(
          uvs, {faceUV[s].x
                  + (faceUV[s].z - faceUV[s].x) * static_cast<float>(j)
                      / static_cast<float>(tessellation),
                v});
        if (!faceColors.empty()) {
          stl_util::concat(colors, {faceColors[s].r, faceColors[s].g,
                                    faceColors[s].b, faceColors[s].a});
        }
      }

      // if enclose, add four vertices and their dedicated normals
      if (!stl_util::almost_equal(arc, 1.f) && enclose) {
        stl_util::concat(positions, {ringVertex.x, ringVertex.y, ringVertex.z});
        stl_util::concat(positions, {0.f, ringVertex.y, 0.f});
        stl_util::concat(positions, {0.f, ringVertex.y, 0.f});
        stl_util::concat(
          positions, {ringFirstVertex.x, ringFirstVertex.y, ringFirstVertex.z});
        Vector3::CrossToRef(Y, ringNormal, quadNormal);
        quadNormal.normalize();
        stl_util::concat(normals, {quadNormal.x, quadNormal.y, quadNormal.z,
                                   quadNormal.x, quadNormal.y, quadNormal.z});
        Vector3::CrossToRef(ringFirstNormal, Y, quadNormal);
        quadNormal.normalize();
        stl_util::concat(normals, {quadNormal.x, quadNormal.y, quadNormal.z,
                                   quadNormal.x, quadNormal.y, quadNormal.z});
        if (hasRings) {
          v = (cs != s) ? faceUV[s + 1].y : faceUV[s + 1].w;
        }
        else {
          v = faceUV[s + 1].y + (faceUV[s + 1].w - faceUV[s + 1].y) * h;
        }
        stl_util::concat(uvs, {faceUV[s + 1].x, v});
        stl_util::concat(uvs, {faceUV[s + 1].z, v});
        if (hasRings) {
          v = (cs != s) ? faceUV[s + 2].y : faceUV[s + 2].w;
        }
        else {
          v = faceUV[s + 2].y + (faceUV[s + 2].w - faceUV[s + 2].y) * h;
        }
        stl_util::concat(uvs, {faceUV[s + 2].x, v});
        stl_util::concat(uvs, {faceUV[s + 2].z, v});
        if (!faceColors.empty()) {
          stl_util::concat(colors, {faceColors[s + 1].r, faceColors[s + 1].g,
                                    faceColors[s + 1].b, faceColors[s + 1].a});
          stl_util::concat(colors, {faceColors[s + 1].r, faceColors[s + 1].g,
                                    faceColors[s + 1].b, faceColors[s + 1].a});
          stl_util::concat(colors, {faceColors[s + 2].r, faceColors[s + 2].g,
                                    faceColors[s + 2].b, faceColors[s + 2].a});
          stl_util::concat(colors, {faceColors[s + 2].r, faceColors[s + 2].g,
                                    faceColors[s + 2].b, faceColors[s + 2].a});
        }
      }
      if (cs != s) {
        cs = s;
      }
    }
  }

  // indices
  // correction of number of iteration if enclose
  unsigned int e = (!stl_util::almost_equal(arc, 1.f) && enclose) ?
                     tessellation + 4 :
                     tessellation;
  i = 0;
  for (s = 0; s < subdivisions; ++s) {
    unsigned int i0 = 0, i1 = 0, i2 = 0, i3 = 0;
    for (j = 0; j < tessellation; ++j) {
      i0 = i * (e + 1) + j;
      i1 = (i + 1) * (e + 1) + j;
      i2 = i * (e + 1) + (j + 1);
      i3 = (i + 1) * (e + 1) + (j + 1);
      stl_util::concat(indices, {i0, i1, i2});
      stl_util::concat(indices, {i3, i2, i1});
    }
    if (!stl_util::almost_equal(arc, 1.f) && enclose) {
      // if enclose, add two quads
      stl_util::concat(indices, {i0 + 2, i1 + 2, i2 + 2});
      stl_util::concat(indices, {i3 + 2, i2 + 2, i1 + 2});
      stl_util::concat(indices, {i0 + 4, i1 + 4, i2 + 4});
      stl_util::concat(indices, {i3 + 4, i2 + 4, i1 + 4});
    }
    i = (hasRings) ? (i + 2) : (i + 1);
  }

  // Caps
  auto createCylinderCap = [&](bool isTop) -> void {
    float _radius = isTop ? diameterTop / 2.f : diameterBottom / 2.f;
    if (stl_util::almost_equal(_radius, 0.f)) {
      return;
    }

    // Cap positions, normals & uvs
    float _angle;
    Vector3 circleVector;
    Vector2 textureCoordinate;
    Vector4 u               = (isTop) ? faceUV[surfaceNb - 1] : faceUV[0];
    std::optional<Color4> c = std::nullopt;
    if (!faceColors.empty()) {
      c = (isTop) ? faceColors[surfaceNb - 1] : faceColors[0];
    }
    // cap center
    uint32_t vbase = static_cast<uint32_t>(positions.size() / 3);
    float offset   = isTop ? height / 2.f : -height / 2.f;
    Vector3 center(0.f, offset, 0.f);
    stl_util::concat(positions, {center.x, center.y, center.z});
    stl_util::concat(normals, {0, isTop ? 1.f : -1.f, 0.f});
    stl_util::concat(uvs, {u.x + (u.z - u.x) * 0.5f, u.y + (u.w - u.y) * 0.5f});
    if (c) {
      auto& _c = *c;
      stl_util::concat(colors, {_c.r, _c.g, _c.b, _c.a});
    }

    Vector2 textureScale(0.5f, 0.5f);
    for (i = 0; i <= tessellation; ++i) {
      _angle = Math::PI * 2.f * static_cast<float>(i) * arc
               / static_cast<float>(tessellation);
      float _cos   = std::cos(-_angle);
      float _sin   = std::sin(-_angle);
      circleVector = Vector3(_cos * _radius, offset, _sin * _radius);
      textureCoordinate
        = Vector2(_cos * textureScale.x + 0.5f, _sin * textureScale.y + 0.5f);
      stl_util::concat(positions,
                       {circleVector.x, circleVector.y, circleVector.z});
      stl_util::concat(normals, {0.f, isTop ? 1.f : -1.f, 0.f});
      stl_util::concat(uvs, {u.x + (u.z - u.x) * textureCoordinate.x,
                             u.y + (u.w - u.y) * textureCoordinate.y});
      if (c) {
        auto& _c = *c;
        stl_util::concat(colors, {_c.r, _c.g, _c.b, _c.a});
      }
    }
    // Cap indices
    for (i = 0; i < tessellation; ++i) {
      if (!isTop) {
        indices.emplace_back(vbase);
        indices.emplace_back(vbase + (i + 1));
        indices.emplace_back(vbase + (i + 2));
      }
      else {
        indices.emplace_back(vbase);
        indices.emplace_back(vbase + (i + 2));
        indices.emplace_back(vbase + (i + 1));
      }
    }
  };

  // add caps to geometry
  createCylinderCap(false);
  createCylinderCap(true);

  // Sides
  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            options.frontUVs, options.backUVs);

  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);
  if (!faceColors.empty()) {
    vertexData->colors = std::move(colors);
  }

  return vertexData;
}

std::unique_ptr<VertexData> VertexData::CreateTorus(TorusOptions& options)
{
  Uint32Array indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;

  const auto& diameter        = options.diameter;
  const auto& thickness       = options.thickness;
  const auto& tessellation    = options.tessellation;
  const auto& sideOrientation = options.sideOrientation;

  auto stride = tessellation + 1;

  auto tessellationf = static_cast<float>(tessellation);

  for (unsigned int i = 0; i <= tessellation; ++i) {
    float u = static_cast<float>(i) / tessellationf;

    float outerAngle
      = static_cast<float>(i) * Math::PI * 2.f / tessellationf - Math::PI / 2.f;

    auto rotationY = Matrix::RotationY(outerAngle);
    auto transform
      = Matrix::Translation(diameter / 2.f, 0.f, 0.f).multiply(rotationY);

    for (unsigned int j = 0; j <= tessellation; ++j) {
      float v = 1.f - static_cast<float>(j) / tessellationf;

      float innerAngle
        = static_cast<float>(j) * Math::PI * 2.f / tessellationf + Math::PI;
      float dx = std::cos(innerAngle);
      float dy = std::sin(innerAngle);

      // Create a vertex.
      Vector3 normal(dx, dy, 0.f);
      Vector3 position = normal.scale(thickness / 2.f);
      Vector2 textureCoordinate(u, v);

      position = Vector3::TransformCoordinates(position, transform);
      normal   = Vector3::TransformNormal(normal, transform);

      stl_util::concat(positions, {position.x, position.y, position.z});
      stl_util::concat(normals, {normal.x, normal.y, normal.z});
      stl_util::concat(uvs, {textureCoordinate.x, textureCoordinate.y});

      // And create indices for two triangles.
      unsigned int nextI = (i + 1) % stride;
      unsigned int nextJ = (j + 1) % stride;

      indices.emplace_back(i * stride + j);
      indices.emplace_back(i * stride + nextJ);
      indices.emplace_back(nextI * stride + j);

      indices.emplace_back(i * stride + nextJ);
      indices.emplace_back(nextI * stride + nextJ);
      indices.emplace_back(nextI * stride + j);
    }
  }

  // Sides
  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            options.frontUVs, options.backUVs);

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  return vertexData;
}

std::unique_ptr<VertexData>
VertexData::CreateLineSystem(LineSystemOptions& options)
{
  Uint32Array indices;
  Float32Array positions;
  const auto& lines  = options.lines;
  const auto& colors = options.colors;
  Float32Array vertexColors;
  unsigned int idx = 0;

  for (size_t l = 0; l < lines.size(); ++l) {
    const auto& points = lines[l];
    for (size_t index = 0; index < points.size(); ++index) {
      stl_util::concat(positions,
                       {points[index].x, points[index].y, points[index].z});

      if (!colors.empty()) {
        const auto& color = colors[l];
        stl_util::concat(vertexColors, {color[index].r, color[index].g,
                                        color[index].b, color[index].a});
      }

      if (index > 0) {
        indices.emplace_back(idx - 1);
        indices.emplace_back(idx);
      }
      ++idx;
    }
  }

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  if (!colors.empty()) {
    vertexData->colors = vertexColors;
  }

  return vertexData;
}

std::unique_ptr<VertexData>
VertexData::CreateDashedLines(DashedLinesOptions& options)
{
  const auto& dashSize = options.dashSize;
  const auto& gapSize  = options.gapSize;
  const auto& dashNb   = options.dashNb;
  const auto& points   = options.points;

  Float32Array positions;
  Uint32Array indices;

  auto curvect     = Vector3::Zero();
  auto lg          = 0.f;
  auto nb          = 0.f;
  auto shft        = 0.f;
  auto dashshft    = 0.f;
  auto curshft     = 0.f;
  unsigned int idx = 0;
  size_t i         = 0;
  for (i = 0; i < points.size() - 1; ++i) {
    points[i + 1].subtractToRef(points[i], curvect);
    lg += curvect.length();
  }
  shft     = lg / static_cast<float>(dashNb);
  dashshft = dashSize * shft / (dashSize + gapSize);
  for (i = 0; i < points.size() - 1; ++i) {
    points[i + 1].subtractToRef(points[i], curvect);
    nb = std::floor(curvect.length() / shft);
    curvect.normalize();
    for (float j = 0.f; j < nb; ++j) {
      curshft = shft * j;
      stl_util::concat(positions, {points[i].x + curshft * curvect.x,
                                   points[i].y + curshft * curvect.y,
                                   points[i].z + curshft * curvect.z});
      stl_util::concat(positions,
                       {points[i].x + (curshft + dashshft) * curvect.x,
                        points[i].y + (curshft + dashshft) * curvect.y,
                        points[i].z + (curshft + dashshft) * curvect.z});
      stl_util::concat(indices, {idx, idx + 1});
      idx += 2;
    }
  }

  // Result
  auto vertexData       = std::make_unique<VertexData>();
  vertexData->positions = std::move(positions);
  vertexData->indices   = std::move(indices);

  return vertexData;
}

std::unique_ptr<VertexData> VertexData::CreateGround(GroundOptions& options)
{
  IndicesArray indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;
  uint32_t row, col;
  float rowf, colf;
  float x, y, z;

  const auto& width         = options.width;
  const auto& height        = options.height;
  const auto& subdivisionsX = options.subdivisionsX;
  const auto& subdivisionsY = options.subdivisionsY;

  const float widthf         = static_cast<float>(width);
  const float heightf        = static_cast<float>(height);
  const float subdivisionsXf = static_cast<float>(subdivisionsX);
  const float subdivisionsYf = static_cast<float>(subdivisionsY);

  for (row = 0; row <= subdivisionsY; ++row) {
    rowf = static_cast<float>(row);
    for (col = 0; col <= subdivisionsX; ++col) {
      colf = static_cast<float>(col);
      x    = (colf * widthf) / subdivisionsXf - (widthf / 2.f);
      y    = 0.f;
      z    = ((subdivisionsYf - rowf) * heightf) / subdivisionsYf
          - (heightf / 2.f);
      Vector3 position(x, y, z);
      Vector3 normal(0.f, 1.f, 0.f);

      stl_util::concat(positions, {position.x, position.y, position.z});
      stl_util::concat(normals, {normal.x, normal.y, normal.z});
      stl_util::concat(uvs,
                       {colf / subdivisionsXf, 1.f - rowf / subdivisionsYf});
    }
  }

  for (row = 0; row < subdivisionsY; ++row) {
    for (col = 0; col < subdivisionsX; ++col) {
      indices.emplace_back(col + 1 + (row + 1) * (subdivisionsX + 1));
      indices.emplace_back(col + 1 + row * (subdivisionsX + 1));
      indices.emplace_back(col + row * (subdivisionsX + 1));

      indices.emplace_back(col + (row + 1) * (subdivisionsX + 1));
      indices.emplace_back(col + 1 + (row + 1) * (subdivisionsX + 1));
      indices.emplace_back(col + row * (subdivisionsX + 1));
    }
  }

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  return vertexData;
}

std::unique_ptr<VertexData>
VertexData::CreateTiledGround(TiledGroundOptions& options)
{
  const auto& xmin = options.xmin;
  const auto& zmin = options.zmin;
  const auto& xmax = options.xmax;
  const auto& zmax = options.zmax;
  ISize subdivisions
    = {options.subdivisions.width, options.subdivisions.height};
  ISize precision = {options.subdivisions.width, options.subdivisions.height};

  Uint32Array indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;
  unsigned int row, col, tileRow, tileCol;

  subdivisions.height = (subdivisions.width < 1) ? 1 : subdivisions.height;
  subdivisions.width  = (subdivisions.width < 1) ? 1 : subdivisions.width;
  precision.width     = (precision.width < 1) ? 1 : precision.width;
  precision.height    = (precision.height < 1) ? 1 : precision.height;

  unsigned int subdivisions_h = static_cast<unsigned>(subdivisions.height);
  unsigned int subdivisions_w = static_cast<unsigned>(subdivisions.width);
  unsigned int precision_h    = static_cast<unsigned>(precision.height);
  unsigned int precision_w    = static_cast<unsigned>(precision.width);

  SizeF tileSize = {(xmax - xmin) / static_cast<float>(subdivisions.width), //
                    (zmax - zmin) / static_cast<float>(subdivisions.height)};

  auto applyTile
    = [&](float xTileMin, float zTileMin, float xTileMax, float zTileMax) {
        // Indices
        unsigned int base      = static_cast<unsigned>(positions.size() / 3);
        unsigned int rowLength = precision_w + 1;
        for (row = 0; row < precision_h; ++row) {
          for (col = 0; col < precision_w; ++col) {
            std::array<unsigned int, 4> square
              = {{base + col + row * rowLength,             //
                  base + (col + 1) + row * rowLength,       //
                  base + (col + 1) + (row + 1) * rowLength, //
                  base + col + (row + 1) * rowLength}};     //

            indices.emplace_back(square[1]);
            indices.emplace_back(square[2]);
            indices.emplace_back(square[3]);
            indices.emplace_back(square[0]);
            indices.emplace_back(square[1]);
            indices.emplace_back(square[3]);
          }
        }

        // Position, normals and uvs
        Vector3 position = Vector3::Zero();
        Vector3 normal(0.f, 1.f, 0.f);
        for (row = 0; row <= precision_h; ++row) {
          float rowf = static_cast<float>(row);
          position.z
            = (rowf * (zTileMax - zTileMin)) / static_cast<float>(precision_h)
              + zTileMin;
          for (col = 0; col <= precision_w; ++col) {
            float colf = static_cast<float>(col);
            position.x
              = (colf * (xTileMax - xTileMin)) / static_cast<float>(precision_w)
                + xTileMin;
            position.y = 0.f;

            stl_util::concat(positions, {position.x, position.y, position.z});
            stl_util::concat(normals, {normal.x, normal.y, normal.z});
            stl_util::concat(uvs, {colf / static_cast<float>(precision_w),
                                   rowf / static_cast<float>(precision_h)});
          }
        }
      };

  for (tileRow = 0; tileRow < subdivisions_h; ++tileRow) {
    float tileRowf = static_cast<float>(tileRow);
    for (tileCol = 0; tileCol < subdivisions_w; ++tileCol) {
      float tileColf = static_cast<float>(tileCol);
      applyTile(xmin + tileColf * tileSize.width,         //
                zmin + tileRowf * tileSize.height,        //
                xmin + (tileColf + 1.f) * tileSize.width, //
                zmin + (tileRowf + 1.f) * tileSize.height);
    }
  }

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  return vertexData;
}

std::unique_ptr<VertexData>
VertexData::CreateGroundFromHeightMap(GroundFromHeightMapOptions& options)
{
  Uint32Array indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;
  unsigned int row, col;
  float rowf, colf;

  const auto& width        = options.width;
  const auto& height       = options.height;
  const auto& subdivisions = options.subdivisions;
  const auto subdivisionsf = static_cast<float>(options.subdivisions);
  const auto& minHeight    = options.minHeight;
  const auto& maxHeight    = options.maxHeight;
  const auto bufferWidth   = static_cast<float>(options.bufferWidth);
  const auto bufferHeight  = static_cast<float>(options.bufferHeight);
  const auto& buffer       = options.buffer;
  const auto& filter       = options.colorFilter;
  const auto alphaFilter   = options.alphaFilter;

  // Vertices
  for (row = 0; row <= subdivisions; ++row) {
    rowf = static_cast<float>(row);
    for (col = 0; col <= subdivisions; ++col) {
      colf = static_cast<float>(col);
      Vector3 position(
        (colf * width) / subdivisionsf - (width / 2.f),                    // x
        0.f,                                                               // y
        ((subdivisionsf - rowf) * height) / subdivisionsf - (height / 2.f) // z
      );

      // Compute height
      unsigned int heightMapX = static_cast<unsigned>(
        ((position.x + width / 2.f) / width) * (bufferWidth - 1.f));
      unsigned int heightMapY = static_cast<unsigned>(
        (1.f - (position.z + height / 2.f) / height) * (bufferHeight - 1.f));

      const unsigned int pos
        = (heightMapX + heightMapY * options.bufferWidth) * 4;
      const auto r = static_cast<float>(buffer[pos]) / 255.f;
      const auto g = static_cast<float>(buffer[pos + 1]) / 255.f;
      const auto b = static_cast<float>(buffer[pos + 2]) / 255.f;
      const auto a = static_cast<float>(buffer[pos + 3]) / 255.f;

      const auto gradient = r * filter.r + g * filter.g + b * filter.b;

      // If our alpha channel is not within our filter then we will assign a
      // 'special' height Then when building the indices, we will ignore any
      // vertex that is using the special height
      if (a >= alphaFilter) {
        position.y = minHeight + (maxHeight - minHeight) * gradient;
      }
      else {
        // We can't have a height below minHeight, normally.
        position.y = minHeight - Math::Epsilon;
      }

      // Add  vertex
      stl_util::concat(positions, {position.x, position.y, position.z});
      stl_util::concat(normals, {0.f, 0.f, 0.f});
      stl_util::concat(uvs, {colf / subdivisionsf, 1.f - rowf / subdivisionsf});
    }
  }

  // Indices
  for (row = 0; row < subdivisions; ++row) {
    for (col = 0; col < subdivisions; ++col) {
      // Calculate Indices
      const auto idx1 = col + 1 + (row + 1) * (subdivisions + 1);
      const auto idx2 = col + 1 + row * (subdivisions + 1);
      const auto idx3 = col + row * (subdivisions + 1);

      const auto idx4 = col + (row + 1) * (subdivisions + 1);

      // Check that all indices are visible (based on our special height)
      // Only display the vertex if all Indices are visible
      // Positions are stored x,y,z for each vertex, hence the * 3 and + 1 for
      // height
      const auto isVisibleIdx1 = positions[idx1 * 3 + 1] >= minHeight;
      const auto isVisibleIdx2 = positions[idx2 * 3 + 1] >= minHeight;
      const auto isVisibleIdx3 = positions[idx3 * 3 + 1] >= minHeight;
      if (isVisibleIdx1 && isVisibleIdx2 && isVisibleIdx3) {
        indices.emplace_back(idx1);
        indices.emplace_back(idx2);
        indices.emplace_back(idx3);
      }

      auto isVisibleIdx4 = positions[idx4 * 3 + 1] >= minHeight;
      if (isVisibleIdx4 && isVisibleIdx1 && isVisibleIdx3) {
        indices.emplace_back(idx4);
        indices.emplace_back(idx1);
        indices.emplace_back(idx3);
      }
    }
  }

  // Normals
  VertexData::ComputeNormals(positions, indices, normals);

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  return vertexData;
}

std::unique_ptr<VertexData> VertexData::CreatePlane(PlaneOptions& options)
{
  Uint32Array indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;

  // Vertices
  float halfWidth  = options.width / 2.f;
  float halfHeight = options.height / 2.f;

  stl_util::concat(positions, {-halfWidth, -halfHeight, 0.f});
  stl_util::concat(normals, {0.f, 0.f, -1.f});
  stl_util::concat(uvs, {0.f, 0.f});

  stl_util::concat(positions, {halfWidth, -halfHeight, 0.f});
  stl_util::concat(normals, {0.f, 0.f, -1.f});
  stl_util::concat(uvs, {1.f, 0.f});

  stl_util::concat(positions, {halfWidth, halfHeight, 0.f});
  stl_util::concat(normals, {0.f, 0.f, -1.f});
  stl_util::concat(uvs, {1.f, 1.f});

  stl_util::concat(positions, {-halfWidth, halfHeight, 0.f});
  stl_util::concat(normals, {0.f, 0.f, -1.f});
  stl_util::concat(uvs, {0.f, 1.f});

  // Indices
  indices.emplace_back(0);
  indices.emplace_back(1);
  indices.emplace_back(2);

  indices.emplace_back(0);
  indices.emplace_back(2);
  indices.emplace_back(3);

  // Sides
  VertexData::_ComputeSides(options.sideOrientation, positions, indices,
                            normals, uvs, options.frontUVs, options.backUVs);

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  return vertexData;
}

std::unique_ptr<VertexData> VertexData::CreateDisc(DiscOptions& options)
{
  Float32Array positions;
  Uint32Array indices;
  Float32Array normals;
  Float32Array uvs;

  const auto& radius          = options.radius;
  const auto& tessellation    = options.tessellation;
  const auto arc              = options.arc();
  const auto& sideOrientation = options.sideOrientation;

  // positions and uvs
  stl_util::concat(positions, {0.f, 0.f, 0.f}); // disc center first
  stl_util::concat(uvs, {0.5f, 0.5f});

  float theta = Math::PI2 * arc;
  float step  = theta / static_cast<float>(tessellation);
  for (float a = 0.f; a < theta; a += step) {
    float x = std::cos(a);
    float y = std::sin(a);
    float u = (x + 1.f) / 2.f;
    float v = (1.f - y) / 2.f;
    stl_util::concat(positions, {radius * x, radius * y, 0.f});
    stl_util::concat(uvs, {u, v});
  }
  if (stl_util::almost_equal(arc, 1.f)) {
    // close the circle
    stl_util::concat(positions, {positions[3], positions[4], positions[5]});
    stl_util::concat(uvs, {uvs[2], uvs[3]});
  }

  // indices
  size_t vertexNb = positions.size() / 3;
  for (unsigned int i = 1; i < vertexNb - 1; ++i) {
    stl_util::concat(indices, {i + 1, 0, i});
  }

  // result
  VertexData::ComputeNormals(positions, indices, normals);

  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            options.frontUVs, options.backUVs);

  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  return vertexData;
}

std::unique_ptr<VertexData> VertexData::CreatePolygon(
  Mesh* polygon, unsigned int sideOrientation, const std::vector<Vector4>& fUV,
  const std::vector<Color4>& fColors, Vector4& frontUVs, Vector4& backUVs)
{
  auto faceUV     = fUV;
  auto faceColors = fColors;
  Float32Array colors;

  faceUV.resize(3);
  faceColors.resize(3);

  // default face colors and UV if undefined
  for (unsigned int f = 0; f < 3; ++f) {
    if (f >= fUV.size()) {
      faceUV[f] = Vector4(0, 0, 1, 1);
    }
    if (f >= fColors.size()) {
      faceColors[f] = Color4(1, 1, 1, 1);
    }
  }

  auto positions = polygon->getVerticesData(VertexBuffer::PositionKind);
  auto normals   = polygon->getVerticesData(VertexBuffer::NormalKind);
  auto uvs       = polygon->getVerticesData(VertexBuffer::UVKind);
  auto indices   = polygon->getIndices();

  // set face colours and textures
  unsigned int idx  = 0;
  unsigned int face = 0;
  for (unsigned int index = 0; index < normals.size(); index += 3) {
    // Edge Face  no. 1
    if (std::abs(normals[index + 1]) < 0.001f) {
      face = 1;
    }
    // Top Face  no. 0
    if (std::abs(normals[index + 1] - 1) < 0.001f) {
      face = 0;
    }
    // Bottom Face  no. 2
    if (std::abs(normals[index + 1] + 1) < 0.001f) {
      face = 2;
    }
    idx = index / 3;
    uvs[2 * idx]
      = (1 - uvs[2 * idx]) * faceUV[face].x + uvs[2 * idx] * faceUV[face].z;
    uvs[2 * idx + 1] = (1.f - uvs[2 * idx + 1]) * faceUV[face].y
                       + uvs[2 * idx + 1] * faceUV[face].w;
    if (!faceColors.empty()) {
      stl_util::concat(colors, {faceColors[face].r, faceColors[face].g,
                                faceColors[face].b, faceColors[face].a});
    }
  }

  // sides
  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            frontUVs, backUVs);

  // Result
  auto vertexData       = std::make_unique<VertexData>();
  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  if (!faceColors.empty()) {
    auto totalColors = (sideOrientation == Mesh::DOUBLESIDE) ?
                         stl_util::concat(colors, colors) :
                         colors;
    vertexData->colors = std::move(totalColors);
  }

  return vertexData;
}

std::unique_ptr<VertexData>
VertexData::CreateIcoSphere(IcoSphereOptions& options)
{
  const auto& sideOrientation = options.sideOrientation;
  const auto& radius          = options.radius;
  const auto& flat            = options.flat;
  const auto& subdivisions    = options.subdivisions;
  const auto subdivisionsf    = static_cast<float>(subdivisions);
  const auto& radiusX         = options.radiusX;
  const auto& radiusY         = options.radiusY;
  const auto& radiusZ         = options.radiusZ;

  const float t = (1.f + std::sqrt(5.f)) / 2.f;

  // 12 vertex x,y,z
  const std::array<float, 36> ico_vertices = {{
    -1.f, t,    -0.f, 1.f, t,   0.f,  -1.f, -t,   0.f, 1.f, -t,  0.f, // v0-3
    0.f,  -1.f, -t,   0.f, 1.f, -t,   0.f,  -1.f, t,   0.f, 1.f, t,   // v4-7
    t,    0.f,  1.f,  t,   0.f, -1.f, -t,   0.f,  1.f, -t,  0.f, -1.f // v8-11
  }};

  // index of 3 vertex makes a face of icopshere
  const std::array<unsigned int, 60> ico_indices = {{
    0,  11, 5,  0,  5,  1,  0,  1,  7,  0,  7,  10, 12, 22, 23, //
    1,  5,  20, 5,  11, 4,  23, 22, 13, 22, 18, 6,  7,  1,  8,  //
    14, 21, 4,  14, 4,  2,  16, 13, 6,  15, 6,  19, 3,  8,  9,  //
    4,  21, 5,  13, 17, 23, 6,  13, 22, 19, 6,  18, 9,  8,  1   //
  }};

  // vertex for uv have aliased position, not for UV
  const std::array<unsigned int, 24> vertices_unalias_id = {{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    // vertex alias
    0,  // 12: 0 + 12
    2,  // 13: 2 + 11
    3,  // 14: 3 + 11
    3,  // 15: 3 + 12
    3,  // 16: 3 + 13
    4,  // 17: 4 + 13
    7,  // 18: 7 + 11
    8,  // 19: 8 + 11
    9,  // 20: 9 + 11
    9,  // 21: 9 + 12
    10, // 22: A + 12
    11  // 23: B + 12
  }};

  // uv as integer step (not pixels !)
  const std::array<float, 48> ico_vertexuv = {{
    5, 1, 3, 1, 6, 4, 0, 0, // v0-3
    5, 3, 4, 2, 2, 2, 4, 0, // v4-7
    2, 0, 1, 1, 6, 0, 6, 2, // v8-11
    // vertex alias (for same vertex on different faces)
    0, 4, // 12: 0 + 12
    3, 3, // 13: 2 + 11
    4, 4, // 14: 3 + 11
    3, 1, // 15: 3 + 12
    4, 2, // 16: 3 + 13
    4, 4, // 17: 4 + 13
    0, 2, // 18: 7 + 11
    1, 1, // 19: 8 + 11
    2, 2, // 20: 9 + 11
    3, 3, // 21: 9 + 12
    1, 3, // 22: A + 12
    2, 4  // 23: B + 12
  }};

  /**
   * Vertices[0, 1, ...9, A, B] : position on UV plane
   *  '+' indicate duplicate position to be fixed (3,9:0,2,3,4,7,8,A,B)
   *  First island of uv mapping
   *  v = 4h          3+  2
   *  v = 3h        9+  4
   *  v = 2h      9+  5   B
   *  v = 1h    9   1   0
   *  v = 0h  3   8   7   A
   *      u = 0 1 2 3 4 5 6  *a
   *
   * Second island of uv mapping
   * v = 4h  0+  B+  4+
   * v = 3h    A+  2+
   * v = 2h  7+  6   3+
   * v = 1h    8+  3+
   * v = 0h
   *     u = 0 1 2 3 4 5 6  *a
   *
   * Face layout on texture UV mapping
   * ============
   * \ 4  /\ 16 /   ======
   *  \  /  \  /   /\ 11 /
   *   \/ 7  \/   /  \  /
   *    ======   / 10 \/
   *   /\ 17 /\  ======
   *  /  \  /  \ \ 15 /\
   * / 8  \/ 12 \ \  /  \
   * ============  \/ 6  \
   * \ 18 /\  ============
   *  \  /  \ \ 5  /\ 0  /
   *   \/ 13 \ \  /  \  /
   *    ======  \/ 1  \/
   *       ============
   *      /\ 19 /\  2 /\
   *     /  \  /  \  /  \
   *    / 14 \/ 9  \/  3 \
   *   ===================
   */

  // uv step is u:1 or 0.5, v:cos(30)=sqrt(3)/2, ratio approx is 84/97
  const float ustep   = 138.f / 1024.f;
  const float vstep   = 239.f / 1024.f;
  const float uoffset = 60.f / 1024.f;
  const float voffset = 26.f / 1024.f;
  // Second island should have margin, not to touch the first island
  // avoid any borderline artefact in pixel rounding
  const float island_u_offset = -40.f / 1024.f;
  const float island_v_offset = +20.f / 1024.f;
  // face is either island 0 or 1 :
  // second island is for faces : [4, 7, 8, 12, 13, 16, 17, 18]
  std::array<float, 20> island = {{
    0, 0, 0, 0, 1, //  0 - 4
    0, 0, 1, 1, 0, //  5 - 9
    0, 0, 1, 1, 0, //  10 - 14
    0, 1, 1, 1, 0  //  15 - 19
  }};

  Float32Array positions;
  Uint32Array indices;
  Float32Array normals;
  Float32Array uvs;

  unsigned int current_indice = 0;
  // prepare array of 3 vector (empty) (to be worked in place, shared for each
  // face)
  std::array<Vector3, 3> face_vertex_pos;
  std::array<Vector2, 3> face_vertex_uv;
  unsigned int v012;
  for (v012 = 0; v012 < 3; ++v012) {
    face_vertex_pos[v012] = Vector3::Zero();
    face_vertex_uv[v012]  = Vector2::Zero();
  }
  // create all with normals
  for (unsigned int face = 0; face < 20; ++face) {
    // 3 vertex per face
    for (v012 = 0; v012 < 3; ++v012) {
      // look up vertex 0,1,2 to its index in 0 to 11 (or 23 including alias)
      const unsigned int v_id = ico_indices[3 * face + v012];
      // vertex have 3D position (x,y,z)
      face_vertex_pos[v012].copyFromFloats(
        ico_vertices[3 * vertices_unalias_id[v_id]],
        ico_vertices[3 * vertices_unalias_id[v_id] + 1],
        ico_vertices[3 * vertices_unalias_id[v_id] + 2]);
      // Normalize to get normal, then scale to radius
      face_vertex_pos[v012].normalize().scaleInPlace(radius);

      // uv Coordinates from vertex ID
      face_vertex_uv[v012].copyFromFloats(
        ico_vertexuv[2 * v_id] * ustep + uoffset
          + island[face] * island_u_offset,
        ico_vertexuv[2 * v_id + 1] * vstep + voffset
          + island[face] * island_v_offset);
    }

    /**
     * Subdivide the face (interpolate pos, norm, uv)
     * - pos is linear interpolation, then projected to sphere (converge
     * polyhedron to sphere)
     * - norm is linear interpolation of vertex corner normal
     *   (to be checked if better to re-calc from face vertex, or if
     * approximation is OK ??? )
     * - uv is linear interpolation
     *
     * Topology is as below for sub-divide by 2
     * vertex shown as v0,v1,v2
     * interp index is i1 to progress in range [v0,v1[
     * nterp index is i2 to progress in range [v0,v2[
     * face index as  (i1,i2)  for /\  : (i1,i2),(i1+1,i2),(i1,i2+1)
     *            and (i1,i2)' for \/  : (i1+1,i2),(i1+1,i2+1),(i1,i2+1)
     *
     *
     *                    i2    v2
     *                    ^    ^
     *                   /    / \
     *                  /    /   \
     *                 /    /     \
     *                /    / (0,1) \
     *               /    #---------\
     *              /    / \ (0,0)'/ \
     *             /    /   \     /   \
     *            /    /     \   /     \
     *           /    / (0,0) \ / (1,0) \
     *          /    #---------#---------\
     *              v0                    v1
     *
     *              --------------------> i1
     *
     * interp of (i1,i2):
     *  along i2 :  x0=lerp(v0,v2, i2/S) <---> x1=lerp(v1,v2, i2/S)
     *  along i1 :  lerp(x0,x1, i1/(S-i2))
     *
     * centroid of triangle is needed to get help normal computation
     *  (c1,c2) are used for centroid location
     */

    const auto interp_vertex
      = [&](float i1, unsigned int i2, float c1, float c2) {
          // vertex is interpolated from
          //   - face_vertex_pos[0..2]
          //   - face_vertex_uv[0..2]
          const auto _div = static_cast<float>(i2) / subdivisionsf;
          const auto pos_x0
            = Vector3::Lerp(face_vertex_pos[0], face_vertex_pos[2], _div);
          const auto pos_x1
            = Vector3::Lerp(face_vertex_pos[1], face_vertex_pos[2], _div);
          auto pos_interp
            = (subdivisions == i2) ?
                face_vertex_pos[2] :
                Vector3::Lerp(pos_x0, pos_x1,
                              i1 / static_cast<float>(subdivisions - i2));
          pos_interp.normalize();

          Vector3 vertex_normal;
          if (flat) {
            // in flat mode, recalculate normal as face centroid normal
            const auto centroid_x0 = Vector3::Lerp(
              face_vertex_pos[0], face_vertex_pos[2], c2 / subdivisionsf);
            const auto centroid_x1 = Vector3::Lerp(
              face_vertex_pos[1], face_vertex_pos[2], c2 / subdivisionsf);
            vertex_normal = Vector3::Lerp(centroid_x0, centroid_x1,
                                          c1 / (subdivisionsf - c2));
          }
          else {
            // in smooth mode, recalculate normal from each single vertex
            // position
            vertex_normal = Vector3(pos_interp.x, pos_interp.y, pos_interp.z);
          }
          // Vertex normal need correction due to X,Y,Z radius scaling
          vertex_normal.x /= radiusX;
          vertex_normal.y /= radiusY;
          vertex_normal.z /= radiusZ;
          vertex_normal.normalize();

          const auto uv_x0
            = Vector2::Lerp(face_vertex_uv[0], face_vertex_uv[2], _div);
          const auto uv_x1
            = Vector2::Lerp(face_vertex_uv[1], face_vertex_uv[2], _div);
          const auto uv_interp
            = (subdivisions == i2) ?
                face_vertex_uv[2] :
                Vector2::Lerp(uv_x0, uv_x1,
                              i1 / (subdivisionsf - static_cast<float>(i2)));
          stl_util::concat(positions,
                           {pos_interp.x * radiusX, pos_interp.y * radiusY,
                            pos_interp.z * radiusZ});
          stl_util::concat(normals,
                           {vertex_normal.x, vertex_normal.y, vertex_normal.z});
          stl_util::concat(uvs, {uv_interp.x, uv_interp.y});
          // push each vertex has member of a face
          // Same vertex can bleong to multiple face, it is pushed multiple time
          // (duplicate vertex are present)
          indices.emplace_back(current_indice);
          ++current_indice;
        };

    for (unsigned int i2 = 0; i2 < subdivisions; ++i2) {
      const auto i2f = static_cast<float>(i2);
      for (unsigned int i1 = 0; i1 + i2 < subdivisions; ++i1) {
        const auto i1f = static_cast<float>(i1);
        // face : (i1,i2)  for /\  :
        // interp for : (i1,i2),(i1+1,i2),(i1,i2+1)
        interp_vertex(i1f, i2, i1f + 1.f / 3.f, i2f + 1.f / 3.f);
        interp_vertex(i1f + 1.f, i2, i1f + 1.f / 3.f, i2f + 1.f / 3.f);
        interp_vertex(i1f, i2 + 1, i1f + 1.f / 3.f, i2f + 1.f / 3.f);
        if (i1 + i2 + 1 < subdivisions) {
          // face : (i1,i2)' for \/  :
          // interp for (i1+1,i2),(i1+1,i2+1),(i1,i2+1)
          interp_vertex(i1f + 1.f, i2, i1f + 2.f / 3.f, i2f + 2.f / 3.f);
          interp_vertex(i1f + 1.f, i2 + 1, i1f + 2.f / 3, i2f + 2.f / 3.f);
          interp_vertex(i1f, i2 + 1, i1f + 2.f / 3.f, i2f + 2.f / 3.f);
        }
      }
    }
  }

  // Sides
  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            options.frontUVs, options.backUVs);

  // Result
  auto vertexData       = std::make_unique<VertexData>();
  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);
  return vertexData;
}

std::unique_ptr<VertexData>
VertexData::CreatePolyhedron(PolyhedronOptions& options)
{
  // inspired from // http://stemkoski.github.io/Three.js/Polyhedra.html
  //
  // Provided polyhedron types:
  // 0 : Tetrahedron,
  // 1 : Octahedron,
  // 2 : Dodecahedron,
  // 3 : Icosahedron,
  // 4 : Rhombicuboctahedron,
  // 5 : Triangular Prism,
  // 6 : Pentagonal Prism,
  // 7 : Hexagonal Prism,
  // 8 : Square Pyramid (J1)
  // 9 : Pentagonal Pyramid (J2),
  // 10 : Triangular Dipyramid (J12),
  // 11 : Pentagonal Dipyramid (J13),
  // 12 : Elongated Square Dipyramid (J15),
  // 13 : Elongated Pentagonal Dipyramid (J16),
  // 14 : Elongated Pentagonal Cupola (J20)
  std::vector<std::vector<Float32Array>> polyhedraVertices = {
    // polyhedra[0].vertex
    {{0.f, 0.f, 1.732051f},
     {1.632993f, 0.f, -0.5773503f},
     {-0.8164966f, 1.414214f, -0.5773503f},
     {-0.8164966f, -1.414214f, -0.5773503f}},
    // polyhedra[1].vertex
    {{0.f, 0.f, 1.414214f},
     {1.414214f, 0.f, 0.f},
     {0.f, 1.414214f, 0.f},
     {-1.414214f, 0.f, 0.f},
     {0.f, -1.414214f, 0.f},
     {0.f, 0.f, -1.414214f}},
    // polyhedra[2].vertex
    {{0.f, 0.f, 1.070466f},
     {0.7136442f, 0.f, 0.7978784f},
     {-0.3568221f, 0.618034f, 0.7978784f},
     {-0.3568221f, -0.618034f, 0.7978784f},
     {0.7978784f, 0.618034f, 0.3568221f},
     {0.7978784f, -0.618034f, 0.3568221f},
     {-0.9341724f, 0.381966f, 0.3568221f},
     {0.1362939f, 1.f, 0.3568221f},
     {0.1362939f, -1.f, 0.3568221f},
     {-0.9341724f, -0.381966f, 0.3568221f},
     {0.9341724f, 0.381966f, -0.3568221f},
     {0.9341724f, -0.381966f, -0.3568221f},
     {-0.7978784f, 0.618034f, -0.3568221f},
     {-0.1362939f, 1.f, -0.3568221f},
     {-0.1362939f, -1.f, -0.3568221f},
     {-0.7978784f, -0.618034f, -0.3568221f},
     {0.3568221f, 0.618034f, -0.7978784f},
     {0.3568221f, -0.618034f, -0.7978784f},
     {-0.7136442f, 0.f, -0.7978784f},
     {0.f, 0.f, -1.070466f}},
    // polyhedra[3].vertex
    {{0.f, 0.f, 1.175571f},
     {1.051462f, 0.f, 0.5257311f},
     {0.3249197f, 1.f, 0.5257311f},
     {-0.8506508f, 0.618034f, 0.5257311f},
     {-0.8506508f, -0.618034f, 0.5257311f},
     {0.3249197f, -1.f, 0.5257311f},
     {0.8506508f, 0.618034f, -0.5257311f},
     {0.8506508f, -0.618034f, -0.5257311f},
     {-0.3249197f, 1.f, -0.5257311f},
     {-1.051462f, 0.f, -0.5257311f},
     {-0.3249197f, -1.f, -0.5257311f},
     {0.f, 0.f, -1.175571f}},
    // polyhedra[4].vertex
    {{0.f, 0.f, 1.070722f},
     {0.7148135f, 0.f, 0.7971752f},
     {-0.104682f, 0.7071068f, 0.7971752f},
     {-0.6841528f, 0.2071068f, 0.7971752f},
     {-0.104682f, -0.7071068f, 0.7971752f},
     {0.6101315f, 0.7071068f, 0.5236279f},
     {1.04156f, 0.2071068f, 0.1367736f},
     {0.6101315f, -0.7071068f, 0.5236279f},
     {-0.3574067f, 1.f, 0.1367736f},
     {-0.7888348f, -0.5f, 0.5236279f},
     {-0.9368776f, 0.5f, 0.1367736f},
     {-0.3574067f, -1.f, 0.1367736f},
     {0.3574067f, 1.f, -0.1367736f},
     {0.9368776f, -0.5f, -0.1367736f},
     {0.7888348f, 0.5f, -0.5236279f},
     {0.3574067f, -1.f, -0.1367736f},
     {-0.6101315f, 0.7071068f, -0.5236279f},
     {-1.04156f, -0.2071068f, -0.1367736f},
     {-0.6101315f, -0.7071068f, -0.5236279f},
     {0.104682f, 0.7071068f, -0.7971752f},
     {0.6841528f, -0.2071068f, -0.7971752f},
     {0.104682f, -0.7071068f, -0.7971752f},
     {-0.7148135f, 0.f, -0.7971752f},
     {0.f, 0.f, -1.070722f}},
    // polyhedra[5].vertex
    {{0.f, 0.f, 1.322876f},
     {1.309307f, 0.f, 0.1889822f},
     {-0.9819805f, 0.8660254f, 0.1889822f},
     {0.1636634f, -1.299038f, 0.1889822f},
     {0.3273268f, 0.8660254f, -0.9449112f},
     {-0.8183171f, -0.4330127f, -0.9449112f}},
    // polyhedra[6].vertex
    {{0.f, 0.f, 1.159953f},
     {1.013464f, 0.f, 0.5642542f},
     {-0.3501431f, 0.9510565f, 0.5642542f},
     {-0.7715208f, -0.6571639f, 0.5642542f},
     {0.6633206f, 0.9510565f, -0.03144481f},
     {0.8682979f, -0.6571639f, -0.3996071f},
     {-1.121664f, 0.2938926f, -0.03144481f},
     {-0.2348831f, -1.063314f, -0.3996071f},
     {0.5181548f, 0.2938926f, -0.9953061f},
     {-0.5850262f, -0.112257f, -0.9953061f}},
    // polyhedra[7].vertex
    {{0.f, 0.f, 1.118034f},
     {0.8944272f, 0.f, 0.6708204f},
     {-0.2236068f, 0.8660254f, 0.6708204f},
     {-0.7826238f, -0.4330127f, 0.6708204f},
     {0.6708204f, 0.8660254f, 0.2236068f},
     {1.006231f, -0.4330127f, -0.2236068f},
     {-1.006231f, 0.4330127f, 0.2236068f},
     {-0.6708204f, -0.8660254f, -0.2236068f},
     {0.7826238f, 0.4330127f, -0.6708204f},
     {0.2236068f, -0.8660254f, -0.6708204f},
     {-0.8944272f, 0.f, -0.6708204f},
     {0.f, 0.f, -1.118034f}},
    // polyhedra[8].vertex
    {{-0.729665f, 0.670121f, 0.319155f},
     {-0.655235f, -0.29213f, -0.754096f},
     {-0.093922f, -0.607123f, 0.537818f},
     {0.702196f, 0.595691f, 0.485187f},
     {0.776626f, -0.36656f, -0.588064f}},
    // polyhedra[9].vertex
    {{-0.868849f, -0.100041f, 0.61257f},
     {-0.329458f, 0.976099f, 0.28078f},
     {-0.26629f, -0.013796f, -0.477654f},
     {-0.13392f, -1.034115f, 0.229829f},
     {0.738834f, 0.707117f, -0.307018f},
     {0.859683f, -0.535264f, -0.338508f}},
    // polyhedra[10].vertex
    {{-0.610389f, 0.243975f, 0.531213f},
     {-0.187812f, -0.48795f, -0.664016f},
     {-0.187812f, 0.9759f, -0.664016f},
     {0.187812f, -0.9759f, 0.664016f},
     {0.798201f, 0.243975f, 0.132803f}},
    // polyhedra[11].vertex
    {{-1.028778f, 0.392027f, -0.048786f},
     {-0.640503f, -0.646161f, 0.621837f},
     {-0.125162f, -0.395663f, -0.540059f},
     {0.004683f, 0.888447f, -0.651988f},
     {0.125161f, 0.395663f, 0.540059f},
     {0.632925f, -0.791376f, 0.433102f},
     {1.031672f, 0.157063f, -0.354165f}},
    // polyhedra[12].vertex
    {{-0.669867f, 0.334933f, -0.529576f},
     {-0.669867f, 0.334933f, 0.529577f},
     {-0.4043f, 1.212901f, 0.f},
     {-0.334933f, -0.669867f, -0.529576f},
     {-0.334933f, -0.669867f, 0.529577f},
     {0.334933f, 0.669867f, -0.529576f},
     {0.334933f, 0.669867f, 0.529577f},
     {0.4043f, -1.212901f, 0.f},
     {0.669867f, -0.334933f, -0.529576f},
     {0.669867f, -0.334933f, 0.529577f}},
    // polyhedra[13].vertex
    {{-0.931836f, 0.219976f, -0.264632f},
     {-0.636706f, 0.318353f, 0.692816f},
     {-0.613483f, -0.735083f, -0.264632f},
     {-0.326545f, 0.979634f, 0.f},
     {-0.318353f, -0.636706f, 0.692816f},
     {-0.159176f, 0.477529f, -0.856368f},
     {0.159176f, -0.477529f, -0.856368f},
     {0.318353f, 0.636706f, 0.692816f},
     {0.326545f, -0.979634f, 0.f},
     {0.613482f, 0.735082f, -0.264632f},
     {0.636706f, -0.318353f, 0.692816f},
     {0.931835f, -0.219977f, -0.264632f}},
    // polyhedra[14].vertex
    {{-0.93465f, 0.300459f, -0.271185f},  {-0.838689f, -0.260219f, -0.516017f},
     {-0.711319f, 0.717591f, 0.128359f},  {-0.710334f, -0.156922f, 0.080946f},
     {-0.599799f, 0.556003f, -0.725148f}, {-0.503838f, -0.004675f, -0.969981f},
     {-0.487004f, 0.26021f, 0.48049f},    {-0.460089f, -0.750282f, -0.512622f},
     {-0.376468f, 0.973135f, -0.325605f}, {-0.331735f, -0.646985f, 0.084342f},
     {-0.254001f, 0.831847f, 0.530001f},  {-0.125239f, -0.494738f, -0.966586f},
     {0.029622f, 0.027949f, 0.730817f},   {0.056536f, -0.982543f, -0.262295f},
     {0.08085f, 1.087391f, 0.076037f},    {0.125583f, -0.532729f, 0.485984f},
     {0.262625f, 0.599586f, 0.780328f},   {0.391387f, -0.726999f, -0.716259f},
     {0.513854f, -0.868287f, 0.139347f},  {0.597475f, 0.85513f, 0.326364f},
     {0.641224f, 0.109523f, 0.783723f},   {0.737185f, -0.451155f, 0.538891f},
     {0.848705f, -0.612742f, -0.314616f}, {0.976075f, 0.365067f, 0.32976f},
     {1.072036f, -0.19561f, 0.084927f}}};
  std::vector<std::vector<Uint32Array>> polyhedraFaces
    = {// polyhedra[0].face
       {{0, 1, 2}, {0, 2, 3}, {0, 3, 1}, {1, 3, 2}},
       // polyhedra[1].face
       {{0, 1, 2},
        {0, 2, 3},
        {0, 3, 4},
        {0, 4, 1},
        {1, 4, 5},
        {1, 5, 2},
        {2, 5, 3},
        {3, 5, 4}},
       // polyhedra[2].face
       {{0, 1, 4, 7, 2},
        {0, 2, 6, 9, 3},
        {0, 3, 8, 5, 1},
        {1, 5, 11, 10, 4},
        {2, 7, 13, 12, 6},
        {3, 9, 15, 14, 8},
        {4, 10, 16, 13, 7},
        {5, 8, 14, 17, 11},
        {6, 12, 18, 15, 9},
        {10, 11, 17, 19, 16},
        {12, 13, 16, 19, 18},
        {14, 15, 18, 19, 17}},
       // polyhedra[3].face
       {{0, 1, 2},  {0, 2, 3},  {0, 3, 4},   {0, 4, 5},  {0, 5, 1},
        {1, 5, 7},  {1, 7, 6},  {1, 6, 2},   {2, 6, 8},  {2, 8, 3},
        {3, 8, 9},  {3, 9, 4},  {4, 9, 10},  {4, 10, 5}, {5, 10, 7},
        {6, 7, 11}, {6, 11, 8}, {7, 10, 11}, {8, 11, 9}, {9, 11, 10}},
       // polyhedra[4].face
       {{0, 2, 3},        {1, 6, 5},        {4, 9, 11},       {7, 15, 13},
        {8, 16, 10},      {12, 14, 19},     {17, 22, 18},     {20, 21, 23},
        {0, 1, 5, 2},     {0, 3, 9, 4},     {0, 4, 7, 1},     {1, 7, 13, 6},
        {2, 5, 12, 8},    {2, 8, 10, 3},    {3, 10, 17, 9},   {4, 11, 15, 7},
        {5, 6, 14, 12},   {6, 13, 20, 14},  {8, 12, 19, 16},  {9, 17, 18, 11},
        {10, 16, 22, 17}, {11, 18, 21, 15}, {13, 15, 21, 20}, {14, 20, 23, 19},
        {16, 19, 23, 22}, {18, 22, 23, 21}},
       // polyhedra[5].face
       {{0, 3, 1}, {2, 4, 5}, {0, 1, 4, 2}, {0, 2, 5, 3}, {1, 3, 5, 4}},
       // polyhedra[6].face
       {{0, 1, 4, 2},
        {0, 2, 6, 3},
        {1, 5, 8, 4},
        {3, 6, 9, 7},
        {5, 7, 9, 8},
        {0, 3, 7, 5, 1},
        {2, 4, 8, 9, 6}},
       // polyhedra[7].face
       {{0, 1, 4, 2},
        {0, 2, 6, 3},
        {1, 5, 8, 4},
        {3, 6, 10, 7},
        {5, 9, 11, 8},
        {7, 10, 11, 9},
        {0, 3, 7, 9, 5, 1},
        {2, 4, 8, 11, 10, 6}},
       // polyhedra[8].face
       {{1, 4, 2}, {0, 1, 2}, {3, 0, 2}, {4, 3, 2}, {4, 1, 0, 3}},
       // polyhedra[9].face
       {{3, 0, 2}, {5, 3, 2}, {4, 5, 2}, {1, 4, 2}, {0, 1, 2}, {0, 3, 5, 4, 1}},
       // polyhedra[10].face
       {{1, 3, 0}, {3, 4, 0}, {3, 1, 4}, {0, 2, 1}, {0, 4, 2}, {2, 4, 1}},
       // polyhedra[11].face
       {{3, 2, 0},
        {2, 1, 0},
        {2, 5, 1},
        {0, 4, 3},
        {0, 1, 4},
        {4, 1, 5},
        {2, 3, 6},
        {3, 4, 6},
        {5, 2, 6},
        {4, 5, 6}},
       // polyhedra[12].face
       {{8, 9, 7},
        {6, 5, 2},
        {3, 8, 7},
        {5, 0, 2},
        {4, 3, 7},
        {0, 1, 2},
        {9, 4, 7},
        {1, 6, 2},
        {9, 8, 5, 6},
        {8, 3, 0, 5},
        {3, 4, 1, 0},
        {4, 9, 6, 1}},
       // polyhedra[13].face
       {{11, 10, 8},
        {7, 9, 3},
        {6, 11, 8},
        {9, 5, 3},
        {2, 6, 8},
        {5, 0, 3},
        {4, 2, 8},
        {0, 1, 3},
        {10, 4, 8},
        {1, 7, 3},
        {10, 11, 9, 7},
        {11, 6, 5, 9},
        {6, 2, 0, 5},
        {2, 4, 1, 0},
        {4, 10, 7, 1}},
       // polyhedra[14].face
       {{15, 18, 21},      {12, 20, 16},
        {6, 10, 2},        {3, 0, 1},
        {9, 7, 13},        {2, 8, 4, 0},
        {0, 4, 5, 1},      {1, 5, 11, 7},
        {7, 11, 17, 13},   {13, 17, 22, 18},
        {18, 22, 24, 21},  {21, 24, 23, 20},
        {20, 23, 19, 16},  {16, 19, 14, 10},
        {10, 14, 8, 2},    {15, 9, 13, 18},
        {12, 15, 21, 20},  {6, 12, 16, 10},
        {3, 6, 2, 0},      {9, 3, 1, 7},
        {9, 15, 12, 6, 3}, {22, 17, 11, 5, 4, 8, 14, 19, 23, 24}}};

  auto type                = options.type;
  type                     = (type >= polyhedraVertices.size()) ? 0 : type;
  const auto& sizeX        = options.sizeX;
  const auto& sizeY        = options.sizeY;
  const auto& sizeZ        = options.sizeZ;
  const auto& dataVertices = options.custom.vertices.empty() ?
                               polyhedraVertices[type] :
                               options.custom.vertices;
  const auto& dataFaces = options.custom.faces.empty() ? polyhedraFaces[type] :
                                                         options.custom.faces;
  const auto nbfaces          = dataFaces.size();
  const auto& flat            = options.flat;
  const auto& sideOrientation = options.sideOrientation;

  Float32Array positions;
  Uint32Array indices;
  Float32Array normals;
  Float32Array uvs;
  Float32Array colors;
  unsigned int index = 0;
  size_t faceIdx     = 0; // face cursor in the array "indexes"
  Uint32Array indexes;
  unsigned int i = 0;
  unsigned int f = 0;
  float u, v, ang, x, y, tmp;

  // default face colors and UV if undefined
  std::vector<Color4> faceColors;
  std::vector<Vector4> faceUV;
  if (flat) {
    for (f = 1; f <= nbfaces; ++f) {
      if (f > faceColors.size()) {
        faceColors.emplace_back(Color4(1.f, 1.f, 1.f, 1.f));
      }
      if (f > faceUV.size())
        faceUV.emplace_back(Vector4(0.f, 0.f, 1.f, 1.f));
    }
  }
  if (!flat) {
    for (i = 0; i < dataVertices.size(); ++i) {
      stl_util::concat(positions,
                       {dataVertices[i][0] * sizeX, dataVertices[i][1] * sizeY,
                        dataVertices[i][2] * sizeZ});
      stl_util::concat(uvs, {0.f, 0.f});
    }
    for (f = 0; f < nbfaces; ++f) {
      for (i = 0; i < dataFaces[f].size() - 2; ++i) {
        stl_util::concat(
          indices, {dataFaces[f][0], dataFaces[f][i + 2], dataFaces[f][i + 1]});
      }
    }
  }
  else {
    for (f = 0; f < nbfaces; ++f) {
      size_t fl = dataFaces[f].size(); // number of vertices of the current face
      ang       = Math::PI2 / static_cast<float>(fl);
      x         = 0.5f * std::tan(ang / 2.f);
      y         = 0.5f;

      // positions, uvs, colors
      for (i = 0; i < fl; ++i) {
        // positions
        stl_util::concat(positions, {dataVertices[dataFaces[f][i]][0] * sizeX,
                                     dataVertices[dataFaces[f][i]][1] * sizeY,
                                     dataVertices[dataFaces[f][i]][2] * sizeZ});
        indexes.emplace_back(index);
        ++index;
        // uvs
        u = faceUV[f].x + (faceUV[f].z - faceUV[f].x) * (0.5f + x);
        v = faceUV[f].y + (faceUV[f].w - faceUV[f].y) * (y - 0.5f);
        stl_util::concat(uvs, {u, v});
        tmp = x * std::cos(ang) - y * std::sin(ang);
        y   = x * std::sin(ang) + y * std::cos(ang);
        x   = tmp;
        // colors
        if (!faceColors.empty()) {
          stl_util::concat(colors, {faceColors[f].r, faceColors[f].g,
                                    faceColors[f].b, faceColors[f].a});
        }
      }

      // indices from indexes
      for (i = 0; i < fl - 2; ++i) {
        stl_util::concat(indices,
                         {indexes[0 + faceIdx], indexes[i + 2 + faceIdx],
                          indexes[i + 1 + faceIdx]});
      }
      faceIdx += fl;
    }
  }

  VertexData::ComputeNormals(positions, indices, normals);
  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            options.frontUVs, options.backUVs);

  auto vertexData       = std::make_unique<VertexData>();
  vertexData->positions = std::move(positions);
  vertexData->indices   = std::move(indices);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);
  if (!faceColors.empty() && flat) {
    vertexData->colors = std::move(colors);
  }
  return vertexData;
}

std::unique_ptr<VertexData>
VertexData::CreateTorusKnot(TorusKnotOptions& options)
{
  Uint32Array indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;

  const auto& radius          = options.radius;
  const auto& tube            = options.tube;
  const auto& radialSegments  = options.radialSegments;
  const auto radialSegmentsf  = static_cast<float>(radialSegments);
  const auto& tubularSegments = options.tubularSegments;
  const auto tubularSegmentsf = static_cast<float>(tubularSegments);
  const auto& p               = options.p;
  const auto& q               = options.q;
  const auto& sideOrientation = options.sideOrientation;

  // Helper
  auto getPos = [&](float angle) {
    auto cu      = std::cos(angle);
    auto su      = std::sin(angle);
    auto quOverP = q / p * angle;
    auto cs      = std::cos(quOverP);

    auto tx = radius * (2.f + cs) * 0.5f * cu;
    auto ty = radius * (2.f + cs) * su * 0.5f;
    auto tz = radius * std::sin(quOverP) * 0.5f;

    return Vector3(tx, ty, tz);
  };

  // Vertices
  unsigned int i;
  unsigned int j;
  unsigned int modJ;
  float v, cx, cy;
  for (i = 0; i <= radialSegments; ++i) {
    unsigned int modI = i % radialSegments;

    auto u    = static_cast<float>(modI) / radialSegmentsf * 2.f * p * Math::PI;
    auto p1   = getPos(u);
    auto p2   = getPos(u + 0.01f);
    auto tang = p2.subtract(p1);
    auto n    = p2.add(p1);

    auto bitan = Vector3::Cross(tang, n);
    n          = Vector3::Cross(bitan, tang);

    bitan.normalize();
    n.normalize();

    for (j = 0; j < tubularSegments; ++j) {
      modJ = j % tubularSegments;

      v  = static_cast<float>(modJ) / tubularSegmentsf * 2.f * Math::PI;
      cx = -tube * std::cos(v);
      cy = tube * std::sin(v);

      positions.emplace_back(p1.x + cx * n.x + cy * bitan.x);
      positions.emplace_back(p1.y + cx * n.y + cy * bitan.y);
      positions.emplace_back(p1.z + cx * n.z + cy * bitan.z);

      uvs.emplace_back(static_cast<float>(i) / radialSegmentsf);
      uvs.emplace_back(static_cast<float>(j) / tubularSegmentsf);
    }
  }

  unsigned int jNext;
  unsigned int a, b, c, d;
  for (i = 0; i < radialSegments; ++i) {
    for (j = 0; j < tubularSegments; ++j) {
      jNext = (j + 1) % tubularSegments;
      a     = i * tubularSegments + j;
      b     = (i + 1) * tubularSegments + j;
      c     = (i + 1) * tubularSegments + jNext;
      d     = i * tubularSegments + jNext;

      indices.emplace_back(d);
      indices.emplace_back(b);
      indices.emplace_back(a);
      indices.emplace_back(d);
      indices.emplace_back(c);
      indices.emplace_back(b);
    }
  }

  // Normals
  VertexData::ComputeNormals(positions, indices, normals);

  // Sides
  VertexData::_ComputeSides(sideOrientation, positions, indices, normals, uvs,
                            options.frontUVs, options.backUVs);

  // Result
  auto vertexData = std::make_unique<VertexData>();

  vertexData->indices   = std::move(indices);
  vertexData->positions = std::move(positions);
  vertexData->normals   = std::move(normals);
  vertexData->uvs       = std::move(uvs);

  return vertexData;
}

// Tools

void VertexData::ComputeNormals(const Float32Array& positions,
                                const Uint32Array& indices,
                                Float32Array& normals,
                                std::optional<FacetParameters> options)
{
  if (normals.size() < positions.size()) {
    normals.resize(positions.size());
  }

  // temporary scalar variables
  unsigned int index         = 0;   // facet index
  float p1p2x                = 0.f; // p1p2 vector x coordinate
  float p1p2y                = 0.f; // p1p2 vector y coordinate
  float p1p2z                = 0.f; // p1p2 vector z coordinate
  float p3p2x                = 0.f; // p3p2 vector x coordinate
  float p3p2y                = 0.f; // p3p2 vector y coordinate
  float p3p2z                = 0.f; // p3p2 vector z coordinate
  float faceNormalx          = 0.f; // facet normal x coordinate
  float faceNormaly          = 0.f; // facet normal y coordinate
  float faceNormalz          = 0.f; // facet normal z coordinate
  float length               = 0.f; // facet normal length before normalization
  unsigned int v1x           = 0;   // vector1 x index in the positions array
  unsigned int v1y           = 0;   // vector1 y index in the positions array
  unsigned int v1z           = 0;   // vector1 z index in the positions array
  unsigned int v2x           = 0;   // vector2 x index in the positions array
  unsigned int v2y           = 0;   // vector2 y index in the positions array
  unsigned int v2z           = 0;   // vector2 z index in the positions array
  unsigned int v3x           = 0;   // vector3 x index in the positions array
  unsigned int v3y           = 0;   // vector3 y index in the positions array
  unsigned int v3z           = 0;   // vector3 z index in the positions array
  bool computeFacetNormals   = false;
  bool computeFacetPositions = false;
  bool computeFacetPartitioning     = false;
  bool computeDepthSort             = false;
  float faceNormalSign              = 1.f;
  float ratio                       = 0.f;
  std::optional<Vector3> distanceTo = std::nullopt;
  std::vector<DepthSortedFacet> depthSortedFacets;
  if (options) {
    computeFacetNormals   = (!options->facetNormals.empty()) ? true : false;
    computeFacetPositions = (!options->facetPositions.empty()) ? true : false;
    computeFacetPartitioning
      = (!options->facetPartitioning.empty()) ? true : false;
    faceNormalSign   = (options->useRightHandedSystem == true) ? -1.f : 1.f;
    ratio            = options->ratio ? *options->ratio : 0.f;
    computeDepthSort = (options->depthSort) ? true : false;
    distanceTo       = options->distanceTo;
    if (computeDepthSort) {
      if (!distanceTo.has_value()) {
        distanceTo = Vector3::Zero();
      }
      depthSortedFacets = options->depthSortedFacets;
    }
  }

  // facetPartitioning reinit if needed
  float xSubRatio    = 0.f;
  float ySubRatio    = 0.f;
  float zSubRatio    = 0.f;
  unsigned int subSq = 0;

  unsigned int ox           = 0; // X partitioning index for facet position
  unsigned int oy           = 0; // Y partinioning index for facet position
  unsigned int oz           = 0; // Z partinioning index for facet position
  unsigned int b1x          = 0; // X partitioning index for facet v1 vertex
  unsigned int b1y          = 0; // Y partitioning index for facet v1 vertex
  unsigned int b1z          = 0; // z partitioning index for facet v1 vertex
  unsigned int b2x          = 0; // X partitioning index for facet v2 vertex
  unsigned int b2y          = 0; // Y partitioning index for facet v2 vertex
  unsigned int b2z          = 0; // Z partitioning index for facet v2 vertex
  unsigned int b3x          = 0; // X partitioning index for facet v3 vertex
  unsigned int b3y          = 0; // Y partitioning index for facet v3 vertex
  unsigned int b3z          = 0; // Z partitioning index for facet v3 vertex
  unsigned int block_idx_o  = 0; // facet barycenter block index
  unsigned int block_idx_v1 = 0; // v1 vertex block index
  unsigned int block_idx_v2 = 0; // v2 vertex block index
  unsigned int block_idx_v3 = 0; // v3 vertex block index

  if (computeFacetPartitioning && options && (*options).bbSize) {
    const auto& bbSize = *options->bbSize;
    float bbSizeMax    = (bbSize.x > bbSize.y) ? bbSize.x : bbSize.y;
    bbSizeMax          = (bbSizeMax > bbSize.z) ? bbSizeMax : bbSize.z;
    xSubRatio          = options->subDiv.X * ratio / bbSize.x;
    ySubRatio          = options->subDiv.Y * ratio / bbSize.y;
    zSubRatio          = options->subDiv.Z * ratio / bbSize.z;
    subSq              = options->subDiv.max * options->subDiv.max;
    options->facetPartitioning.clear();
  }

  // reset the normals
  normals.resize(positions.size());
  std::fill(normals.begin(), normals.end(), 0.f);

  // Loop : 1 indice triplet = 1 facet
  size_t nbFaces = indices.size() / 3;
  for (index = 0; index < nbFaces; index++) {

    // get the indexes of the coordinates of each vertex of the facet
    v1x = indices[index * 3] * 3;
    v1y = v1x + 1;
    v1z = v1x + 2;
    v2x = indices[index * 3 + 1] * 3;
    v2y = v2x + 1;
    v2z = v2x + 2;
    v3x = indices[index * 3 + 2] * 3;
    v3y = v3x + 1;
    v3z = v3x + 2;

    // compute two vectors per facet : p1p2 and p3p2
    p1p2x = positions[v1x] - positions[v2x];
    p1p2y = positions[v1y] - positions[v2y];
    p1p2z = positions[v1z] - positions[v2z];

    p3p2x = positions[v3x] - positions[v2x];
    p3p2y = positions[v3y] - positions[v2y];
    p3p2z = positions[v3z] - positions[v2z];

    // compute the face normal with the cross product
    faceNormalx = faceNormalSign * (p1p2y * p3p2z - p1p2z * p3p2y);
    faceNormaly = faceNormalSign * (p1p2z * p3p2x - p1p2x * p3p2z);
    faceNormalz = faceNormalSign * (p1p2x * p3p2y - p1p2y * p3p2x);

    // normalize this normal and store it in the array facetData
    length = std::sqrt(faceNormalx * faceNormalx + faceNormaly * faceNormaly
                       + faceNormalz * faceNormalz);
    length = stl_util::almost_equal(length, 0.f) ? 1.f : length;
    faceNormalx /= length;
    faceNormaly /= length;
    faceNormalz /= length;

    if (computeFacetNormals && options) {
      options->facetNormals[index].x = faceNormalx;
      options->facetNormals[index].y = faceNormaly;
      options->facetNormals[index].z = faceNormalz;
    }

    if (computeFacetPositions && options) {
      // compute and the facet barycenter coordinates in the array
      // facetPositions
      options->facetPositions[index].x
        = (positions[v1x] + positions[v2x] + positions[v3x]) / 3.f;
      options->facetPositions[index].y
        = (positions[v1y] + positions[v2y] + positions[v3y]) / 3.f;
      options->facetPositions[index].z
        = (positions[v1z] + positions[v2z] + positions[v3z]) / 3.f;
    }

    if (computeFacetPartitioning && options) {
      // store the facet indexes in arrays in the main facetPartitioning array :
      // compute each facet vertex (+ facet barycenter) index in the partiniong
      // array
      ox  = static_cast<unsigned>(std::floor(
        (options->facetPositions[index].x - options->bInfo.minimum().x * ratio)
        * xSubRatio));
      oy  = static_cast<unsigned>(std::floor(
        (options->facetPositions[index].y - options->bInfo.minimum().y * ratio)
        * ySubRatio));
      oz  = static_cast<unsigned>(std::floor(
        (options->facetPositions[index].z - options->bInfo.minimum().z * ratio)
        * zSubRatio));
      b1x = static_cast<unsigned>(std::floor(
        (positions[v1x] - options->bInfo.minimum().x * ratio) * xSubRatio));
      b1y = static_cast<unsigned>(std::floor(
        (positions[v1y] - options->bInfo.minimum().y * ratio) * ySubRatio));
      b1z = static_cast<unsigned>(std::floor(
        (positions[v1z] - options->bInfo.minimum().z * ratio) * zSubRatio));
      b2x = static_cast<unsigned>(std::floor(
        (positions[v2x] - options->bInfo.minimum().x * ratio) * xSubRatio));
      b2y = static_cast<unsigned>(std::floor(
        (positions[v2y] - options->bInfo.minimum().y * ratio) * ySubRatio));
      b2z = static_cast<unsigned>(std::floor(
        (positions[v2z] - options->bInfo.minimum().z * ratio) * zSubRatio));
      b3x = static_cast<unsigned>(std::floor(
        (positions[v3x] - options->bInfo.minimum().x * ratio) * xSubRatio));
      b3y = static_cast<unsigned>(std::floor(
        (positions[v3y] - options->bInfo.minimum().y * ratio) * ySubRatio));
      b3z = static_cast<unsigned>(std::floor(
        (positions[v3z] - options->bInfo.minimum().z * ratio) * zSubRatio));

      block_idx_v1 = b1x + options->subDiv.max * b1y + subSq * b1z;
      block_idx_v2 = b2x + options->subDiv.max * b2y + subSq * b2z;
      block_idx_v3 = b3x + options->subDiv.max * b3y + subSq * b3z;
      block_idx_o  = ox + options->subDiv.max * oy + subSq * oz;

      const std::array<unsigned int, 4> block_idxs{
        {block_idx_o, block_idx_v1, block_idx_v2, block_idx_v3}};
      for (auto& block_idx : block_idxs) {
        // Check if facetPartitioning needs to be resized
        if (options->facetPartitioning.size() <= block_idx) {
          options->facetPartitioning.resize(block_idx + 1);
        }
      }

      // push each facet index in each block containing the vertex
      options->facetPartitioning[block_idx_v1].emplace_back(index);
      if (block_idx_v2 != block_idx_v1) {
        options->facetPartitioning[block_idx_v2].emplace_back(index);
      }
      if (!(block_idx_v3 == block_idx_v2 || block_idx_v3 == block_idx_v1)) {
        options->facetPartitioning[block_idx_v3].emplace_back(index);
      }
      if (!(block_idx_o == block_idx_v1 || block_idx_o == block_idx_v2
            || block_idx_o == block_idx_v3)) {
        options->facetPartitioning[block_idx_o].emplace_back(index);
      }
    }

    if (computeDepthSort && options && !options->facetPositions.empty()) {
      if (nbFaces >= depthSortedFacets.size()) {
        depthSortedFacets.resize(nbFaces);
      }
      auto& dsf = depthSortedFacets[index];
      dsf.ind   = index * 3;
      dsf.sqDistance
        = Vector3::DistanceSquared(options->facetPositions[index], *distanceTo);
    }

    // compute the normals anyway
    normals[v1x] += faceNormalx; // accumulate all the normals per face
    normals[v1y] += faceNormaly;
    normals[v1z] += faceNormalz;
    normals[v2x] += faceNormalx;
    normals[v2y] += faceNormaly;
    normals[v2z] += faceNormalz;
    normals[v3x] += faceNormalx;
    normals[v3y] += faceNormaly;
    normals[v3z] += faceNormalz;
  }
  // last normalization of each normal
  for (index = 0; index < normals.size() / 3; ++index) {
    faceNormalx = normals[index * 3];
    faceNormaly = normals[index * 3 + 1];
    faceNormalz = normals[index * 3 + 2];

    length = std::sqrt(faceNormalx * faceNormalx + faceNormaly * faceNormaly
                       + faceNormalz * faceNormalz);
    length = stl_util::almost_equal(length, 0.f) ? 1.f : length;
    faceNormalx /= length;
    faceNormaly /= length;
    faceNormalz /= length;

    normals[index * 3]     = faceNormalx;
    normals[index * 3 + 1] = faceNormaly;
    normals[index * 3 + 2] = faceNormalz;
  }
}

void VertexData::_ComputeSides(unsigned int sideOrientation,
                               Float32Array& positions, Uint32Array& indices,
                               Float32Array& normals, Float32Array& uvs,
                               const std::optional<Vector4>& iFrontUVs,
                               const std::optional<Vector4>& iBackUVs)
{
  size_t li = indices.size();
  size_t ln = normals.size();
  size_t i;
  size_t n;

  switch (sideOrientation) {
    case Mesh::FRONTSIDE:
    default:
      // nothing changed
      break;
    case Mesh::BACKSIDE:
      uint16_t tmp;
      // indices
      for (i = 0; i < li; i += 3) {
        tmp            = static_cast<uint16_t>(indices[i]);
        indices[i]     = indices[i + 2];
        indices[i + 2] = tmp;
      }
      // normals
      for (n = 0; n < ln; ++n) {
        normals[n] = -normals[n];
      }
      break;
    case Mesh::DOUBLESIDE:
      // positions
      size_t lp      = positions.size();
      unsigned int l = static_cast<unsigned>(lp / 3);
      for (unsigned int p = 0; p < lp; ++p) {
        positions[lp + p] = positions[p];
      }
      // indices
      for (i = 0; i < li; i += 3) {
        indices[i + li]     = indices[i + 2] + l;
        indices[i + 1 + li] = indices[i + 1] + l;
        indices[i + 2 + li] = indices[i] + l;
      }
      // normals
      for (n = 0; n < ln; ++n) {
        normals[ln + n] = -normals[n];
      }
      // uvs
      const std::size_t lu = uvs.size();
      for (std::size_t u = 0; u < lu; ++u) {
        uvs[u + lu] = uvs[u];
      }
      auto frontUVs
        = iFrontUVs.has_value() ? *iFrontUVs : Vector4(0.f, 0.f, 1.f, 1.f);
      auto backUVs
        = iBackUVs.has_value() ? *iBackUVs : Vector4(0.f, 0.f, 1.f, 1.f);
      for (std::size_t u = 0, i = 0; i < lu / 2; ++i) {
        uvs[u]          = frontUVs.x + (frontUVs.z - frontUVs.x) * uvs[u];
        uvs[u + 1]      = frontUVs.y + (frontUVs.w - frontUVs.y) * uvs[u + 1];
        uvs[u + lu]     = backUVs.x + (backUVs.z - backUVs.x) * uvs[u + lu];
        uvs[u + lu + 1] = backUVs.y + (backUVs.w - backUVs.y) * uvs[u + lu + 1];
        u += 2;
      }
      break;
  }
}

void VertexData::ImportVertexData(const json& parsedVertexData,
                                  Geometry& geometry)
{
  auto vertexData = std::make_unique<VertexData>();

  // positions
  Float32Array positions
    = json_util::get_array<float>(parsedVertexData, "positions");
  if (json_util::has_key(parsedVertexData, "positions")) {
    vertexData->set(positions, VertexBuffer::PositionKind);
  }

  // normals
  if (json_util::has_key(parsedVertexData, "normals")) {
    vertexData->set(json_util::get_array<float>(parsedVertexData, "normals"),
                    VertexBuffer::NormalKind);
  }

  // tangents
  if (json_util::has_key(parsedVertexData, "tangents")) {
    vertexData->set(json_util::get_array<float>(parsedVertexData, "tangents"),
                    VertexBuffer::TangentKind);
  }

  // uvs
  if (json_util::has_key(parsedVertexData, "uvs")) {
    vertexData->set(json_util::get_array<float>(parsedVertexData, "uvs"),
                    VertexBuffer::UVKind);
  }

  // uv2s
  if (json_util::has_key(parsedVertexData, "uv2s")) {
    vertexData->set(json_util::get_array<float>(parsedVertexData, "uv2s"),
                    VertexBuffer::UV2Kind);
  }

  // uv3s
  if (json_util::has_key(parsedVertexData, "uv3s")) {
    vertexData->set(json_util::get_array<float>(parsedVertexData, "uv3s"),
                    VertexBuffer::UV3Kind);
  }

  // uv4s
  if (json_util::has_key(parsedVertexData, "uv4s")) {
    vertexData->set(json_util::get_array<float>(parsedVertexData, "uv4s"),
                    VertexBuffer::UV4Kind);
  }

  // uv5s
  if (json_util::has_key(parsedVertexData, "uv5s")) {
    vertexData->set(json_util::get_array<float>(parsedVertexData, "uv5s"),
                    VertexBuffer::UV5Kind);
  }

  // uv6s
  if (json_util::has_key(parsedVertexData, "uv6s")) {
    vertexData->set(json_util::get_array<float>(parsedVertexData, "uv6s"),
                    VertexBuffer::UV6Kind);
  }

  // colors
  if (json_util::has_key(parsedVertexData, "colors")) {
    const Float32Array colors
      = json_util::get_array<float>(parsedVertexData, "colors");
    vertexData->set(Color4::CheckColors4(colors, positions.size() / 3),
                    VertexBuffer::ColorKind);
  }

  // matricesIndices
  if (json_util::has_key(parsedVertexData, "matricesIndices")) {
    vertexData->set(
      json_util::get_array<float>(parsedVertexData, "matricesIndices"),
      VertexBuffer::MatricesIndicesKind);
  }

  // matricesWeights
  if (json_util::has_key(parsedVertexData, "matricesWeights")) {
    vertexData->set(
      json_util::get_array<float>(parsedVertexData, "matricesWeights"),
      VertexBuffer::MatricesWeightsKind);
  }

  // indices
  if (json_util::has_key(parsedVertexData, "indices")) {
    vertexData->indices
      = json_util::get_array<uint32_t>(parsedVertexData, "indices");
  }

  geometry.setAllVerticesData(
    vertexData.get(),
    json_util::get_bool(parsedVertexData, "updatable", false));
}

} // end of namespace BABYLON
