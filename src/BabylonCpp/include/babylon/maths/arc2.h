#ifndef BABYLON_MATHS_ARC2_H
#define BABYLON_MATHS_ARC2_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>
#include <babylon/maths/angle.h>
#include <babylon/maths/vector2.h>

namespace BABYLON {

/**
 * @brief This represents an arc in a 2d space.
 */
class BABYLON_SHARED_EXPORT Arc2 {

public:
  /**
   * @brief Creates an Arc object from the three given points : start, middle
   * and end.
   * @param startPoint Defines the start point of the arc
   * @param midPoint Defines the midlle point of the arc
   * @param endPoint Defines the end point of the arc
   */
  Arc2(const Vector2& startPoint, const Vector2& midPoint, const Vector2& endPoint);
  Arc2(const Arc2& otherArc);
  Arc2(Arc2&& otherArc);
  Arc2& operator=(const Arc2& otherArc);
  Arc2& operator=(Arc2&& otherArc);
  ~Arc2(); // = default
  [[nodiscard]] Arc2 copy() const;
  [[nodiscard]] std::unique_ptr<Arc2> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Arc2& arc);
  friend bool operator==(const Arc2& lhs, const Arc2& rhs);
  friend bool operator!=(const Arc2& lhs, const Arc2& rhs);

public:
  /**
   * Defines the start point of the arc
   */
  Vector2 startPoint;

  /**
   * Defines the mid point of the arc
   */
  Vector2 midPoint;

  /**
   * Defines the end point of the arc
   */
  Vector2 endPoint;

  /**
   * Defines the center point of the arc.
   */
  Vector2 centerPoint;

  /**
   * Defines the radius of the arc.
   */
  float radius;

  /**
   * Defines the angle of the arc (from mid point to end point).
   */
  Angle angle;

  /**
   * Defines the start angle of the arc (from start point to middle point).
   */
  Angle startAngle;

  /**
   * Defines the orientation of the arc (clock wise/counter clock wise).
   */
  Orientation orientation;

}; // end of class Arc2

std::ostream& operator<<(std::ostream& os, const Arc2& arc);
bool operator==(const Arc2& lhs, const Arc2& rhs);
bool operator!=(const Arc2& lhs, const Arc2& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MATHS_ARC2_H
