#include <babylon/materialslibrary/gradient/gradient_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

GradientMaterialDefines::GradientMaterialDefines()
{
  boolDef = {
    {"EMISSIVE", false},     //
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
    {"NORMAL", false},       //
    {"UV1", false},          //
    {"UV2", false},          //
    {"VERTEXCOLOR", false},  //
    {"VERTEXALPHA", false},  //
    {"INSTANCES", false},    //
  };

  intDef = {
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0}          //
  };
}

GradientMaterialDefines::~GradientMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
