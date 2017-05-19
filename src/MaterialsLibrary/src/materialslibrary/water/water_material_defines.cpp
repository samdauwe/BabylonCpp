#include <babylon/materialslibrary/water/water_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

WaterMaterialDefines::WaterMaterialDefines() : MaterialDefines{}
{
  _keys = {"BUMP",
           "REFLECTION",
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
           "SPECULARTERM",
           "LOGARITHMICDEPTH",
           "FRESNELSEPARATE",
           "BUMPSUPERIMPOSE",
           "BUMPAFFECTSREFLECTION",
           "SHADOWS",
           "SHADOWFULLFLOAT"};
  rebuild();
}

WaterMaterialDefines::~WaterMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
