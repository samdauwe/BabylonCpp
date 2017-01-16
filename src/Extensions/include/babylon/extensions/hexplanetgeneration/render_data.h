#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_RENDER_DATA_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_RENDER_DATA_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct RenderObject {
  void colour(){}
  void normal(){}
  void position(){}
  void triangle(size_t /*p1*/, size_t /*p2*/, size_t /*p3*/){}
}; // end of struct RenderObject

struct RenderData {
  RenderObject surface;
  RenderObject plateBoundaries;
  RenderObject plateMovements;
  RenderObject airCurrents;
}; // end of struct RenderData

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_RENDER_DATA_H
