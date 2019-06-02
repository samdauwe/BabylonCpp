#include <babylon/misc/color3_gradient.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

Color3Gradient::Color3Gradient()
{
}

Color3Gradient::~Color3Gradient()
{
}

bool operator==(const Color3Gradient& lhs, const Color3Gradient& rhs)
{
  return stl_util::almost_equal(lhs.gradient, rhs.gradient)
         && lhs.color == rhs.color;
}

bool operator!=(const Color3Gradient& lhs, const Color3Gradient& rhs)
{
  return !(lhs == rhs);
}

} // end of namespace BABYLON
