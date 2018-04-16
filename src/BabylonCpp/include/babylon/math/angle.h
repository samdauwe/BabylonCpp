#ifndef BABYLON_MATH_ANGLE_H
#define BABYLON_MATH_ANGLE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Defines angle representation.
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
  unique_ptr_t<Angle> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Angle& angle);
  friend bool operator==(const Angle& lhs, const Angle& rhs);
  friend bool operator!=(const Angle& lhs, const Angle& rhs);

  /**
   * @brief Get value in degrees.
   * @returns the Angle value in degrees (float)
   */
  float degrees() const;

  /**
   * @brief Get value in radians.
   * @returns the Angle value in radians (float)
   */
  float radians() const;

  /** Statics **/

  /**
   * @brief Gets a new Angle object valued with the angle value in radians.
   * between the two given vectors
   * @param a defines first vector
   * @param b defines second vector
   * @returns a new Angle
   */
  static Angle BetweenTwoPoints(const Vector2& a, const Vector2& b);

  /**
   * @brief Gets a new Angle object from the given float in radians.
   * @param radians defines the angle value in radians
   * @returns a new Angle
   */
  static Angle FromRadians(float radians);

  /**
   * @brief Gets a new Angle object from the given float in degrees.
   * @param degrees defines the angle value in degrees
   * @returns a new Angle
   */
  static Angle FromDegrees(float degrees);

private:
  float _radians;

}; // end of class Angle

std::ostream& operator<<(std::ostream& os, const Angle& angle);
bool operator==(const Angle& lhs, const Angle& rhs);
bool operator!=(const Angle& lhs, const Angle& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_ANGLE_H
