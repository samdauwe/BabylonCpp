#ifndef BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PBRMaterialDefines
  : public MaterialDefines,
    public IImageProcessingConfigurationDefines {

  static constexpr unsigned int PBR = 0;

  static constexpr unsigned int MAINUV1 = 1;
  static constexpr unsigned int MAINUV2 = 2;
  static constexpr unsigned int UV1     = 3;
  static constexpr unsigned int UV2     = 4;

  static constexpr unsigned int ALBEDO      = 5;
  static constexpr unsigned int VERTEXCOLOR = 6;

  static constexpr unsigned int AMBIENT            = 7;
  static constexpr unsigned int AMBIENTINGRAYSCALE = 8;

  static constexpr unsigned int OPACITY           = 9;
  static constexpr unsigned int VERTEXALPHA       = 10;
  static constexpr unsigned int OPACITYRGB        = 11;
  static constexpr unsigned int ALPHATEST         = 12;
  static constexpr unsigned int ALPHABLEND        = 13;
  static constexpr unsigned int ALPHAFROMALBEDO   = 14;
  static constexpr unsigned int SPECULAROVERALPHA = 15;
  static constexpr unsigned int RADIANCEOVERALPHA = 16;
  static constexpr unsigned int ALPHAFRESNEL      = 17;
  static constexpr unsigned int PREMULTIPLYALPHA  = 18;

  static constexpr unsigned int EMISSIVE = 19;

  static constexpr unsigned int REFLECTIVITY = 20;
  static constexpr unsigned int SPECULARTERM = 21;

  static constexpr unsigned int MICROSURFACEFROMREFLECTIVITYMAP = 22;
  static constexpr unsigned int MICROSURFACEAUTOMATIC           = 23;
  static constexpr unsigned int LODBASEDMICROSFURACE            = 24;
  static constexpr unsigned int MICROSURFACEMAP                 = 25;

  static constexpr unsigned int METALLICWORKFLOW              = 26;
  static constexpr unsigned int ROUGHNESSSTOREINMETALMAPALPHA = 27;
  static constexpr unsigned int ROUGHNESSSTOREINMETALMAPGREEN = 28;
  static constexpr unsigned int METALLNESSSTOREINMETALMAPBLUE = 29;
  static constexpr unsigned int AOSTOREINMETALMAPRED          = 30;
  static constexpr unsigned int ENVIRONMENTBRDF               = 31;

  static constexpr unsigned int NORMAL            = 32;
  static constexpr unsigned int TANGENT           = 33;
  static constexpr unsigned int BUMP              = 34;
  static constexpr unsigned int PARALLAX          = 35;
  static constexpr unsigned int PARALLAXOCCLUSION = 36;
  static constexpr unsigned int NORMALXYSCALE     = 37;

  static constexpr unsigned int LIGHTMAP               = 38;
  static constexpr unsigned int USELIGHTMAPASSHADOWMAP = 39;

  static constexpr unsigned int REFLECTION                          = 40;
  static constexpr unsigned int REFLECTIONMAP_3D                    = 41;
  static constexpr unsigned int REFLECTIONMAP_SPHERICAL             = 42;
  static constexpr unsigned int REFLECTIONMAP_PLANAR                = 43;
  static constexpr unsigned int REFLECTIONMAP_CUBIC                 = 44;
  static constexpr unsigned int REFLECTIONMAP_PROJECTION            = 45;
  static constexpr unsigned int REFLECTIONMAP_SKYBOX                = 46;
  static constexpr unsigned int REFLECTIONMAP_EXPLICIT              = 47;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR       = 48;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR_FIXED = 49;
  static constexpr unsigned int REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED
    = 50;
  static constexpr unsigned int INVERTCUBICMAP                = 51;
  static constexpr unsigned int USESPHERICALFROMREFLECTIONMAP = 52;
  static constexpr unsigned int USESPHERICALINFRAGMENT        = 53;
  static constexpr unsigned int REFLECTIONMAP_OPPOSITEZ       = 54;
  static constexpr unsigned int LODINREFLECTIONALPHA          = 55;
  static constexpr unsigned int GAMMAREFLECTION               = 56;

  static constexpr unsigned int REFRACTION                   = 57;
  static constexpr unsigned int REFRACTIONMAP_3D             = 58;
  static constexpr unsigned int REFRACTIONMAP_OPPOSITEZ      = 59;
  static constexpr unsigned int LODINREFRACTIONALPHA         = 60;
  static constexpr unsigned int GAMMAREFRACTION              = 61;
  static constexpr unsigned int LINKREFRACTIONTOTRANSPARENCY = 62;

  static constexpr unsigned int INSTANCES = 63;

  static constexpr unsigned int MORPHTARGETS         = 64;
  static constexpr unsigned int MORPHTARGETS_NORMAL  = 65;
  static constexpr unsigned int MORPHTARGETS_TANGENT = 66;

  static constexpr unsigned int IMAGEPROCESSING            = 67;
  static constexpr unsigned int VIGNETTE                   = 68;
  static constexpr unsigned int VIGNETTEBLENDMODEMULTIPLY  = 69;
  static constexpr unsigned int VIGNETTEBLENDMODEOPAQUE    = 70;
  static constexpr unsigned int TONEMAPPING                = 71;
  static constexpr unsigned int CONTRAST                   = 72;
  static constexpr unsigned int COLORCURVES                = 73;
  static constexpr unsigned int COLORGRADING               = 74;
  static constexpr unsigned int SAMPLER3DGREENDEPTH        = 75;
  static constexpr unsigned int SAMPLER3DBGRMAP            = 76;
  static constexpr unsigned int IMAGEPROCESSINGPOSTPROCESS = 77;
  static constexpr unsigned int EXPOSURE                   = 78;

  static constexpr unsigned int USEPHYSICALLIGHTFALLOFF = 81;
  static constexpr unsigned int TWOSIDEDLIGHTING        = 82;
  static constexpr unsigned int SHADOWFLOAT             = 83;
  static constexpr unsigned int USERIGHTHANDEDSYSTEM    = 84;
  static constexpr unsigned int CLIPPLANE               = 85;
  static constexpr unsigned int POINTSIZE               = 86;
  static constexpr unsigned int FOG                     = 87;
  static constexpr unsigned int LOGARITHMICDEPTH        = 88;

  static constexpr unsigned int FORCENORMALFORWARD = 89;

  PBRMaterialDefines();
  ~PBRMaterialDefines();

  void reset() override;

}; // end of struct PBRMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H
