#ifndef BABYLON_MATH_PATH2_H
#define BABYLON_MATH_PATH2_H

#include <memory>
#include <ostream>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Vector2;

/**
 * @brief Represents a 2D path made up of multiple 2D points.
 */
class BABYLON_SHARED_EXPORT Path2 {

public:
  /**
   * @brief Creates a Path2 object from the starting 2D coordinates x and y.
   * @param x the starting points x value
   * @param y the starting points y value
   */
  Path2(float x, float y);
  Path2(const Path2& otherPath);
  Path2(Path2&& otherPath);
  Path2& operator=(const Path2& otherPath);
  Path2& operator=(Path2&& otherPath);
  ~Path2();
  Path2 copy() const;
  std::unique_ptr<Path2> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Path2& path);

  /**
   * @brief Adds a new segment until the given coordinates (x, y) to the current
   * Path2.
   * @param x the added points x value
   * @param y the added points y value
   * @returns the updated Path2.
   */
  Path2& addLineTo(float x, float y);

  /**
   * @brief Adds _numberOfSegments_ segments according to the arc definition
   * (middle point coordinates, end point coordinates, the arc start point being
   * the current Path2 last point) to the current Path2.
   * @param midX middle point x value
   * @param midY middle point y value
   * @param endX end point x value
   * @param endY end point y value
   * @param numberOfSegments (default: 36)
   * @returns the updated Path2.
   */
  Path2& addArcTo(float midX, float midY, float endX, float endY,
                  unsigned int numberOfSegments = 36);

  /**
   * @brief Closes the Path2.
   * @returns the Path2.
   */
  Path2& close();

  /**
   * @brief Gets the sum of the distance between each sequential point in the
   * path.
   * @returns the Path2 total length (float).
   */
  float length() const;

  /**
   * @brief Gets the points which construct the path
   * @returns the Path2 internal array of points.
   */
  std::vector<Vector2>& getPoints();

  /**
   * @brief Gets the points which construct the path
   * @returns the Path2 internal array of points.
   */
  const std::vector<Vector2>& getPoints() const;

  /**
   * @brief Retreives the point at the distance aways from the starting point.
   * @param normalizedLengthPosition the length along the path to retreive the
   * point from
   * @returns a new Vector2 located at a percentage of the Path2 total length on
   * this path.
   */
  Vector2 getPointAtLengthPosition(float normalizedLengthPosition) const;

  /** Statics **/

  /**
   * @brief Creates a new path starting from an x and y position.
   * @param x starting x value
   * @param y starting y value
   * @returns a new Path2 starting at the coordinates (x, y).
   */
  static Path2 StartingAt(float x, float y);

public:
  /**
   * If the path start and end point are the same
   */
  bool closed;

private:
  std::vector<Vector2> _points;
  float _length;

}; // end of class Path2

std::ostream& operator<<(std::ostream& os, const Path2& path);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_PATH2_H
