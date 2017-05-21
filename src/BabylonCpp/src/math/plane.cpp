#include <babylon/math/plane.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

Plane::Plane()
{
}

Plane::Plane(Vector3 _normal, float _d) : normal{_normal}, d{_d}
{
}

Plane::Plane(float a, float b, float c, float id)
    : normal{Vector3(a, b, c)}, d{id}
{
}

Plane::Plane(const Plane& otherPlane)
    : normal{otherPlane.normal}, d{otherPlane.d}
{
}

Plane::Plane(Plane&& otherPlane)
    : normal{std::move(otherPlane.normal)}, d{std::move(otherPlane.d)}
{
}

Plane& Plane::operator=(const Plane& otherPlane)
{
  if (&otherPlane != this) {
    normal = otherPlane.normal;
    d      = otherPlane.d;
  }

  return *this;
}

Plane& Plane::operator=(Plane&& otherPlane)
{
  if (&otherPlane != this) {
    normal = std::move(otherPlane.normal);
    d      = std::move(otherPlane.d);
  }

  return *this;
}

Plane::~Plane()
{
}

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
  float magnitude = 0.f;

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
  const auto transposedMatrix = Matrix::Transpose(transformation);
  const float x               = normal.x;
  const float y               = normal.y;
  const float z               = normal.z;
  const float dTemp           = d;

  const float normalX
    = (((x * transposedMatrix.m[0]) + (y * transposedMatrix.m[1]))
       + (z * transposedMatrix.m[2]))
      + (dTemp * transposedMatrix.m[3]);
  const float normalY
    = (((x * transposedMatrix.m[4]) + (y * transposedMatrix.m[5]))
       + (z * transposedMatrix.m[6]))
      + (dTemp * transposedMatrix.m[7]);
  const float normalZ
    = (((x * transposedMatrix.m[8]) + (y * transposedMatrix.m[9]))
       + (z * transposedMatrix.m[10]))
      + (dTemp * transposedMatrix.m[11]);
  const float finalD
    = (((x * transposedMatrix.m[12]) + (y * transposedMatrix.m[13]))
       + (z * transposedMatrix.m[14]))
      + (dTemp * transposedMatrix.m[15]);

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
  const float x1   = point2.x - point1.x;
  const float y1   = point2.y - point1.y;
  const float z1   = point2.z - point1.z;
  const float x2   = point3.x - point1.x;
  const float y2   = point3.y - point1.y;
  const float z2   = point3.z - point1.z;
  const float yz   = (y1 * z2) - (z1 * y2);
  const float xz   = (z1 * x2) - (x1 * z2);
  const float xy   = (x1 * y2) - (y1 * x2);
  const float pyth = sqrtf((yz * yz) + (xz * xz) + (xy * xy));
  float invPyth    = 0.f;

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
  const float dot = Vector3::Dot(normal, direction);

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

Plane Plane::FromPositionAndNormal(const Vector3& origin, Vector3& normal)
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
  const float id
    = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);

  return Vector3::Dot(point, normal) + id;
}

} // end of namespace BABYLON
