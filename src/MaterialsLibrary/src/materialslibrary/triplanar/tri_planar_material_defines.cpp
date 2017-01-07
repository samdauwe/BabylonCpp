#include <babylon/materialslibrary/triplanar/tri_planar_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

TriPlanarMaterialDefines::TriPlanarMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSEX",    "DIFFUSEY",    "DIFFUSEZ",     "BUMPX",
           "BUMPY",       "BUMPZ",       "CLIPPLANE",    "ALPHATEST",
           "POINTSIZE",   "FOG",         "SPECULARTERM", "NORMAL",
           "VERTEXCOLOR", "VERTEXALPHA", "INSTANCES",    "SHADOWS"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

TriPlanarMaterialDefines::~TriPlanarMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
