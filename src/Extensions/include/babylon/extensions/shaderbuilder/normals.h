#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_NORMALS_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_NORMALS_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT Normals {
  static const std::string Default;
  static const std::string Inverse;
  static const std::string Pointed;
  static const std::string Flat;
  static const std::string Map;
}; // end of struct Normals

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_NORMALS_H
