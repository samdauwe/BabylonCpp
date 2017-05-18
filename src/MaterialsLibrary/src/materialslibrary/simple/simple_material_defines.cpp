#include <babylon/materialslibrary/simple/simple_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

SimpleMaterialDefines::SimpleMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",      "CLIPPLANE",      "ALPHATEST", "POINTSIZE",
           "FOG",          "NORMAL",         "UV1",       "UV2",
           "VERTEXCOLOR",  "VERTEXALPHA",    "INSTANCES", "LOGARITHMICDEPTH",
           "SPECULARTERM", "SHADOWFULLFLOAT"};
  rebuild();
}

SimpleMaterialDefines::~SimpleMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
