#include <babylon/materialslibrary/grid/grid_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

GridMaterialDefines::GridMaterialDefines() : MaterialDefines{}
{
  _keys = {"TRANSPARENT", "FOG", "LOGARITHMICDEPTH", "POINTSIZE"};
  rebuild();
}

GridMaterialDefines::~GridMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
