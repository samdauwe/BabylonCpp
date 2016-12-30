#include <babylon/tools/hdr/cmg_bounding_box.h>

namespace BABYLON {
namespace Internals {

float CMGBoundinBox::MAX = std::numeric_limits<float>::max();
float CMGBoundinBox::MIN = std::numeric_limits<float>::max();

CMGBoundinBox::CMGBoundinBox()
    : min{Vector3(0.f, 0.f, 0.f)}, max{Vector3(0.f, 0.f, 0.f)}
{
  clear();
}

CMGBoundinBox::~CMGBoundinBox()
{
}

void CMGBoundinBox::clear()
{
  min.x = CMGBoundinBox::MAX;
  min.y = CMGBoundinBox::MAX;
  min.z = CMGBoundinBox::MAX;

  max.x = CMGBoundinBox::MIN;
  max.y = CMGBoundinBox::MIN;
  max.z = CMGBoundinBox::MIN;
}

void CMGBoundinBox::augment(float x, float y, float z)
{
  min.x = std::min(min.x, x);
  min.y = std::min(min.y, y);
  min.z = std::min(min.z, z);
  max.x = std::max(max.x, x);
  max.y = std::max(max.y, y);
  max.z = std::max(max.z, z);
}

void CMGBoundinBox::clampMin(float x, float y, float z)
{
  min.x = std::max(min.x, x);
  min.y = std::max(min.y, y);
  min.z = std::max(min.z, z);
}

void CMGBoundinBox::clampMax(float x, float y, float z)
{
  max.x = std::min(max.x, x);
  max.y = std::min(max.y, y);
  max.z = std::min(max.z, z);
}

bool CMGBoundinBox::empty() const
{
  if ((min.x > max.y) || (min.y > max.y) || (min.z > max.y)) {
    return true;
  }
  else {
    return false;
  }
}

} // end of namespace Internals
} // end of namespace BABYLON
