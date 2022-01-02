#include <babylon/materials/pbr/imaterial_sheen_defines.h>

#include <sstream>

namespace BABYLON {

IMaterialSheenDefines::IMaterialSheenDefines()
{
  boolDef = {
    {"SHEEN", false},                                //
    {"SHEEN_TEXTURE", false},                        //
    {"SHEEN_TEXTURE_ROUGHNESS", false},              //
    {"SHEEN_LINKWITHALBEDO", false},                 //
    {"SHEEN_ROUGHNESS", false},                      //
    {"SHEEN_ALBEDOSCALING", false},                  //
    {"SHEEN_USE_ROUGHNESS_FROM_MAINTEXTURE", false}, //
    {"SHEEN_TEXTURE_ROUGHNESS_IDENTICAL", false},    //
  };

  intDef = {
    {"SHEEN_TEXTUREDIRECTUV", 0},           //
    {"SHEEN_TEXTURE_ROUGHNESSDIRECTUV", 0}, //
  };
}

IMaterialSheenDefines::~IMaterialSheenDefines() = default;

std::string IMaterialSheenDefines::toString() const
{
  std::ostringstream oss;
  oss << MaterialDefines::toString();

  return oss.str();
}

} // end of namespace BABYLON
