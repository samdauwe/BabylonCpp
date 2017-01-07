#include <babylon/materialslibrary/fur/fur_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

FurMaterialDefines::FurMaterialDefines() : MaterialDefines{}
{
  _keys
    = {"DIFFUSE",     "HEIGHTMAP", "CLIPPLANE", "ALPHATEST",    "POINTSIZE",
       "FOG",         "NORMAL",    "UV1",       "UV2",          "VERTEXCOLOR",
       "VERTEXALPHA", "INSTANCES", "HIGHLEVEL", "SPECULARTERM", "SHADOWS"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

FurMaterialDefines::~FurMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
