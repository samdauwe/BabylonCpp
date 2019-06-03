#ifndef BABYLON_MATERIALS_MATERIAL_FLAGS_H
#define BABYLON_MATERIALS_MATERIAL_FLAGS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This groups all the flags used to control the materials channel.
 */
class BABYLON_SHARED_EXPORT MaterialFlags {

public:
  /**
   * Are diffuse textures enabled in the application.
   */
  static bool DiffuseTextureEnabled();
  static void setDiffuseTextureEnabled(bool value);

  /**
   * Are ambient textures enabled in the application.
   */
  static bool AmbientTextureEnabled();
  static void setAmbientTextureEnabled(bool value);

  /**
   * Are opacity textures enabled in the application.
   */
  static bool OpacityTextureEnabled();
  static void setOpacityTextureEnabled(bool value);

  /**
   * Are reflection textures enabled in the application.
   */
  static bool ReflectionTextureEnabled();
  static void setReflectionTextureEnabled(bool value);

  /**
   * Are emissive textures enabled in the application.
   */
  static bool EmissiveTextureEnabled();
  static void setEmissiveTextureEnabled(bool value);

  /**
   * Are specular textures enabled in the application.
   */
  static bool SpecularTextureEnabled();
  static void setSpecularTextureEnabled(bool value);

  /**
   * Are bump textures enabled in the application.
   */
  static bool BumpTextureEnabled();
  static void setBumpTextureEnabled(bool value);

  /**
   * Are lightmap textures enabled in the application.
   */
  static bool LightmapTextureEnabled();
  static void setLightmapTextureEnabled(bool value);

  /**
   * Are refraction textures enabled in the application.
   */
  static bool RefractionTextureEnabled();
  static void setRefractionTextureEnabled(bool value);

  /**
   * Are color grading textures enabled in the application.
   */
  static bool ColorGradingTextureEnabled();
  static void setColorGradingTextureEnabled(bool value);

  /**
   * Are fresnels enabled in the application.
   */
  static bool FresnelEnabled();
  static void setFresnelEnabled(bool value);

  /**
   * Are clear coat textures enabled in the application.
   */
  static bool ClearCoatTextureEnabled();
  static void setClearCoatTextureEnabled(bool value);

  /**
   * Are clear coat bump textures enabled in the application.
   */
  static bool ClearCoatBumpTextureEnabled();
  static void setClearCoatBumpTextureEnabled(bool value);

  /**
   * Are clear coat tint textures enabled in the application.
   */
  static bool ClearCoatTintTextureEnabled();
  static void setClearCoatTintTextureEnabled(bool value);

  /**
   * Are sheen textures enabled in the application.
   */
  static bool SheenTextureEnabled();
  static void setSheenTextureEnabled(bool value);

  /**
   * Are anisotropic textures enabled in the application.
   */
  static bool AnisotropicTextureEnabled();
  static void setAnisotropicTextureEnabled(bool value);

  /**
   * Are thickness textures enabled in the application.
   */
  static bool ThicknessTextureEnabled();
  static void setThicknessTextureEnabled(bool value);

private:
  // Flags used to enable or disable a type of texture for all Standard
  // Materials
  static bool _DiffuseTextureEnabled;
  static bool _AmbientTextureEnabled;
  static bool _OpacityTextureEnabled;
  static bool _ReflectionTextureEnabled;
  static bool _EmissiveTextureEnabled;
  static bool _SpecularTextureEnabled;
  static bool _BumpTextureEnabled;
  static bool _LightmapTextureEnabled;
  static bool _RefractionTextureEnabled;
  static bool _ColorGradingTextureEnabled;
  static bool _FresnelEnabled;
  static bool _ClearCoatTextureEnabled;
  static bool _ClearCoatBumpTextureEnabled;
  static bool _ClearCoatTintTextureEnabled;
  static bool _SheenTextureEnabled;
  static bool _AnisotropicTextureEnabled;
  static bool _ThicknessTextureEnabled;

}; // end of struct MaterialFlags

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_FLAGS_H
