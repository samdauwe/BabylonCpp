#include <babylon/extensions/shaderbuilder/normals.h>

#include <babylon/core/string.h>
#include <babylon/extensions/shaderbuilder/shader_material_helper_statics.h>

namespace BABYLON {
namespace Extensions {

const std::string Normals::Default
  = std::string(ShaderMaterialHelperStatics::Normal);
const std::string Normals::Inverse
  = String::concat("-1.*", std::string(ShaderMaterialHelperStatics::Normal));
const std::string Normals::Pointed = String::concat(
  "normalize(", std::string(ShaderMaterialHelperStatics::Position), "-",
  std::string(ShaderMaterialHelperStatics::Center), ")");
const std::string Normals::Flat = String::concat(
  "normalize(cross(dFdx(", std::string(ShaderMaterialHelperStatics::Position),
  " * -1.), dFdy(", std::string(ShaderMaterialHelperStatics::Position), ")))");
const std::string Normals::Map = "normalMap()";

} // end of namespace Extensions
} // end of namespace BABYLON
