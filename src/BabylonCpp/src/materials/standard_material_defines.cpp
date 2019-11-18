#include <babylon/materials/standard_material_defines.h>

#include <array>
#include <sstream>

namespace BABYLON {

StandardMaterialDefines::StandardMaterialDefines()
{
  boolDef = {
    {"MAINUV1", false},                                     //
    {"MAINUV2", false},                                     //
    {"DIFFUSE", false},                                     //
    {"AMBIENT", false},                                     //
    {"OPACITY", false},                                     //
    {"OPACITYRGB", false},                                  //
    {"REFLECTION", false},                                  //
    {"EMISSIVE", false},                                    //
    {"SPECULAR", false},                                    //
    {"BUMP", false},                                        //
    {"PARALLAX", false},                                    //
    {"PARALLAXOCCLUSION", false},                           //
    {"SPECULAROVERALPHA", false},                           //
    {"CLIPPLANE", false},                                   //
    {"CLIPPLANE2", false},                                  //
    {"CLIPPLANE3", false},                                  //
    {"CLIPPLANE4", false},                                  //
    {"ALPHATEST", false},                                   //
    {"DEPTHPREPASS", false},                                //
    {"ALPHAFROMDIFFUSE", false},                            //
    {"POINTSIZE", false},                                   //
    {"FOG", false},                                         //
    {"SPECULARTERM", false},                                //
    {"DIFFUSEFRESNEL", false},                              //
    {"OPACITYFRESNEL", false},                              //
    {"REFLECTIONFRESNEL", false},                           //
    {"REFRACTIONFRESNEL", false},                           //
    {"EMISSIVEFRESNEL", false},                             //
    {"FRESNEL", false},                                     //
    {"NORMAL", false},                                      //
    {"UV1", false},                                         //
    {"UV2", false},                                         //
    {"VERTEXCOLOR", false},                                 //
    {"VERTEXALPHA", false},                                 //
    {"BONETEXTURE", false},                                 //
    {"INSTANCES", false},                                   //
    {"GLOSSINESS", false},                                  //
    {"ROUGHNESS", false},                                   //
    {"EMISSIVEASILLUMINATION", false},                      //
    {"LINKEMISSIVEWITHDIFFUSE", false},                     //
    {"REFLECTIONFRESNELFROMSPECULAR", false},               //
    {"LIGHTMAP", false},                                    //
    {"OBJECTSPACE_NORMALMAP", false},                       //
    {"USELIGHTMAPASSHADOWMAP", false},                      //
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
    {"LOGARITHMICDEPTH", false},                            //
    {"REFRACTION", false},                                  //
    {"REFRACTIONMAP_3D", false},                            //
    {"REFLECTIONOVERALPHA", false},                         //
    {"TWOSIDEDLIGHTING", false},                            //
    {"SHADOWFLOAT", false},                                 //
    {"MORPHTARGETS", false},                                //
    {"MORPHTARGETS_NORMAL", false},                         //
    {"MORPHTARGETS_TANGENT", false},                        //
    {"MORPHTARGETS_UV", false},                             //
    {"NONUNIFORMSCALING", false}, // https://playground.babylonjs.com#V6DWIH
    {"PREMULTIPLYALPHA", false},  // https://playground.babylonjs.com#LNVJJ7

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
    {"MULTIVIEW", false},                  //
    /**
     * If the reflection texture on this material is in linear color space
     * @hidden
     */
    {"IS_REFLECTION_LINEAR", false}, //
    /**
     * If the refraction texture on this material is in linear color space
     * @hidden
     */
    {"IS_REFRACTION_LINEAR", false}, //
    {"EXPOSURE", false},             //
  };

  intDef = {
    {"DIFFUSEDIRECTUV", 0},       //
    {"AMBIENTDIRECTUV", 0},       //
    {"OPACITYDIRECTUV", 0},       //
    {"EMISSIVEDIRECTUV", 0},      //
    {"SPECULARDIRECTUV", 0},      //
    {"BUMPDIRECTUV", 0},          //
    {"NUM_BONE_INFLUENCERS", 0},  //
    {"BonesPerMesh", 0},          //
    {"LIGHTMAPDIRECTUV", 0},      //
    {"NUM_MORPH_INFLUENCERS", 0}, //
  };
}

StandardMaterialDefines::~StandardMaterialDefines() = default;

void StandardMaterialDefines::setReflectionMode(const std::string& modeToEnable)
{
  static const std::array<std::string, 10> modes{
    {"REFLECTIONMAP_CUBIC", "REFLECTIONMAP_EXPLICIT", "REFLECTIONMAP_PLANAR",
     "REFLECTIONMAP_PROJECTION", "REFLECTIONMAP_PROJECTION",
     "REFLECTIONMAP_SKYBOX", "REFLECTIONMAP_SPHERICAL",
     "REFLECTIONMAP_EQUIRECTANGULAR", "REFLECTIONMAP_EQUIRECTANGULAR_FIXED",
     "REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED"}};

  for (const auto& mode : modes) {
    boolDef[mode] = (mode == modeToEnable);
  }
}

std::string StandardMaterialDefines::toString() const
{
  std::ostringstream oss;
  oss << MaterialDefines::toString();
  oss << IImageProcessingConfigurationDefines::convertToString();

  return oss.str();
}

} // end of namespace BABYLON
