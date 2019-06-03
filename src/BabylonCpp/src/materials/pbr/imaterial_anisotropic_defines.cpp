#include <babylon/materials/pbr/imaterial_anisotropic_defines.h>

#include <sstream>

namespace BABYLON {

IMaterialAnisotropicDefines::IMaterialAnisotropicDefines() : MaterialDefines{}
{
  boolDef = {
    {"ANISOTROPIC", false},                 //
    {"ANISOTROPIC_TEXTURE", false},         //
    {"ANISOTROPIC_TEXTUREDIRECTUV", false}, //
    {"MAINUV1", false},                     //
  };
}

IMaterialAnisotropicDefines::~IMaterialAnisotropicDefines()
{
}

std::string IMaterialAnisotropicDefines::toString() const
{
  std::ostringstream oss;
  oss << MaterialDefines::toString();

  return oss.str();
}

} // end of namespace BABYLON
