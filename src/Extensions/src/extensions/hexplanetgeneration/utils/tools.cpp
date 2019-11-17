#include <babylon/extensions/hexplanetgeneration/utils/tools.h>

#include <babylon/culling/bounding_sphere.h>
#include <babylon/culling/ray.h>
#include <babylon/math/color4.h>
#include <babylon/math/plane.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/misc/tools.h>

namespace BABYLON {
namespace Extensions {

Vector3 Tools::slerp(const Vector3& p0, const Vector3& p1, float t)
{
  float omega = std::acos(Vector3::Dot(p0, p1));
  return p0 * std::sin((1.f - t) * omega)
         + p1 * std::sin(t * omega) / std::sin(omega);
}

Vector3 Tools::randomUnitVector(IRandomFunction& random)
{
  float theta  = random.real(0, Math::PI2);
  float phi    = std::acos(random.realInclusive(-1.f, 1.f));
  float sinPhi = std::sin(phi);
  return Vector3(std::cos(theta) * sinPhi, std::sin(theta) * sinPhi,
                 std::cos(phi));
}

Quaternion Tools::randomQuaternion(IRandomFunction& random)
{
  float theta    = random.real(0, Math::PI2);
  float phi      = std::acos(random.realInclusive(-1.f, 1.f));
  float sinPhi   = std::sin(phi);
  float gamma    = random.real(0, Math::PI2);
  float sinGamma = std::sin(gamma);
  return Quaternion(std::cos(theta) * sinPhi * sinGamma,
                    std::sin(theta) * sinPhi * sinGamma,
                    std::cos(phi) * sinGamma, std::cos(gamma));
}

float Tools::angleBetween(const Vector3& v1, const Vector3& v2)
{
  auto lenProduct = v1.length() * v2.length();

  // Divide by zero check
  if (lenProduct < 1e-6f) {
    lenProduct = 1e-6f;
  }

  auto f = Vector3::Dot(v1, v2) / lenProduct;
  f      = Scalar::Clamp(f, -1.f, 1.f);
  return std::acos(f);
}

Vector3 Tools::projectOnVector(const Vector3& v1, const Vector3& v2)
{
  float ang   = angleBetween(v1, v2);
  Vector3 ret = v1 / v1.length() * std::cos(BABYLON::Tools::ToRadians(ang));
  return ret;
}

Vector3 Tools::projectOnPlane(const Vector3& v, const Vector3& rkNormal)
{
  // Create a plane (defined by normal)
  Vector3 normal(0.f, 0.f, 0.f);
  // Redefine this plane based on a normal and a point
  normal = rkNormal;
  // Project a vector onto the plane
  std::array<std::array<float, 3>, 3> m;
  m[0][0] = 1.f - normal.x * normal.x;
  m[0][1] = -normal.x * normal.y;
  m[0][2] = -normal.x * normal.z;
  m[1][0] = -normal.y * normal.x;
  m[1][1] = 1.f - normal.y * normal.y;
  m[1][2] = -normal.y * normal.z;
  m[2][0] = -normal.z * normal.x;
  m[2][1] = -normal.z * normal.y;
  m[2][2] = 1.f - normal.z * normal.z;
  // Matrix * vector [3x3 * 3x1 = 3x1]
  Vector3 kProd;
  kProd.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
  kProd.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
  kProd.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;

  return kProd;
}

bool Tools::intersectRayWithSphere(const Ray& ray, const BoundingSphere& sphere)
{
  auto v1 = sphere.center - ray.origin;
  auto v2 = projectOnVector(v1, ray.direction);
  float d = Vector3::Distance(v1, v2);
  return d <= sphere.radius;
}

Plane Tools::redefinedPlane(const Vector3& rkNormal, const Vector3& rkPoint)
{
  Vector3 normal = rkNormal;
  float d        = -Vector3::Dot(rkNormal, rkPoint);
  return Plane(normal, d);
}

Plane Tools::redefinedPlane(const Vector3& rkPoint0, const Vector3& rkPoint1,
                            const Vector3& rkPoint2)
{
  Vector3 kEdge1 = rkPoint1 - rkPoint0;
  Vector3 kEdge2 = rkPoint2 - rkPoint0;
  Vector3 normal = Vector3::Cross(kEdge1, kEdge2);
  normal.normalize();
  float d = -Vector3::Dot(normal, rkPoint0);
  return Plane(normal, d);
}

float Tools::calculateTriangleArea(const Vector3& pa, const Vector3& pb,
                                   const Vector3& pc)
{
  auto vab        = pb - pa;
  auto vac        = pc - pa;
  auto faceNormal = Vector3::Cross(vab, vac);
  auto vabNormal  = Vector3::Cross(faceNormal, vab);
  vabNormal.normalize();
  auto plane  = redefinedPlane(vabNormal, pa);
  auto height = plane.signedDistanceTo(pc);
  auto width  = vab.length();
  float area  = width * height * 0.5f;
  return area;
}

float Tools::adjustRange(float value, float oldMin, float oldMax, float newMin,
                         float newMax)
{
  return (value - oldMin) / (oldMax - oldMin) * (newMax - newMin) + newMin;
}

void Tools::setLength(Vector3& v, float len)
{
  v.normalize();
  v *= len;
}

Vector3 Tools::setLength(Vector3&& v, float len)
{
  v.normalize();
  v *= len;
  return std::move(v);
}

Color4 Tools::ocv(unsigned int val)
{
  uint32_t val32 = val;

  // Convert from 32bit pattern
  // (ARGB = 8888)

  // Alpha
  float a = ((val32 >> 24) & 0xFF) / 255.0f;

  // Red
  float r = ((val32 >> 16) & 0xFF) / 255.0f;

  // Green
  float g = ((val32 >> 8) & 0xFF) / 255.0f;

  // Blue
  float b = (val32 & 0xFF) / 255.0f;

  return Color4(r, g, b, a);
}

} // end of namespace Extensions
} // namespace BABYLON
