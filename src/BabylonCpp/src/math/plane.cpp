#include <babylon/math/plane.h>

#include <cmath>

#include <babylon/babylon_stl_util.h>
#include <babylon/math/math_tmp.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

Plane::Plane() = default;

Plane::Plane(Vector3 _normal, float _d) : normal{_normal}, d{_d}
{
}

Plane::Plane(float a, float b, float c, float id)
    : normal{Vector3(a, b, c)}, d{id}
{
}

Plane::Plane(const Plane& otherPlane) = default;

Plane::Plane(Plane&& otherPlane) = default;

Plane& Plane::operator=(const Plane& otherPlane) = default;

Plane& Plane::operator=(Plane&& otherPlane) = default;

Plane::~Plane() = default;

Plane Plane::copy() const
{
  return Plane(*this);
}

std::unique_ptr<Plane> Plane::clone() const
{
  return std::make_unique<Plane>(*this);
}

std::ostream& operator<<(std::ostream& os, const Plane& plane)
{
  os << "{\"Normal\":" << plane.normal << ",\"D\":" << plane.d << "}";
  return os;
}

const char* Plane::getClassName() const
{
  return "Plane";
}

size_t Plane::getHashCode() const
{
  size_t hash = normal.getHashCode();
  hash        = (hash * 397) + stl_util::to_bitset(d).to_ullong();
  return hash;
}

std::array<float, 4> Plane::asArray() const
{
  return {{normal.x, normal.y, normal.z, d}};
}

// Methods
Plane& Plane::normalize()
{
  const float norm = sqrtf((normal.x * normal.x) + (normal.y * normal.y)
                           + (normal.z * normal.z));
  float magnitude  = 0.f;

  if (!stl_util::almost_equal(norm, 0.f)) {
    magnitude = 1.f / norm;
  }

  normal.x = normal.x * magnitude;
  normal.y = normal.y * magnitude;
  normal.z = normal.z * magnitude;

  d *= magnitude;

  return *this;
}

Plane Plane::transform(const Matrix& transformation) const
{
  auto& transposedMatrix = MathTmp::MatrixArray[0];
  Matrix::TransposeToRef(transformation, transposedMatrix);
  const auto& m = transposedMatrix.m();
  const auto x  = normal.x;
  const auto y  = normal.y;
  const auto z  = normal.z;

  const auto normalX = x * m[0] + y * m[1] + z * m[2] + d * m[3];
  const auto normalY = x * m[4] + y * m[5] + z * m[6] + d * m[7];
  const auto normalZ = x * m[8] + y * m[9] + z * m[10] + d * m[11];
  const auto finalD  = x * m[12] + y * m[13] + z * m[14] + d * m[15];

  return Plane(normalX, normalY, normalZ, finalD);
}

float Plane::dotCoordinate(const Vector3& point) const
{
  return ((((normal.x * point.x) + (normal.y * point.y)) + (normal.z * point.z))
          + d);
}

Plane& Plane::copyFromPoints(const Vector3& point1, const Vector3& point2,
                             const Vector3& point3)
{
  const auto x1   = point2.x - point1.x;
  const auto y1   = point2.y - point1.y;
  const auto z1   = point2.z - point1.z;
  const auto x2   = point3.x - point1.x;
  const auto y2   = point3.y - point1.y;
  const auto z2   = point3.z - point1.z;
  const auto yz   = (y1 * z2) - (z1 * y2);
  const auto xz   = (z1 * x2) - (x1 * z2);
  const auto xy   = (x1 * y2) - (y1 * x2);
  const auto pyth = sqrtf((yz * yz) + (xz * xz) + (xy * xy));
  auto invPyth    = 0.f;

  if (!stl_util::almost_equal(pyth, 0.f)) {
    invPyth = 1.f / pyth;
  }
  else {
    invPyth = 0.f;
  }

  normal.x = yz * invPyth;
  normal.y = xz * invPyth;
  normal.z = xy * invPyth;
  d = -((normal.x * point1.x) + (normal.y * point1.y) + (normal.z * point1.z));

  return *this;
}

bool Plane::isFrontFacingTo(const Vector3& direction, float epsilon) const
{
  const auto dot = Vector3::Dot(normal, direction);

  return (dot <= epsilon);
}

float Plane::signedDistanceTo(const Vector3& point) const
{
  return Vector3::Dot(point, normal) + d;
}

// Statics
Plane Plane::FromArray(const Float32Array& array)
{
  return Plane(array[0], array[1], array[2], array[3]);
}

Plane Plane::FromPoints(const Vector3& point1, const Vector3& point2,
                        const Vector3& point3)
{
  Plane result(0.f, 0.f, 0.f, 0.f);
  result.copyFromPoints(point1, point2, point3);
  return result;
}

Plane Plane::FromPositionAndNormal(const Vector3& origin, Vector3 normal)
{
  Plane result(0.f, 0.f, 0.f, 0.f);
  normal.normalize();
  result.normal = normal;
  result.d = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
  return result;
}

float Plane::SignedDistanceToPlaneFromPositionAndNormal(const Vector3& origin,
                                                        const Vector3& normal,
                                                        const Vector3& point)
{
  const auto id
    = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);

  return Vector3::Dot(point, normal) + id;
}

} // end of namespace BABYLON
