#include <babylon/materials/pbr/imaterial_clear_coat_defines.h>

#include <sstream>

namespace BABYLON {

IMaterialClearCoatDefines::IMaterialClearCoatDefines()
{
  boolDef = {
    {"CLEARCOAT", false},                                //
    {"CLEARCOAT_DEFAULTIOR", false},                     //
    {"CLEARCOAT_TEXTURE", false},                        //
    {"CLEARCOAT_TEXTURE_ROUGHNESS", false},              //
    {"CLEARCOAT_BUMP", false},                           //
    {"CLEARCOAT_BUMPDIRECTUV", false},                   //
    {"CLEARCOAT_USE_ROUGHNESS_FROM_MAINTEXTURE", false}, //
    {"CLEARCOAT_TEXTURE_ROUGHNESS_IDENTICAL", false},    //
    {"CLEARCOAT_REMAP_F0", false},                       //

    {"CLEARCOAT_TINT", false},                 //
    {"CLEARCOAT_TINT_TEXTURE", false},         //
    {"CLEARCOAT_TINT_TEXTUREDIRECTUV", false}, //
  };

  intDef = {
    {"CLEARCOAT_TEXTUREDIRECTUV", 0},           //
    {"CLEARCOAT_TEXTURE_ROUGHNESSDIRECTUV", 0}, //
  };
}

IMaterialClearCoatDefines::~IMaterialClearCoatDefines() = default;

std::string IMaterialClearCoatDefines::toString() const
{
  std::ostringstream oss;
  oss << MaterialDefines::toString();

  return oss.str();
}

} // end of namespace BABYLON
