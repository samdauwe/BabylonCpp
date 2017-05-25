#include <babylon/materialslibrary/cell/cell_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

CellMaterialDefines::CellMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",
           "CLIPPLANE",
           "ALPHATEST",
           "POINTSIZE",
           "FOG",
           "NORMAL",
           "UV1",
           "UV2",
           "VERTEXCOLOR",
           "VERTEXALPHA",
           "INSTANCES",
           "NDOTL",
           "CUSTOMUSERLIGHTING",
           "CELLBASIC",
           "LOGARITHMICDEPTH",
           "SPECULARTERM",
           "SHADOWFULLFLOAT"};
  rebuild();

  const std::array<unsigned int, 3> activatedProps{
    {NDOTL, CUSTOMUSERLIGHTING, CELLBASIC}};

  for (const auto& activatedProp : activatedProps) {
    defines[activatedProp] = true;
  }
}

CellMaterialDefines::~CellMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
