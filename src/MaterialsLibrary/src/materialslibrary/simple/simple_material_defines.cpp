#include <babylon/materialslibrary/simple/simple_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

SimpleMaterialDefines::SimpleMaterialDefines() : MaterialDefines{}
{
  boolDef = {
    {"DIFFUSE", false},      //
    {"CLIPPLANE", false},    //
    {"CLIPPLANE2", false},   //
    {"CLIPPLANE3", false},   //
    {"CLIPPLANE4", false},   //
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

SimpleMaterialDefines::~SimpleMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
