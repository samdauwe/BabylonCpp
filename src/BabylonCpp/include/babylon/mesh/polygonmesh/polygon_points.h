#ifndef BABYLON_MESH_POLYGONMESH_POLYGON_POINTS_H
#define BABYLON_MESH_POLYGONMESH_POLYGON_POINTS_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/mesh/polygonmesh/indexed_vector2.h>

namespace BABYLON {

/**
 * @brief
 */
struct BABYLON_SHARED_EXPORT PolygonPoints {

  PolygonPoints();
  ~PolygonPoints();

  std::vector<IndexedVector2> add(const std::vector<Vector2>& originalPoints);
  Bounds computeBounds();

  std::vector<IndexedVector2> elements;

}; // end of struct PolygonPoints

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_POLYGONMESH_POLYGON_POINTS_H
