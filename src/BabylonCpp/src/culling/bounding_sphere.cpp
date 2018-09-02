#include <babylon/culling/bounding_sphere.h>

#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

BoundingSphere::BoundingSphere(const Vector3& min, const Vector3& max)
    : center{Vector3::Zero()}
    , radius{0.f}
    , centerWorld{Vector3::Zero()}
    , radiusWorld{0.f}
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
    : center{::std::move(other.center)}
    , radius{::std::move(other.radius)}
    , centerWorld{::std::move(other.centerWorld)}
    , radiusWorld{::std::move(other.radiusWorld)}
    , minimum{::std::move(other.minimum)}
    , maximum{::std::move(other.maximum)}
    , _identityMatrix{::std::move(other._identityMatrix)}
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
    _identityMatrix   = other._identityMatrix;
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
    _identityMatrix   = ::std::move(other._identityMatrix);
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

  Vector3::LerpToRef(min, max, 0.5f, center);
  radius = distance * 0.5f;

  centerWorld.set(0.f, 0.f, 0.f);
  _update(_identityMatrix);
}

BoundingSphere& BoundingSphere::scale(float factor)
{
  auto newRadius = radius * factor;
  _tempRadiusVector.set(newRadius, newRadius, newRadius);

  auto min = center.subtract(_tempRadiusVector);
  auto max = center.add(_tempRadiusVector);

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
