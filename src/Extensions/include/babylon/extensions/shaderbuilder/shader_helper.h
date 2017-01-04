#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_HELPER_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT ShaderHelper {
  std::vector<std::string> uniforms;
  std::vector<std::string> attributes;
}; // end of struct ShaderHelper

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_HELPER_H
