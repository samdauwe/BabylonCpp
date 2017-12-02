#ifndef BABYLON_MESH_CSG_POLYGON_H
#define BABYLON_MESH_CSG_POLYGON_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/mesh/csg/plane.h>

namespace BABYLON {
namespace CSG {

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
  Polygon(const vector_t<Vertex>& vertices, const PolygonOptions& shared);
  Polygon(const Polygon& otherPolygon);
  Polygon(Polygon&& otherPolygon);
  Polygon& operator=(const Polygon& otherPolygon);
  Polygon& operator=(Polygon&& otherPolygon);
  ~Polygon();
  Polygon clone() const;
  unique_ptr_t<Polygon> cloneToNewObject() const;
  friend std::ostream& operator<<(std::ostream& os, const Polygon& polygon);
  string_t toString() const;

  void flip();

public:
  vector_t<Vertex> vertices;
  PolygonOptions shared;
  Nullable<Plane> plane;

}; // end of class Polygon

} // end of namespace CSG
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_CSG_POLYGON_H
