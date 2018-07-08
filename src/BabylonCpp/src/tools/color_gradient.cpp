#include <babylon/tools/color_gradient.h>

#include <babylon/core/random.h>

namespace BABYLON {

ColorGradient::ColorGradient() : color2{nullptr}
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

} // end of namespace BABYLON
