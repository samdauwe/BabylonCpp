#include <babylon/materialslibrary/sky/sky_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

SkyMaterialDefines::SkyMaterialDefines() : MaterialDefines{}
{
  boolDef = {
    {"CLIPPLANE", false},   //
    {"CLIPPLANE2", false},  //
    {"CLIPPLANE3", false},  //
    {"CLIPPLANE4", false},  //
    {"POINTSIZE", false},   //
    {"FOG", false},         //
    {"VERTEXCOLOR", false}, //
    {"VERTEXALPHA", false}  //
  };
}

SkyMaterialDefines::~SkyMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
