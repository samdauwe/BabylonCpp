#include <babylon/culling/bounding_box.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/math/plane.h>

namespace BABYLON {

std::array<Vector3, 3> BoundingBox::TmpVector3{Vector3::Zero(), Vector3::Zero(),
                                               Vector3::Zero()};

BoundingBox::BoundingBox(const Vector3& min, const Vector3& max,
                         const std::optional<Matrix>& worldMatrix)
    : vectors{{Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
               Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
               Vector3::Zero(), Vector3::Zero()}}
    , center{Vector3::Zero()}
    , centerWorld{Vector3::Zero()}
    , extendSize{Vector3::Zero()}
    , extendSizeWorld{Vector3::Zero()}
    , directions{{Vector3::Zero(), Vector3::Zero(), Vector3::Zero()}}
    , vectorsWorld{{Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
                    Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
                    Vector3::Zero(), Vector3::Zero()}}
    , minimumWorld{Vector3::Zero()}
    , maximumWorld{Vector3::Zero()}
    , minimum{Vector3::Zero()}
    , maximum{Vector3::Zero()}
    , _tag{-1}
    , _worldMatrix{Matrix::Identity()}
{
  reConstruct(min, max, worldMatrix);
}

void BoundingBox::reConstruct(Vector3 min, Vector3 max,
                              const std::optional<Matrix>& worldMatrix)
{
  const auto minX = min.x, minY = min.y, minZ = min.z, maxX = max.x,
             maxY = max.y, maxZ = max.z;

  minimum.copyFromFloats(minX, minY, minZ);
  maximum.copyFromFloats(maxX, maxY, maxZ);
  vectors[0].copyFromFloats(minX, minY, minZ);
  vectors[1].copyFromFloats(maxX, maxY, maxZ);
  vectors[2].copyFromFloats(maxX, minY, minZ);
  vectors[3].copyFromFloats(minX, maxY, minZ);
  vectors[4].copyFromFloats(minX, minY, maxZ);
  vectors[5].copyFromFloats(maxX, maxY, minZ);
  vectors[6].copyFromFloats(minX, maxY, maxZ);
  vectors[7].copyFromFloats(maxX, minY, maxZ);

  // OBB
  max.addToRef(min, center).scaleInPlace(0.5f);
  max.subtractToRef(min, extendSize).scaleInPlace(0.5f);

  _worldMatrix = worldMatrix.value_or(Matrix::IdentityReadOnly());

  _update(_worldMatrix);
}

BoundingBox::BoundingBox(const BoundingBox& other)
    : vectors{other.vectors}
    , center{other.center}
    , extendSize{other.extendSize}
    , directions{other.directions}
    , vectorsWorld{other.vectorsWorld}
    , minimumWorld{other.minimumWorld}
    , maximumWorld{other.maximumWorld}
    , minimum{other.minimum}
    , maximum{other.maximum}
    , _worldMatrix{other._worldMatrix}
{
}

BoundingBox::BoundingBox(BoundingBox&& other)
    : vectors{std::move(other.vectors)}
    , center{std::move(other.center)}
    , extendSize{std::move(other.extendSize)}
    , directions{std::move(other.directions)}
    , vectorsWorld{std::move(other.vectorsWorld)}
    , minimumWorld{std::move(other.minimumWorld)}
    , maximumWorld{std::move(other.maximumWorld)}
    , minimum{std::move(other.minimum)}
    , maximum{std::move(other.maximum)}
    , _worldMatrix{std::move(other._worldMatrix)}
{
}

BoundingBox& BoundingBox::operator=(const BoundingBox& other)
{
  if (&other != this) {
    vectors      = other.vectors;
    center       = other.center;
    extendSize   = other.extendSize;
    directions   = other.directions;
    vectorsWorld = other.vectorsWorld;
    minimumWorld = other.minimumWorld;
    maximumWorld = other.maximumWorld;
    minimum      = other.minimum;
    maximum      = other.maximum;
    _worldMatrix = other._worldMatrix;
  }

  return *this;
}

BoundingBox& BoundingBox::operator=(BoundingBox&& other)
{
  if (&other != this) {
    vectors      = std::move(other.vectors);
    center       = std::move(other.center);
    extendSize   = std::move(other.extendSize);
    directions   = std::move(other.directions);
    vectorsWorld = std::move(other.vectorsWorld);
    minimumWorld = std::move(other.minimumWorld);
    maximumWorld = std::move(other.maximumWorld);
    minimum      = std::move(other.minimum);
    maximum      = std::move(other.maximum);
    _worldMatrix = std::move(other._worldMatrix);
  }

  return *this;
}

BoundingBox::~BoundingBox() = default;

BoundingBox& BoundingBox::scale(float factor)
{
  auto& tmpVectors = BoundingBox::TmpVector3;
  auto& diff       = maximum.subtractToRef(minimum, tmpVectors[0]);
  const auto len   = diff.length();
  diff.normalizeFromLength(len);
  const auto distance  = len * factor;
  const auto newRadius = diff.scaleInPlace(distance * 0.5f);

  auto& min = center.subtractToRef(newRadius, tmpVectors[1]);
  auto& max = center.addToRef(newRadius, tmpVectors[2]);

  reConstruct(min, max, _worldMatrix);

  return *this;
}

Matrix& BoundingBox::getWorldMatrix()
{
  return _worldMatrix;
}

void BoundingBox::_update(const Matrix& world)
{
  auto& minWorld = minimumWorld;
  auto& maxWorld = maximumWorld;

  auto _world = world;
  if (!_world.isIdentity()) {
    minWorld.setAll(std::numeric_limits<float>::max());
    maxWorld.setAll(std::numeric_limits<float>::lowest());

    for (auto index = 0u; index < 8; ++index) {
      auto& v = vectorsWorld[index];
      Vector3::TransformCoordinatesToRef(vectors[index], world, v);
      minWorld.minimizeInPlace(v);
      maxWorld.maximizeInPlace(v);
    }

    // Extend
    maxWorld.subtractToRef(minWorld, extendSizeWorld).scaleInPlace(0.5f);
    maxWorld.addToRef(minWorld, centerWorld).scaleInPlace(0.5f);
  }
  else {
    minWorld.copyFrom(minimum);
    maxWorld.copyFrom(maximum);
    for (auto index = 0u; index < 8; ++index) {
      vectorsWorld[index].copyFrom(vectors[index]);
    }

    // Extend
    extendSizeWorld.copyFrom(extendSize);
    centerWorld.copyFrom(center);
  }

  Vector3::FromArrayToRef(world.m(), 0, directions[0]);
  Vector3::FromArrayToRef(world.m(), 4, directions[1]);
  Vector3::FromArrayToRef(world.m(), 8, directions[2]);

  _worldMatrix = world;
}

bool BoundingBox::isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                              unsigned int /*strategy*/)
{
  return BoundingBox::IsInFrustum(vectorsWorld, frustumPlanes);
}

bool BoundingBox::isCompletelyInFrustum(
  const std::array<Plane, 6>& frustumPlanes)
{
  return BoundingBox::IsCompletelyInFrustum(vectorsWorld, frustumPlanes);
}

bool BoundingBox::intersectsPoint(const Vector3& point)
{
  const auto& min = minimumWorld;
  const auto& max = maximumWorld;
  const auto minX = min.x, minY = min.y, minZ = min.z, maxX = max.x,
             maxY = max.y, maxZ = max.z;
  const auto pointX = point.x, pointY = point.y, pointZ = point.z;
  const auto delta = -Math::Epsilon;

  if (maxX - pointX < delta || delta > pointX - minX) {
    return false;
  }

  if (maxY - pointY < delta || delta > pointY - minY) {
    return false;
  }

  if (maxZ - pointZ < delta || delta > pointZ - minZ) {
    return false;
  }

  return true;
}

bool BoundingBox::intersectsSphere(const BoundingSphere& sphere)
{
  return BoundingBox::IntersectsSphere(minimumWorld, maximumWorld,
                                       sphere.centerWorld, sphere.radiusWorld);
}

bool BoundingBox::intersectsMinMax(const Vector3& min, const Vector3& max) const
{
  const auto& myMin = minimumWorld;
  const auto& myMax = maximumWorld;
  const auto myMinX = myMin.x, myMinY = myMin.y, myMinZ = myMin.z,
             myMaxX = myMax.x, myMaxY = myMax.y, myMaxZ = myMax.z;
  const auto minX = min.x, minY = min.y, minZ = min.z, maxX = max.x,
             maxY = max.y, maxZ = max.z;
  if (myMaxX < minX || myMinX > maxX) {
    return false;
  }

  if (myMaxY < minY || myMinY > maxY) {
    return false;
  }

  if (myMaxZ < minZ || myMinZ > maxZ) {
    return false;
  }

  return true;
}

// Statics
bool BoundingBox::Intersects(const BoundingBox& box0, const BoundingBox& box1)
{
  return box0.intersectsMinMax(box1.minimumWorld, box1.maximumWorld);
}

bool BoundingBox::IntersectsSphere(const Vector3& minPoint,
                                   const Vector3& maxPoint,
                                   const Vector3& sphereCenter,
                                   float sphereRadius)
{
  auto& vector = BoundingBox::TmpVector3[0];
  Vector3::ClampToRef(sphereCenter, minPoint, maxPoint, vector);
  const auto num = Vector3::DistanceSquared(sphereCenter, vector);
  return (num <= (sphereRadius * sphereRadius));
}

bool BoundingBox::IsCompletelyInFrustum(
  const std::array<Vector3, 8>& boundingVectors,
  const std::array<Plane, 6>& frustumPlanes)
{
  for (unsigned p = 0u; p < 6; ++p) {
    const auto& frustumPlane = frustumPlanes[p];
    for (unsigned i = 0u; i < 8; ++i) {
      if (frustumPlane.dotCoordinate(boundingVectors[i]) < 0.f) {
        return false;
      }
    }
  }
  return true;
}

bool BoundingBox::IsInFrustum(const std::array<Vector3, 8>& boundingVectors,
                              const std::array<Plane, 6>& frustumPlanes)
{
  for (unsigned p = 0u; p < 6; ++p) {
    auto canReturnFalse      = true;
    const auto& frustumPlane = frustumPlanes[p];
    for (unsigned i = 0u; i < 8; ++i) {
      if (frustumPlane.dotCoordinate(boundingVectors[i]) >= 0) {
        canReturnFalse = false;
        break;
      }
    }
    if (canReturnFalse) {
      return false;
    }
  }
  return true;
}

} // end of namespace BABYLON
