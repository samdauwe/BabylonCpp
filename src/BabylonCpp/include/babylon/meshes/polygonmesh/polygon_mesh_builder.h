#ifndef BABYLON_MESHES_POLYGONMESH_POLYGON_MESH_BUILDER_H
#define BABYLON_MESHES_POLYGONMESH_POLYGON_MESH_BUILDER_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/meshes/polygonmesh/polygon_points.h>

namespace BABYLON {

class Mesh;
class Path2;
class Scene;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief Builds a polygon
 * @see https://doc.babylonjs.com/how_to/polygonmeshbuilder
 */
class BABYLON_SHARED_EXPORT PolygonMeshBuilder {

public:
  using Point2D = std::array<float, 2>;

public:
  /**
   * @brief Creates a PolygonMeshBuilder.
   * @param name name of the builder
   * @param contours Path of the polygon
   * @param scene scene to add to
   */
  PolygonMeshBuilder(const std::string& name, const Path2& contours,
                     Scene* scene);
  PolygonMeshBuilder(const std::string& name,
                     const std::vector<Vector2>& contours, Scene* scene);
  ~PolygonMeshBuilder();

  /**
   * @brief Adds a whole within the polygon.
   * @param hole Array of points defining the hole
   * @returns this
   */
  PolygonMeshBuilder& addHole(const std::vector<Vector2>& hole);

  /**
   * @brief Creates the polygon.
   * @param updatable If the mesh should be updatable
   * @param depth The depth of the mesh created
   * @returns the created mesh
   */
  MeshPtr build(bool updatable = false, float depth = 0.f);
  std::pair<Float32Array, Uint32Array> buildWall(const Vector3& wall0Corner,
                                                 const Vector3& wall1Corner);

  PolygonPoints& points();
  const PolygonPoints& points() const;

private:
  void _addToepoint(const std::vector<Vector2>& points);

  /**
   * @brief Adds a side to the polygon.
   * @param positions points that make the polygon
   * @param normals normals of the polygon
   * @param uvs uvs of the polygon
   * @param indices indices of the polygon
   * @param bounds bounds of the polygon
   * @param points points of the polygon
   * @param depth depth of the polygon
   * @param flip flip of the polygon
   */
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

#endif // end of BABYLON_MESHES_POLYGONMESH_POLYGON_MESH_BUILDER_H
