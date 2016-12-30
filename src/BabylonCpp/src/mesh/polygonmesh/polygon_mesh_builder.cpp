#include <babylon/mesh/polygonmesh/polygon_mesh_builder.h>

#include <babylon/math/path2.h>
#include <babylon/math/vector2.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/polygonmesh/indexed_vector2.h>
#include <babylon/mesh/vertex_buffer.h>
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion" // conversion from int to char
#endif
#include <earcut.hpp>
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

namespace BABYLON {

PolygonMeshBuilder::PolygonMeshBuilder(const std::string& name,
                                       const Path2& contours, Scene* scene)
    : _name{name}, _scene{scene}
{
  std::vector<Vector2> points = contours.getPoints();
  _addToepoint(points);

  _points.add(points);
  _outlinepoints.add(points);
}

PolygonMeshBuilder::PolygonMeshBuilder(const std::string& name,
                                       const std::vector<Vector2>& contours,
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

void PolygonMeshBuilder::_addToepoint(const std::vector<Vector2>& points)
{
  for (auto& p : points) {
    Point2D point{{p.x, p.y}};
    _epoints.emplace_back(point);
  }
}

PolygonMeshBuilder&
PolygonMeshBuilder::addHole(const std::vector<Vector2>& hole)
{
  _points.add(hole);
  PolygonPoints holepoints;
  holepoints.add(hole);
  _holes.emplace_back(holepoints);
  _eholes.emplace_back(_epoints.size());
  _addToepoint(hole);

  return *this;
}

Mesh* PolygonMeshBuilder::build(bool updatable, float depth)
{
  auto result = Mesh::New(_name, _scene);

  Float32Array normals;
  Float32Array positions;
  Float32Array uvs;

  Bounds bounds                              = _points.computeBounds();
  std::vector<IndexedVector2>& pointElements = _points.elements;
  for (auto& p : pointElements) {
    std_util::concat(normals, {0.f, 1.f, 0.f});
    std_util::concat(positions, {p.x, 0.f, p.y});
    std_util::concat(uvs, {(p.x - bounds.min.x) / bounds.width,
                           (p.y - bounds.min.y) / bounds.height});
  }

  auto addHoles
    = [](const std::vector<Point2D>& epoints, const Uint32Array& holeIndices,
         std::vector<std::vector<Point2D>>& polygon) {
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
            size_t endIndex = i < len - 1 ? holeIndices[i + 1] : epoints.size();
            IndexRange range{{startIndex, endIndex}};
            holes.emplace_back(range);
          }
          // Add outer ring
          std::vector<Point2D> ring(
            epoints.begin(), epoints.begin() + static_cast<long>(holes[0][0]));
          polygon.emplace_back(ring);
          // Add holes
          for (auto& holeRange : holes) {
            std::vector<Point2D> hole(
              epoints.begin() + static_cast<long>(holeRange[0]),
              epoints.begin() + static_cast<long>(holeRange[1]));
            polygon.emplace_back(hole);
          }
        }
      };

  std::vector<std::vector<Point2D>> polygon;
  // Earcut.hpp has no 'holes' argument, adding the holes to the input array
  addHoles(_epoints, _eholes, polygon);
  std::vector<int32_t> res = mapbox::earcut<int32_t>(polygon);

  Uint32Array indices;
  for (auto r : res) {
    indices.emplace_back(r);
  }

  if (depth > 0.f) {
    size_t positionscount
      = (positions.size() / 3); // get the current pointcount

    for (auto& p : pointElements) { // add the elements at the depth
      std_util::concat(normals, {0.f, -1.f, 0.f});
      std_util::concat(positions, {p.x, -depth, p.y});
      std_util::concat(uvs, {1.f - (p.x - bounds.min.x) / bounds.width,
                             1.f - (p.y - bounds.min.y) / bounds.height});
    }

    size_t totalCount = indices.size();
    for (size_t i = 0; i < totalCount; i += 3) {
      unsigned int i0 = indices[i + 0];
      unsigned int i1 = indices[i + 1];
      unsigned int i2 = indices[i + 2];

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

void PolygonMeshBuilder::addSide(Float32Array& positions, Float32Array& normals,
                                 Float32Array& uvs, Uint32Array& indices,
                                 const Bounds& bounds,
                                 const PolygonPoints& points, float depth,
                                 bool flip)
{
  size_t startIndex = positions.size() / 3;
  float ulength     = 0.f;
  for (size_t i = 0; i < points.elements.size(); ++i) {
    IndexedVector2 p = points.elements[i];
    IndexedVector2 p1;
    if ((i + 1) > points.elements.size() - 1) {
      p1 = points.elements[0];
    }
    else {
      p1 = points.elements[i + 1];
    }

    std_util::concat(positions, {p.x, 0.f, p.y});
    std_util::concat(positions, {p.x, -depth, p.y});
    std_util::concat(positions, {p1.x, 0.f, p1.y});
    std_util::concat(positions, {p1.x, -depth, p1.y});

    Vector3 v1(p.x, 0.f, p.y);
    Vector3 v2(p1.x, 0.f, p1.y);
    Vector3 v3 = v2.subtract(v1);
    Vector3 v4(0.f, 1.f, 0.f);
    Vector3 vn = Vector3::Cross(v3, v4);
    vn         = vn.normalize();

    std_util::concat(uvs, {ulength / bounds.width, 0.f});
    std_util::concat(uvs, {ulength / bounds.width, 1.f});
    ulength += v3.length();
    std_util::concat(uvs, {(ulength / bounds.width), 0.f});
    std_util::concat(uvs, {(ulength / bounds.width), 1.f});

    if (!flip) {
      std_util::concat(normals, {-vn.x, -vn.y, -vn.z});
      std_util::concat(normals, {-vn.x, -vn.y, -vn.z});
      std_util::concat(normals, {-vn.x, -vn.y, -vn.z});
      std_util::concat(normals, {-vn.x, -vn.y, -vn.z});

      indices.emplace_back(startIndex);
      indices.emplace_back(startIndex + 1);
      indices.emplace_back(startIndex + 2);

      indices.emplace_back(startIndex + 1);
      indices.emplace_back(startIndex + 3);
      indices.emplace_back(startIndex + 2);
    }
    else {
      std_util::concat(normals, {vn.x, vn.y, vn.z});
      std_util::concat(normals, {vn.x, vn.y, vn.z});
      std_util::concat(normals, {vn.x, vn.y, vn.z});
      std_util::concat(normals, {vn.x, vn.y, vn.z});

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

} // end of namespace BABYLON
