#include <babylon/materialslibrary/terrain/terrain_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

TerrainMaterialDefines::TerrainMaterialDefines() : MaterialDefines{}
{
  boolDef = {
    {"DIFFUSE", false},      //
    {"BUMP", false},         //
    {"CLIPPLANE", false},    //
    {"CLIPPLANE2", false},   //
    {"CLIPPLANE3", false},   //
    {"CLIPPLANE4", false},   //
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
  };

  intDef = {
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0}          //
  };
}

TerrainMaterialDefines::~TerrainMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
