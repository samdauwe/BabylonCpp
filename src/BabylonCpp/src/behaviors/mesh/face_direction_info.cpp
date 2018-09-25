#include <babylon/behaviors/mesh/face_direction_info.h>

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

FaceDirectionInfo::FaceDirectionInfo(const FaceDirectionInfo& other)
    : direction{other.direction}
    , rotatedDirection{other.rotatedDirection}
    , diff{other.diff}
    , ignore{other.ignore}
{
}

FaceDirectionInfo::FaceDirectionInfo(FaceDirectionInfo&& other)
    : direction{std::move(other.direction)}
    , rotatedDirection{std::move(other.rotatedDirection)}
    , diff{std::move(other.diff)}
    , ignore{std::move(other.ignore)}
{
}

FaceDirectionInfo& FaceDirectionInfo::operator=(const FaceDirectionInfo& other)
{
  if (&other != this) {
    direction        = other.direction;
    rotatedDirection = other.rotatedDirection;
    diff             = other.diff;
    ignore           = other.ignore;
  }

  return *this;
}

FaceDirectionInfo& FaceDirectionInfo::operator=(FaceDirectionInfo&& other)
{
  if (&other != this) {
    direction        = std::move(other.direction);
    rotatedDirection = std::move(other.rotatedDirection);
    diff             = std::move(other.diff);
    ignore           = std::move(other.ignore);
  }

  return *this;
}

FaceDirectionInfo::~FaceDirectionInfo()
{
}

FaceDirectionInfo FaceDirectionInfo::copy() const
{
  return FaceDirectionInfo(*this);
}

unique_ptr_t<FaceDirectionInfo> FaceDirectionInfo::clone() const
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
