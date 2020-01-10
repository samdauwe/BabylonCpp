#include <babylon/materialslibrary/shadowonly/shadow_only_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

ShadowOnlyMaterialDefines::ShadowOnlyMaterialDefines()
{
  boolDef = {
    {"CLIPPLANE", false},  //
    {"CLIPPLANE2", false}, //
    {"CLIPPLANE3", false}, //
    {"CLIPPLANE4", false}, //
    {"CLIPPLANE5", false}, //
    {"CLIPPLANE6", false}, //
    {"POINTSIZE", false},  //
    {"FOG", false},        //
    {"NORMAL", false},     //
    {"INSTANCES", false}   //
  };

  intDef = {
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0}          //
  };
}

ShadowOnlyMaterialDefines::~ShadowOnlyMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
