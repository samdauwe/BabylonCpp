#include <babylon/meshes/polygonmesh/polygon_mesh_builder.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/maths/path2.h>
#include <babylon/maths/vector2.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/polygonmesh/indexed_vector2.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
// Conversion from int to char
#pragma GCC diagnostic ignored "-Wconversion"
#if __GNUC__ > 6
// Use of GNU statement expression extension
#endif
#endif
#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
#include <earcut.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace BABYLON {

PolygonMeshBuilder::PolygonMeshBuilder(const std::string& name, const Path2& contours, Scene* scene)
    : _name{name}
{
  _scene = scene ? scene : Engine::LastCreatedScene();

  auto points = contours.getPoints();
  _addToepoint(points);

  _points.add(points);
  _outlinepoints.add(points);
}

PolygonMeshBuilder::PolygonMeshBuilder(const std::string& name,
                                       const std::vector<Vector2>& contours, Scene* scene)
    : _name{name}
{
  _scene = scene ? scene : Engine::LastCreatedScene();

  _addToepoint(contours);

  _points.add(contours);
  _outlinepoints.add(contours);
}

PolygonMeshBuilder::~PolygonMeshBuilder() = default;

void PolygonMeshBuilder::_addToepoint(const std::vector<Vector2>& points)
{
  for (const auto& p : points) {
    _epoints.emplace_back(Point2D{{p.x, p.y}});
  }
}

PolygonMeshBuilder& PolygonMeshBuilder::addHole(const std::vector<Vector2>& hole)
{
  _points.add(hole);
  PolygonPoints holepoints;
  holepoints.add(hole);
  _holes.emplace_back(holepoints);

  _eholes.emplace_back(static_cast<int32_t>(_epoints.size()));
  _addToepoint(hole);

  return *this;
}

MeshPtr PolygonMeshBuilder::build(bool updatable, float depth, float smoothingThreshold)
{
  const auto result = Mesh::New(_name, _scene);

  const auto vertexData = buildVertexData(
    depth,
    smoothingThreshold); // NOLINT (this leads to a clang-tidy warning in external earcut.hpp)

  result->setVerticesData(VertexBuffer::PositionKind, vertexData->positions, updatable);
  result->setVerticesData(VertexBuffer::NormalKind, vertexData->normals, updatable);
  result->setVerticesData(VertexBuffer::UVKind, vertexData->uvs, updatable);
  result->setIndices(vertexData->indices);

  return result;
}

std::unique_ptr<VertexData> PolygonMeshBuilder::buildVertexData(float depth,
                                                                float smoothingThreshold)
{
  auto result = std::make_unique<VertexData>();

  Float32Array normals;
  Float32Array positions;
  Float32Array uvs;

  auto bounds               = _points.computeBounds();
  const auto& pointElements = _points.elements;
  for (const auto& p : pointElements) {
    stl_util::concat(normals, {0.f, 1.f, 0.f});
    stl_util::concat(positions, {p.x, 0.f, p.y});
    stl_util::concat(uvs,
                     {(p.x - bounds.min.x) / bounds.width, (p.y - bounds.min.y) / bounds.height});
  }

  std::vector<std::vector<Point2D>> polygon;
  // Earcut.hpp has no 'holes' argument, adding the holes to the input array
  addHoles(_epoints, _eholes, polygon);
  auto res = mapbox::earcut<uint32_t>(
    polygon); // NOLINT (this leads to a clang-tidy warning in external earcut.hpp)

  Uint32Array indices;
  for (auto r : res) {
    indices.emplace_back(r);
  }

  if (depth > 0.f) {
    // get the current pointcount
    auto positionscount = static_cast<uint32_t>(positions.size() / 3);

    for (const auto& p : pointElements) { // add the elements at the depth
      stl_util::concat(normals, {0.f, -1.f, 0.f});
      stl_util::concat(positions, {p.x, -depth, p.y});
      stl_util::concat(uvs, {1.f - (p.x - bounds.min.x) / bounds.width,
                             1.f - (p.y - bounds.min.y) / bounds.height});
    }

    size_t totalCount = indices.size();
    for (size_t i = 0; i < totalCount; i += 3) {
      const auto i0 = indices[i + 0];
      const auto i1 = indices[i + 1];
      const auto i2 = indices[i + 2];

      indices.emplace_back(i2 + positionscount);
      indices.emplace_back(i1 + positionscount);
      indices.emplace_back(i0 + positionscount);
    }

    // Add the sides
    addSide(positions, normals, uvs, indices, bounds, _outlinepoints, depth, false,
            smoothingThreshold);

    for (const auto& hole : _holes) {
      addSide(positions, normals, uvs, indices, bounds, hole, depth, true, smoothingThreshold);
    }
  }

  result->indices   = std::move(indices);
  result->positions = std::move(positions);
  result->normals   = std::move(normals);
  result->uvs       = std::move(uvs);

  return result;
}

std::pair<Float32Array, Uint32Array> PolygonMeshBuilder::buildWall(const Vector3& wall0Corner,
                                                                   const Vector3& wall1Corner)
{
  Float32Array positions;
  Uint32Array indices;
  auto direction = wall1Corner.subtract(wall0Corner).normalize();
  auto angle     = std::acos(direction.x);
  if (direction.z != 0.f) {
    angle *= direction.z / std::abs(direction.z);
  }
  const auto& pointElements = _points.elements;
  for (const auto& p : pointElements) {
    stl_util::concat(positions, {p.x * std::cos(angle) + wall0Corner.x, p.y,
                                 p.x * std::sin(angle) + wall0Corner.z});
  }
  std::vector<std::vector<Point2D>> polygon;
  // Earcut.hpp has no 'holes' argument, adding the holes to the input array
  addHoles(_epoints, _eholes, polygon);
  auto res = mapbox::earcut<int32_t>(polygon);
  for (size_t i = res.size(); i-- > 0;) {
    indices.emplace_back(res[i]);
  }

  return std::make_pair(positions, indices);
}

PolygonPoints& PolygonMeshBuilder::points()
{
  return _points;
}

const PolygonPoints& PolygonMeshBuilder::points() const
{
  return _points;
}

void PolygonMeshBuilder::addSide(Float32Array& positions, Float32Array& normals, Float32Array& uvs,
                                 Uint32Array& indices, const Bounds& bounds,
                                 const PolygonPoints& points, float depth, bool flip,
                                 float smoothingThreshold)
{
  auto startIndex = static_cast<uint32_t>(positions.size() / 3);
  auto ulength    = 0.f;
  for (size_t i = 0; i < points.elements.size(); ++i) {
    const auto& p  = points.elements[i];
    const auto& p1 = points.elements[(i + 1) % points.elements.size()];

    stl_util::concat(positions, {p.x, 0.f, p.y});
    stl_util::concat(positions, {p.x, -depth, p.y});
    stl_util::concat(positions, {p1.x, 0.f, p1.y});
    stl_util::concat(positions, {p1.x, -depth, p1.y});

    const auto& p0 = points.elements[(i + points.elements.size() - 1) % points.elements.size()];
    const auto& p2 = points.elements[(i + 2) % points.elements.size()];

    Vector3 vc(-(p1.y - p.y), 0.f, p1.x - p.x);
    Vector3 vp(-(p.y - p0.y), 0.f, p.x - p0.x);
    Vector3 vn(-(p2.y - p1.y), 0.f, p2.x - p1.x);

    if (!flip) {
      vc = vc.scale(-1.f);
      vp = vp.scale(-1.f);
      vn = vn.scale(-1.f);
    }

    auto vc_norm = vc.normalizeToNew();
    auto vp_norm = vp.normalizeToNew();
    auto vn_norm = vn.normalizeToNew();

    const auto dotp = Vector3::Dot(vp_norm, vc_norm);
    if (dotp > smoothingThreshold) {
      if (dotp < Math::Epsilon - 1.f) {
        vp_norm = (Vector3(p.x, 0.f, p.y)).subtract(Vector3(p1.x, 0.f, p1.y)).normalize();
      }
      else {
        // cheap average weighed by side length
        vp_norm = vp.add(vc).normalize();
      }
    }
    else {
      vp_norm = vc_norm;
    }

    const auto dotn = Vector3::Dot(vn, vc);
    if (dotn > smoothingThreshold) {
      if (dotn < Math::Epsilon - 1) {
        // back to back
        vn_norm = (Vector3(p1.x, 0.f, p1.y)).subtract(Vector3(p.x, 0.f, p.y)).normalize();
      }
      else {
        // cheap average weighed by side length
        vn_norm = vn.add(vc).normalize();
      }
    }
    else {
      vn_norm = vc_norm;
    }

    stl_util::concat(uvs, {ulength / bounds.width, 0});
    stl_util::concat(uvs, {ulength / bounds.width, 1});
    ulength += vc.length();
    stl_util::concat(uvs, {(ulength / bounds.width), 0});
    stl_util::concat(uvs, {(ulength / bounds.width), 1});

    stl_util::concat(normals, {vp_norm.x, vp_norm.y, vp_norm.z});
    stl_util::concat(normals, {vp_norm.x, vp_norm.y, vp_norm.z});
    stl_util::concat(normals, {vn_norm.x, vn_norm.y, vn_norm.z});
    stl_util::concat(normals, {vn_norm.x, vn_norm.y, vn_norm.z});

    if (!flip) {
      indices.emplace_back(startIndex);
      indices.emplace_back(startIndex + 1);
      indices.emplace_back(startIndex + 2);

      indices.emplace_back(startIndex + 1);
      indices.emplace_back(startIndex + 3);
      indices.emplace_back(startIndex + 2);
    }
    else {
      indices.emplace_back(startIndex);
      indices.emplace_back(startIndex + 2);
      indices.emplace_back(startIndex + 1);

      indices.emplace_back(startIndex + 1);
      indices.emplace_back(startIndex + 2);
      indices.emplace_back(startIndex + 3);
    }
    startIndex += 4;
  }
}

void PolygonMeshBuilder::addHoles(const std::vector<Point2D>& epoints,
                                  const Uint32Array& holeIndices,
                                  std::vector<std::vector<Point2D>>& polygon)
{
  // Check if polygon has holes
  if (holeIndices.empty()) {
    polygon.emplace_back(epoints);
  }
  else {
    // Determine hole indices
    using IndexRange = std::array<size_t, 2>;
    std::vector<IndexRange> holes;
    for (size_t i = 0, len = holeIndices.size(); i < len; i++) {
      size_t startIndex = holeIndices[i];
      size_t endIndex   = i < len - 1 ? holeIndices[i + 1] : epoints.size();
      IndexRange range{{startIndex, endIndex}};
      holes.emplace_back(range);
    }
    // Add outer ring
    std::vector<Point2D> ring(epoints.begin(), epoints.begin() + static_cast<long>(holes[0][0]));
    polygon.emplace_back(ring);
    // Add holes
    for (const auto& holeRange : holes) {
      std::vector<Point2D> hole(epoints.begin() + static_cast<long>(holeRange[0]),
                                epoints.begin() + static_cast<long>(holeRange[1]));
      polygon.emplace_back(hole);
    }
  }
}

} // end of namespace BABYLON
