#include <babylon/collisions/intersection_info.h>

namespace BABYLON {

IntersectionInfo::IntersectionInfo()
    : bu{std::nullopt}, bv{std::nullopt}, distance{0.f}, faceId{0}, subMeshId{0}
{
}

IntersectionInfo::IntersectionInfo(float _distance)
    : bu{std::nullopt}
    , bv{std::nullopt}
    , distance{_distance}
    , faceId{0}
    , subMeshId{0}
{
}

IntersectionInfo::IntersectionInfo(float _bu, float _bv, float _distance)
    : bu{_bu}, bv{_bv}, distance{_distance}, faceId{0}, subMeshId{0}
{
}

IntersectionInfo::IntersectionInfo(const IntersectionInfo& other)
    : bu{other.bu}
    , bv{other.bv}
    , distance{other.distance}
    , faceId{other.faceId}
    , subMeshId{other.subMeshId}
{
}

IntersectionInfo::IntersectionInfo(IntersectionInfo&& other)
    : bu{std::move(other.bu)}
    , bv{std::move(other.bv)}
    , distance{std::move(other.distance)}
    , faceId{std::move(other.faceId)}
    , subMeshId{std::move(other.subMeshId)}
{
}

IntersectionInfo& IntersectionInfo::operator=(const IntersectionInfo& other)
{
  if (&other != this) {
    bu        = other.bu;
    bv        = other.bv;
    distance  = other.distance;
    faceId    = other.faceId;
    subMeshId = other.subMeshId;
  }

  return *this;
}

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

IntersectionInfo::~IntersectionInfo()
{
}

} // end of namespace BABYLON
