#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_ILIGHT_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_ILIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/shaderbuilder/icolor.h>
#include <babylon/extensions/shaderbuilder/ivector3.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT ILight {
  std::string direction;
  IVector3 rotation;
  IColor color;
  bool darkColorMode;
  IColor specular;
  float specularPower;
  int specularLevel;
  IColor phonge;
  float phongePower;
  float phongeLevel;
  std::string normal;
  std::string reducer;
  bool supplement;
  bool parallel;
}; // end of struct ILight

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_ILIGHT_H
