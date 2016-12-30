#include <babylon/collisions/intersection_info.h>

namespace BABYLON {

IntersectionInfo::IntersectionInfo(float _distance) : distance{_distance}
{
}

IntersectionInfo::IntersectionInfo(float _bu, float _bv, float _distance)
    : bu{_bu}, bv{_bv}, distance{_distance}
{
}

IntersectionInfo::~IntersectionInfo()
{
}

} // end of namespace BABYLON
