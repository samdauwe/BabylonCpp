#ifndef BABYLON_MESH_CSG_POLYGON_H
#define BABYLON_MESH_CSG_POLYGON_H

#include <utility>

#include <babylon/babylon_api.h>
#include <babylon/mesh/csg/plane.h>

namespace BABYLON {
namespace CSG {

class Vertex;

struct BABYLON_SHARED_EXPORT PolygonOptions {
  unsigned int subMeshId;
  unsigned int meshId;
  unsigned int materialIndex;
}; //  end of struct PolygonOptions

/**
 * @brief Represents a convex polygon. The vertices used to initialize a polygon
 * must be coplanar and form a convex loop. Each convex polygon has a `shared`
 * property, which is shared between all polygons that are clones of each other
 * or were split from the same polygon. This can be used to define per-polygon
 * properties (such as surface color).
 */
class BABYLON_SHARED_EXPORT Polygon {

public:
  Polygon(const std::vector<Vertex>& vertices, const PolygonOptions& shared);
  Polygon(const Polygon& otherPolygon);
  Polygon(Polygon&& otherPolygon);
  Polygon& operator=(const Polygon& otherPolygon);
  Polygon& operator=(Polygon&& otherPolygon);
  ~Polygon();
  Polygon clone() const;
  std::unique_ptr<Polygon> cloneToNewObject() const;
  friend std::ostream& operator<<(std::ostream& os, const Polygon& polygon);
  std::string toString() const;

  void flip();

public:
  std::vector<Vertex> vertices;
  PolygonOptions shared;
  std::pair<bool, Plane> plane;

}; // end of class Polygon

} // end of namespace CSG
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_CSG_POLYGON_H
