#include <babylon/materials/pbr/pbr_material_defines.h>

namespace BABYLON {

PBRMaterialDefines::PBRMaterialDefines() : MaterialDefines{}
{
  _keys = {"PBR",
           "MAINUV1",
           "MAINUV2",
           "UV1",
           "UV2",
           "ALBEDO",
           "VERTEXCOLOR",
           "AMBIENT",
           "AMBIENTINGRAYSCALE",
           "OPACITY",
           "VERTEXALPHA",
           "OPACITYRGB",
           "ALPHATEST",
           "DEPTHPREPASS",
           "ALPHABLEND",
           "ALPHAFROMALBEDO",
           "SPECULAROVERALPHA",
           "RADIANCEOVERALPHA",
           "ALPHAFRESNEL",
           "LINEARALPHAFRESNEL",
           "PREMULTIPLYALPHA",
           "EMISSIVE",
           "REFLECTIVITY",
           "SPECULARTERM",
           "MICROSURFACEFROMREFLECTIVITYMAP",
           "MICROSURFACEAUTOMATIC",
           "LODBASEDMICROSFURACE",
           "MICROSURFACEMAP",
           "METALLICWORKFLOW",
           "ROUGHNESSSTOREINMETALMAPALPHA",
           "ROUGHNESSSTOREINMETALMAPGREEN",
           "METALLNESSSTOREINMETALMAPBLUE",
           "AOSTOREINMETALMAPRED",
           "ENVIRONMENTBRDF ",
           "NORMAL",
           "TANGENT",
           "BUMP",
           "OBJECTSPACE_NORMALMAP",
           "PARALLAX",
           "PARALLAXOCCLUSION",
           "NORMALXYSCALE",
           "LIGHTMAP",
           "USELIGHTMAPASSHADOWMAP",
           "GAMMALIGHTMAP",
           "REFLECTION",
           "REFLECTIONMAP_3D",
           "REFLECTIONMAP_SPHERICAL",
           "REFLECTIONMAP_PLANAR",
           "REFLECTIONMAP_CUBIC",
           "USE_LOCAL_REFLECTIONMAP_CUBIC",
           "REFLECTIONMAP_PROJECTION",
           "REFLECTIONMAP_SKYBOX",
           "REFLECTIONMAP_SKYBOX_TRANSFORMED",
           "REFLECTIONMAP_EXPLICIT",
           "REFLECTIONMAP_EQUIRECTANGULAR",
           "REFLECTIONMAP_EQUIRECTANGULAR_FIXED",
           "REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED",
           "INVERTCUBICMAP",
           "USESPHERICALFROMREFLECTIONMAP",
           "USESPHERICALINVERTEX",
           "REFLECTIONMAP_OPPOSITEZ",
           "LODINREFLECTIONALPHA",
           "GAMMAREFLECTION",
           "RGBDREFLECTION",
           "RADIANCEOCCLUSION",
           "HORIZONOCCLUSION",
           "REFRACTION",
           "REFRACTIONMAP_3D",
           "REFRACTIONMAP_OPPOSITEZ",
           "LODINREFRACTIONALPHA",
           "GAMMAREFRACTION",
           "RGBDREFRACTION",
           "LINKREFRACTIONTOTRANSPARENCY",
           "INSTANCES",
           "NONUNIFORMSCALING",
           "MORPHTARGETS",
           "MORPHTARGETS_NORMAL",
           "MORPHTARGETS_TANGENT",
           "IMAGEPROCESSING",
           "VIGNETTE",
           "VIGNETTEBLENDMODEMULTIPLY",
           "VIGNETTEBLENDMODEOPAQUE",
           "TONEMAPPING",
           "TONEMAPPING_ACES",
           "CONTRAST",
           "COLORCURVES",
           "COLORGRADING",
           "COLORGRADING3D",
           "SAMPLER3DGREENDEPTH",
           "SAMPLER3DBGRMAP",
           "IMAGEPROCESSINGPOSTPROCESS",
           "EXPOSURE",
           "USEPHYSICALLIGHTFALLOFF",
           "USEGLTFLIGHTFALLOFF",
           "TWOSIDEDLIGHTING",
           "SHADOWFLOAT",
           "CLIPPLANE",
           "CLIPPLANE2",
           "CLIPPLANE3",
           "CLIPPLANE4",
           "POINTSIZE",
           "FOG",
           "LOGARITHMICDEPTH",
           "FORCENORMALFORWARD",
           "SPECULARAA",
           "UNLIT"};
  rebuild();
} // namespace BABYLON

PBRMaterialDefines::~PBRMaterialDefines()
{
}

void PBRMaterialDefines::reset()
{
  MaterialDefines::reset();
  ALPHATESTVALUE          = "0.5";
  defines[PBR]            = true;
  ALBEDODIRECTUV          = 0;
  AMBIENTDIRECTUV         = 0;
  OPACITYDIRECTUV         = 0;
  EMISSIVEDIRECTUV        = 0;
  REFLECTIVITYDIRECTUV    = 0;
  MICROSURFACEMAPDIRECTUV = 0;
  BUMPDIRECTUV            = 0;
  LIGHTMAPDIRECTUV        = 0;
  NUM_BONE_INFLUENCERS    = 0;
  BonesPerMesh            = 0;
  NUM_MORPH_INFLUENCERS   = 0;
}

} // end of namespace BABYLON
