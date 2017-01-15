#include <babylon/extensions/hexplanetgeneration/utils/tools.h>

#include <babylon/math/color3.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/tools.h>

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

float Tools::angleBetween(const Vector3& /*v1*/, const Vector3& /*v2*/)
{
  return 0.f;
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

bool Tools::intersectRayWithSphere(const Ray& /*ray*/,
                                   const BoundingSphere& /*sphere*/)
{
  return false;
}

float Tools::calculateTriangleArea(const Vector3& /*pa*/, const Vector3& /*pb*/,
                                   const Vector3& /*pc*/)
{
  return 0.f;
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
  return v;
}

Color3 Tools::ocv(unsigned int /*rgb = 0xFF000000*/)
{
  Color3 ret;
  return ret;
}

} // end of namespace Extensions
} // end of namespace babylon
