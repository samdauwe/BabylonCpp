#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_HELPER_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT Helper {
  static constexpr unsigned int Red    = 0;
  static constexpr unsigned int Yellow = 1;
  static constexpr unsigned int White  = 2;
  static constexpr unsigned int Cyan   = 4;
  static constexpr unsigned int Blue   = 5;
  static constexpr unsigned int Pink   = 6;
  static constexpr unsigned int Black  = 7;
  static constexpr unsigned int Green  = 8;
}; // end of struct Helper

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_HELPER_H
