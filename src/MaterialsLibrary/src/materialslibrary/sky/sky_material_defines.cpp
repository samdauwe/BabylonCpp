#include <babylon/materialslibrary/sky/sky_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

SkyMaterialDefines::SkyMaterialDefines() : MaterialDefines{}
{
  _keys = {"CLIPPLANE", "POINTSIZE", "FOG", "VERTEXCOLOR", "VERTEXALPHA"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

SkyMaterialDefines::~SkyMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
