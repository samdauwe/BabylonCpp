#include <babylon/materialslibrary/terrain/terrain_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

TerrainMaterialDefines::TerrainMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",     "BUMP",         "CLIPPLANE", "ALPHATEST", "POINTSIZE",
           "FOG",         "SPECULARTERM", "NORMAL",    "UV1",       "UV2",
           "VERTEXCOLOR", "VERTEXALPHA",  "INSTANCES", "SHADOWS"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

TerrainMaterialDefines::~TerrainMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
