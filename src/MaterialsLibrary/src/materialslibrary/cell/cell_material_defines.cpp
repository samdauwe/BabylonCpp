#include <babylon/materialslibrary/cell/cell_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

CellMaterialDefines::CellMaterialDefines()
{
  boolDef = {
    {"DIFFUSE", false},           //
    {"CLIPPLANE", false},         //
    {"CLIPPLANE2", false},        //
    {"CLIPPLANE3", false},        //
    {"CLIPPLANE4", false},        //
    {"ALPHATEST", false},         //
    {"POINTSIZE", false},         //
    {"FOG", false},               //
    {"NORMAL", false},            //
    {"UV1", false},               //
    {"UV2", false},               //
    {"VERTEXCOLOR", false},       //
    {"VERTEXALPHA", false},       //
    {"INSTANCES", false},         //
    {"NDOTL", true},              //
    {"CUSTOMUSERLIGHTING", true}, //
    {"CELLBASIC", true},          //
    {"DEPTHPREPASS", false}       //
  };

  intDef = {
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0}          //
  };
}

CellMaterialDefines::~CellMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
