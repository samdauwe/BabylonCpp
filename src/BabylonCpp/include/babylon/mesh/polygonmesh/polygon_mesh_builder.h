#ifndef BABYLON_MESH_POLYGONMESH_POLYGON_MESH_BUILDER_H
#define BABYLON_MESH_POLYGONMESH_POLYGON_MESH_BUILDER_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/mesh/polygonmesh/polygon_points.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PolygonMeshBuilder {

public:
  using Point2D = array_t<float, 2>;

public:
  PolygonMeshBuilder(const string_t& name, const Path2& contours, Scene* scene);
  PolygonMeshBuilder(const string_t& name, const vector_t<Vector2>& contours,
                     Scene* scene);
  ~PolygonMeshBuilder();

  PolygonMeshBuilder& addHole(const vector_t<Vector2>& hole);
  Mesh* build(bool updatable = false, float depth = 0.f);
  ::std::pair<Float32Array, Uint32Array> buildWall(const Vector3& wall0Corner,
                                                   const Vector3& wall1Corner);

  PolygonPoints& points();
  const PolygonPoints& points() const;

private:
  void _addToepoint(const vector_t<Vector2>& points);
  void addSide(Float32Array& positions, Float32Array& normals,
               Float32Array& uvs, Uint32Array& indices, const Bounds& bounds,
               const PolygonPoints& points, float depth, bool flip);
  void addHoles(const vector_t<Point2D>& epoints,
                const Uint32Array& holeIndices,
                vector_t<vector_t<Point2D>>& polygon);

private:
  PolygonPoints _points;
  PolygonPoints _outlinepoints;
  vector_t<PolygonPoints> _holes;

  string_t _name;
  Scene* _scene;

  vector_t<Point2D> _epoints;
  Uint32Array _eholes;

}; // end of class PolygonMeshBuilder

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_POLYGONMESH_POLYGON_MESH_BUILDER_H
