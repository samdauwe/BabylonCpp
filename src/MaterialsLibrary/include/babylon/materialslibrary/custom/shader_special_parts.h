#ifndef BABYLON_MATERIALS_LIBRARY_CUSTOM_SHADER_SPECIAL_PARTS_H
#define BABYLON_MATERIALS_LIBRARY_CUSTOM_SHADER_SPECIAL_PARTS_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT ShaderSpecialParts {

  ShaderSpecialParts();
  ~ShaderSpecialParts();

  string_t Fragment_Begin;
  string_t Fragment_Definitions;
  string_t Fragment_MainBegin;

  // diffuseColor
  string_t Fragment_Custom_Diffuse;

  // alpha
  string_t Fragment_Custom_Alpha;

  string_t Fragment_Before_FragColor;

  string_t Vertex_Begin;
  string_t Vertex_Definitions;
  string_t Vertex_MainBegin;

  // positionUpdated
  string_t Vertex_Before_PositionUpdated;

  // normalUpdated
  string_t Vertex_Before_NormalUpdated;

}; // end of class ShaderSpecialParts

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_CUSTOM_SHADER_SPECIAL_PARTS_H
