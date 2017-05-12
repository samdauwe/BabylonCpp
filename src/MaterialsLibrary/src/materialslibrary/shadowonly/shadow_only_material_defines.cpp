#include <babylon/materialslibrary/shadowonly/shadow_only_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

ShadowOnlyMaterialDefines::ShadowOnlyMaterialDefines() : MaterialDefines{}
{
  _keys = {"CLIPPLANE",        "POINTSIZE",       "FOG",
           "NORMAL",           "INSTANCES",       "ALPHATEST",
           "LOGARITHMICDEPTH", "SHADOWFULLFLOAT", "SPECULARTERM"};
  rebuild();
}

ShadowOnlyMaterialDefines::~ShadowOnlyMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
