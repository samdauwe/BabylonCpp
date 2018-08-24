#include <babylon/mesh/polygonmesh/polygon_mesh_builder.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/math/path2.h>
#include <babylon/math/vector2.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/polygonmesh/indexed_vector2.h>
#include <babylon/mesh/vertex_buffer.h>
#ifdef __GNUC__
#pragma GCC diagnostic push
// Conversion from int to char
#pragma GCC diagnostic ignored "-Wconversion"
#if __GNUC__ > 6
// Use of GNU statement expression extension
#endif
#endif
#include <earcut.hpp>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace BABYLON {

PolygonMeshBuilder::PolygonMeshBuilder(const string_t& name,
                                       const Path2& contours, Scene* scene)
    : _name{name}, _scene{scene}
{
  auto points = contours.getPoints();
  _addToepoint(points);

  _points.add(points);
  _outlinepoints.add(points);
}

PolygonMeshBuilder::PolygonMeshBuilder(const string_t& name,
                                       const vector_t<Vector2>& contours,
                                       Scene* scene)
    : _name{name}, _scene{scene}
{
  _addToepoint(contours);

  _points.add(contours);
  _outlinepoints.add(contours);
}

PolygonMeshBuilder::~PolygonMeshBuilder()
{
}

void PolygonMeshBuilder::_addToepoint(const vector_t<Vector2>& points)
{
  for (auto& p : points) {
    Point2D point{{p.x, p.y}};
    _epoints.emplace_back(point);
  }
}

PolygonMeshBuilder& PolygonMeshBuilder::addHole(const vector_t<Vector2>& hole)
{
  _points.add(hole);
  PolygonPoints holepoints;
  holepoints.add(hole);
  _holes.emplace_back(holepoints);
  _eholes.emplace_back(static_cast<int32_t>(_epoints.size()));
  _addToepoint(hole);

  return *this;
}

MeshPtr PolygonMeshBuilder::build(bool updatable, float depth)
{
  auto result = Mesh::New(_name, _scene);

  Float32Array normals;
  Float32Array positions;
  Float32Array uvs;

  auto bounds               = _points.computeBounds();
  const auto& pointElements = _points.elements;
  for (const auto& p : pointElements) {
    stl_util::concat(normals, {0.f, 1.f, 0.f});
    stl_util::concat(positions, {p.x, 0.f, p.y});
    stl_util::concat(uvs, {(p.x - bounds.min.x) / bounds.width,
                           (p.y - bounds.min.y) / bounds.height});
  }

  vector_t<vector_t<Point2D>> polygon;
  // Earcut.hpp has no 'holes' argument, adding the holes to the input array
  addHoles(_epoints, _eholes, polygon);
  auto res = mapbox::earcut<int32_t>(polygon);

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
    addSide(positions, normals, uvs, indices, bounds, _outlinepoints, depth,
            false);

    for (auto& hole : _holes) {
      addSide(positions, normals, uvs, indices, bounds, hole, depth, true);
    }
  }

  result->setVerticesData(VertexBuffer::PositionKind, positions, updatable);
  result->setVerticesData(VertexBuffer::NormalKind, normals, updatable);
  result->setVerticesData(VertexBuffer::UVKind, uvs, updatable);
  result->setIndices(indices);

  return result;
}

::std::pair<Float32Array, Uint32Array>
PolygonMeshBuilder::buildWall(const Vector3& wall0Corner,
                              const Vector3& wall1Corner)
{
  Float32Array positions;
  Uint32Array indices;
  auto direction = wall1Corner.subtract(wall0Corner).normalize();
  auto angle     = ::std::acos(direction.x);
  if (direction.z != 0.f) {
    angle *= direction.z / ::std::abs(direction.z);
  }
  const auto& pointElements = _points.elements;
  for (const auto& p : pointElements) {
    stl_util::concat(positions, {p.x * ::std::cos(angle) + wall0Corner.x, p.y,
                                 p.x * ::std::sin(angle) + wall0Corner.z});
  };
  vector_t<vector_t<Point2D>> polygon;
  // Earcut.hpp has no 'holes' argument, adding the holes to the input array
  addHoles(_epoints, _eholes, polygon);
  auto res = mapbox::earcut<int32_t>(polygon);
  for (size_t i = res.size(); i-- > 0;) {
    indices.emplace_back(res[i]);
  }

  return ::std::make_pair(positions, indices);
}

PolygonPoints& PolygonMeshBuilder::points()
{
  return _points;
}

const PolygonPoints& PolygonMeshBuilder::points() const
{
  return _points;
}

void PolygonMeshBuilder::addSide(Float32Array& positions, Float32Array& normals,
                                 Float32Array& uvs, Uint32Array& indices,
                                 const Bounds& bounds,
                                 const PolygonPoints& points, float depth,
                                 bool flip)
{
  auto startIndex = static_cast<uint32_t>(positions.size() / 3);
  float ulength   = 0.f;
  for (size_t i = 0; i < points.elements.size(); ++i) {
    const auto& p = points.elements[i];
    IndexedVector2 p1;
    if ((i + 1) > points.elements.size() - 1) {
      p1 = points.elements[0];
    }
    else {
      p1 = points.elements[i + 1];
    }

    stl_util::concat(positions, {p.x, 0.f, p.y});
    stl_util::concat(positions, {p.x, -depth, p.y});
    stl_util::concat(positions, {p1.x, 0.f, p1.y});
    stl_util::concat(positions, {p1.x, -depth, p1.y});

    Vector3 v1(p.x, 0.f, p.y);
    Vector3 v2(p1.x, 0.f, p1.y);
    Vector3 v3 = v2.subtract(v1);
    Vector3 v4(0.f, 1.f, 0.f);
    Vector3 vn = Vector3::Cross(v3, v4);
    vn         = vn.normalize();

    stl_util::concat(uvs, {ulength / bounds.width, 0.f});
    stl_util::concat(uvs, {ulength / bounds.width, 1.f});
    ulength += v3.length();
    stl_util::concat(uvs, {(ulength / bounds.width), 0.f});
    stl_util::concat(uvs, {(ulength / bounds.width), 1.f});

    if (!flip) {
      stl_util::concat(normals, {-vn.x, -vn.y, -vn.z});
      stl_util::concat(normals, {-vn.x, -vn.y, -vn.z});
      stl_util::concat(normals, {-vn.x, -vn.y, -vn.z});
      stl_util::concat(normals, {-vn.x, -vn.y, -vn.z});

      indices.emplace_back(startIndex + 0);
      indices.emplace_back(startIndex + 1);
      indices.emplace_back(startIndex + 2);

      indices.emplace_back(startIndex + 1);
      indices.emplace_back(startIndex + 3);
      indices.emplace_back(startIndex + 2);
    }
    else {
      stl_util::concat(normals, {vn.x, vn.y, vn.z});
      stl_util::concat(normals, {vn.x, vn.y, vn.z});
      stl_util::concat(normals, {vn.x, vn.y, vn.z});
      stl_util::concat(normals, {vn.x, vn.y, vn.z});

      indices.emplace_back(startIndex + 0);
      indices.emplace_back(startIndex + 2);
      indices.emplace_back(startIndex + 1);

      indices.emplace_back(startIndex + 1);
      indices.emplace_back(startIndex + 2);
      indices.emplace_back(startIndex + 3);
    }
    startIndex += 4;
  }
}

void PolygonMeshBuilder::addHoles(const vector_t<Point2D>& epoints,
                                  const Uint32Array& holeIndices,
                                  vector_t<vector_t<Point2D>>& polygon)
{
  // Check if polygon has holes
  if (holeIndices.empty()) {
    polygon.emplace_back(epoints);
  }
  else {
    // Determine hole indices
    using IndexRange = array_t<size_t, 2>;
    vector_t<IndexRange> holes;
    for (size_t i = 0, len = holeIndices.size(); i < len; i++) {
      size_t startIndex = holeIndices[i];
      size_t endIndex   = i < len - 1 ? holeIndices[i + 1] : epoints.size();
      IndexRange range{{startIndex, endIndex}};
      holes.emplace_back(range);
    }
    // Add outer ring
    vector_t<Point2D> ring(epoints.begin(),
                           epoints.begin() + static_cast<long>(holes[0][0]));
    polygon.emplace_back(ring);
    // Add holes
    for (auto& holeRange : holes) {
      vector_t<Point2D> hole(epoints.begin() + static_cast<long>(holeRange[0]),
                             epoints.begin() + static_cast<long>(holeRange[1]));
      polygon.emplace_back(hole);
    }
  }
}

} // end of namespace BABYLON
