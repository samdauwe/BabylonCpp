#ifndef BABYLON_MATERIALS_PBR_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_PBR_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PBRMaterialDefines : public MaterialDefines {

  static constexpr unsigned int ALBEDO                              = 0;
  static constexpr unsigned int AMBIENT                             = 1;
  static constexpr unsigned int AMBIENTINGRAYSCALE                  = 2;
  static constexpr unsigned int OPACITY                             = 3;
  static constexpr unsigned int OPACITYRGB                          = 4;
  static constexpr unsigned int REFLECTION                          = 5;
  static constexpr unsigned int EMISSIVE                            = 6;
  static constexpr unsigned int REFLECTIVITY                        = 7;
  static constexpr unsigned int BUMP                                = 8;
  static constexpr unsigned int PARALLAX                            = 9;
  static constexpr unsigned int PARALLAXOCCLUSION                   = 10;
  static constexpr unsigned int SPECULAROVERALPHA                   = 11;
  static constexpr unsigned int CLIPPLANE                           = 12;
  static constexpr unsigned int ALPHATEST                           = 13;
  static constexpr unsigned int ALPHAFROMALBEDO                     = 14;
  static constexpr unsigned int POINTSIZE                           = 15;
  static constexpr unsigned int FOG                                 = 16;
  static constexpr unsigned int SPECULARTERM                        = 17;
  static constexpr unsigned int OPACITYFRESNEL                      = 18;
  static constexpr unsigned int EMISSIVEFRESNEL                     = 19;
  static constexpr unsigned int FRESNEL                             = 20;
  static constexpr unsigned int NORMAL                              = 21;
  static constexpr unsigned int TANGENT                             = 22;
  static constexpr unsigned int UV1                                 = 23;
  static constexpr unsigned int UV2                                 = 24;
  static constexpr unsigned int VERTEXCOLOR                         = 25;
  static constexpr unsigned int VERTEXALPHA                         = 26;
  static constexpr unsigned int INSTANCES                           = 27;
  static constexpr unsigned int MICROSURFACEFROMREFLECTIVITYMAP     = 28;
  static constexpr unsigned int MICROSURFACEAUTOMATIC               = 29;
  static constexpr unsigned int EMISSIVEASILLUMINATION              = 30;
  static constexpr unsigned int LINKEMISSIVEWITHALBEDO              = 31;
  static constexpr unsigned int LIGHTMAP                            = 32;
  static constexpr unsigned int USELIGHTMAPASSHADOWMAP              = 33;
  static constexpr unsigned int REFLECTIONMAP_3D                    = 34;
  static constexpr unsigned int REFLECTIONMAP_SPHERICAL             = 35;
  static constexpr unsigned int REFLECTIONMAP_PLANAR                = 36;
  static constexpr unsigned int REFLECTIONMAP_CUBIC                 = 37;
  static constexpr unsigned int REFLECTIONMAP_PROJECTION            = 38;
  static constexpr unsigned int REFLECTIONMAP_SKYBOX                = 39;
  static constexpr unsigned int REFLECTIONMAP_EXPLICIT              = 40;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR       = 41;
  static constexpr unsigned int REFLECTIONMAP_EQUIRECTANGULAR_FIXED = 42;
  static constexpr unsigned int REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED
    = 43;
  static constexpr unsigned int INVERTCUBICMAP                = 44;
  static constexpr unsigned int LOGARITHMICDEPTH              = 45;
  static constexpr unsigned int CAMERATONEMAP                 = 46;
  static constexpr unsigned int CAMERACONTRAST                = 47;
  static constexpr unsigned int CAMERACOLORGRADING            = 48;
  static constexpr unsigned int CAMERACOLORCURVES             = 49;
  static constexpr unsigned int USESPHERICALFROMREFLECTIONMAP = 50;
  static constexpr unsigned int REFRACTION                    = 51;
  static constexpr unsigned int REFRACTIONMAP_3D              = 52;
  static constexpr unsigned int LINKREFRACTIONTOTRANSPARENCY  = 53;
  static constexpr unsigned int REFRACTIONMAPINLINEARSPACE    = 54;
  static constexpr unsigned int LODBASEDMICROSFURACE          = 55;
  static constexpr unsigned int USEPHYSICALLIGHTFALLOFF       = 56;
  static constexpr unsigned int RADIANCEOVERALPHA             = 57;
  static constexpr unsigned int USEPMREMREFLECTION            = 58;
  static constexpr unsigned int USEPMREMREFRACTION            = 59;
  static constexpr unsigned int INVERTNORMALMAPX              = 60;
  static constexpr unsigned int INVERTNORMALMAPY              = 61;
  static constexpr unsigned int TWOSIDEDLIGHTING              = 62;
  static constexpr unsigned int SHADOWFULLFLOAT               = 63;
  static constexpr unsigned int METALLICWORKFLOW              = 64;
  static constexpr unsigned int METALLICMAP                   = 65;
  static constexpr unsigned int ROUGHNESSSTOREINMETALMAPALPHA = 66;
  static constexpr unsigned int ROUGHNESSSTOREINMETALMAPGREEN = 67;
  static constexpr unsigned int METALLNESSSTOREINMETALMAPBLUE = 68;
  static constexpr unsigned int AOSTOREINMETALMAPRED          = 69;
  static constexpr unsigned int MICROSURFACEMAP               = 70;
  static constexpr unsigned int MORPHTARGETS                  = 71;
  static constexpr unsigned int MORPHTARGETS_NORMAL           = 72;
  static constexpr unsigned int MORPHTARGETS_TANGENT          = 73;

  PBRMaterialDefines();
  ~PBRMaterialDefines();

}; // end of struct PBRMaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_MATERIAL_DEFINES_H
