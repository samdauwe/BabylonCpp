#include <babylon/culling/bounding_sphere.h>

#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

BoundingSphere::BoundingSphere(const Vector3& minimum, const Vector3& maximum)
    : center{Vector3::Lerp(minimum, maximum, 0.5f)}
    , radius{Vector3::Distance(minimum, maximum) * 0.5f}
    , centerWorld{Vector3::Zero()}
    , radiusWorld{0.f}
    , _tempRadiusVector{Vector3::Zero()}
{
  _update(Matrix::Identity());
}

BoundingSphere::BoundingSphere(const BoundingSphere& other)
    : center{other.center}
    , radius{other.radius}
    , centerWorld{other.centerWorld}
    , radiusWorld{other.radiusWorld}
    , _tempRadiusVector{other._tempRadiusVector}
{
}

BoundingSphere::BoundingSphere(BoundingSphere&& other)
    : center{::std::move(other.center)}
    , radius{::std::move(other.radius)}
    , centerWorld{::std::move(other.centerWorld)}
    , radiusWorld{::std::move(other.radiusWorld)}
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
    _tempRadiusVector = ::std::move(other._tempRadiusVector);
  }

  return *this;
}

BoundingSphere::~BoundingSphere()
{
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
  float x = centerWorld.x - point.x;
  float y = centerWorld.y - point.y;
  float z = centerWorld.z - point.z;

  float distance = ::std::sqrt((x * x) + (y * y) + (z * z));

  if (::std::abs(radiusWorld - distance) < Math::Epsilon) {
    return false;
  }

  return true;
}

bool BoundingSphere::Intersects(const BoundingSphere& sphere0,
                                const BoundingSphere& sphere1)
{
  float x = sphere0.centerWorld.x - sphere1.centerWorld.x;
  float y = sphere0.centerWorld.y - sphere1.centerWorld.y;
  float z = sphere0.centerWorld.z - sphere1.centerWorld.z;

  float distance = ::std::sqrt((x * x) + (y * y) + (z * z));

  if (sphere0.radiusWorld + sphere1.radiusWorld < distance) {
    return false;
  }

  return true;
}

} // end of namespace BABYLON
