#include <babylon/materials/standard_material_defines.h>

namespace BABYLON {

StandardMaterialDefines::StandardMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",
           "AMBIENT",
           "OPACITY",
           "OPACITYRGB",
           "REFLECTION",
           "EMISSIVE",
           "SPECULAR",
           "BUMP",
           "PARALLAX",
           "PARALLAXOCCLUSION",
           "SPECULAROVERALPHA",
           "CLIPPLANE",
           "ALPHATEST",
           "ALPHAFROMDIFFUSE",
           "POINTSIZE",
           "FOG",
           "SPECULARTERM",
           "DIFFUSEFRESNEL",
           "OPACITYFRESNEL",
           "REFLECTIONFRESNEL",
           "REFRACTIONFRESNEL",
           "EMISSIVEFRESNEL",
           "FRESNEL",
           "NORMAL",
           "UV1",
           "UV2",
           "VERTEXCOLOR",
           "VERTEXALPHA",
           "INSTANCES",
           "GLOSSINESS",
           "ROUGHNESS",
           "EMISSIVEASILLUMINATION",
           "LINKEMISSIVEWITHDIFFUSE",
           "REFLECTIONFRESNELFROMSPECULAR",
           "LIGHTMAP",
           "USELIGHTMAPASSHADOWMAP",
           "REFLECTIONMAP_3D",
           "REFLECTIONMAP_SPHERICAL",
           "REFLECTIONMAP_PLANAR",
           "REFLECTIONMAP_CUBIC",
           "REFLECTIONMAP_PROJECTION",
           "REFLECTIONMAP_SKYBOX",
           "REFLECTIONMAP_EXPLICIT",
           "REFLECTIONMAP_EQUIRECTANGULAR",
           "REFLECTIONMAP_EQUIRECTANGULAR_FIXED",
           "INVERTCUBICMAP",
           "LOGARITHMICDEPTH",
           "REFRACTION",
           "REFRACTIONMAP_3D",
           "REFLECTIONOVERALPHA",
           "INVERTNORMALMAPX",
           "INVERTNORMALMAPY",
           "SHADOWS",
           "SHADOWFULLFLOAT",
           "CAMERACOLORGRADING",
           "CAMERACOLORCURVES"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

StandardMaterialDefines::~StandardMaterialDefines()
{
}

} // end of namespace BABYLON
