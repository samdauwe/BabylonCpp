#include <babylon/materials/pbr/imaterial_sub_surface_defines.h>

#include <sstream>

namespace BABYLON {

IMaterialSubSurfaceDefines::IMaterialSubSurfaceDefines()
{
  boolDef = {
    {"SUBSURFACE", false}, //

    {"SS_REFRACTION", false},                              //
    {"SS_REFRACTION_USE_INTENSITY_FROM_TEXTURE", false},   //
    {"SS_TRANSLUCENCY", false},                            //
    {"SS_TRANSLUCENCY_USE_INTENSITY_FROM_TEXTURE", false}, //
    {"SS_SCATTERING", false},                              //

    {"SS_THICKNESSANDMASK_TEXTURE", false},         //
    {"SS_THICKNESSANDMASK_TEXTUREDIRECTUV", false}, //
    {"SS_HAS_THICKNESS", false},                    //
    {"SS_REFRACTIONINTENSITY_TEXTURE", false},      //
    {"SS_TRANSLUCENCYINTENSITY_TEXTURE", false},    //

    {"SS_REFRACTIONMAP_3D", false},              //
    {"SS_REFRACTIONMAP_OPPOSITEZ", false},       //
    {"SS_LODINREFRACTIONALPHA", false},          //
    {"SS_GAMMAREFRACTION", false},               //
    {"SS_RGBDREFRACTION", false},                //
    {"SS_LINEARSPECULARREFRACTION", false},      //
    {"SS_LINKREFRACTIONTOTRANSPARENCY", false},  //
    {"SS_ALBEDOFORREFRACTIONTINT", false},       //
    {"SS_ALBEDOFORTRANSLUCENCYTINT", false},     //
    {"SS_USE_LOCAL_REFRACTIONMAP_CUBIC", false}, //
    {"SS_USE_THICKNESS_AS_DEPTH", false},        //

    {"SS_MASK_FROM_THICKNESS_TEXTURE", false}, //
    {"SS_USE_GLTF_TEXTURES", false},           //
  };

  intDef = {
    {"SS_REFRACTIONINTENSITY_TEXTUREDIRECTUV", 0},   //
    {"SS_TRANSLUCENCYINTENSITY_TEXTUREDIRECTUV", 0}, //
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
