#include <babylon/materials/node/node_material_defines.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

NodeMaterialDefines::NodeMaterialDefines() : MaterialDefines{}
{
  boolDef = {
    {"NORMAL", false},  //
    {"TANGENT", false}, //
    {"UV1", false},     //

    /** BONES */
    {"BONETEXTURE", false}, //

    /** MORPH TARGETS */
    {"MORPHTARGETS", false},         //
    {"MORPHTARGETS_NORMAL", false},  //
    {"MORPHTARGETS_TANGENT", false}, //
    {"MORPHTARGETS_UV", false},      //
    {"MORPHTARGETS_TEXTURE", false}, //

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

void NodeMaterialDefines::setValue(const std::string& name, bool value,
                                   bool markAsUnprocessedIfDirty)
{
  if (markAsUnprocessedIfDirty && (!stl_util::contains(boolDef, name) || boolDef[name] != value)) {
    markAsUnprocessed();
  }

  boolDef[name] = value;
}

void NodeMaterialDefines::setValue(const std::string& name, const std::string& value,
                                   bool markAsUnprocessedIfDirty)
{
  if (markAsUnprocessedIfDirty
      && (!stl_util::contains(stringDef, name) || stringDef[name] != value)) {
    markAsUnprocessed();
  }

  stringDef[name] = value;
}

} // end of namespace BABYLON
