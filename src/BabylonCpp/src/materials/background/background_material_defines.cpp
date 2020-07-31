#include <babylon/materials/background/background_material_defines.h>

namespace BABYLON {

BackgroundMaterialDefines::BackgroundMaterialDefines()
{
  boolDef = {
    /**
     * True if the diffuse texture is in use.
     */
    {"DIFFUSE", false}, //

    /**
     * True if the diffuse texture is in gamma space.
     */
    {"GAMMADIFFUSE", false}, //

    /**
     * True if the diffuse texture has opacity in the alpha channel.
     */
    {"DIFFUSEHASALPHA", false}, //

    /**
     * True if you want the material to fade to transparent at grazing angle.
     */
    {"OPACITYFRESNEL", false}, //

    /**
     * True if an extra blur needs to be added in the reflection.
     */
    {"REFLECTIONBLUR", false}, //

    /**
     * True if you want the material to fade to reflection at grazing angle.
     */
    {"REFLECTIONFRESNEL", false}, //

    /**
     * True if you want the material to falloff as far as you move away from the
     * scene center.
     */
    {"REFLECTIONFALLOFF", false}, //

    /**
     * False if the current Webgl implementation does not support the texture
     * lod extension.
     */
    {"TEXTURELODSUPPORT", false}, //

    /**
     * True to ensure the data are premultiplied.
     */
    {"PREMULTIPLYALPHA", false}, //

    /**
     * True if the texture contains cooked RGB values and not gray scaled
     * multipliers.
     */
    {"USERGBCOLOR", false}, //

    /**
     * True if highlight and shadow levels have been specified. It can help
     * ensuring the main perceived color stays aligned with the desired
     * configuration.
     */
    {"USEHIGHLIGHTANDSHADOWCOLORS", false}, //

    /**
     * True if only shadows must be rendered
     */
    {"BACKMAT_SHADOWONLY", false}, //

    /**
     * True to add noise in order to reduce the banding effect.
     */
    {"NOISE", false}, //

    /**
     * is the reflection texture in BGR color scheme?
     * Mainly used to solve a bug in ios10 video tag
     */
    {"REFLECTIONBGR", false}, //

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
    {"MULTIVIEW", false},                  //

    // Reflection.
    {"REFLECTION", false},                                  //
    {"REFLECTIONMAP_3D", false},                            //
    {"REFLECTIONMAP_SPHERICAL", false},                     //
    {"REFLECTIONMAP_PLANAR", false},                        //
    {"REFLECTIONMAP_CUBIC", false},                         //
    {"REFLECTIONMAP_PROJECTION", false},                    //
    {"REFLECTIONMAP_SKYBOX", false},                        //
    {"REFLECTIONMAP_EXPLICIT", false},                      //
    {"REFLECTIONMAP_EQUIRECTANGULAR", false},               //
    {"REFLECTIONMAP_EQUIRECTANGULAR_FIXED", false},         //
    {"REFLECTIONMAP_MIRROREDEQUIRECTANGULAR_FIXED", false}, //
    {"INVERTCUBICMAP", false},                              //
    {"REFLECTIONMAP_OPPOSITEZ", false},                     //
    {"LODINREFLECTIONALPHA", false},                        //
    {"GAMMAREFLECTION", false},                             //
    {"RGBDREFLECTION", false},                              //
    {"EQUIRECTANGULAR_RELFECTION_FOV", false},              //

    // Default BJS.
    {"MAINUV1", false},    //
    {"MAINUV2", false},    //
    {"UV1", false},        //
    {"UV2", false},        //
    {"CLIPPLANE", false},  //
    {"CLIPPLANE2", false}, //
    {"CLIPPLANE3", false}, //
    {"CLIPPLANE4", false}, //
    {"CLIPPLANE5", false}, //
    {"CLIPPLANE6", false}, //
    {"POINTSIZE", false},  //
    {"FOG", false},        //
    {"NORMAL", false},     //

    {"INSTANCES", false},   //
    {"SHADOWFLOAT", false}, //
  };

  intDef = {
    /**
     * The direct UV channel to use.
     */
    {"DIFFUSEDIRECTUV", 0},      //
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0},         //
  };
}

BackgroundMaterialDefines::~BackgroundMaterialDefines() = default;

void BackgroundMaterialDefines::reset()
{
  MaterialDefines::reset();
}

} // end of namespace BABYLON
