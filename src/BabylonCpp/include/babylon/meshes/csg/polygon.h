#ifndef BABYLON_MESHES_CSG_POLYGON_H
#define BABYLON_MESHES_CSG_POLYGON_H

#include <utility>

#include <babylon/babylon_api.h>
#include <babylon/meshes/csg/plane.h>

namespace BABYLON {
namespace CSG {

class Vertex;

struct BABYLON_SHARED_EXPORT PolygonOptions {
  unsigned int subMeshId;
  unsigned int meshId;
  unsigned int materialIndex;
}; //  end of struct PolygonOptions

/**
 * @brief Represents a convex polygon. The vertices used to initialize a polygon must
 * be coplanar and form a convex loop.
 *
 * Each convex polygon has a `shared` property, which is shared between all
 * polygons that are clones of each other or were split from the same polygon.
 * This can be used to define per-polygon properties (such as surface color)
 */
class BABYLON_SHARED_EXPORT Polygon {

public:
  Polygon();

  /**
   * @brief Initializes the polygon.
   * @param vertices The vertices of the polygon
   * @param shared The properties shared across all polygons
   */
  Polygon(const std::vector<Vertex>& vertices, const PolygonOptions& shared);
  Polygon(const Polygon& otherPolygon);
  Polygon(Polygon&& otherPolygon);
  Polygon& operator=(const Polygon& otherPolygon);
  Polygon& operator=(Polygon&& otherPolygon);
  ~Polygon(); // = default

  /**
   * @brief Clones, or makes a deep copy, or the polygon.
   */
  Polygon clone() const;

  std::unique_ptr<Polygon> cloneToNewObject() const;
  friend std::ostream& operator<<(std::ostream& os, const Polygon& polygon);
  std::string toString() const;

  /**
   * @brief Flips the faces of the polygon.
   */
  void flip();

public:
  /**
   * Vertices of the polygon
   */
  std::vector<Vertex> vertices;

  /**
   * Properties that are shared across all polygons
   */
  PolygonOptions shared;

  /**
   * A plane formed from the vertices of the polygon
   */
  std::optional<Plane> plane;

}; // end of class Polygon

} // end of namespace CSG
} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_CSG_POLYGON_H
