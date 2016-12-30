#include <babylon/materials/pbr_material_defines.h>

namespace BABYLON {

PBRMaterialDefines::PBRMaterialDefines() : MaterialDefines{}
{
  _keys = {"ALBEDO",
           "AMBIENT",
           "OPACITY",
           "OPACITYRGB",
           "REFLECTION",
           "EMISSIVE",
           "REFLECTIVITY",
           "BUMP",
           "PARALLAX",
           "PARALLAXOCCLUSION",
           "SPECULAROVERALPHA",
           "CLIPPLANE",
           "ALPHATEST",
           "ALPHAFROMALBEDO",
           "POINTSIZE",
           "FOG",
           "SPECULARTERM",
           "OPACITYFRESNEL",
           "EMISSIVEFRESNEL",
           "FRESNEL",
           "NORMAL",
           "UV1",
           "UV2",
           "VERTEXCOLOR",
           "VERTEXALPHA",
           "INSTANCES",
           "MICROSURFACEFROMREFLECTIVITYMAP",
           "MICROSURFACEAUTOMATIC",
           "EMISSIVEASILLUMINATION",
           "LINKEMISSIVEWITHALBEDO",
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
           "INVERTCUBICMAP",
           "LOGARITHMICDEPTH",
           "CAMERATONEMAP",
           "CAMERACONTRAST",
           "CAMERACOLORGRADING",
           "CAMERACOLORCURVES",
           "OVERLOADEDVALUES",
           "OVERLOADEDSHADOWVALUES",
           "USESPHERICALFROMREFLECTIONMAP",
           "REFRACTION",
           "REFRACTIONMAP_3D",
           "LINKREFRACTIONTOTRANSPARENCY",
           "REFRACTIONMAPINLINEARSPACE",
           "LODBASEDMICROSFURACE",
           "USEPHYSICALLIGHTFALLOFF",
           "RADIANCEOVERALPHA",
           "USEPMREMREFLECTION",
           "USEPMREMREFRACTION",
           "OPENGLNORMALMAP",
           "INVERTNORMALMAPX",
           "INVERTNORMALMAPY",
           "SHADOWS",
           "SHADOWFULLFLOAT",
           "METALLICWORKFLOW",
           "METALLICROUGHNESSGSTOREINALPHA",
           "METALLICROUGHNESSGSTOREINGREEN"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }

  NUM_BONE_INFLUENCERS = 0;
  BonesPerMesh         = 0;
}

PBRMaterialDefines::~PBRMaterialDefines()
{
}

} // end of namespace BABYLON
