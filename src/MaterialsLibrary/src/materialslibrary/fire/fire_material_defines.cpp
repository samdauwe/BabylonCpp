#include <babylon/materialslibrary/fire/fire_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

FireMaterialDefines::FireMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",     "CLIPPLANE",   "ALPHATEST", "POINTSIZE",
           "FOG",         "NORMAL",      "UV1",       "UV2",
           "VERTEXCOLOR", "VERTEXALPHA", "INSTANCES"};
  rebuild();
}

FireMaterialDefines::~FireMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
