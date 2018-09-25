#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_COLOR_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_COLOR_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/hexplanetgeneration/utils/gradient.h>
#include <babylon/extensions/hexplanetgeneration/utils/color_gradient_2d.h>

namespace BABYLON {
namespace Extensions {

class ColorGradient2D;

class Color {

public:
  Color(float maxHeight);
  ~Color();

  void setupGradient();
  Color3 getColor(float heightNorm, float rainNorm, float tempNorm);
  Color3 get2DColor(float u, float v);

private:
  float _maxHeight;
  float _parchmentFactor;
  Gradient<Color3> _terrainGradient;
  ColorGradient2D _terrainGradient2D;

}; // end of class Color

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_COLOR_H
