#ifndef BABYLON_MATH_ANGLE_H
#define BABYLON_MATH_ANGLE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents an Angle.
 */
class BABYLON_SHARED_EXPORT Angle {

public:
  /**
   * @brief Creates an Angle object of "radians" radians (float).
   */
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
  friend bool operator==(const Angle& lhs, const Angle& rhs);
  friend bool operator!=(const Angle& lhs, const Angle& rhs);

  /**
   * @brief Returns the Angle value in degrees (float).
   */
  float degrees() const;

  /**
   * @brief Returns the Angle value in radians (float).
   */
  float radians() const;

  /** Statics **/

  /**
   * @brief Returns a new Angle object valued with the angle value in radians
   * between the two passed vectors.
   */
  static Angle BetweenTwoPoints(const Vector2& a, const Vector2& b);

  /**
   * @brief Returns a new Angle object from the passed float in radians.
   */
  static Angle FromRadians(float radians);

  /**
   * @brief Returns a new Angle object from the passed float in degrees.
   */
  static Angle FromDegrees(float degrees);

private:
  float _radians;

}; // end of class Angle

std::ostream& operator<<(std::ostream& os, const Angle& angle);
inline bool operator==(const Angle& lhs, const Angle& rhs);
inline bool operator!=(const Angle& lhs, const Angle& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_ANGLE_H
