#include <babylon/collisions/intersection_info.h>

namespace BABYLON {

IntersectionInfo::IntersectionInfo()
    : bu{std::nullopt}, bv{std::nullopt}, distance{0.f}, faceId{0}, subMeshId{0}
{
}

IntersectionInfo::IntersectionInfo(float _distance)
    : bu{std::nullopt}, bv{std::nullopt}, distance{_distance}, faceId{0}, subMeshId{0}
{
}

IntersectionInfo::IntersectionInfo(float _bu, float _bv, float _distance)
    : bu{_bu}, bv{_bv}, distance{_distance}, faceId{0}, subMeshId{0}
{
}

IntersectionInfo::IntersectionInfo(const IntersectionInfo& other) = default;

IntersectionInfo::IntersectionInfo(IntersectionInfo&& other) = default;

IntersectionInfo& IntersectionInfo::operator=(const IntersectionInfo& other) = default;

IntersectionInfo& IntersectionInfo::operator=(IntersectionInfo&& other)
{
  if (&other != this) {
    bu        = std::move(other.bu);
    bv        = std::move(other.bv);
    distance  = std::move(other.distance);
    faceId    = std::move(other.faceId);
    subMeshId = std::move(other.subMeshId);
  }

  return *this;
}

IntersectionInfo::~IntersectionInfo() = default;

} // end of namespace BABYLON
