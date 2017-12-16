#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_COLOR_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_COLOR_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/hexplanetgeneration/utils/gradient.h>

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
  std::unique_ptr<Gradient<Color3>> _terrainGradient;
  std::unique_ptr<ColorGradient2D> _terrainGradient2D;

}; // end of class Color

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_COLOR_H
