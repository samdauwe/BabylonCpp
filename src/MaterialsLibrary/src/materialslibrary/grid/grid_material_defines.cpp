#include <babylon/materialslibrary/grid/grid_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

GridMaterialDefines::GridMaterialDefines()
{
  boolDef = {
    {"OPACITY", false},          //
    {"TRANSPARENT", false},      //
    {"FOG", false},              //
    {"PREMULTIPLYALPHA", false}, //
    {"UV1", false},              //
    {"UV2", false},              //
    {"INSTANCES", false},        //
    {"THIN_INSTANCES", false}    //
  };
}

GridMaterialDefines::~GridMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
