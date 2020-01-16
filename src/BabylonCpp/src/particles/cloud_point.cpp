#include <babylon/particles/cloud_point.h>

#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/mesh.h>
#include <babylon/particles/points_cloud_system.h>

namespace BABYLON {

CloudPoint::CloudPoint(size_t particleIndex, const PointsGroupPtr& group, size_t iGroupId,
                       size_t iIdxInGroup, const PointsCloudSystemPtr& pcs)
    : idx{0}
    , color{std::make_unique<Color4>(1.f, 1.f, 1.f, 1.f)}
    , position{Vector3::Zero()}
    , rotation{Vector3::Zero()}
    , rotationQuaternion{nullptr}
    , uv{std::make_unique<Vector2>(0.f, 0.f)}
    , velocity{Vector3::Zero()}
    , pivot{Vector3::Zero()}
    , translateFromPivot{false}
    , _pos{0}
    , _ind{0}
    , groupId{0}
    , idxInGroup{0}
    , _boundingInfo{nullptr}
    , _pcs{nullptr}
    , _stillInvisible{false}
    , _rotationMatrix{1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f}
    , parentId{std::nullopt}
    , _globalPosition{Vector3::Zero()}
    , size{this, &CloudPoint::get_size, &CloudPoint::set_size}
    , quaternion{this, &CloudPoint::get_quaternion, &CloudPoint::set_quaternion}
{
  idx        = particleIndex;
  _group     = group;
  groupId    = iGroupId;
  idxInGroup = iIdxInGroup;
  _pcs       = pcs;
}

CloudPoint::~CloudPoint() = default;

Vector3& CloudPoint::get_size()
{
  return size;
}

void CloudPoint::set_size(const Vector3& scale)
{
  size = scale;
}

std::unique_ptr<Quaternion>& CloudPoint::get_quaternion()
{
  return rotationQuaternion;
}

void CloudPoint::set_quaternion(const std::unique_ptr<Quaternion>& q)
{
  rotationQuaternion = q ? std::make_unique<Quaternion>(*q) : nullptr;
}

bool CloudPoint::intersectsMesh(const MeshPtr& target, bool isSphere) const
{
  if (!target->_boundingInfo) {
    return false;
  }
  isSphere = isSphere ? isSphere : false;

  if (isSphere) {
    return target->getBoundingInfo()->boundingSphere.intersectsPoint(
      position.add(_pcs->mesh->position));
  }
  else {
    const auto maxX = target->getBoundingInfo()->boundingBox.maximumWorld.x;
    const auto minX = target->getBoundingInfo()->boundingBox.minimumWorld.x;
    const auto maxY = target->getBoundingInfo()->boundingBox.maximumWorld.y;
    const auto minY = target->getBoundingInfo()->boundingBox.minimumWorld.y;
    const auto maxZ = target->getBoundingInfo()->boundingBox.maximumWorld.z;
    const auto minZ = target->getBoundingInfo()->boundingBox.minimumWorld.z;

    const auto x = position.x + _pcs->mesh->position().x;
    const auto y = position.y + _pcs->mesh->position().y;
    const auto z = position.z + _pcs->mesh->position().z;
    return minX <= x && x <= maxX && minY <= y && y <= maxY && minZ <= z && z <= maxZ;
  }
}

void CloudPoint::getRotationMatrix(Matrix& m)
{
  Quaternion quaternion_;
  if (rotationQuaternion) {
    quaternion_ = *rotationQuaternion;
  }
  else {
    quaternion_ = TmpVectors::QuaternionArray[0];
    Quaternion::RotationYawPitchRollToRef(rotation.y, rotation.x, rotation.z, quaternion_);
  }

  quaternion_.toRotationMatrix(m);
}

} // end of namespace BABYLON
