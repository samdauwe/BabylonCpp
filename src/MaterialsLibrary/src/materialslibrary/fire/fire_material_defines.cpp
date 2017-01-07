#include <babylon/materialslibrary/fire/fire_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

FireMaterialDefines::FireMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE", "CLIPPLANE",   "ALPHATEST",   "POINTSIZE", "FOG",
           "UV1",     "VERTEXCOLOR", "VERTEXALPHA", "INSTANCES"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

FireMaterialDefines::~FireMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
