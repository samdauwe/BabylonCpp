#ifndef BABYLON_MATH_ANGLE_H
#define BABYLON_MATH_ANGLE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents an Angle.
 */
class BABYLON_SHARED_EXPORT Angle {

public:
  Angle();
  Angle(float radians);
  Angle(const Angle& otherAngle);
  Angle(Angle&& otherAngle);
  Angle& operator=(const Angle& otherAngle);
  Angle& operator=(Angle&& otherAngle);
  ~Angle();
  Angle copy() const;
  std::unique_ptr<Angle> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Angle& angle);

  float degrees() const;
  float radians() const;

  /** Statics **/
  static Angle BetweenTwoPoints(const Vector2& a, const Vector2& b);
  static Angle FromRadians(float radians);
  static Angle FromDegrees(float degrees);

private:
  float _radians;

}; // end of class Angle

std::ostream& operator<<(std::ostream& os, const Angle& angle);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_ANGLE_H
