#ifndef BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT StandardMaterialDefines : public MaterialDefines {

  static constexpr unsigned int DIFFUSE                             = 0;
  static constexpr unsigned int AMBIENT                             = 1;
  static constexpr unsigned int OPACITY                             = 2;
  static constexpr unsigned int OPACITYRGB                          = 3;
  static constexpr unsigned int REFLECTION                          = 4;
  static constexpr unsigned int EMISSIVE                            = 5;
  static constexpr unsigned int SPECULAR                            = 6;
  static constexpr unsigned int BUMP                                = 7;
  static constexpr unsigned int PARALLAX                            = 8;
  static constexpr unsigned int PARALLAXOCCLUSION                   = 9;
  static constexpr unsigned int SPECULAROVERALPHA                   = 10;
  static constexpr unsigned int CLIPPLANE                           = 11;
  static constexpr unsigned int ALPHATEST                           = 12;
  static constexpr unsigned int ALPHAFROMDIFFUSE                    = 13;
  static constexpr unsigned int POINTSIZE                           = 14;
  static constexpr unsigned int FOG                                 = 15;
  static constexpr unsigned int SPECULARTERM                        = 16;
  static constexpr unsigned int DIFFUSEFRESNEL                      = 17;
  static constexpr unsigned int OPACITYFRESNEL                      = 18;
  static constexpr unsigned int REFLECTIONFRESNEL                   = 19;
  static constexpr unsigned int REFRACTIONFRESNEL                   = 20;
  static constexpr unsigned int EMISSIVEFRESNEL                     = 21;
  static constexpr unsigned int FRESNEL                             = 22;
  static constexpr unsigned int NORMAL                              = 23;
  static constexpr unsigned int UV1                                 = 24;
  static constexpr unsigned int UV2                                 = 25;
  static constexpr unsigned int VERTEXCOLOR                         = 26;
  static constexpr unsigned int VERTEXALPHA                         = 27;
  static constexpr unsigned int INSTANCES                           = 28;
  static constexpr unsigned int GLOSSINESS                          = 29;
  static constexpr unsigned int ROUGHNESS                           = 30;
  static constexpr unsigned int EMISSIVEASILLUMINATION              = 31;
  static constexpr unsigned int LINKEMISSIVEWITHDIFFUSE             = 32;
  static constexpr unsigned int REFLECTIONFRESNELFROMSPECULAR       = 33;
  static constexpr unsigned int LIGHTMAP                            = 34;
  static constexpr unsigned int USELIGHTMAPASSHADOWMAP              = 35;
  static constexpr unsigned int REFLECTIONMAP_3D                    = 36;
  static constexpr unsigned int REFLECTIONMAP_SPHERICAL             = 37;
  static constexpr unsigned int REFLECTIONMAP_PLANAR                = 38;
  static constexpr unsigned int REFLECTIONMAP_CUBIC                 = 39;
  static constexpr unsigned int REFLECTIONMAP_PROJECTION            = 40;
  static constexpr unsigned int REFLECTIONMAP_SKYBOX                = 41;
  static constexpr unsigned int REFLECTIONMAP_EXPLICIT              = 42;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR       = 43;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR_FIXED = 44;
  static constexpr unsigned int INVERTCUBICMAP                      = 45;
  static constexpr unsigned int LOGARITHMICDEPTH                    = 46;
  static constexpr unsigned int REFRACTION                          = 47;
  static constexpr unsigned int REFRACTIONMAP_3D                    = 48;
  static constexpr unsigned int REFLECTIONOVERALPHA                 = 49;
  static constexpr unsigned int INVERTNORMALMAPX                    = 50;
  static constexpr unsigned int INVERTNORMALMAPY                    = 51;
  static constexpr unsigned int TWOSIDEDLIGHTING                    = 52;
  static constexpr unsigned int SHADOWS                             = 53;
  static constexpr unsigned int SHADOWFULLFLOAT                     = 54;
  static constexpr unsigned int CAMERACOLORGRADING                  = 55;
  static constexpr unsigned int CAMERACOLORCURVES                   = 56;

  StandardMaterialDefines();
  ~StandardMaterialDefines();

}; // end of struct StandardMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_STANDARD_MATERIAL_DEFINES_H
