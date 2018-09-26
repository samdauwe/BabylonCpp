#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_STRUCT_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_STRUCT_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT ShaderStruct {
  std::string Pixel;
  std::string Vertex;
}; // end of struct ShaderStruct

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_STRUCT_H
