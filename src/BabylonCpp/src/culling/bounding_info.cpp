#include <babylon/culling/bounding_info.h>

#include <babylon/collisions/collider.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_sphere.h>

namespace BABYLON {

BoundingInfo::BoundingInfo(const Vector3& iMinimum, const Vector3& iMaximum)
    : minimum{iMinimum}
    , maximum{iMaximum}
    , boundingBox{BoundingBox(iMinimum, iMaximum)}
    , boundingSphere{BoundingSphere(iMinimum, iMaximum)}
    , _isLocked{false}
{
}

BoundingInfo::BoundingInfo(const BoundingInfo& boundingInfo)
    : minimum{boundingInfo.minimum}
    , maximum{boundingInfo.maximum}
    , boundingBox{BoundingBox(boundingInfo.boundingBox)}
    , boundingSphere{BoundingSphere(boundingInfo.boundingSphere)}
    , _isLocked{boundingInfo._isLocked}
{
}

BoundingInfo::BoundingInfo(BoundingInfo&& other)
    : minimum{std::move(other.minimum)}
    , maximum{std::move(other.maximum)}
    , boundingBox{BoundingBox(std::move(other.boundingBox))}
    , boundingSphere{BoundingSphere(std::move(other.boundingSphere))}
    , _isLocked{std::move(other._isLocked)}
{
}

BoundingInfo& BoundingInfo::operator=(const BoundingInfo& other)
{
  if (&other != this) {
    minimum        = other.minimum;
    maximum        = other.maximum;
    boundingBox    = other.boundingBox;
    boundingSphere = other.boundingSphere;
    _isLocked      = other._isLocked;
  }

  return *this;
}

BoundingInfo& BoundingInfo::operator=(BoundingInfo&& other)
{
  if (&other != this) {
    minimum        = std::move(other.minimum);
    maximum        = std::move(other.maximum);
    boundingBox    = std::move(other.boundingBox);
    boundingSphere = std::move(other.boundingSphere);
    _isLocked      = std::move(other._isLocked);
  }

  return *this;
}

BoundingInfo::~BoundingInfo()
{
}

bool BoundingInfo::isLocked() const
{
  return _isLocked;
}

void BoundingInfo::setIsLocked(bool value)
{
  _isLocked = value;
}

// Methods
void BoundingInfo::update(const Matrix& world)
{
  if (_isLocked) {
    return;
  }
  boundingBox._update(world);
  boundingSphere._update(world);
}

bool BoundingInfo::isInFrustum(const std::array<Plane, 6>& frustumPlanes)
{
  if (!boundingSphere.isInFrustum(frustumPlanes)) {
    return false;
  }

  return boundingBox.isInFrustum(frustumPlanes);
}

float BoundingInfo::diagonalLength() const
{
  auto size = boundingBox.maximumWorld.subtract(boundingBox.minimumWorld);
  return size.length();
}

bool BoundingInfo::isCompletelyInFrustum(
  const std::array<Plane, 6>& frustumPlanes) const
{
  return boundingBox.isCompletelyInFrustum(frustumPlanes);
}

bool BoundingInfo::_checkCollision(const Collider& collider) const
{
  return collider._canDoCollision(
    boundingSphere.centerWorld, boundingSphere.radiusWorld,
    boundingBox.minimumWorld, boundingBox.maximumWorld);
}

bool BoundingInfo::intersectsPoint(const Vector3& point)
{
  if (!boundingSphere.intersectsPoint(point)) {
    return false;
  }

  if (!boundingBox.intersectsPoint(point)) {
    return false;
  }

  return true;
}

bool BoundingInfo::intersects(const BoundingInfo& boundingInfo, bool precise)
{
  if (!BoundingSphere::Intersects(boundingSphere,
                                  boundingInfo.boundingSphere)) {
    return false;
  }

  if (!BoundingBox::Intersects(boundingBox, boundingInfo.boundingBox)) {
    return false;
  }

  if (!precise) {
    return true;
  }

  const auto& box0 = boundingBox;
  const auto& box1 = boundingInfo.boundingBox;

  if (!axisOverlap(box0.directions[0], box0, box1)) {
    return false;
  }
  if (!axisOverlap(box0.directions[1], box0, box1)) {
    return false;
  }
  if (!axisOverlap(box0.directions[2], box0, box1)) {
    return false;
  }
  if (!axisOverlap(box1.directions[0], box0, box1)) {
    return false;
  }
  if (!axisOverlap(box1.directions[1], box0, box1)) {
    return false;
  }
  if (!axisOverlap(box1.directions[2], box0, box1)) {
    return false;
  }
  if (!axisOverlap(Vector3::Cross(box0.directions[0], box1.directions[0]), box0,
                   box1)) {
    return false;
  }
  if (!axisOverlap(Vector3::Cross(box0.directions[0], box1.directions[1]), box0,
                   box1)) {
    return false;
  }
  if (!axisOverlap(Vector3::Cross(box0.directions[0], box1.directions[2]), box0,
                   box1)) {
    return false;
  }
  if (!axisOverlap(Vector3::Cross(box0.directions[1], box1.directions[0]), box0,
                   box1)) {
    return false;
  }
  if (!axisOverlap(Vector3::Cross(box0.directions[1], box1.directions[1]), box0,
                   box1)) {
    return false;
  }
  if (!axisOverlap(Vector3::Cross(box0.directions[1], box1.directions[2]), box0,
                   box1)) {
    return false;
  }
  if (!axisOverlap(Vector3::Cross(box0.directions[2], box1.directions[0]), box0,
                   box1)) {
    return false;
  }
  if (!axisOverlap(Vector3::Cross(box0.directions[2], box1.directions[1]), box0,
                   box1)) {
    return false;
  }
  if (!axisOverlap(Vector3::Cross(box0.directions[2], box1.directions[2]), box0,
                   box1)) {
    return false;
  }

  return true;
}

Extents BoundingInfo::computeBoxExtents(const Vector3& axis,
                                        const BoundingBox& box) const
{
  float p = Vector3::Dot(box.centerWorld, axis);

  float r0 = std::abs(Vector3::Dot(box.directions[0], axis)) * box.extendSize.x;
  float r1 = std::abs(Vector3::Dot(box.directions[1], axis)) * box.extendSize.y;
  float r2 = std::abs(Vector3::Dot(box.directions[2], axis)) * box.extendSize.z;

  float r = r0 + r1 + r2;
  return {p - r, p + r};
}

bool BoundingInfo::extentsOverlap(float min0, float max0, float min1,
                                  float max1) const
{
  return !(min0 > max1 || min1 > max0);
}

bool BoundingInfo::axisOverlap(const Vector3& axis, const BoundingBox& box0,
                               const BoundingBox& box1) const
{
  auto result0 = computeBoxExtents(axis, box0);
  auto result1 = computeBoxExtents(axis, box1);

  return extentsOverlap(result0.min, result0.max, result1.min, result1.max);
}

} // end of namespace BABYLON
