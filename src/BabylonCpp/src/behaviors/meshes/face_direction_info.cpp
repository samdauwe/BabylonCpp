#include <babylon/behaviors/meshes/face_direction_info.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

FaceDirectionInfo::FaceDirectionInfo(const Vector3& iDirection,
                                     const Vector3& iRotatedDirection,
                                     float iDiff, bool iIgnore)
    : direction{iDirection}
    , rotatedDirection{iRotatedDirection}
    , diff{iDiff}
    , ignore{iIgnore}
{
}

FaceDirectionInfo::FaceDirectionInfo(const FaceDirectionInfo& other) = default;

FaceDirectionInfo::FaceDirectionInfo(FaceDirectionInfo&& other) = default;

FaceDirectionInfo& FaceDirectionInfo::operator=(const FaceDirectionInfo& other) = default;

FaceDirectionInfo& FaceDirectionInfo::operator=(FaceDirectionInfo&& other) = default;

FaceDirectionInfo::~FaceDirectionInfo() = default;

FaceDirectionInfo FaceDirectionInfo::copy() const
{
  return FaceDirectionInfo(*this);
}

std::unique_ptr<FaceDirectionInfo> FaceDirectionInfo::clone() const
{
  return std::make_unique<FaceDirectionInfo>(*this);
}

std::ostream& operator<<(std::ostream& os,
                         const FaceDirectionInfo& faceDirectionInfo)
{
  os << "{\"direction\":" << faceDirectionInfo.direction
     << "\",rotatedDirection\":" << faceDirectionInfo.rotatedDirection
     << "\",diff\":" << faceDirectionInfo.diff
     << "\",ignore\":" << faceDirectionInfo.ignore << "}";
  return os;
}

bool operator==(const FaceDirectionInfo& lhs, const FaceDirectionInfo& rhs)
{
  return lhs.direction == rhs.direction
         && lhs.rotatedDirection == rhs.rotatedDirection
         && stl_util::almost_equal(lhs.diff, rhs.diff)
         && lhs.ignore == rhs.ignore;
}

bool operator!=(const FaceDirectionInfo& lhs, const FaceDirectionInfo& rhs)
{
  return !(lhs == rhs);
}

} // end of namespace BABYLON
