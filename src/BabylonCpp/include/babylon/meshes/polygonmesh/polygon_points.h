#ifndef BABYLON_MESHES_POLYGONMESH_POLYGON_POINTS_H
#define BABYLON_MESHES_POLYGONMESH_POLYGON_POINTS_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/meshes/polygonmesh/indexed_vector2.h>

namespace BABYLON {

/**
 * @brief Defines points to create a polygon.
 */
struct BABYLON_SHARED_EXPORT PolygonPoints {

  PolygonPoints();
  ~PolygonPoints(); // = default

  std::vector<IndexedVector2> add(const std::vector<Vector2>& originalPoints);
  Bounds computeBounds();

  std::vector<IndexedVector2> elements;

}; // end of struct PolygonPoints

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_POLYGONMESH_POLYGON_POINTS_H
