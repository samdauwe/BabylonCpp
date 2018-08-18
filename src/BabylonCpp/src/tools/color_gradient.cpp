#include <babylon/tools/color_gradient.h>

#include <babylon/core/random.h>

namespace BABYLON {

ColorGradient::ColorGradient() : color2{nullopt_t}
{
}

ColorGradient::~ColorGradient()
{
}

void ColorGradient::getColorToRef(Color4& result)
{
  if (!color2) {
    result.copyFrom(color1);
    return;
  }

  Color4::LerpToRef(color1, *color2, Math::random(), result);
}

bool operator==(const ColorGradient& lhs, const ColorGradient& rhs)
{
  return lhs.color1 == rhs.color1 && lhs.color2 == rhs.color2;
}

bool operator!=(const ColorGradient& lhs, const ColorGradient& rhs)
{
  return !(lhs == rhs);
}

} // end of namespace BABYLON
