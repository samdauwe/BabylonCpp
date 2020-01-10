#include <babylon/materialslibrary/mix/mix_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

MixMaterialDefines::MixMaterialDefines()
{
  boolDef = {
    {"DIFFUSE", false},      //
    {"CLIPPLANE", false},    //
    {"CLIPPLANE2", false},   //
    {"CLIPPLANE3", false},   //
    {"CLIPPLANE4", false},   //
    {"CLIPPLANE5", false},   //
    {"CLIPPLANE6", false},   //
    {"ALPHATEST", false},    //
    {"DEPTHPREPASS", false}, //
    {"POINTSIZE", false},    //
    {"FOG", false},          //
    {"SPECULARTERM", false}, //
    {"NORMAL", false},       //
    {"UV1", false},          //
    {"UV2", false},          //
    {"VERTEXCOLOR", false},  //
    {"VERTEXALPHA", false},  //
    {"INSTANCES", false},    //
    {"MIXMAP2", false},      //
  };

  intDef = {
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0}          //
  };
}

MixMaterialDefines::~MixMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
