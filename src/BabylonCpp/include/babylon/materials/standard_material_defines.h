#ifndef BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT StandardMaterialDefines
    : public MaterialDefines,
      public IImageProcessingConfigurationDefines {

  static constexpr unsigned int MAINUV1                             = 0;
  static constexpr unsigned int MAINUV2                             = 1;
  static constexpr unsigned int DIFFUSE                             = 2;
  static constexpr unsigned int AMBIENT                             = 3;
  static constexpr unsigned int OPACITY                             = 4;
  static constexpr unsigned int OPACITYRGB                          = 5;
  static constexpr unsigned int REFLECTION                          = 6;
  static constexpr unsigned int EMISSIVE                            = 7;
  static constexpr unsigned int SPECULAR                            = 8;
  static constexpr unsigned int BUMP                                = 9;
  static constexpr unsigned int PARALLAX                            = 10;
  static constexpr unsigned int PARALLAXOCCLUSION                   = 11;
  static constexpr unsigned int SPECULAROVERALPHA                   = 12;
  static constexpr unsigned int CLIPPLANE                           = 13;
  static constexpr unsigned int ALPHATEST                           = 14;
  static constexpr unsigned int DEPTHPREPASS                        = 15;
  static constexpr unsigned int ALPHAFROMDIFFUSE                    = 16;
  static constexpr unsigned int POINTSIZE                           = 17;
  static constexpr unsigned int FOG                                 = 18;
  static constexpr unsigned int SPECULARTERM                        = 19;
  static constexpr unsigned int DIFFUSEFRESNEL                      = 20;
  static constexpr unsigned int OPACITYFRESNEL                      = 21;
  static constexpr unsigned int REFLECTIONFRESNEL                   = 22;
  static constexpr unsigned int REFRACTIONFRESNEL                   = 23;
  static constexpr unsigned int EMISSIVEFRESNEL                     = 24;
  static constexpr unsigned int FRESNEL                             = 25;
  static constexpr unsigned int NORMAL                              = 26;
  static constexpr unsigned int UV1                                 = 27;
  static constexpr unsigned int UV2                                 = 28;
  static constexpr unsigned int VERTEXCOLOR                         = 29;
  static constexpr unsigned int VERTEXALPHA                         = 30;
  static constexpr unsigned int INSTANCES                           = 31;
  static constexpr unsigned int GLOSSINESS                          = 32;
  static constexpr unsigned int ROUGHNESS                           = 33;
  static constexpr unsigned int EMISSIVEASILLUMINATION              = 34;
  static constexpr unsigned int LINKEMISSIVEWITHDIFFUSE             = 35;
  static constexpr unsigned int REFLECTIONFRESNELFROMSPECULAR       = 36;
  static constexpr unsigned int LIGHTMAP                            = 37;
  static constexpr unsigned int USELIGHTMAPASSHADOWMAP              = 38;
  static constexpr unsigned int REFLECTIONMAP_3D                    = 39;
  static constexpr unsigned int REFLECTIONMAP_SPHERICAL             = 40;
  static constexpr unsigned int REFLECTIONMAP_PLANAR                = 41;
  static constexpr unsigned int REFLECTIONMAP_CUBIC                 = 42;
  static constexpr unsigned int USE_LOCAL_REFLECTIONMAP_CUBIC       = 43;
  static constexpr unsigned int REFLECTIONMAP_PROJECTION            = 44;
  static constexpr unsigned int REFLECTIONMAP_SKYBOX                = 45;
  static constexpr unsigned int REFLECTIONMAP_EXPLICIT              = 45;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR       = 47;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR_FIXED = 48;
  static constexpr unsigned int REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED
    = 49;
  static constexpr unsigned int INVERTCUBICMAP       = 50;
  static constexpr unsigned int LOGARITHMICDEPTH     = 51;
  static constexpr unsigned int REFRACTION           = 52;
  static constexpr unsigned int REFRACTIONMAP_3D     = 53;
  static constexpr unsigned int REFLECTIONOVERALPHA  = 54;
  static constexpr unsigned int TWOSIDEDLIGHTING     = 55;
  static constexpr unsigned int SHADOWFLOAT          = 56;
  static constexpr unsigned int MORPHTARGETS         = 57;
  static constexpr unsigned int MORPHTARGETS_NORMAL  = 58;
  static constexpr unsigned int MORPHTARGETS_TANGENT = 59;
  static constexpr unsigned int NONUNIFORMSCALING
    = 60; // https://playground.babylonjs.com#V6DWIH
  static constexpr unsigned int PREMULTIPLYALPHA
    = 61; // https://playground.babylonjs.com#LNVJJ7

  static constexpr unsigned int IMAGEPROCESSING            = 62;
  static constexpr unsigned int VIGNETTE                   = 63;
  static constexpr unsigned int VIGNETTEBLENDMODEMULTIPLY  = 64;
  static constexpr unsigned int VIGNETTEBLENDMODEOPAQUE    = 65;
  static constexpr unsigned int TONEMAPPING                = 66;
  static constexpr unsigned int CONTRAST                   = 67;
  static constexpr unsigned int COLORCURVES                = 68;
  static constexpr unsigned int COLORGRADING               = 68;
  static constexpr unsigned int COLORGRADING3D             = 70;
  static constexpr unsigned int SAMPLER3DGREENDEPTH        = 71;
  static constexpr unsigned int SAMPLER3DBGRMAP            = 72;
  static constexpr unsigned int IMAGEPROCESSINGPOSTPROCESS = 73;
  static constexpr unsigned int EXPOSURE                   = 74;

  StandardMaterialDefines();
  ~StandardMaterialDefines();

  void setReflectionMode(unsigned int modeToEnable);

}; // end of struct StandardMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H
