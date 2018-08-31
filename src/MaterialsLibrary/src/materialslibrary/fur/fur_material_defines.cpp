#include <babylon/materialslibrary/fur/fur_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

FurMaterialDefines::FurMaterialDefines() : MaterialDefines{}
{
  boolDef = {
    {"DIFFUSE", false},      //
    {"HEIGHTMAP", false},    //
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
    {"HIGHLEVEL", false},    //
  };

  intDef = {
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0}          //
  };
}

FurMaterialDefines::~FurMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
