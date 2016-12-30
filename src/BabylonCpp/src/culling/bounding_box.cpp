#include <babylon/culling/bounding_box.h>

#include <babylon/culling/bounding_sphere.h>
#include <babylon/math/math_tools.h>
#include <babylon/math/plane.h>

namespace BABYLON {

BoundingBox::BoundingBox(const Vector3& _minimum, const Vector3& _maximum)
    : minimum{_minimum}, maximum{_maximum}
{
  // Bounding vectors
  vectors.reserve(8);
  vectors.emplace_back(minimum);
  vectors.emplace_back(maximum);

  vectors.emplace_back(minimum);
  vectors[2].x = maximum.x;

  vectors.emplace_back(minimum);
  vectors[3].y = maximum.y;

  vectors.emplace_back(minimum);
  vectors[4].z = maximum.z;

  vectors.emplace_back(maximum);
  vectors[5].z = minimum.z;

  vectors.emplace_back(maximum);
  vectors[6].x = minimum.x;

  vectors.emplace_back(maximum);
  vectors[7].y = minimum.y;

  // OBB
  center     = maximum.add(minimum).scale(0.5f);
  extendSize = maximum.subtract(minimum).scale(0.5f);
  std_util::concat(directions,
                   {Vector3::Zero(), Vector3::Zero(), Vector3::Zero()});

  // World
  vectorsWorld.clear();
  vectorsWorld.reserve(vectors.size());
  std::fill(vectorsWorld.begin(), vectorsWorld.end(), Vector3::Zero());
  minimumWorld = Vector3::Zero();
  maximumWorld = Vector3::Zero();

  _update(Matrix::Identity());
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

BoundingBox::~BoundingBox()
{
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
  Vector3::FromFloatsToRef(-std::numeric_limits<float>::max(),
                           -std::numeric_limits<float>::max(),
                           -std::numeric_limits<float>::max(), maximumWorld);

  unsigned int index = 0;
  for (auto& vector : vectors) {
    auto& v = vectorsWorld[index];
    Vector3::TransformCoordinatesToRef(vector, world, v);

    if (v.x < minimumWorld.x) {
      minimumWorld.x = v.x;
    }
    if (v.y < minimumWorld.y) {
      minimumWorld.y = v.y;
    }
    if (v.z < minimumWorld.z) {
      minimumWorld.z = v.z;
    }

    if (v.x > maximumWorld.x) {
      maximumWorld.x = v.x;
    }
    if (v.y > maximumWorld.y) {
      maximumWorld.y = v.y;
    }
    if (v.z > maximumWorld.z) {
      maximumWorld.z = v.z;
    }

    ++index;
  }

  // OBB
  maximumWorld.addToRef(minimumWorld, center);
  center.scaleInPlace(0.5f);

  Vector3::FromArrayToRef(world.m, 0, directions[0]);
  Vector3::FromArrayToRef(world.m, 4, directions[1]);
  Vector3::FromArrayToRef(world.m, 8, directions[2]);

  _worldMatrix = world;
}

bool BoundingBox::isInFrustum(const std::array<Plane, 6>& frustumPlanes)
{
  return BoundingBox::IsInFrustum(vectorsWorld, frustumPlanes);
}

bool BoundingBox::isCompletelyInFrustum(
  const std::array<Plane, 6>& frustumPlanes) const
{
  return BoundingBox::IsCompletelyInFrustum(vectorsWorld, frustumPlanes);
}

bool BoundingBox::intersectsPoint(const Vector3& point)
{
  float delta = -MathTools::Epsilon;

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
  Vector3 vector = Vector3::Clamp(sphereCenter, minPoint, maxPoint);
  float num      = Vector3::DistanceSquared(sphereCenter, vector);
  return (num <= (sphereRadius * sphereRadius));
}

bool BoundingBox::IsCompletelyInFrustum(
  const std::vector<Vector3>& boundingVectors,
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

bool BoundingBox::IsInFrustum(const std::vector<Vector3>& boundingVectors,
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
