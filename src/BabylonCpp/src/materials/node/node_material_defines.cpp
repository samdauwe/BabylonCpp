#include <babylon/materials/node/node_material_defines.h>

namespace BABYLON {

NodeMaterialDefines::NodeMaterialDefines() : MaterialDefines{}
{
  boolDef = {
    /** BONES */
    {"BONETEXTURE", false}, //

    /** MORPH TARGETS */
    {"MORPHTARGETS", false},         //
    {"MORPHTARGETS_NORMAL", false},  //
    {"MORPHTARGETS_TANGENT", false}, //
    {"MORPHTARGETS_UV", false},      //

    /** IMAGE PROCESSING */
    {"IMAGEPROCESSING", false},            //
    {"VIGNETTE", false},                   //
    {"VIGNETTEBLENDMODEMULTIPLY", false},  //
    {"VIGNETTEBLENDMODEOPAQUE", false},    //
    {"TONEMAPPING", false},                //
    {"TONEMAPPING_ACES", false},           //
    {"CONTRAST", false},                   //
    {"EXPOSURE", false},                   //
    {"COLORCURVES", false},                //
    {"COLORGRADING", false},               //
    {"COLORGRADING3D", false},             //
    {"SAMPLER3DGREENDEPTH", false},        //
    {"SAMPLER3DBGRMAP", false},            //
    {"IMAGEPROCESSINGPOSTPROCESS", false}, //
  };

  intDef = {
    /** BONES */
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0},         //

    /** MORPH TARGETS */
    {"NUM_MORPH_INFLUENCERS", 0}, //

    /** MISC. */
    {"BUMPDIRECTUV", 0}, //
  };
}

NodeMaterialDefines::~NodeMaterialDefines() = default;

void NodeMaterialDefines::setValue(const std::string& name, bool value)
{
  boolDef[name] = value;
}

} // end of namespace BABYLON
