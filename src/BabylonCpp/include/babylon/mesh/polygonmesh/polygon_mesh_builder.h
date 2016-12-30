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
  using Point2D = std::array<float, 2>;

public:
  PolygonMeshBuilder(const std::string& name, const Path2& contours,
                     Scene* scene);
  PolygonMeshBuilder(const std::string& name,
                     const std::vector<Vector2>& contours, Scene* scene);
  ~PolygonMeshBuilder();

  PolygonMeshBuilder& addHole(const std::vector<Vector2>& hole);
  Mesh* build(bool updatable = false, float depth = 0);

private:
  void _addToepoint(const std::vector<Vector2>& points);
  void addSide(Float32Array& positions, Float32Array& normals,
               Float32Array& uvs, Uint32Array& indices, const Bounds& bounds,
               const PolygonPoints& points, float depth, bool flip);

private:
  PolygonPoints _points;
  PolygonPoints _outlinepoints;
  std::vector<PolygonPoints> _holes;

  std::string _name;
  Scene* _scene;

  std::vector<Point2D> _epoints;
  Uint32Array _eholes;

}; // end of class PolygonMeshBuilder

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_POLYGONMESH_POLYGON_MESH_BUILDER_H
