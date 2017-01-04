#include <babylon/extensions/shaderbuilder/normals.h>

#include <babylon/core/string.h>
#include <babylon/extensions/shaderbuilder/shader_material_helper_statics.h>

namespace BABYLON {
namespace Extensions {

const std::string Normals::Default = ShaderMaterialHelperStatics::Normal;
const std::string Normals::Inverse
  = String::concat("-1.*", ShaderMaterialHelperStatics::Normal);
const std::string Normals::Pointed
  = String::concat("normalize(", ShaderMaterialHelperStatics::Position, "-",
                   ShaderMaterialHelperStatics::Center, ")");
const std::string Normals::Flat = String::concat(
  "normalize(cross(dFdx(", ShaderMaterialHelperStatics::Position,
  " * -1.), dFdy(", ShaderMaterialHelperStatics::Position, ")))");
const std::string Normals::Map = "normalMap()";

} // end of namespace Extensions
} // end of namespace BABYLON
