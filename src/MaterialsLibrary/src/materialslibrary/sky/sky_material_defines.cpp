#include <babylon/materialslibrary/sky/sky_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

SkyMaterialDefines::SkyMaterialDefines()
{
  boolDef = {
    {"CLIPPLANE", false},                 //
    {"CLIPPLANE2", false},                //
    {"CLIPPLANE3", false},                //
    {"CLIPPLANE4", false},                //
    {"CLIPPLANE5", false},                //
    {"CLIPPLANE6", false},                //
    {"POINTSIZE", false},                 //
    {"FOG", false},                       //
    {"VERTEXCOLOR", false},               //
    {"VERTEXALPHA", false},               //
    {"IMAGEPROCESSINGPOSTPROCESS", false} //
  };
}

SkyMaterialDefines::~SkyMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
