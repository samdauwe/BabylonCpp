#include <babylon/materials/pbr/pbr_material_defines.h>

namespace BABYLON {

PBRMaterialDefines::PBRMaterialDefines() : MaterialDefines{}
{
  boolDef = {
    {"PBR", true}, //

    {"MAINUV1", false}, //
    {"MAINUV2", false}, //
    {"UV1", false},     //
    {"UV2", false},     //

    {"ALBEDO", false},      //
    {"VERTEXCOLOR", false}, //

    {"AMBIENT", false},            //
    {"AMBIENTINGRAYSCALE", false}, //

    {"OPACITY", false},            //
    {"VERTEXALPHA", false},        //
    {"OPACITYRGB", false},         //
    {"ALPHATEST", false},          //
    {"DEPTHPREPASS", false},       //
    {"ALPHABLEND", false},         //
    {"ALPHAFROMALBEDO", false},    //
    {"SPECULAROVERALPHA", false},  //
    {"RADIANCEOVERALPHA", false},  //
    {"ALPHAFRESNEL", false},       //
    {"LINEARALPHAFRESNEL", false}, //
    {"PREMULTIPLYALPHA", false},   //

    {"EMISSIVE", false}, //

    {"REFLECTIVITY", false}, //
    {"SPECULARTERM", false}, //

    {"MICROSURFACEFROMREFLECTIVITYMAP", false}, //
    {"MICROSURFACEAUTOMATIC", false},           //
    {"LODBASEDMICROSFURACE", false},            //
    {"MICROSURFACEMAP", false},                 //

    {"METALLICWORKFLOW", false},              //
    {"ROUGHNESSSTOREINMETALMAPALPHA", false}, //
    {"ROUGHNESSSTOREINMETALMAPGREEN", false}, //
    {"METALLNESSSTOREINMETALMAPBLUE", false}, //
    {"AOSTOREINMETALMAPRED", false},          //
    {"ENVIRONMENTBRDF", false},               //

    {"NORMAL", false},                //
    {"TANGENT", false},               //
    {"BUMP", false},                  //
    {"OBJECTSPACE_NORMALMAP", false}, //
    {"PARALLAX", false},              //
    {"PARALLAXOCCLUSION", false},     //
    {"NORMALXYSCALE", true},          //

    {"LIGHTMAP", false},               //
    {"USELIGHTMAPASSHADOWMAP", false}, //
    {"GAMMALIGHTMAP", false},          //

    {"REFLECTION", false},                                  //
    {"REFLECTIONMAP_3D", false},                            //
    {"REFLECTIONMAP_SPHERICAL", false},                     //
    {"REFLECTIONMAP_PLANAR", false},                        //
    {"REFLECTIONMAP_CUBIC", false},                         //
    {"USE_LOCAL_REFLECTIONMAP_CUBIC", false},               //
    {"REFLECTIONMAP_PROJECTION", false},                    //
    {"REFLECTIONMAP_SKYBOX", false},                        //
    {"REFLECTIONMAP_SKYBOX_TRANSFORMED", false},            //
    {"REFLECTIONMAP_EXPLICIT", false},                      //
    {"REFLECTIONMAP_EQUIRECTANGULAR", false},               //
    {"REFLECTIONMAP_EQUIRECTANGULAR_FIXED", false},         //
    {"REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED", false}, //
    {"INVERTCUBICMAP", false},                              //
    {"USESPHERICALFROMREFLECTIONMAP", false},               //
    {"USESPHERICALINVERTEX", false},                        //
    {"REFLECTIONMAP_OPPOSITEZ", false},                     //
    {"LODINREFLECTIONALPHA", false},                        //
    {"GAMMAREFLECTION", false},                             //
    {"RGBDREFLECTION", false},                              //
    {"RADIANCEOCCLUSION", false},                           //
    {"HORIZONOCCLUSION", false},                            //

    {"REFRACTION", false},                   //
    {"REFRACTIONMAP_3D", false},             //
    {"REFRACTIONMAP_OPPOSITEZ", false},      //
    {"LODINREFRACTIONALPHA", false},         //
    {"GAMMAREFRACTION", false},              //
    {"RGBDREFRACTION", false},               //
    {"LINKREFRACTIONTOTRANSPARENCY", false}, //

    {"INSTANCES", false}, //

    {"NONUNIFORMSCALING", false}, //

    {"MORPHTARGETS", false},         //
    {"MORPHTARGETS_NORMAL", false},  //
    {"MORPHTARGETS_TANGENT", false}, //

    {"IMAGEPROCESSING", false},            //
    {"VIGNETTE", false},                   //
    {"VIGNETTEBLENDMODEMULTIPLY", false},  //
    {"VIGNETTEBLENDMODEOPAQUE", false},    //
    {"TONEMAPPING", false},                //
    {"TONEMAPPING_ACES", false},           //
    {"CONTRAST", false},                   //
    {"COLORCURVES", false},                //
    {"COLORGRADING", false},               //
    {"COLORGRADING3D", false},             //
    {"SAMPLER3DGREENDEPTH", false},        //
    {"SAMPLER3DBGRMAP", false},            //
    {"IMAGEPROCESSINGPOSTPROCESS", false}, //
    {"EXPOSURE", false},                   //

    {"USEPHYSICALLIGHTFALLOFF", false}, //
    {"USEGLTFLIGHTFALLOFF", false},     //
    {"TWOSIDEDLIGHTING", false},        //
    {"SHADOWFLOAT", false},             //
    {"CLIPPLANE", false},               //
    {"CLIPPLANE2", false},              //
    {"CLIPPLANE3", false},              //
    {"CLIPPLANE4", false},              //
    {"POINTSIZE", false},               //
    {"FOG", false},                     //
    {"LOGARITHMICDEPTH", false},        //

    {"FORCENORMALFORWARD", false}, //

    {"SPECULARAA", false}, //

    {"UNLIT", false}, //
  };

  intDef = {
    {"AMBIENTDIRECTUV", 0},         //
    {"ALBEDODIRECTUV", 0},          //
    {"OPACITYDIRECTUV", 0},         //
    {"EMISSIVEDIRECTUV", 0},        //
    {"REFLECTIVITYDIRECTUV", 0},    //
    {"MICROSURFACEMAPDIRECTUV", 0}, //
    {"BUMPDIRECTUV", 0},            //
    {"LIGHTMAPDIRECTUV", 0},        //
    {"NUM_BONE_INFLUENCERS", 0},    //
    {"BonesPerMesh", 0},            //
    {"NUM_MORPH_INFLUENCERS", 0},   //
  };

  stringDef = {
    {"ALPHATESTVALUE", "0.5"}, //
  };

} // namespace BABYLON

PBRMaterialDefines::~PBRMaterialDefines()
{
}

void PBRMaterialDefines::reset()
{
  MaterialDefines::reset();

  boolDef["PBR"]           = true;
  boolDef["NORMALXYSCALE"] = true;

  stringDef["ALPHATESTVALUE"] = "0.5";
}

} // end of namespace BABYLON
