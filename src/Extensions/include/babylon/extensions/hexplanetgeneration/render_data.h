#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_RENDER_DATA_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_PLANET_RENDER_DATA_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Color4;
class Vector3;

namespace Extensions {

struct RenderObject {
  void colour(const Color4& /*color*/){}
  void normal(const Vector3& /*normal*/){}
  void position(const Vector3& /*position*/){}
  void triangle(size_t /*p1*/, size_t /*p2*/, size_t /*p3*/){}
  size_t getCurrentVertexCount(){return 0;}
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
