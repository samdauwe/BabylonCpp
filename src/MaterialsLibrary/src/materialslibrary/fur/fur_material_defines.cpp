#include <babylon/materialslibrary/fur/fur_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

FurMaterialDefines::FurMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",      "HEIGHTMAP",
           "CLIPPLANE",    "ALPHATEST",
           "POINTSIZE",    "FOG",
           "NORMAL",       "UV1",
           "UV2",          "VERTEXCOLOR",
           "VERTEXALPHA",  "INSTANCES",
           "HIGHLEVEL",    "LOGARITHMICDEPTH",
           "SPECULARTERM", "SHADOWFULLFLOAT"};
  rebuild();
}

FurMaterialDefines::~FurMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
