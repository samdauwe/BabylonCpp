#include <babylon/materials/background/background_material_defines.h>

namespace BABYLON {

BackgroundMaterialDefines::BackgroundMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",
           "GAMMADIFFUSE",
           "DIFFUSEHASALPHA",
           "OPACITYFRESNEL",
           "REFLECTIONBLUR",
           "REFLECTIONFRESNEL",
           "REFLECTIONFALLOFF",
           "TEXTURELODSUPPORT",
           "PREMULTIPLYALPHA",
           "USERGBCOLOR",
           "NOISE",
           "REFLECTIONBGR",
           "IMAGEPROCESSING",
           "VIGNETTE",
           "VIGNETTEBLENDMODEMULTIPLY",
           "VIGNETTEBLENDMODEOPAQUE",
           "TONEMAPPING",
           "CONTRAST",
           "COLORCURVES",
           "COLORGRADING",
           "COLORGRADING3D",
           "SAMPLER3DGREENDEPTH",
           "SAMPLER3DBGRMAP",
           "IMAGEPROCESSINGPOSTPROCESS",
           "EXPOSURE",
           "GRAIN",
           "REFLECTION",
           "REFLECTIONMAP_3D",
           "REFLECTIONMAP_SPHERICAL",
           "REFLECTIONMAP_PLANAR",
           "REFLECTIONMAP_CUBIC",
           "REFLECTIONMAP_PROJECTION",
           "REFLECTIONMAP_SKYBOX",
           "REFLECTIONMAP_EXPLICIT",
           "REFLECTIONMAP_EQUIRECTANGULAR",
           "REFLECTIONMAP_EQUIRECTANGULAR_FIXED",
           "REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED",
           "INVERTCUBICMAP",
           "REFLECTIONMAP_OPPOSITEZ",
           "LODINREFLECTIONALPHA",
           "GAMMAREFLECTION",
           "EQUIRECTANGULAR_RELFECTION_FOV",
           "MAINUV1",
           "MAINUV2",
           "UV1",
           "UV2",
           "CLIPPLANE",
           "POINTSIZE",
           "LOGARITHMICDEPTH",
           "FOG",
           "NORMAL",
           "INSTANCES",
           "SHADOWFLOAT",
           "SPECULARTERM",
           "VERTEXCOLOR",
           "VERTEXALPHA",
           "MORPHTARGETS_NORMAL",
           "MORPHTARGETS",
           "ALPHATEST",
           "DEPTHPREPASS",
           "NONUNIFORMSCALING"};
  rebuild();

  DIFFUSEDIRECTUV      = 0;
  NUM_BONE_INFLUENCERS = 0;
  BonesPerMesh         = 0;
}

BackgroundMaterialDefines::~BackgroundMaterialDefines()
{
}

void BackgroundMaterialDefines::reset()
{
  MaterialDefines::reset();

  DIFFUSEDIRECTUV      = 0;
  NUM_BONE_INFLUENCERS = 0;
  BonesPerMesh         = 0;
}

} // end of namespace BABYLON
