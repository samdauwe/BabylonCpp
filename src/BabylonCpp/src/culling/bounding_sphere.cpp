#include <babylon/culling/bounding_sphere.h>

#include <cmath>

#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

std::array<Vector3, 3> BoundingSphere::TmpVector3{
  Vector3::Zero(), Vector3::Zero(), Vector3::Zero()};

BoundingSphere::BoundingSphere(const Vector3& min, const Vector3& max,
                               const std::optional<Matrix>& worldMatrix)
    : center{Vector3::Zero()}
    , radius{0.f}
    , centerWorld{Vector3::Zero()}
    , radiusWorld{0.f}
    , minimum{Vector3::Zero()}
    , maximum{Vector3::Zero()}
    , _worldMatrix{Matrix::Identity()}
{
  reConstruct(min, max, worldMatrix);
}

BoundingSphere::BoundingSphere(const BoundingSphere& other)
    : center{other.center}
    , radius{other.radius}
    , centerWorld{other.centerWorld}
    , radiusWorld{other.radiusWorld}
    , minimum{other.minimum}
    , maximum{other.maximum}
    , _worldMatrix{other._worldMatrix}
{
}

BoundingSphere::BoundingSphere(BoundingSphere&& other)
    : center{std::move(other.center)}
    , radius{std::move(other.radius)}
    , centerWorld{std::move(other.centerWorld)}
    , radiusWorld{std::move(other.radiusWorld)}
    , minimum{std::move(other.minimum)}
    , maximum{std::move(other.maximum)}
    , _worldMatrix{std::move(other._worldMatrix)}
{
}

BoundingSphere& BoundingSphere::operator=(const BoundingSphere& other)
{
  if (&other != this) {
    center       = other.center;
    radius       = other.radius;
    centerWorld  = other.centerWorld;
    radiusWorld  = other.radiusWorld;
    minimum      = other.minimum;
    maximum      = other.maximum;
    _worldMatrix = other._worldMatrix;
  }

  return *this;
}

BoundingSphere& BoundingSphere::operator=(BoundingSphere&& other)
{
  if (&other != this) {
    center       = std::move(other.center);
    radius       = std::move(other.radius);
    centerWorld  = std::move(other.centerWorld);
    radiusWorld  = std::move(other.radiusWorld);
    minimum      = std::move(other.minimum);
    maximum      = std::move(other.maximum);
    _worldMatrix = std::move(other._worldMatrix);
  }

  return *this;
}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::reConstruct(const Vector3& min, Vector3 max,
                                 const std::optional<Matrix>& worldMatrix)
{
  minimum.copyFrom(min);
  maximum.copyFrom(max);

  auto distance = Vector3::Distance(min, max);

  max.addToRef(min, center).scaleInPlace(0.5f);
  radius = distance * 0.5f;

  _update(worldMatrix.value_or(Matrix::IdentityReadOnly()));
}

BoundingSphere& BoundingSphere::scale(float factor)
{
  const auto newRadius   = radius * factor;
  auto& tmpVectors       = BoundingSphere::TmpVector3;
  auto& tempRadiusVector = tmpVectors[0].setAll(newRadius);
  auto& min = center.subtractToRef(tempRadiusVector, tmpVectors[1]);
  auto& max = center.addToRef(tempRadiusVector, tmpVectors[2]);

  reConstruct(min, max, _worldMatrix);

  return *this;
}

Matrix& BoundingSphere::getWorldMatrix()
{
  return _worldMatrix;
}

void BoundingSphere::_update(Matrix worldMatrix)
{
  if (!worldMatrix.isIdentity()) {
    Vector3::TransformCoordinatesToRef(center, worldMatrix, centerWorld);
    auto& tempVector = BoundingSphere::TmpVector3[0];
    Vector3::TransformNormalFromFloatsToRef(1.f, 1.f, 1.f, worldMatrix,
                                            tempVector);
    radiusWorld
      = std::max(std::max(std::abs(tempVector.x), std::abs(tempVector.y)),
                 std::abs(tempVector.z))
        * radius;
  }
  else {
    centerWorld.copyFrom(center);
    radiusWorld = radius;
  }
}

bool BoundingSphere::isInFrustum(
  const std::array<Plane, 6>& frustumPlanes) const
{
  const auto& iCenter = centerWorld;
  const auto& iRadius = radiusWorld;
  for (auto i = 0u; i < 6; ++i) {
    if (frustumPlanes[i].dotCoordinate(iCenter) <= -iRadius) {
      return false;
    }
  }
  return true;
}

bool BoundingSphere::isCenterInFrustum(
  const std::array<Plane, 6>& frustumPlanes) const
{
  const auto& iCenter = centerWorld;
  for (auto i = 0u; i < 6; ++i) {
    if (frustumPlanes[i].dotCoordinate(iCenter) < 0.f) {
      return false;
    }
  }
  return true;
}

bool BoundingSphere::intersectsPoint(const Vector3& point)
{
  const auto squareDistance = Vector3::DistanceSquared(centerWorld, point);
  if (radiusWorld * radiusWorld < squareDistance) {
    return false;
  }

  return true;
}

bool BoundingSphere::Intersects(const BoundingSphere& sphere0,
                                const BoundingSphere& sphere1)
{
  const auto squareDistance
    = Vector3::DistanceSquared(sphere0.centerWorld, sphere1.centerWorld);
  const auto radiusSum = sphere0.radiusWorld + sphere1.radiusWorld;

  if (radiusSum * radiusSum < squareDistance) {
    return false;
  }

  return true;
}

} // end of namespace BABYLON
