#ifndef BABYLON_MATH_PLANE_H
#define BABYLON_MATH_PLANE_H

#include <array>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Matrix;

/**
 * @brief Represents a Plane.
 */
class BABYLON_SHARED_EXPORT Plane {

public:
  /**
   * @brief Creates a Plane object according to the given floats a, b, c, d and
   * the plane equation : ax + by + cz + d = 0
   */
  Plane();
  Plane(Vector3 _normal, float _d);
  Plane(float a, float b, float c, float id);
  Plane(const Plane& otherPlane);
  Plane(Plane&& otherPlane);
  Plane& operator=(const Plane& otherPlane);
  Plane& operator=(Plane&& otherPlane);
  ~Plane();
  Plane copy() const;

  /**
   * @brief Returns a new plane copied from the current Plane.
   */
  std::unique_ptr<Plane> clone() const;

  friend std::ostream& operator<<(std::ostream& os, const Plane& plane);

  /**
   * @brief Returns the string "Plane".
   */
  const char* getClassName() const;

  /**
   * @brief Returns the Plane hash code.
   */
  size_t getHashCode() const;

  /**
   * @brief Returns the plane coordinates as a new array of 4 elements [a, b, c,
   * d].
   */
  std::array<float, 4> asArray() const;

  /** Methods **/

  /**
   * @brief Normalize the current Plane in place.
   * @returns The updated Plane.
   */
  Plane& normalize();

  /**
   * @brief Returns a new Plane as the result of the transformation of the
   * current Plane by the given matrix.
   */
  Plane transform(const Matrix& transformation) const;

  /**
   * @brief Returns the dot product (float) of the point coordinates and the
   * plane normal.
   */
  float dotCoordinate(const Vector3& point) const;

  /**
   * @brief Updates the current Plane from the plane defined by the three given
   * points. Returns the updated Plane.
   */
  Plane& copyFromPoints(const Vector3& point1, const Vector3& point2,
                        const Vector3& point3);

  /**
   * @brief Returns is the vector "direction"  is the same side than the plane
   * normal.
   */
  bool isFrontFacingTo(const Vector3& direction, float epsilon) const;

  /**
   * @brief Returns the signed distance (float) from the given point to the
   * Plane.
   */
  float signedDistanceTo(const Vector3& point) const;

  /** Statics **/

  /**
   * @brief Returns a new Plane from the given array.
   */
  static Plane FromArray(const Float32Array& array);

  /**
   * @brief Returns a new Plane defined by the three given points.
   */
  static Plane FromPoints(const Vector3& point1, const Vector3& point2,
                          const Vector3& point3);

  /**
   * @brief Returns a new Plane the normal vector to this plane at the given
   * origin point. Note : the vector "normal" is updated because normalized.
   */
  static Plane FromPositionAndNormal(const Vector3& origin, Vector3& normal);

  /**
   * @brief Returns the signed distance between the plane defined by the normal
   * vector at the "origin"" point and the given other point.
   */
  static float SignedDistanceToPlaneFromPositionAndNormal(const Vector3& origin,
                                                          const Vector3& normal,
                                                          const Vector3& point);

public:
  Vector3 normal;
  float d;

}; // end of class Plane

std::ostream& operator<<(std::ostream& os, const Plane& plane);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_PLANE_H
