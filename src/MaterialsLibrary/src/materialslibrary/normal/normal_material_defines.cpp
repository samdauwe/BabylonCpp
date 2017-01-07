#include <babylon/materialslibrary/normal/normal_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

NormalMaterialDefines::NormalMaterialDefines() : MaterialDefines{}
{
  _keys = {
    "DIFFUSE",    "CLIPPLANE",   "ALPHATEST",   "POINTSIZE",   "FOG",
    "LIGHT0",     "LIGHT1",      "LIGHT2",      "LIGHT3",      "SPOTLIGHT0",
    "SPOTLIGHT1", "SPOTLIGHT2",  "SPOTLIGHT3",  "HEMILIGHT0",  "HEMILIGHT1",
    "HEMILIGHT2", "HEMILIGHT3",  "DIRLIGHT0",   "DIRLIGHT1",   "DIRLIGHT2",
    "DIRLIGHT3",  "POINTLIGHT0", "POINTLIGHT1", "POINTLIGHT2", "POINTLIGHT3",
    "SHADOW0",    "SHADOW1",     "SHADOW2",     "SHADOW3",     "SHADOWS",
    "SHADOWVSM0", "SHADOWVSM1",  "SHADOWVSM2",  "SHADOWVSM3",  "SHADOWPCF0",
    "SHADOWPCF1", "SHADOWPCF2",  "SHADOWPCF3",  "NORMAL",      "UV1",
    "UV2",        "VERTEXCOLOR", "VERTEXALPHA", "INSTANCES",   "SPECULARTERM"};
  defines.resize(_keys.size());
  for (size_t i = 0; i < _keys.size(); ++i) {
    defines[i] = false;
  }
}

NormalMaterialDefines::~NormalMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
