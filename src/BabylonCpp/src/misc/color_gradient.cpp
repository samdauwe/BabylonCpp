#include <babylon/misc/color_gradient.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/random.h>

namespace BABYLON {

ColorGradient::ColorGradient(float iGradient, const Color4& iColor1,
                             const std::optional<Color4>& iColor2)
    : IValueGradient{iGradient}, color1{iColor1}, color2{iColor2}
{
}

ColorGradient::~ColorGradient() = default;

void ColorGradient::getColorToRef(Color4& result)
{
  if (!color2.has_value()) {
    result.copyFrom(color1);
    return;
  }

  Color4::LerpToRef(color1, *color2, Math::random(), result);
}

bool operator==(const ColorGradient& lhs, const ColorGradient& rhs)
{
  return stl_util::almost_equal(lhs.gradient, rhs.gradient) && lhs.color1 == rhs.color1
         && lhs.color2 == rhs.color2;
}

bool operator!=(const ColorGradient& lhs, const ColorGradient& rhs)
{
  return !(lhs == rhs);
}

} // end of namespace BABYLON
