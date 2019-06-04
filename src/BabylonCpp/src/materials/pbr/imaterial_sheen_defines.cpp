#include <babylon/materials/pbr/imaterial_sheen_defines.h>

#include <sstream>

namespace BABYLON {

IMaterialSheenDefines::IMaterialSheenDefines() : MaterialDefines{}
{
  boolDef = {
    {"SHEEN", false},                 //
    {"SHEEN_TEXTURE", false},         //
    {"SHEEN_TEXTUREDIRECTUV", false}, //
    {"SHEEN_LINKWITHALBEDO", false},  //
  };
}

IMaterialSheenDefines::~IMaterialSheenDefines()
{
}

std::string IMaterialSheenDefines::toString() const
{
  std::ostringstream oss;
  oss << MaterialDefines::toString();

  return oss.str();
}

} // end of namespace BABYLON
