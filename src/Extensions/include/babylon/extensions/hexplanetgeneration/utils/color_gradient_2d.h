#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_COLOR_GRADIENT_2D_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_COLOR_GRADIENT_2D_H

#include <babylon/babylon_global.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace Extensions {

struct ColorStop {
  float u;
  float v;
  Color3 color;
  bool operator==(const ColorStop& rhs) const;
}; // end of struct ColorStop

struct NearestNeighbors {
  ColorStop topLeft;
  ColorStop topRight;
  ColorStop bottomLeft;
  ColorStop bottomRight;
}; // end of struct NearestNeighbors

class ColorGradient2D {

public:
  ColorGradient2D(float resolution);
  ~ColorGradient2D();

  void addColorStop(float u, float v, const Color3& color);
  Color3 getColor(float u, float v) const;
  void calculate();
  bool isConcentricWithColorStop(const NearestNeighbors& neighbors) const;
  bool onUEdge(const NearestNeighbors& neighbors) const;
  bool onVEdge(const NearestNeighbors& neighbors) const;
  NearestNeighbors getNearestNeighbors(float u, float v) const;

private:
  std::vector<ColorStop> _colorStops;
  float _resolution;
  std::vector<std::vector<Color3>> _gradient;

}; // end of class ColorGradient2D

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_COLOR_GRADIENT_2D_H
