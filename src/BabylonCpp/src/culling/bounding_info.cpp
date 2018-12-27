#include <babylon/culling/bounding_info.h>

#include <babylon/collisions/collider.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

BoundingInfo::BoundingInfo(const Vector3& iMinimum, const Vector3& iMaximum)
    : boundingBox{BoundingBox(iMinimum, iMaximum)}
    , boundingSphere{BoundingSphere(iMinimum, iMaximum)}
    , minimum{this, &BoundingInfo::get_minimum}
    , maximum{this, &BoundingInfo::get_maximum}
    , isLocked{this, &BoundingInfo::get_isLocked, &BoundingInfo::set_isLocked}
    , _isLocked{false}
{
}

BoundingInfo::BoundingInfo(const BoundingInfo& boundingInfo)
    : boundingBox{BoundingBox(boundingInfo.boundingBox)}
    , boundingSphere{BoundingSphere(boundingInfo.boundingSphere)}
    , minimum{this, &BoundingInfo::get_minimum}
    , maximum{this, &BoundingInfo::get_maximum}
    , isLocked{this, &BoundingInfo::get_isLocked, &BoundingInfo::set_isLocked}
    , _isLocked{boundingInfo._isLocked}
{
}

BoundingInfo::BoundingInfo(BoundingInfo&& other)
    : boundingBox{BoundingBox(std::move(other.boundingBox))}
    , boundingSphere{BoundingSphere(std::move(other.boundingSphere))}
    , minimum{this, &BoundingInfo::get_minimum}
    , maximum{this, &BoundingInfo::get_maximum}
    , isLocked{this, &BoundingInfo::get_isLocked, &BoundingInfo::set_isLocked}
    , _isLocked{std::move(other._isLocked)}
{
}

BoundingInfo& BoundingInfo::operator=(const BoundingInfo& other)
{
  if (&other != this) {
    boundingBox    = other.boundingBox;
    boundingSphere = other.boundingSphere;
    _isLocked      = other._isLocked;
  }

  return *this;
}

BoundingInfo& BoundingInfo::operator=(BoundingInfo&& other)
{
  if (&other != this) {
    boundingBox    = std::move(other.boundingBox);
    boundingSphere = std::move(other.boundingSphere);
    _isLocked      = std::move(other._isLocked);
  }

  return *this;
}

BoundingInfo::~BoundingInfo()
{
}

Vector3& BoundingInfo::get_minimum()
{
  return boundingBox.minimum;
}

Vector3& BoundingInfo::get_maximum()
{
  return boundingBox.maximum;
}

bool BoundingInfo::get_isLocked() const
{
  return _isLocked;
}

void BoundingInfo::set_isLocked(bool value)
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

BoundingInfo& BoundingInfo::centerOn(const Vector3& center,
                                     const Vector3& extend)
{
  const auto iMinimum
    = Tmp::Vector3Array[0].copyFrom(center).subtractInPlace(extend);
  const auto iMaximum
    = Tmp::Vector3Array[1].copyFrom(center).addInPlace(extend);

  boundingBox.reConstruct(iMinimum, iMaximum);
  boundingSphere.reConstruct(iMinimum, iMaximum);

  return *this;
}

BoundingInfo& BoundingInfo::scale(float factor)
{
  boundingBox.scale(factor);
  boundingSphere.scale(factor);

  return *this;
}

bool BoundingInfo::isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                               unsigned int strategy)
{
  if (!boundingSphere.isInFrustum(frustumPlanes)) {
    return false;
  }

  if (strategy == AbstractMesh::CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY) {
    return true;
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
  const float p = Vector3::Dot(box.centerWorld, axis);

  const float r0
    = std::abs(Vector3::Dot(box.directions[0], axis)) * box.extendSize.x;
  const float r1
    = std::abs(Vector3::Dot(box.directions[1], axis)) * box.extendSize.y;
  const float r2
    = std::abs(Vector3::Dot(box.directions[2], axis)) * box.extendSize.z;

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
