#ifndef BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/iimage_processing_configuration_defines.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

/**
 * @brief Background material defines definition.
 * Ignore Mainly internal Use
 */
struct BABYLON_SHARED_EXPORT BackgroundMaterialDefines
    : public MaterialDefines,
      public IImageProcessingConfigurationDefines {

  /**
   * True if the diffuse texture is in use.
   */
  static constexpr unsigned int DIFFUSE = 0;

  /**
   * True if the diffuse texture is in gamma space.
   */
  static constexpr unsigned int GAMMADIFFUSE = 1;

  /**
   * True if the diffuse texture has opacity in the alpha channel.
   */
  static constexpr unsigned int DIFFUSEHASALPHA = 2;

  /**
   * True if you want the material to fade to transparent at grazing angle.
   */
  static constexpr unsigned int OPACITYFRESNEL = 3;

  /**
   * True if an extra blur needs to be added in the reflection.
   */
  static constexpr unsigned int REFLECTIONBLUR = 4;

  /**
   * True if you want the material to fade to reflection at grazing angle.
   */
  static constexpr unsigned int REFLECTIONFRESNEL = 5;

  /**
   * True if you want the material to falloff as far as you move away from the
   * scene center.
   */
  static constexpr unsigned int REFLECTIONFALLOFF = 6;

  /**
   * False if the current Webgl implementation does not support the texture lod
   * extension.
   */
  static constexpr unsigned int TEXTURELODSUPPORT = 7;

  /**
   * True to ensure the data are premultiplied.
   */
  static constexpr unsigned int PREMULTIPLYALPHA = 8;

  /**
   * True if the texture contains cooked RGB values and not gray scaled
   * multipliers.
   */
  static constexpr unsigned int USERGBCOLOR = 9;

  /**
   * True to add noise in order to reduce the banding effect.
   */
  static constexpr unsigned int NOISE = 10;

  // Image Processing Configuration.
  static constexpr unsigned int IMAGEPROCESSING            = 11;
  static constexpr unsigned int VIGNETTE                   = 12;
  static constexpr unsigned int VIGNETTEBLENDMODEMULTIPLY  = 13;
  static constexpr unsigned int VIGNETTEBLENDMODEOPAQUE    = 14;
  static constexpr unsigned int TONEMAPPING                = 15;
  static constexpr unsigned int CONTRAST                   = 16;
  static constexpr unsigned int COLORCURVES                = 17;
  static constexpr unsigned int COLORGRADING               = 18;
  static constexpr unsigned int COLORGRADING3D             = 19;
  static constexpr unsigned int SAMPLER3DGREENDEPTH        = 20;
  static constexpr unsigned int SAMPLER3DBGRMAP            = 21;
  static constexpr unsigned int IMAGEPROCESSINGPOSTPROCESS = 22;
  static constexpr unsigned int EXPOSURE                   = 23;

  // Reflection.
  static constexpr unsigned int REFLECTION                          = 24;
  static constexpr unsigned int REFLECTIONMAP_3D                    = 25;
  static constexpr unsigned int REFLECTIONMAP_SPHERICAL             = 26;
  static constexpr unsigned int REFLECTIONMAP_PLANAR                = 27;
  static constexpr unsigned int REFLECTIONMAP_CUBIC                 = 28;
  static constexpr unsigned int REFLECTIONMAP_PROJECTION            = 29;
  static constexpr unsigned int REFLECTIONMAP_SKYBOX                = 30;
  static constexpr unsigned int REFLECTIONMAP_EXPLICIT              = 31;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR       = 32;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR_FIXED = 33;
  static constexpr unsigned int REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED
    = 34;
  static constexpr unsigned int INVERTCUBICMAP                 = 35;
  static constexpr unsigned int REFLECTIONMAP_OPPOSITEZ        = 36;
  static constexpr unsigned int LODINREFLECTIONALPHA           = 37;
  static constexpr unsigned int GAMMAREFLECTION                = 38;
  static constexpr unsigned int EQUIRECTANGULAR_RELFECTION_FOV = 39;

  // Default BJS.
  static constexpr unsigned int MAINUV1             = 40;
  static constexpr unsigned int MAINUV2             = 41;
  static constexpr unsigned int UV1                 = 42;
  static constexpr unsigned int UV2                 = 43;
  static constexpr unsigned int CLIPPLANE           = 44;
  static constexpr unsigned int POINTSIZE           = 45;
  static constexpr unsigned int LOGARITHMICDEPTH    = 46;
  static constexpr unsigned int FOG                 = 47;
  static constexpr unsigned int NORMAL              = 48;
  static constexpr unsigned int INSTANCES           = 49;
  static constexpr unsigned int SHADOWFLOAT         = 50;
  static constexpr unsigned int SPECULARTERM        = 51;
  static constexpr unsigned int VERTEXCOLOR         = 52;
  static constexpr unsigned int VERTEXALPHA         = 53;
  static constexpr unsigned int MORPHTARGETS_NORMAL = 54;
  static constexpr unsigned int MORPHTARGETS        = 55;
  static constexpr unsigned int ALPHATEST           = 56;
  static constexpr unsigned int DEPTHPREPASS        = 57;
  static constexpr unsigned int NONUNIFORMSCALING   = 58;

  /**
   * @brief Constructor of the defines.
   */
  BackgroundMaterialDefines();
  ~BackgroundMaterialDefines() override;

  void reset() override;

}; // end of struct BackgroundMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_BACKGROUND_BACKGROUND_MATERIAL_DEFINES_H
