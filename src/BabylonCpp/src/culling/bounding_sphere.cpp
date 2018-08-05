#include <babylon/culling/bounding_sphere.h>

#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

BoundingSphere::BoundingSphere(const Vector3& min, const Vector3& max)
    : center{Vector3::Lerp(minimum, maximum, 0.5f)}
    , radius{Vector3::Distance(minimum, maximum) * 0.5f}
    , centerWorld{Vector3::Zero()}
    , radiusWorld{0.f}
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
    , _tempRadiusVector{other._tempRadiusVector}
{
}

BoundingSphere::BoundingSphere(BoundingSphere&& other)
    : center{::std::move(other.center)}
    , radius{::std::move(other.radius)}
    , centerWorld{::std::move(other.centerWorld)}
    , radiusWorld{::std::move(other.radiusWorld)}
    , minimum{::std::move(other.minimum)}
    , maximum{::std::move(other.maximum)}
    , _tempRadiusVector{::std::move(other._tempRadiusVector)}
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
    _tempRadiusVector = other._tempRadiusVector;
  }

  return *this;
}

BoundingSphere& BoundingSphere::operator=(BoundingSphere&& other)
{
  if (&other != this) {
    center            = ::std::move(other.center);
    radius            = ::std::move(other.radius);
    centerWorld       = ::std::move(other.centerWorld);
    radiusWorld       = ::std::move(other.radiusWorld);
    minimum           = ::std::move(other.minimum);
    maximum           = ::std::move(other.maximum);
    _tempRadiusVector = ::std::move(other._tempRadiusVector);
  }

  return *this;
}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::reConstruct(const Vector3& min, const Vector3& max)
{
  minimum = min;
  maximum = max;

  auto distance = Vector3::Distance(min, max);

  center = Vector3::Lerp(min, max, 0.5f);
  radius = distance * 0.5f;

  centerWorld = Vector3::Zero();
  _update(Matrix::Identity());
}

BoundingSphere& BoundingSphere::scale(float factor)
{
  auto newRadius = radius * factor;
  Vector3 newRadiusVector(newRadius, newRadius, newRadius);

  auto min = center.subtract(newRadiusVector);
  auto max = center.add(newRadiusVector);

  reConstruct(min, max);

  return *this;
}

// Methods
void BoundingSphere::_update(const Matrix& world)
{
  Vector3::TransformCoordinatesToRef(center, world, centerWorld);
  Vector3::TransformNormalFromFloatsToRef(1.f, 1.f, 1.f, world,
                                          _tempRadiusVector);

  radiusWorld
    = static_cast<float>(::std::max(::std::max(::std::abs(_tempRadiusVector.x),
                                               ::std::abs(_tempRadiusVector.y)),
                                    ::std::abs(_tempRadiusVector.z))
                         * radius);
}

bool BoundingSphere::isInFrustum(const array_t<Plane, 6>& frustumPlanes) const
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
  const float x = centerWorld.x - point.x;
  const float y = centerWorld.y - point.y;
  const float z = centerWorld.z - point.z;

  const float distance = ::std::sqrt((x * x) + (y * y) + (z * z));

  if (radiusWorld < distance) {
    return false;
  }

  return true;
}

bool BoundingSphere::Intersects(const BoundingSphere& sphere0,
                                const BoundingSphere& sphere1)
{
  const float x = sphere0.centerWorld.x - sphere1.centerWorld.x;
  const float y = sphere0.centerWorld.y - sphere1.centerWorld.y;
  const float z = sphere0.centerWorld.z - sphere1.centerWorld.z;

  const float distance = ::std::sqrt((x * x) + (y * y) + (z * z));

  if (sphere0.radiusWorld + sphere1.radiusWorld < distance) {
    return false;
  }

  return true;
}

} // end of namespace BABYLON
