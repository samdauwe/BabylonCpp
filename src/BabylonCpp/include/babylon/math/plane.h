#ifndef BABYLON_MATH_PLANE_H
#define BABYLON_MATH_PLANE_H

#include <babylon/babylon_global.h>

#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Represents a Plane.
 */
class BABYLON_SHARED_EXPORT Plane {

public:
  Plane();
  Plane(Vector3 _normal, float _d);
  Plane(float a, float b, float c, float id);
  Plane(const Plane& otherPlane);
  Plane(Plane&& otherPlane);
  Plane& operator=(const Plane& otherPlane);
  Plane& operator=(Plane&& otherPlane);
  ~Plane();
  Plane copy() const;
  std::unique_ptr<Plane> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Plane& plane);
  const char* getClassName() const;
  size_t getHashCode() const;

  std::array<float, 4> asArray() const;

  /** Methods **/
  Plane& normalize();
  Plane transform(const Matrix& transformation) const;
  float dotCoordinate(const Vector3& point) const;
  Plane& copyFromPoints(const Vector3& point1, const Vector3& point2,
                        const Vector3& point3);
  bool isFrontFacingTo(const Vector3& direction, float epsilon) const;
  float signedDistanceTo(const Vector3& point) const;

  /** Statics **/
  static Plane FromArray(const Float32Array& array);
  static Plane FromPoints(const Vector3& point1, const Vector3& point2,
                          const Vector3& point3);
  static Plane FromPositionAndNormal(const Vector3& origin, Vector3& normal);
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
