#include <babylon/materialslibrary/grid/grid_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

GridMaterialDefines::GridMaterialDefines()
{
  boolDef = {
    {"TRANSPARENT", false},     //
    {"FOG", false},             //
    {"PREMULTIPLYALPHA", false} //
  };
}

GridMaterialDefines::~GridMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
