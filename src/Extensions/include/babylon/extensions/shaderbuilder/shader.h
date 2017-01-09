#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/shaderbuilder/shader_builder.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT Shader {

  static constexpr const char* _null           = "set null anyway";
  static constexpr unsigned int Indexer        = 1;
  static constexpr unsigned int ShaderIdentity = 0;
  static ShaderBuilder Me;

  static std::string Replace(const std::string s, const std::string t,
                             const std::string d);
  static std::string Join(const std::vector<std::string>& s);
  static std::string Print(const std::string& s);

}; // end of struct Shader

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_H
