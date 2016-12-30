#ifndef BABYLON_MATERIALS_PBR_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_PBR_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PBRMaterialDefines : public MaterialDefines {

  static constexpr unsigned int ALBEDO                          = 0;
  static constexpr unsigned int AMBIENT                         = 1;
  static constexpr unsigned int OPACITY                         = 2;
  static constexpr unsigned int OPACITYRGB                      = 3;
  static constexpr unsigned int REFLECTION                      = 4;
  static constexpr unsigned int EMISSIVE                        = 5;
  static constexpr unsigned int REFLECTIVITY                    = 6;
  static constexpr unsigned int BUMP                            = 7;
  static constexpr unsigned int PARALLAX                        = 8;
  static constexpr unsigned int PARALLAXOCCLUSION               = 9;
  static constexpr unsigned int SPECULAROVERALPHA               = 10;
  static constexpr unsigned int CLIPPLANE                       = 11;
  static constexpr unsigned int ALPHATEST                       = 12;
  static constexpr unsigned int ALPHAFROMALBEDO                 = 13;
  static constexpr unsigned int POINTSIZE                       = 14;
  static constexpr unsigned int FOG                             = 15;
  static constexpr unsigned int SPECULARTERM                    = 16;
  static constexpr unsigned int OPACITYFRESNEL                  = 17;
  static constexpr unsigned int EMISSIVEFRESNEL                 = 18;
  static constexpr unsigned int FRESNEL                         = 19;
  static constexpr unsigned int NORMAL                          = 20;
  static constexpr unsigned int UV1                             = 21;
  static constexpr unsigned int UV2                             = 22;
  static constexpr unsigned int VERTEXCOLOR                     = 23;
  static constexpr unsigned int VERTEXALPHA                     = 24;
  static constexpr unsigned int INSTANCES                       = 25;
  static constexpr unsigned int MICROSURFACEFROMREFLECTIVITYMAP = 26;
  static constexpr unsigned int MICROSURFACEAUTOMATIC           = 27;
  static constexpr unsigned int EMISSIVEASILLUMINATION          = 28;
  static constexpr unsigned int LINKEMISSIVEWITHALBEDO          = 29;
  static constexpr unsigned int LIGHTMAP                        = 30;
  static constexpr unsigned int USELIGHTMAPASSHADOWMAP          = 31;
  static constexpr unsigned int REFLECTIONMAP_3D                = 32;
  static constexpr unsigned int REFLECTIONMAP_SPHERICAL         = 33;
  static constexpr unsigned int REFLECTIONMAP_PLANAR            = 34;
  static constexpr unsigned int REFLECTIONMAP_CUBIC             = 35;
  static constexpr unsigned int REFLECTIONMAP_PROJECTION        = 36;
  static constexpr unsigned int REFLECTIONMAP_SKYBOX            = 37;
  static constexpr unsigned int REFLECTIONMAP_EXPLICIT          = 38;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR   = 39;
  static constexpr unsigned int INVERTCUBICMAP                  = 40;
  static constexpr unsigned int LOGARITHMICDEPTH                = 41;
  static constexpr unsigned int CAMERATONEMAP                   = 42;
  static constexpr unsigned int CAMERACONTRAST                  = 43;
  static constexpr unsigned int CAMERACOLORGRADING              = 44;
  static constexpr unsigned int CAMERACOLORCURVES               = 45;
  static constexpr unsigned int OVERLOADEDVALUES                = 46;
  static constexpr unsigned int OVERLOADEDSHADOWVALUES          = 47;
  static constexpr unsigned int USESPHERICALFROMREFLECTIONMAP   = 48;
  static constexpr unsigned int REFRACTION                      = 49;
  static constexpr unsigned int REFRACTIONMAP_3D                = 50;
  static constexpr unsigned int LINKREFRACTIONTOTRANSPARENCY    = 51;
  static constexpr unsigned int REFRACTIONMAPINLINEARSPACE      = 52;
  static constexpr unsigned int LODBASEDMICROSFURACE            = 53;
  static constexpr unsigned int USEPHYSICALLIGHTFALLOFF         = 54;
  static constexpr unsigned int RADIANCEOVERALPHA               = 55;
  static constexpr unsigned int USEPMREMREFLECTION              = 56;
  static constexpr unsigned int USEPMREMREFRACTION              = 57;
  static constexpr unsigned int OPENGLNORMALMAP                 = 58;
  static constexpr unsigned int INVERTNORMALMAPX                = 59;
  static constexpr unsigned int INVERTNORMALMAPY                = 60;
  static constexpr unsigned int SHADOWS                         = 61;
  static constexpr unsigned int SHADOWFULLFLOAT                 = 62;
  static constexpr unsigned int METALLICWORKFLOW                = 63;
  static constexpr unsigned int METALLICROUGHNESSGSTOREINALPHA  = 64;
  static constexpr unsigned int METALLICROUGHNESSGSTOREINGREEN  = 65;

  PBRMaterialDefines();
  ~PBRMaterialDefines();

}; // end of struct PBRMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_MATERIAL_DEFINES_H
