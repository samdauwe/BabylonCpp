#include <babylon/culling/bounding_sphere.h>

#include <cmath>

#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>
#include <babylon/math/tmp.h>

namespace BABYLON {

BoundingSphere::BoundingSphere(const Vector3& min, const Vector3& max)
    : center{Vector3::Zero()}
    , radius{0.f}
    , centerWorld{Vector3::Zero()}
    , radiusWorld{0.f}
    , minimum{Vector3::Zero()}
    , maximum{Vector3::Zero()}
    , _identityMatrix{Matrix::Identity()}
    , _tempRadiusVector{Vector3::Zero()}
{
  reConstruct(min, max);
}

BoundingSphere::BoundingSphere(const BoundingSphere& other)
    : center{other.center}
    , radius{other.radius}
    , centerWorld{other.centerWorld}
    , radiusWorld{other.radiusWorld}
    , minimum{other.minimum}
    , maximum{other.maximum}
    , _identityMatrix{other._identityMatrix}
    , _tempRadiusVector{other._tempRadiusVector}
{
}

BoundingSphere::BoundingSphere(BoundingSphere&& other)
    : center{std::move(other.center)}
    , radius{std::move(other.radius)}
    , centerWorld{std::move(other.centerWorld)}
    , radiusWorld{std::move(other.radiusWorld)}
    , minimum{std::move(other.minimum)}
    , maximum{std::move(other.maximum)}
    , _identityMatrix{std::move(other._identityMatrix)}
    , _tempRadiusVector{std::move(other._tempRadiusVector)}
{
}

BoundingSphere& BoundingSphere::operator=(const BoundingSphere& other)
{
  if (&other != this) {
    center            = other.center;
    radius            = other.radius;
    centerWorld       = other.centerWorld;
    radiusWorld       = other.radiusWorld;
    minimum           = other.minimum;
    maximum           = other.maximum;
    _identityMatrix   = other._identityMatrix;
    _tempRadiusVector = other._tempRadiusVector;
  }

  return *this;
}

BoundingSphere& BoundingSphere::operator=(BoundingSphere&& other)
{
  if (&other != this) {
    center            = std::move(other.center);
    radius            = std::move(other.radius);
    centerWorld       = std::move(other.centerWorld);
    radiusWorld       = std::move(other.radiusWorld);
    minimum           = std::move(other.minimum);
    maximum           = std::move(other.maximum);
    _identityMatrix   = std::move(other._identityMatrix);
    _tempRadiusVector = std::move(other._tempRadiusVector);
  }

  return *this;
}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::reConstruct(const Vector3& min, const Vector3& max)
{
  minimum.copyFrom(min);
  maximum.copyFrom(max);

  auto distance = Vector3::Distance(min, max);

  Vector3::LerpToRef(min, max, 0.5f, center);
  radius = distance * 0.5f;

  centerWorld.set(0.f, 0.f, 0.f);
  _update(_identityMatrix);
}

BoundingSphere& BoundingSphere::scale(float factor)
{
  const auto newRadius = radius * factor;
  const auto tempRadiusVector
    = Tmp::Vector3Array[0].set(newRadius, newRadius, newRadius);
  const auto min
    = Tmp::Vector3Array[1].copyFrom(center).subtractInPlace(tempRadiusVector);
  const auto max
    = Tmp::Vector3Array[2].copyFrom(center).addInPlace(tempRadiusVector);

  reConstruct(min, max);

  return *this;
}

// Methods
void BoundingSphere::_update(const Matrix& world)
{
  Vector3::TransformCoordinatesToRef(center, world, centerWorld);
  auto& tempVector = Tmp::Vector3Array[0];
  Vector3::TransformNormalFromFloatsToRef(1.f, 1.f, 1.f, world, tempVector);
  radiusWorld
    = std::max(std::abs(tempVector.x),
               std::max(std::abs(tempVector.y), std::abs(tempVector.z)))
      * radius;
}

bool BoundingSphere::isInFrustum(
  const std::array<Plane, 6>& frustumPlanes) const
{
  for (unsigned int i = 0; i < 6; ++i) {
    if (frustumPlanes[i].dotCoordinate(centerWorld) <= -radiusWorld) {
      return false;
    }
  }

  return true;
}

bool BoundingSphere::intersectsPoint(const Vector3& point)
{
  const auto x = centerWorld.x - point.x;
  const auto y = centerWorld.y - point.y;
  const auto z = centerWorld.z - point.z;

  const auto distance = std::sqrt((x * x) + (y * y) + (z * z));

  if (radiusWorld < distance) {
    return false;
  }

  return true;
}

bool BoundingSphere::Intersects(const BoundingSphere& sphere0,
                                const BoundingSphere& sphere1)
{
  const auto x = sphere0.centerWorld.x - sphere1.centerWorld.x;
  const auto y = sphere0.centerWorld.y - sphere1.centerWorld.y;
  const auto z = sphere0.centerWorld.z - sphere1.centerWorld.z;

  const auto distance = std::sqrt((x * x) + (y * y) + (z * z));

  if (sphere0.radiusWorld + sphere1.radiusWorld < distance) {
    return false;
  }

  return true;
}

} // end of namespace BABYLON
