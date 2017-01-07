#include <babylon/materialslibrary/grid/grid_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

GridMaterialDefines::GridMaterialDefines() : MaterialDefines{}
{
  _keys = {"TRANSPARENT", "FOG"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

GridMaterialDefines::~GridMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
