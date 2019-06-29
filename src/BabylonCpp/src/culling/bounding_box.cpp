#include <babylon/culling/bounding_box.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/math/plane.h>
#include <babylon/math/tmp.h>

namespace BABYLON {

BoundingBox::BoundingBox(const Vector3& min, const Vector3& max)
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
  reConstruct(min, max);
}

void BoundingBox::reConstruct(const Vector3& min, const Vector3& max,
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
  auto _max = max.addToRef(min, center);
  _max.scaleInPlace(0.5f);
  auto _min = _max.subtractToRef(min, extendSize);
  _min.scaleInPlace(0.5f);

  _worldMatrix = worldMatrix ? worldMatrix.value() : Matrix::IdentityReadOnly();

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

BoundingBox::~BoundingBox()
{
}

BoundingBox& BoundingBox::scale(float factor)
{
  auto diff = Tmp::Vector3Array[0].copyFrom(maximum).subtractInPlace(minimum);
  auto distance = diff.length() * factor;
  diff.normalize();
  auto newRadius = diff.scaleInPlace(distance * 0.5f);

  const auto min
    = Tmp::Vector3Array[1].copyFrom(center).subtractInPlace(newRadius);
  const auto max = Tmp::Vector3Array[2].copyFrom(center).addInPlace(newRadius);

  reConstruct(min, max);

  return *this;
}

Matrix& BoundingBox::getWorldMatrix()
{
  return _worldMatrix;
}

BoundingBox& BoundingBox::setWorldMatrix(const Matrix& matrix)
{
  _worldMatrix.copyFrom(matrix);
  return *this;
}

void BoundingBox::_update(const Matrix& world)
{
  Vector3::FromFloatsToRef(std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max(), minimumWorld);
  Vector3::FromFloatsToRef(std::numeric_limits<float>::lowest(),
                           std::numeric_limits<float>::lowest(),
                           std::numeric_limits<float>::lowest(), maximumWorld);

  for (unsigned int index = 0; index < 8; ++index) {
    auto& v = vectorsWorld[index];
    Vector3::TransformCoordinatesToRef(vectors[index], world, v);
    minimumWorld.minimizeInPlace(v);
    maximumWorld.maximizeInPlace(v);
  }

  // Extend
  maximumWorld.subtractToRef(minimumWorld, extendSizeWorld);
  extendSizeWorld.scaleInPlace(0.5f);

  // OBB
  maximumWorld.addToRef(minimumWorld, centerWorld);
  centerWorld.scaleInPlace(0.5f);

  Vector3::FromFloatArrayToRef(world.m(), 0, directions[0]);
  Vector3::FromFloatArrayToRef(world.m(), 4, directions[1]);
  Vector3::FromFloatArrayToRef(world.m(), 8, directions[2]);

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
  float delta = -Math::Epsilon;

  if (maximumWorld.x - point.x < delta || delta > point.x - minimumWorld.x) {
    return false;
  }

  if (maximumWorld.y - point.y < delta || delta > point.y - minimumWorld.y) {
    return false;
  }

  if (maximumWorld.z - point.z < delta || delta > point.z - minimumWorld.z) {
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
  if (maximumWorld.x < min.x || minimumWorld.x > max.x) {
    return false;
  }

  if (maximumWorld.y < min.y || minimumWorld.y > max.y) {
    return false;
  }

  if (maximumWorld.z < min.z || minimumWorld.z > max.z) {
    return false;
  }

  return true;
}

// Statics
bool BoundingBox::Intersects(const BoundingBox& box0, const BoundingBox& box1)
{
  if (box0.maximumWorld.x < box1.minimumWorld.x
      || box0.minimumWorld.x > box1.maximumWorld.x) {
    return false;
  }

  if (box0.maximumWorld.y < box1.minimumWorld.y
      || box0.minimumWorld.y > box1.maximumWorld.y) {
    return false;
  }

  if (box0.maximumWorld.z < box1.minimumWorld.z
      || box0.minimumWorld.z > box1.maximumWorld.z) {
    return false;
  }

  return true;
}

bool BoundingBox::IntersectsSphere(const Vector3& minPoint,
                                   const Vector3& maxPoint,
                                   const Vector3& sphereCenter,
                                   float sphereRadius)
{
  auto vector = Vector3::Clamp(sphereCenter, minPoint, maxPoint);
  auto num    = Vector3::DistanceSquared(sphereCenter, vector);
  return (num <= (sphereRadius * sphereRadius));
}

bool BoundingBox::IsCompletelyInFrustum(
  const std::array<Vector3, 8>& boundingVectors,
  const std::array<Plane, 6>& frustumPlanes)
{
  for (unsigned int p = 0; p < 6; ++p) {
    for (unsigned int i = 0; i < 8; ++i) {
      if (frustumPlanes[p].dotCoordinate(boundingVectors[i]) < 0) {
        return false;
      }
    }
  }
  return true;
}

bool BoundingBox::IsInFrustum(const std::array<Vector3, 8>& boundingVectors,
                              const std::array<Plane, 6>& frustumPlanes)
{
  for (size_t p = 0; p < 6; ++p) {
    auto inCount = 8;

    for (size_t i = 0; i < 8; ++i) {
      if (frustumPlanes[p].dotCoordinate(boundingVectors[i]) < 0) {
        --inCount;
      }
      else {
        break;
      }
    }
    if (inCount == 0) {
      return false;
    }
  }
  return true;
}

} // end of namespace BABYLON
