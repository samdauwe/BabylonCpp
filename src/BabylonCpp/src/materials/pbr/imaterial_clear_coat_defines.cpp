#include <babylon/materials/pbr/imaterial_clear_coat_defines.h>

#include <sstream>

namespace BABYLON {

IMaterialClearCoatDefines::IMaterialClearCoatDefines() : MaterialDefines{}
{
  boolDef = {
    {"CLEARCOAT", false},                      //
    {"CLEARCOAT_DEFAULTIOR", false},           //
    {"CLEARCOAT_TEXTURE", false},              //
    {"CLEARCOAT_TEXTUREDIRECTUV", false},      //
    {"CLEARCOAT_BUMP", false},                 //
    {"CLEARCOAT_BUMPDIRECTUV", false},         //
    {"CLEARCOAT_TINT", false},                 //
    {"CLEARCOAT_TINT_TEXTURE", false},         //
    {"CLEARCOAT_TINT_TEXTUREDIRECTUV", false}, //
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
