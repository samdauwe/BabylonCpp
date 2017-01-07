#include <babylon/materialslibrary/simple/simple_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

SimpleMaterialDefines::SimpleMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",   "CLIPPLANE",  "ALPHATEST", "POINTSIZE",   "FOG",
           "NORMAL",    "UV1",        "UV2",       "VERTEXCOLOR", "VERTEXALPHA",
           "INSTANCES", "REFLECTION", "SHADOWS",   "SPECULARTERM"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

SimpleMaterialDefines::~SimpleMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
