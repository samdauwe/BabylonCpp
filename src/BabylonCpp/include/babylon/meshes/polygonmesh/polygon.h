#ifndef BABYLON_MESHES_POLYGONMESH_POLYGON_H
#define BABYLON_MESHES_POLYGONMESH_POLYGON_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Path2;
class Vector2;

/**
 * @brief Polygon
 * @see https://doc.babylonjs.com/how_to/parametric_shapes#non-regular-polygon
 */
struct BABYLON_SHARED_EXPORT Polygon {

  /**
   * @brief Creates a rectangle.
   * @param xmin bottom X coord
   * @param ymin bottom Y coord
   * @param xmax top X coord
   * @param ymax top Y coord
   * @returns points that make the resulting rectangle
   */
  static std::vector<Vector2> Rectangle(float xmin, float ymin, float xmax, float ymax);

  /**
   * @brief Creates a circle.
   * @param radius radius of circle
   * @param cx scale in x
   * @param cy scale in y
   * @param numberOfSides number of sides that make up the circle
   * @returns points that make the resulting circle
   */
  static std::vector<Vector2> Circle(float radius, float cx = 0.f, float cy = 0.f,
                                     size_t numberOfSides = 32);

  /**
   * @brief Creates a polygon from input string.
   * @param input Input polygon data
   * @returns the parsed points
   */
  static std::vector<Vector2> Parse(const std::string& input);

  /**
   * @brief Starts building a polygon from x and y coordinates.
   * @param x x coordinate
   * @param y y coordinate
   * @returns the started path2
   */
  static Path2 StartingAt(float x, float y);

}; // end of class Polygon

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_POLYGONMESH_POLYGON_H
