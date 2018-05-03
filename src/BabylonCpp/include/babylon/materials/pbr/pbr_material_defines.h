#ifndef BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

/**
 * @brief Manages the defines for the PBR Material.
 */
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
  static constexpr unsigned int LODBASEDMICROSFURACE            = 26;
  static constexpr unsigned int MICROSURFACEMAP                 = 27;

  static constexpr unsigned int METALLICWORKFLOW              = 28;
  static constexpr unsigned int ROUGHNESSSTOREINMETALMAPALPHA = 29;
  static constexpr unsigned int ROUGHNESSSTOREINMETALMAPGREEN = 30;
  static constexpr unsigned int METALLNESSSTOREINMETALMAPBLUE = 31;
  static constexpr unsigned int AOSTOREINMETALMAPRED          = 32;
  static constexpr unsigned int ENVIRONMENTBRDF               = 33;

  static constexpr unsigned int NORMAL                = 34;
  static constexpr unsigned int TANGENT               = 35;
  static constexpr unsigned int BUMP                  = 36;
  static constexpr unsigned int OBJECTSPACE_NORMALMAP = 37;
  static constexpr unsigned int PARALLAX              = 38;
  static constexpr unsigned int PARALLAXOCCLUSION     = 39;
  static constexpr unsigned int NORMALXYSCALE         = 40;

  static constexpr unsigned int LIGHTMAP               = 41;
  static constexpr unsigned int USELIGHTMAPASSHADOWMAP = 42;
  static constexpr unsigned int GAMMALIGHTMAP          = 43;

  static constexpr unsigned int REFLECTION                          = 44;
  static constexpr unsigned int REFLECTIONMAP_3D                    = 45;
  static constexpr unsigned int REFLECTIONMAP_SPHERICAL             = 46;
  static constexpr unsigned int REFLECTIONMAP_PLANAR                = 47;
  static constexpr unsigned int REFLECTIONMAP_CUBIC                 = 48;
  static constexpr unsigned int USE_LOCAL_REFLECTIONMAP_CUBIC       = 49;
  static constexpr unsigned int REFLECTIONMAP_PROJECTION            = 50;
  static constexpr unsigned int REFLECTIONMAP_SKYBOX                = 51;
  static constexpr unsigned int REFLECTIONMAP_EXPLICIT              = 52;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR       = 53;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR_FIXED = 54;
  static constexpr unsigned int REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED
    = 55;
  static constexpr unsigned int INVERTCUBICMAP                = 56;
  static constexpr unsigned int USESPHERICALFROMREFLECTIONMAP = 57;
  static constexpr unsigned int USESPHERICALINVERTEX          = 58;
  static constexpr unsigned int REFLECTIONMAP_OPPOSITEZ       = 59;
  static constexpr unsigned int LODINREFLECTIONALPHA          = 60;
  static constexpr unsigned int GAMMAREFLECTION               = 61;
  static constexpr unsigned int RADIANCEOCCLUSION             = 62;
  static constexpr unsigned int HORIZONOCCLUSION              = 63;

  static constexpr unsigned int REFRACTION                   = 64;
  static constexpr unsigned int REFRACTIONMAP_3D             = 65;
  static constexpr unsigned int REFRACTIONMAP_OPPOSITEZ      = 66;
  static constexpr unsigned int LODINREFRACTIONALPHA         = 67;
  static constexpr unsigned int GAMMAREFRACTION              = 68;
  static constexpr unsigned int LINKREFRACTIONTOTRANSPARENCY = 69;

  static constexpr unsigned int INSTANCES = 70;

  static constexpr unsigned int NONUNIFORMSCALING = 71;

  static constexpr unsigned int MORPHTARGETS         = 72;
  static constexpr unsigned int MORPHTARGETS_NORMAL  = 73;
  static constexpr unsigned int MORPHTARGETS_TANGENT = 74;

  static constexpr unsigned int IMAGEPROCESSING            = 75;
  static constexpr unsigned int VIGNETTE                   = 76;
  static constexpr unsigned int VIGNETTEBLENDMODEMULTIPLY  = 77;
  static constexpr unsigned int VIGNETTEBLENDMODEOPAQUE    = 78;
  static constexpr unsigned int TONEMAPPING                = 79;
  static constexpr unsigned int CONTRAST                   = 80;
  static constexpr unsigned int COLORCURVES                = 81;
  static constexpr unsigned int COLORGRADING               = 82;
  static constexpr unsigned int COLORGRADING3D             = 83;
  static constexpr unsigned int SAMPLER3DGREENDEPTH        = 84;
  static constexpr unsigned int SAMPLER3DBGRMAP            = 85;
  static constexpr unsigned int IMAGEPROCESSINGPOSTPROCESS = 86;
  static constexpr unsigned int EXPOSURE                   = 87;

  static constexpr unsigned int USEPHYSICALLIGHTFALLOFF = 88;
  static constexpr unsigned int TWOSIDEDLIGHTING        = 89;
  static constexpr unsigned int SHADOWFLOAT             = 90;
  static constexpr unsigned int CLIPPLANE               = 91;
  static constexpr unsigned int POINTSIZE               = 92;
  static constexpr unsigned int FOG                     = 93;
  static constexpr unsigned int LOGARITHMICDEPTH        = 94;

  static constexpr unsigned int FORCENORMALFORWARD = 95;

  static constexpr unsigned int GEOMETRYAA = 96;

  static constexpr unsigned int UNLIT = 97;

  /**
   * @brief Initializes the PBR Material defines.
   */
  PBRMaterialDefines();
  ~PBRMaterialDefines() override;

  /**
   * @brief Resets the PBR Material defines.
   */
  void reset() override;

}; // end of struct PBRMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_MATERIAL_DEFINES_H
