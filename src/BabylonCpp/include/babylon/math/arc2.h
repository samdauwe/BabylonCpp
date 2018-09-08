#ifndef BABYLON_MATH_ARC2_H
#define BABYLON_MATH_ARC2_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>
#include <babylon/math/angle.h>
#include <babylon/math/vector2.h>

namespace BABYLON {

/**
 * @brief Represents an Arc2.
 */
class BABYLON_SHARED_EXPORT Arc2 {

public:
  /**
   * @brief Creates an Arc object from the three given points : start, middle
   * and end.
   */
  Arc2(const Vector2& startPoint, const Vector2& midPoint,
       const Vector2& endPoint);
  Arc2(const Arc2& otherArc);
  Arc2(Arc2&& otherArc);
  Arc2& operator=(const Arc2& otherArc);
  Arc2& operator=(Arc2&& otherArc);
  ~Arc2();
  Arc2 copy() const;
  std::unique_ptr<Arc2> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Arc2& arc);
  friend bool operator==(const Arc2& lhs, const Arc2& rhs);
  friend bool operator!=(const Arc2& lhs, const Arc2& rhs);

public:
  Vector2 centerPoint;
  float radius;
  Angle angle, startAngle;
  Orientation orientation;

}; // end of class Arc2

std::ostream& operator<<(std::ostream& os, const Arc2& arc);
bool operator==(const Arc2& lhs, const Arc2& rhs);
bool operator!=(const Arc2& lhs, const Arc2& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_ARC2_H
