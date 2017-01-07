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
           "SHADOWS"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

WaterMaterialDefines::~WaterMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
