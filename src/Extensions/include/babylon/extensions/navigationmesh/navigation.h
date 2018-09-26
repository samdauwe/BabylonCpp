#ifndef BABYLON_EXTENSIONS_NAVIGATION_MESH_NAVIGATION_H
#define BABYLON_EXTENSIONS_NAVIGATION_MESH_NAVIGATION_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/extensions/navigationmesh/navigation_structs.h>

namespace BABYLON {

class Geometry;
class Mesh;

namespace Extensions {

/**
 * @brief Navigation mesh toolkit.
 *
 * Helps your AI agents navigate around your world. It uses the A* and Funnel
 * algorithms to calculate a path on a navigation mesh.
 *
 * Based on the Javascript implementation of Babylon-navigation-mesh:
 * https://github.com/wanadev/babylon-navigation-mesh
 */
class BABYLON_SHARED_EXPORT Navigation {

public:
  Navigation();
  ~Navigation();

  GroupedNavigationMesh buildNodes(Mesh* mesh);
  void setZoneData(const std::string& zone, const GroupedNavigationMesh& data);
  int getGroup(const std::string& zone, const Vector3& position);
  std::vector<Vector3> findPath(const Vector3& startPosition,
                                const Vector3& targetPosition,
                                const std::string& zone, std::size_t group);

private:
  bool _isPointInPoly(const std::vector<Vector3>& poly,
                      const Vector3& pt) const;
  bool _isVectorInPolygon(const Vector3& vector, const NavigationGroup& polyon,
                          const Float32Array& vertices);
  void _computeCentroids(Geometry* geometry);
  float _roundNumber(float number, unsigned int decimals);
  void _setPolygonCentroid(NavigationPolygon& polygon,
                           const NavigationMesh& navigationMesh);
  Vector3 getVectorFrom(const Float32Array& vertices, unsigned int id);
  std::vector<std::vector<NavigationPolygon>>
  _buildPolygonGroups(NavigationMesh& navigationMesh);
  Uint32Array _array_intersect(const Uint32Array& array1,
                               const Uint32Array& array2);
  void _buildPolygonNeighbours(NavigationPolygon& polygon,
                               NavigationMesh& navigationMesh);
  NavigationMesh _buildPolygonsFromGeometry(Geometry* geometry);
  NavigationMesh _buildNavigationMesh(Geometry* geometry);
  size_t _mergeVertices(Geometry* geometry);
  Uint32Array _getSharedVerticesInOrder(NavigationPolygon& a,
                                        NavigationPolygon& b);
  GroupedNavigationMesh _groupNavMesh(NavigationMesh& navigationMesh);

private:
  std::unordered_map<std::string, GroupedNavigationMesh> _zoneNodes;

}; // end of class Navigation

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NAVIGATION_MESH_NAVIGATION_H
