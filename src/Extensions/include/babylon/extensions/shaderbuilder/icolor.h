#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_ICOLOR_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_ICOLOR_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT IColor {
  float r = 0.f;
  float g = 0.f;
  float b = 0.f;
  float a = 1.f;
}; // end of struct IColor

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_ICOLOR_H
