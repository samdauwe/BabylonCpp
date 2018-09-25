#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_TOOLS_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_TOOLS_H

#include <algorithm>

#include <babylon/babylon_api.h>
#include <babylon/extensions/hexplanetgeneration/utils/irandom_function.h>

namespace BABYLON {

class BoundingSphere;
class Color4;
class Plane;
class Quaternion;
class Ray;
class Vector3;

namespace Extensions {

template <typename T>
typename T::iterator getAdvancedIt(T& value, size_t off)
{
  typename T::iterator it = value.begin();
  std::advance(it, off);
  return it;
}

template <typename T>
typename T::value_type& getAdvancedItVal(T& value, size_t off)
{
  auto it = value.begin();
  std::advance(it, off);
  return *it;
}

template <typename T, typename F>
inline bool findIndex(const T& container, const F& value, size_t& idx)
{
  auto it = std::find(container.begin(), container.end(), value);
  if (it == container.end())
    return false;
  idx = std::distance(container.begin(), it);
  return true;
}

template <typename T, typename F>
inline bool removeIfFindIndex(T& container, const F& value)
{
  auto it = std::find(container.begin(), container.end(), value);
  if (it == container.end())
    return false;
  // container.remove(it);
  container.erase(it);
  return true;
}

template <typename T, typename F>
inline T lerp(const T& from, const T& to, const F f)
{
  return from + (to - from) * f;
}

struct Tools {
  static Vector3 slerp(const Vector3& p0, const Vector3& p1, float t);
  static Vector3 randomUnitVector(IRandomFunction& random);
  static Quaternion randomQuaternion(IRandomFunction& random);
  static float angleBetween(const Vector3& v1, const Vector3& v2);
  static Vector3 projectOnVector(const Vector3& v1, const Vector3& v2);
  static Vector3 projectOnPlane(const Vector3& v, const Vector3& rkNormal);
  static Plane redefinedPlane(const Vector3& rkNormal, const Vector3& rkPoint);
  static Plane redefinedPlane(const Vector3& rkPoint0, const Vector3& rkPoint1,
                              const Vector3& rkPoint2);
  static bool intersectRayWithSphere(const Ray& ray,
                                     const BoundingSphere& sphere);
  static float calculateTriangleArea(const Vector3& pa, const Vector3& pb,
                                     const Vector3& pc);
  static float adjustRange(float value, float oldMin, float oldMax,
                           float newMin, float newMax);
  static void setLength(Vector3& v, float len);
  static Vector3 setLength(Vector3&& v, float len);
  static Color4 ocv(unsigned int rgba = 0xFF000000);
}; // end of struct Tools

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_TOOLS_H
