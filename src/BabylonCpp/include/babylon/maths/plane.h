#ifndef BABYLON_MATHS_PLANE_H
#define BABYLON_MATHS_PLANE_H

#include <array>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class Matrix;

/**
 * @brief Represents a plane by the equation ax + by + cz + d = 0
 */
class BABYLON_SHARED_EXPORT Plane {

public:
  /**
   * @brief Creates a Plane object according to the given floats a, b, c, d and the plane equation :
   * ax + by + cz + d = 0
   * @param a a component of the plane
   * @param b b component of the plane
   * @param c c component of the plane
   * @param d d component of the plane
   */
  Plane();
  Plane(Vector3 _normal, float _d);
  Plane(float a, float b, float c, float id);
  Plane(const Plane& otherPlane);
  Plane(Plane&& otherPlane);
  Plane& operator=(const Plane& otherPlane);
  Plane& operator=(Plane&& otherPlane);
  ~Plane(); // = default
  [[nodiscard]] Plane copy() const;

  /**
   * @returns a new plane copied from the current Plane.
   */
  [[nodiscard]] std::unique_ptr<Plane> clone() const;

  friend std::ostream& operator<<(std::ostream& os, const Plane& plane);

  /**
   * @returns the string "Plane".
   */
  [[nodiscard]] const char* getClassName() const;

  /**
   * @returns the Plane hash code.
   */
  [[nodiscard]] size_t getHashCode() const;

  /**
   * @returns the plane coordinates as a new array of 4 elements [a, b, c, d].
   */
  [[nodiscard]] std::array<float, 4> asArray() const;

  /** Methods **/

  /**
   * @brief Normalize the current Plane in place.
   * @returns the updated Plane.
   */
  Plane& normalize();

  /**
   * @brief Applies a transformation the plane and returns the result
   * @param transformation the transformation matrix to be applied to the plane
   * @returns a new Plane as the result of the transformation of the current
   * Plane by the given matrix.
   */
  [[nodiscard]] Plane transform(const Matrix& transformation) const;

  /**
   * @brief Compute the dot product between the point and the plane normal
   * @param point point to calculate the dot product with
   * @returns the dot product (float) of the point coordinates and the plane normal.
   */
  [[nodiscard]] float dotCoordinate(const Vector3& point) const;

  /**
   * @brief Updates the current Plane from the plane defined by the three given points.
   * @param point1 one of the points used to contruct the plane
   * @param point2 one of the points used to contruct the plane
   * @param point3 one of the points used to contruct the plane
   * @returns the updated Plane.
   */
  Plane& copyFromPoints(const Vector3& point1, const Vector3& point2, const Vector3& point3);

  /**
   * @brief Checks if the plane is facing a given direction (meaning if the plane's normal is
   * pointing in the opposite direction of the given vector). Note that for this function to work as
   * expected you should make sure that:
   *   - direction and the plane normal are normalized
   *   - epsilon is a number just bigger than -1, something like -0.99 for eg
   * @param direction the direction to check if the plane is facing
   * @param epsilon value the dot product is compared against (returns true if dot <= epsilon)
   * @returns True if the plane is facing the given direction
   */
  [[nodiscard]] bool isFrontFacingTo(const Vector3& direction, float epsilon) const;

  /**
   * @brief Calculates the distance to a point
   * @param point point to calculate distance to
   * @returns the signed distance (float) from the given point to the Plane.
   */
  [[nodiscard]] float signedDistanceTo(const Vector3& point) const;

  /** Statics **/

  /**
   * @brief Creates a plane from an  array
   * @param array the array to create a plane from
   * @returns a new Plane from the given array.
   */
  static Plane FromArray(const Float32Array& array);

  /**
   * @brief Creates a plane from three points
   * @param point1 point used to create the plane
   * @param point2 point used to create the plane
   * @param point3 point used to create the plane
   * @returns a new Plane defined by the three given points.
   */
  static Plane FromPoints(const Vector3& point1, const Vector3& point2, const Vector3& point3);

  /**
   * @brief Creates a plane from an origin point and a normal
   * @param origin origin of the plane to be constructed
   * @param normal normal of the plane to be constructed
   * @returns a new Plane the normal vector to this plane at the given origin
   * point. Note : the vector "normal" is updated because normalized.
   */
  static Plane FromPositionAndNormal(const Vector3& origin, Vector3 normal);

  /**
   * @brief Calculates the distance from a plane and a point
   * @param origin origin of the plane to be constructed
   * @param normal normal of the plane to be constructed
   * @param point point to calculate distance to
   * @returns the signed distance between the plane defined by the normal vector at the "origin""
   * point and the given other point.
   */
  static float SignedDistanceToPlaneFromPositionAndNormal(const Vector3& origin,
                                                          const Vector3& normal,
                                                          const Vector3& point);

public:
  /**
   * Normal of the plane (a,b,c)
   */
  Vector3 normal;

  /**
   * d component of the plane
   */
  float d;

}; // end of class Plane

std::ostream& operator<<(std::ostream& os, const Plane& plane);

} // end of namespace BABYLON

#endif // end of BABYLON_MATHS_PLANE_H
