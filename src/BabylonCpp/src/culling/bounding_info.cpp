#include <babylon/culling/bounding_info.h>

#include <babylon/collisions/collider.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

std::array<Vector3, 2> BoundingInfo::TmpVector3{Vector3::Zero(),
                                                Vector3::Zero()};

BoundingInfo::BoundingInfo(const Vector3& iMinimum, const Vector3& iMaximum,
                           const std::optional<Matrix>& worldMatrix)
    : boundingBox{BoundingBox(iMinimum, iMaximum, worldMatrix)}
    , boundingSphere{BoundingSphere(iMinimum, iMaximum, worldMatrix)}
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

void BoundingInfo::reConstruct(const Vector3& min, const Vector3& max,
                               const std::optional<Matrix>& worldMatrix)
{
  boundingBox.reConstruct(min, max, worldMatrix);
  boundingSphere.reConstruct(min, max, worldMatrix);
}

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
  auto& iMinimum
    = BoundingInfo::TmpVector3[0].copyFrom(center).subtractInPlace(extend);
  auto& iMaximum
    = BoundingInfo::TmpVector3[1].copyFrom(center).addInPlace(extend);

  boundingBox.reConstruct(iMinimum, iMaximum, boundingBox.getWorldMatrix());
  boundingSphere.reConstruct(iMinimum, iMaximum, boundingBox.getWorldMatrix());

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
  const auto inclusionTest
    = (strategy == Constants::MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION
       || strategy
            == Constants::
                 MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION_THEN_BSPHERE_ONLY);
  if (inclusionTest) {
    if (boundingSphere.isCenterInFrustum(frustumPlanes)) {
      return true;
    }
  }

  if (!boundingSphere.isInFrustum(frustumPlanes)) {
    return false;
  }

  const auto bSphereOnlyTest
    = (strategy == Constants::MESHES_CULLINGSTRATEGY_BOUNDINGSPHERE_ONLY
       || strategy
            == Constants::
                 MESHES_CULLINGSTRATEGY_OPTIMISTIC_INCLUSION_THEN_BSPHERE_ONLY);
  if (bSphereOnlyTest) {
    return true;
  }

  return boundingBox.isInFrustum(frustumPlanes);
}

float BoundingInfo::diagonalLength() const
{
  const auto& diag = boundingBox.maximumWorld.subtractToRef(
    boundingBox.minimumWorld, BoundingInfo::TmpVector3[0]);
  return diag.length();
}

bool BoundingInfo::isCompletelyInFrustum(
  const std::array<Plane, 6>& frustumPlanes)
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

void BoundingInfo::computeBoxExtents(const Vector3& axis,
                                     const BoundingBox& box, Extents& result)
{
  const auto p = Vector3::Dot(box.centerWorld, axis);

  const auto r0
    = std::abs(Vector3::Dot(box.directions[0], axis)) * box.extendSize.x;
  const auto r1
    = std::abs(Vector3::Dot(box.directions[1], axis)) * box.extendSize.y;
  const auto r2
    = std::abs(Vector3::Dot(box.directions[2], axis)) * box.extendSize.z;

  const auto r = r0 + r1 + r2;
  result.min   = p - r;
  result.max   = p + r;
}

bool BoundingInfo::axisOverlap(const Vector3& axis, const BoundingBox& box0,
                               const BoundingBox& box1)
{
  Extents _result0{0.f, 0.f};
  Extents _result1{0.f, 0.f};
  computeBoxExtents(axis, box0, _result0);
  computeBoxExtents(axis, box1, _result1);
  return !(_result0.min > _result1.max || _result1.min > _result0.max);
}

} // end of namespace BABYLON
