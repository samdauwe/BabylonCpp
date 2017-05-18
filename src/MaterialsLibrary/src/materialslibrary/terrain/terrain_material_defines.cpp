#include <babylon/materialslibrary/terrain/terrain_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

TerrainMaterialDefines::TerrainMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",   "BUMP",    "CLIPPLANE",        "ALPHATEST",
           "POINTSIZE", "FOG",     "SPECULARTERM",     "NORMAL",
           "UV1",       "UV2",     "VERTEXCOLOR",      "VERTEXALPHA",
           "INSTANCES", "SHADOWS", "LOGARITHMICDEPTH", "SHADOWFULLFLOAT"};
  rebuild();
}

TerrainMaterialDefines::~TerrainMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
