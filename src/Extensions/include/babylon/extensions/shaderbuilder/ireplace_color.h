#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_IREPLACE_COLOR_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_IREPLACE_COLOR_H

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT IReplaceColor {
  int colorIndex;
  float colorStep;
  bool indexToEnd;
  float rangeStep;
  float rangePower;
}; // end of struct IReplaceColor

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_IREPLACE_COLOR_H
