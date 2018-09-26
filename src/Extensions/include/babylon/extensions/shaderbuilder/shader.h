#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/shaderbuilder/shader_builder.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT Shader {

  static constexpr const char* _null = "set null anyway";
  static unsigned int Indexer;
  static unsigned int ShaderIdentity;
  static ShaderBuilder* Me;

  static std::string Replace(const std::string s, const std::string t,
                             const std::string d);
  static float Def(float a, float b);
  static std::string Def(const std::string& a, const std::string& b);
  static std::string Join(const std::vector<std::string>& s);
  static std::string Print(int i);
  static std::string Print(float i);
  static std::string Print(const std::string& s);
  static std::string Print(const IColor& s);
  static std::string Custom();
  static std::string Index();
  static std::string DefCustom(const std::string& t, const std::string& c);
  static IColor toRGB(int a, int b);
  static IColor torgb(int a, int b);
  static float toID(int a, int b);

}; // end of struct Shader

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_H
