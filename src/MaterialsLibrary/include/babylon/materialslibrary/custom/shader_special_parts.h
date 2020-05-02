#ifndef BABYLON_MATERIALS_LIBRARY_CUSTOM_SHADER_SPECIAL_PARTS_H
#define BABYLON_MATERIALS_LIBRARY_CUSTOM_SHADER_SPECIAL_PARTS_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace MaterialsLibrary {

struct BABYLON_SHARED_EXPORT ShaderSpecialParts {

  ShaderSpecialParts();
  ~ShaderSpecialParts(); // = default

  std::string Fragment_Begin;
  std::string Fragment_Definitions;
  std::string Fragment_MainBegin;

  // diffuseColor
  std::string Fragment_Custom_Diffuse;

  // lights
  std::string Fragment_Before_Lights;
  // fog
  std::string Fragment_Before_Fog;
  // alpha
  std::string Fragment_Custom_Alpha;

  std::string Fragment_Before_FragColor;

  std::string Vertex_Begin;
  std::string Vertex_Definitions;
  std::string Vertex_MainBegin;

  // positionUpdated
  std::string Vertex_Before_PositionUpdated;

  // normalUpdated
  std::string Vertex_Before_NormalUpdated;

  // worldPosComputed
  std::string Vertex_After_WorldPosComputed;

  // mainEnd
  std::string Vertex_MainEnd;

}; // end of class ShaderSpecialParts

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_CUSTOM_SHADER_SPECIAL_PARTS_H
