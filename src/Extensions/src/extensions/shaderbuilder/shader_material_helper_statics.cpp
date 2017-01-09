#include <babylon/extensions/shaderbuilder/shader_material_helper_statics.h>

namespace BABYLON {
namespace Extensions {

constexpr bool ShaderMaterialHelperStatics::Dark;
constexpr bool ShaderMaterialHelperStatics::Light;

const std::string ShaderMaterialHelperStatics::PrecisionHighMode   = "highp";
const std::string ShaderMaterialHelperStatics::PrecisionMediumMode = "mediump";

const std::string ShaderMaterialHelperStatics::face_back  = "!gl_FrontFacing";
const std::string ShaderMaterialHelperStatics::face_front = "gl_FrontFacing";

const std::string ShaderMaterialHelperStatics::AttrPosition = "position";
const std::string ShaderMaterialHelperStatics::AttrNormal   = "normal";
const std::string ShaderMaterialHelperStatics::AttrUv       = "uv";
const std::string ShaderMaterialHelperStatics::AttrUv2      = "uv2";

const std::string ShaderMaterialHelperStatics::AttrTypeForPosition = "vec3";
const std::string ShaderMaterialHelperStatics::AttrTypeForNormal   = "vec3";
const std::string ShaderMaterialHelperStatics::AttrTypeForUv       = "vec2";
const std::string ShaderMaterialHelperStatics::AttrTypeForUv2      = "vec2";

const std::string ShaderMaterialHelperStatics::uniformView      = "view";
const std::string ShaderMaterialHelperStatics::uniformWorld     = "world";
const std::string ShaderMaterialHelperStatics::uniformWorldView = "worldView";
const std::string ShaderMaterialHelperStatics::uniformViewProjection
  = "viewProjection";
const std::string ShaderMaterialHelperStatics::uniformWorldViewProjection
  = "worldViewProjection";

const std::string ShaderMaterialHelperStatics::uniformStandardType = "mat4";
const std::string ShaderMaterialHelperStatics::uniformFlags        = "flags";

const std::string ShaderMaterialHelperStatics::Mouse  = "mouse";
const std::string ShaderMaterialHelperStatics::Screen = "screen";
const std::string ShaderMaterialHelperStatics::Camera = "camera";
const std::string ShaderMaterialHelperStatics::Look   = "look";

const std::string ShaderMaterialHelperStatics::Time          = "time";
const std::string ShaderMaterialHelperStatics::GlobalTime    = "gtime";
const std::string ShaderMaterialHelperStatics::Position      = "pos";
const std::string ShaderMaterialHelperStatics::WorldPosition = "wpos";

const std::string ShaderMaterialHelperStatics::Normal      = "nrm";
const std::string ShaderMaterialHelperStatics::WorldNormal = "wnrm";
const std::string ShaderMaterialHelperStatics::Uv          = "vuv";
const std::string ShaderMaterialHelperStatics::Uv2         = "vuv2";
const std::string ShaderMaterialHelperStatics::Center      = "center";

const std::string ShaderMaterialHelperStatics::ReflectMatrix = "refMat";

const std::string ShaderMaterialHelperStatics::Texture2D   = "txtRef_";
const std::string ShaderMaterialHelperStatics::TextureCube = "cubeRef_";

int ShaderMaterialHelperStatics::IdentityHelper;

} // end of namespace Extensions
} // end of namespace BABYLON
