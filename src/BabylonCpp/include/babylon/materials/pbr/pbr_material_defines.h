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

  static constexpr unsigned int OPACITY            = 9;
  static constexpr unsigned int VERTEXALPHA        = 10;
  static constexpr unsigned int OPACITYRGB         = 11;
  static constexpr unsigned int ALPHATEST          = 12;
  static constexpr unsigned int DEPTHPREPASS       = 13;
  static constexpr unsigned int ALPHABLEND         = 14;
  static constexpr unsigned int ALPHAFROMALBEDO    = 15;
  static constexpr unsigned int SPECULAROVERALPHA  = 16;
  static constexpr unsigned int RADIANCEOVERALPHA  = 17;
  static constexpr unsigned int ALPHAFRESNEL       = 18;
  static constexpr unsigned int LINEARALPHAFRESNEL = 19;
  static constexpr unsigned int PREMULTIPLYALPHA   = 20;

  static constexpr unsigned int EMISSIVE = 21;

  static constexpr unsigned int REFLECTIVITY = 22;
  static constexpr unsigned int SPECULARTERM = 23;

  static constexpr unsigned int MICROSURFACEFROMREFLECTIVITYMAP = 24;
  static constexpr unsigned int MICROSURFACEAUTOMATIC           = 25;
  static constexpr unsigned int LODBASEDMICROSFURACE            = 25;
  static constexpr unsigned int MICROSURFACEMAP                 = 27;

  static constexpr unsigned int METALLICWORKFLOW              = 28;
  static constexpr unsigned int ROUGHNESSSTOREINMETALMAPALPHA = 29;
  static constexpr unsigned int ROUGHNESSSTOREINMETALMAPGREEN = 30;
  static constexpr unsigned int METALLNESSSTOREINMETALMAPBLUE = 31;
  static constexpr unsigned int AOSTOREINMETALMAPRED          = 32;
  static constexpr unsigned int ENVIRONMENTBRDF               = 33;

  static constexpr unsigned int NORMAL            = 34;
  static constexpr unsigned int TANGENT           = 35;
  static constexpr unsigned int BUMP              = 36;
  static constexpr unsigned int PARALLAX          = 37;
  static constexpr unsigned int PARALLAXOCCLUSION = 38;
  static constexpr unsigned int NORMALXYSCALE     = 39;

  static constexpr unsigned int LIGHTMAP               = 40;
  static constexpr unsigned int USELIGHTMAPASSHADOWMAP = 41;

  static constexpr unsigned int REFLECTION                          = 42;
  static constexpr unsigned int REFLECTIONMAP_3D                    = 43;
  static constexpr unsigned int REFLECTIONMAP_SPHERICAL             = 44;
  static constexpr unsigned int REFLECTIONMAP_PLANAR                = 45;
  static constexpr unsigned int REFLECTIONMAP_CUBIC                 = 46;
  static constexpr unsigned int REFLECTIONMAP_PROJECTION            = 47;
  static constexpr unsigned int REFLECTIONMAP_SKYBOX                = 48;
  static constexpr unsigned int REFLECTIONMAP_EXPLICIT              = 49;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR       = 50;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR_FIXED = 51;
  static constexpr unsigned int REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED
    = 52;
  static constexpr unsigned int INVERTCUBICMAP                = 53;
  static constexpr unsigned int USESPHERICALFROMREFLECTIONMAP = 54;
  static constexpr unsigned int USESPHERICALINVERTEX          = 55;
  static constexpr unsigned int REFLECTIONMAP_OPPOSITEZ       = 56;
  static constexpr unsigned int LODINREFLECTIONALPHA          = 57;
  static constexpr unsigned int GAMMAREFLECTION               = 58;
  static constexpr unsigned int RADIANCEOCCLUSION             = 59;
  static constexpr unsigned int HORIZONOCCLUSION              = 60;

  static constexpr unsigned int REFRACTION                   = 61;
  static constexpr unsigned int REFRACTIONMAP_3D             = 62;
  static constexpr unsigned int REFRACTIONMAP_OPPOSITEZ      = 63;
  static constexpr unsigned int LODINREFRACTIONALPHA         = 64;
  static constexpr unsigned int GAMMAREFRACTION              = 65;
  static constexpr unsigned int LINKREFRACTIONTOTRANSPARENCY = 66;

  static constexpr unsigned int INSTANCES = 67;

  static constexpr unsigned int NONUNIFORMSCALING = 68;

  static constexpr unsigned int MORPHTARGETS         = 69;
  static constexpr unsigned int MORPHTARGETS_NORMAL  = 70;
  static constexpr unsigned int MORPHTARGETS_TANGENT = 71;

  static constexpr unsigned int IMAGEPROCESSING            = 72;
  static constexpr unsigned int VIGNETTE                   = 73;
  static constexpr unsigned int VIGNETTEBLENDMODEMULTIPLY  = 74;
  static constexpr unsigned int VIGNETTEBLENDMODEOPAQUE    = 75;
  static constexpr unsigned int TONEMAPPING                = 76;
  static constexpr unsigned int CONTRAST                   = 77;
  static constexpr unsigned int COLORCURVES                = 78;
  static constexpr unsigned int COLORGRADING               = 79;
  static constexpr unsigned int COLORGRADING3D             = 80;
  static constexpr unsigned int SAMPLER3DGREENDEPTH        = 81;
  static constexpr unsigned int SAMPLER3DBGRMAP            = 82;
  static constexpr unsigned int IMAGEPROCESSINGPOSTPROCESS = 83;
  static constexpr unsigned int EXPOSURE                   = 84;

  static constexpr unsigned int USEPHYSICALLIGHTFALLOFF = 85;
  static constexpr unsigned int TWOSIDEDLIGHTING        = 86;
  static constexpr unsigned int SHADOWFLOAT             = 87;
  static constexpr unsigned int USERIGHTHANDEDSYSTEM    = 88;
  static constexpr unsigned int CLIPPLANE               = 89;
  static constexpr unsigned int POINTSIZE               = 90;
  static constexpr unsigned int FOG                     = 91;
  static constexpr unsigned int LOGARITHMICDEPTH        = 92;

  static constexpr unsigned int FORCENORMALFORWARD = 93;

  PBRMaterialDefines();
  ~PBRMaterialDefines() override;

  void reset() override;

}; // end of struct PBRMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H
