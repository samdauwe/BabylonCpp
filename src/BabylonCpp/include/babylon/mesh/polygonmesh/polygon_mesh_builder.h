#ifndef BABYLON_MESH_POLYGONMESH_POLYGON_MESH_BUILDER_H
#define BABYLON_MESH_POLYGONMESH_POLYGON_MESH_BUILDER_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/mesh/polygonmesh/polygon_points.h>

namespace BABYLON {

class Mesh;
class Path2;
class Scene;
using MeshPtr = std::shared_ptr<Mesh>;

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
  MeshPtr build(bool updatable = false, float depth = 0.f);
  std::pair<Float32Array, Uint32Array> buildWall(const Vector3& wall0Corner,
                                                   const Vector3& wall1Corner);

  PolygonPoints& points();
  const PolygonPoints& points() const;

private:
  void _addToepoint(const std::vector<Vector2>& points);
  void addSide(Float32Array& positions, Float32Array& normals,
               Float32Array& uvs, Uint32Array& indices, const Bounds& bounds,
               const PolygonPoints& points, float depth, bool flip);
  void addHoles(const std::vector<Point2D>& epoints,
                const Uint32Array& holeIndices,
                std::vector<std::vector<Point2D>>& polygon);

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
