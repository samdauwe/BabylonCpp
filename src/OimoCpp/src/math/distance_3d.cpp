#include <oimo/math/distance_3d.h>

#include <cmath>

namespace OIMO {

float Distance3d::GetDistance3d(const Vec3& p1, const Vec3& p2)
{
  float xd = p2.x - p1.x;
  float yd = p2.y - p1.y;
  float zd = p2.z - p1.z;
  return std::sqrt(xd * xd + yd * yd + zd * zd);
}

} // end of namespace OIMO
