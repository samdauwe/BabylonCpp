#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_MATERIAL_HELPER_STATICS_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_MATERIAL_HELPER_STATICS_H

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT ShaderMaterialHelperStatics {
  static constexpr bool Dark  = false;
  static constexpr bool Light = true;

  static constexpr const char* PrecisionHighMode   = "highp";
  static constexpr const char* PrecisionMediumMode = "mediump";

  static constexpr const char* face_back  = "!gl_FrontFacing";
  static constexpr const char* face_front = "gl_FrontFacing";

  static constexpr const char* AttrPosition = "position";
  static constexpr const char* AttrNormal   = "normal";
  static constexpr const char* AttrUv       = "uv";
  static constexpr const char* AttrUv2      = "uv2";

  static constexpr const char* AttrTypeForPosition = "vec3";
  static constexpr const char* AttrTypeForNormal   = "vec3";
  static constexpr const char* AttrTypeForUv       = "vec2";
  static constexpr const char* AttrTypeForUv2      = "vec2";

  static constexpr const char* uniformView           = "view";
  static constexpr const char* uniformWorld          = "world";
  static constexpr const char* uniformWorldView      = "worldView";
  static constexpr const char* uniformViewProjection = "viewProjection";
  static constexpr const char* uniformWorldViewProjection
    = "worldViewProjection";

  static constexpr const char* uniformStandardType = "mat4";
  static constexpr const char* uniformFlags        = "flags";

  static constexpr const char* Mouse  = "mouse";
  static constexpr const char* Screen = "screen";
  static constexpr const char* Camera = "camera";
  static constexpr const char* Look   = "look";

  static constexpr const char* Time          = "time";
  static constexpr const char* GlobalTime    = "gtime";
  static constexpr const char* Position      = "pos";
  static constexpr const char* WorldPosition = "wpos";

  static constexpr const char* Normal      = "nrm";
  static constexpr const char* WorldNormal = "wnrm";
  static constexpr const char* Uv          = "vuv";
  static constexpr const char* Uv2         = "vuv2";
  static constexpr const char* Center      = "center";

  static constexpr const char* ReflectMatrix = "refMat";

  static constexpr const char* Texture2D   = "txtRef_";
  static constexpr const char* TextureCube = "cubeRef_";

  static int IdentityHelper;
}; // end of struct ShaderMaterialHelperStatics

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_MATERIAL_HELPER_STATICS_H
