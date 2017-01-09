#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_MATERIAL_HELPER_STATICS_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_MATERIAL_HELPER_STATICS_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT ShaderMaterialHelperStatics {
  static constexpr bool Dark  = false;
  static constexpr bool Light = true;

  static const std::string PrecisionHighMode;
  static const std::string PrecisionMediumMode;

  static const std::string face_back;
  static const std::string face_front;

  static const std::string AttrPosition;
  static const std::string AttrNormal;
  static const std::string AttrUv;
  static const std::string AttrUv2;

  static const std::string AttrTypeForPosition;
  static const std::string AttrTypeForNormal;
  static const std::string AttrTypeForUv;
  static const std::string AttrTypeForUv2;

  static const std::string uniformView;
  static const std::string uniformWorld;
  static const std::string uniformWorldView;
  static const std::string uniformViewProjection;
  static const std::string uniformWorldViewProjection;

  static const std::string uniformStandardType;
  static const std::string uniformFlags;

  static const std::string Mouse;
  static const std::string Screen;
  static const std::string Camera;
  static const std::string Look;

  static const std::string Time;
  static const std::string GlobalTime;
  static const std::string Position;
  static const std::string WorldPosition;

  static const std::string Normal;
  static const std::string WorldNormal;
  static const std::string Uv;
  static const std::string Uv2;
  static const std::string Center;

  static const std::string ReflectMatrix;

  static const std::string Texture2D;
  static const std::string TextureCube;

  static int IdentityHelper;
}; // end of struct ShaderMaterialHelperStatics

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_MATERIAL_HELPER_STATICS_H
