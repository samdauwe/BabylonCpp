#include <babylon/materials/pbr/imaterial_sub_surface_defines.h>

#include <sstream>

namespace BABYLON {

IMaterialSubSurfaceDefines::IMaterialSubSurfaceDefines() : MaterialDefines{}
{
  boolDef = {
    {"SUBSURFACE", false},                          //
    {"SS_REFRACTION", false},                       //
    {"SS_TRANSLUCENCY", false},                     //
    {"SS_SCATERRING", false},                       //
    {"SS_THICKNESSANDMASK_TEXTURE", false},         //
    {"SS_THICKNESSANDMASK_TEXTUREDIRECTUV", false}, //
    {"SS_REFRACTIONMAP_3D", false},                 //
    {"SS_REFRACTIONMAP_OPPOSITEZ", false},          //
    {"SS_LODINREFRACTIONALPHA", false},             //
    {"SS_GAMMAREFRACTION", false},                  //
    {"SS_RGBDREFRACTION", false},                   //
    {"SS_LINEARSPECULARREFRACTION", false},         //
    {"SS_LINKREFRACTIONTOTRANSPARENCY", false},     //
    {"SS_MASK_FROM_THICKNESS_TEXTURE", false},      //
  };
}

IMaterialSubSurfaceDefines::~IMaterialSubSurfaceDefines() = default;

std::string IMaterialSubSurfaceDefines::toString() const
{
  std::ostringstream oss;
  oss << MaterialDefines::toString();

  return oss.str();
}

} // end of namespace BABYLON
