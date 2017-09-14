#ifndef BABYLON_MESH_POLYGONMESH_POLYGON_H
#define BABYLON_MESH_POLYGONMESH_POLYGON_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
struct BABYLON_SHARED_EXPORT Polygon {

  static vector_t<Vector2> Rectangle(float xmin, float ymin, float xmax,
                                     float ymax);
  static vector_t<Vector2> Circle(float radius, float cx = 0.f, float cy = 0.f,
                                  size_t numberOfSides = 32);
  static vector_t<Vector2> Parse(const string_t& input);
  static Path2 StartingAt(float x, float y);

}; // end of class Polygon

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_POLYGONMESH_POLYGON_H
