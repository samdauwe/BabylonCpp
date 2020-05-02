#include <babylon/materialslibrary/custom/custom_material.h>

#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {
namespace MaterialsLibrary {

unsigned int CustomMaterial::ShaderIndexer = 1;

CustomMaterial::CustomMaterial(const std::string& iName, Scene* scene)
    : StandardMaterial{iName, scene}
{
  FragmentShader = Effect::ShadersStore()["defaultPixelShader"];
  VertexShader   = Effect::ShadersStore()["defaultVertexShader"];
}

CustomMaterial::~CustomMaterial() = default;

void CustomMaterial::AttachAfterBind(Mesh* /*mesh*/, Effect* effect)
{
  if (!_newUniformInstances.empty()) {
    for (const auto& el : _newUniformInstances) {
      const auto ea                   = StringTools::split(el.first, '-');
      const auto& _newUniformInstance = el.second;
      if (ea[0] == "vec2") {
        effect->setVector2(ea[1], _newUniformInstance.vec2);
      }
      else if (ea[0] == "vec3") {
        effect->setVector3(ea[1], _newUniformInstance.vec3);
      }
      else if (ea[0] == "vec4") {
        effect->setVector4(ea[1], _newUniformInstance.vec4);
      }
      else if (ea[0] == "mat4") {
        effect->setMatrix(ea[1], _newUniformInstance.mat4);
      }
      else if (ea[0] == "float") {
        effect->setFloat(ea[1], _newUniformInstance.floatValue);
      }
    }
  }
  if (!_newSamplerInstances.empty()) {
    for (const auto& el : _newSamplerInstances) {
      const auto ea                   = StringTools::split(el.first, '-');
      const auto& _newSamplerInstance = el.second;
      if (ea[0] == "sampler2D" && _newSamplerInstance->isReady()
          && _newSamplerInstance->isReady()) {
        effect->setTexture(ea[1], _newSamplerInstance);
      }
    }
  }
}

std::vector<std::string> CustomMaterial::ReviewUniform(const std::string& iName,
                                                       std::vector<std::string> arr)
{
  if (iName == "uniform" && !_newUniforms.empty()) {
    for (const auto& ind : _newUniforms) {
      if (!StringTools::contains(ind, "sampler")) {
        arr.emplace_back(ind);
      }
    }
  }
  if (iName == "sampler" && !_newUniforms.empty()) {
    for (const auto& ind : _newUniforms) {
      if (!StringTools::contains(ind, "sampler")) {
        arr.emplace_back(ind);
      }
    }
  }
  return arr;
}

std::string CustomMaterial::Builder(const std::string& /*shaderName*/,
                                    const std::vector<std::string>& uniforms,
                                    const std::vector<std::string>& /*uniformBuffers*/,
                                    const std::vector<std::string>& samplers,
                                    StandardMaterialDefines& /*defines*/,
                                    std::vector<std::string> attributes)
{
  if (_customAttributes.size() > 0) {
    for (const auto& customAttribute : _customAttributes) {
      attributes.emplace_back(customAttribute);
    }
  }

  ReviewUniform("uniform", uniforms);
  ReviewUniform("sampler", samplers);

  if (_isCreatedShader) {
    return _createdShaderName;
  }
  _isCreatedShader = false;

  ++CustomMaterial::ShaderIndexer;
  std::string customName = "custom_" + std::to_string(CustomMaterial::ShaderIndexer);

  auto _VertexShader
    = StringTools::replace(VertexShader, "#define CUSTOM_VERTEX_BEGIN",
                           (!CustomParts.Vertex_Begin.empty() ? CustomParts.Vertex_Begin : ""));
  StringTools::replaceInPlace(
    _VertexShader, "#define CUSTOM_VERTEX_DEFINITIONS",
    (!_customUniform.empty() ? StringTools::join(_customUniform, '\n') : "")
      + (!CustomParts.Vertex_Definitions.empty() ? CustomParts.Vertex_Definitions : ""));
  StringTools::replaceInPlace(
    _VertexShader, "#define CUSTOM_VERTEX_MAIN_BEGIN",
    (!CustomParts.Vertex_MainBegin.empty() ? CustomParts.Vertex_MainBegin : ""));
  StringTools::replaceInPlace(_VertexShader, "#define CUSTOM_VERTEX_UPDATE_POSITION",
                              (!CustomParts.Vertex_Before_PositionUpdated.empty() ?
                                 CustomParts.Vertex_Before_PositionUpdated :
                                 ""));
  StringTools::replaceInPlace(_VertexShader, "#define CUSTOM_VERTEX_UPDATE_NORMAL",
                              (!CustomParts.Vertex_Before_NormalUpdated.empty() ?
                                 CustomParts.Vertex_Before_NormalUpdated :
                                 ""));
  StringTools::replaceInPlace(
    _VertexShader, "#define CUSTOM_VERTEX_MAIN_END",
    (!CustomParts.Vertex_MainEnd.empty() ? CustomParts.Vertex_MainEnd : ""));
  Effect::ShadersStore()[customName + "VertexShader"] = _VertexShader;

  if (!CustomParts.Vertex_After_WorldPosComputed.empty()) {
    Effect::ShadersStore()[name + "VertexShader"] = StringTools::replace(
      Effect::ShadersStore()[name + "VertexShader"], "#define CUSTOM_VERTEX_UPDATE_WORLDPOS",
      CustomParts.Vertex_After_WorldPosComputed);
  }

  auto _FragmentShader
    = StringTools::replace(FragmentShader, "'#define CUSTOM_FRAGMENT_BEGIN",
                           (!CustomParts.Fragment_Begin.empty() ? CustomParts.Fragment_Begin : ""));
  StringTools::replaceInPlace(
    _FragmentShader, "#define CUSTOM_FRAGMENT_MAIN_BEGIN",
    (!CustomParts.Fragment_MainBegin.empty() ? CustomParts.Fragment_MainBegin : ""));
  StringTools::replaceInPlace(
    _FragmentShader, "#define CUSTOM_FRAGMENT_DEFINITIONS",
    (!_customUniform.empty() ? StringTools::join(_customUniform, '\n') : "")
      + (!CustomParts.Fragment_Definitions.empty() ? CustomParts.Fragment_Definitions : ""));
  StringTools::replaceInPlace(
    _FragmentShader, "#define CUSTOM_FRAGMENT_UPDATE_DIFFUSE",
    (!CustomParts.Fragment_Custom_Diffuse.empty() ? CustomParts.Fragment_Custom_Diffuse : ""));
  StringTools::replaceInPlace(
    _FragmentShader, "#define CUSTOM_FRAGMENT_UPDATE_ALPHA",
    (!CustomParts.Fragment_Custom_Alpha.empty() ? CustomParts.Fragment_Custom_Alpha : ""));
  StringTools::replaceInPlace(
    _FragmentShader, "#define CUSTOM_FRAGMENT_BEFORE_LIGHTS",
    (!CustomParts.Fragment_Before_Lights.empty() ? CustomParts.Fragment_Before_Lights : ""));
  StringTools::replaceInPlace(
    _FragmentShader, "#define CUSTOM_FRAGMENT_BEFORE_FOG",
    (!CustomParts.Fragment_Before_Fog.empty() ? CustomParts.Fragment_Before_Fog : ""));
  StringTools::replaceInPlace(
    _FragmentShader, "#define CUSTOM_FRAGMENT_BEFORE_FRAGCOLOR",
    (!CustomParts.Fragment_Before_FragColor.empty() ? CustomParts.Fragment_Before_FragColor : ""));
  Effect::ShadersStore()[customName + "PixelShader"] = _FragmentShader;

  _isCreatedShader   = true;
  _createdShaderName = customName;

  return customName;
}

CustomMaterial&
CustomMaterial::AddUniform(const std::string& iName, const std::string& kind,
                           const std::optional<std::variant<UniformInstance, TexturePtr>>& param)
{
  if (param) {
    if (!StringTools::contains(kind, "sampler") && std::holds_alternative<TexturePtr>(*param)) {
      _newSamplerInstances[kind + "-" + iName] = std::get<TexturePtr>(*param);
    }
    else if (std::holds_alternative<UniformInstance>(*param)) {
      _newUniformInstances[kind + "-" + iName] = std::get<UniformInstance>(*param);
    }
  }
  _customUniform.emplace_back("uniform " + kind + " " + iName + ";");
  _newUniforms.emplace_back(iName);

  return *this;
}

CustomMaterial& CustomMaterial::AddAttribute(const std::string& iName)
{
  _customAttributes.emplace_back(iName);

  return *this;
}

CustomMaterial& CustomMaterial::Fragment_Begin(const std::string& shaderPart)
{
  CustomParts.Fragment_Begin = shaderPart;
  return *this;
}

CustomMaterial& CustomMaterial::Fragment_Definitions(const std::string& shaderPart)
{
  CustomParts.Fragment_Definitions = shaderPart;
  return *this;
}

CustomMaterial& CustomMaterial::Fragment_MainBegin(const std::string& shaderPart)
{
  CustomParts.Fragment_MainBegin = shaderPart;
  return *this;
}

CustomMaterial& CustomMaterial::Fragment_Custom_Diffuse(const std::string& shaderPart)
{
  CustomParts.Fragment_Custom_Diffuse = StringTools::replace(shaderPart, "result", "diffuseColor");
  return *this;
}

CustomMaterial& CustomMaterial::Fragment_Custom_Alpha(const std::string& shaderPart)
{
  CustomParts.Fragment_Custom_Alpha = StringTools::replace(shaderPart, "result", "alpha");
  return *this;
}

CustomMaterial& CustomMaterial::Fragment_Before_Lights(const std::string& shaderPart)
{
  CustomParts.Fragment_Before_Lights = shaderPart;
  return *this;
}

CustomMaterial& CustomMaterial::Fragment_Before_Fog(const std::string& shaderPart)
{
  CustomParts.Fragment_Before_Fog = shaderPart;
  return *this;
}

CustomMaterial& CustomMaterial::Fragment_Before_FragColor(const std::string& shaderPart)
{
  CustomParts.Fragment_Before_FragColor = StringTools::replace(shaderPart, "result", "color");
  return *this;
}

CustomMaterial& CustomMaterial::Vertex_Begin(const std::string& shaderPart)
{
  CustomParts.Vertex_Begin = shaderPart;
  return *this;
}

CustomMaterial& CustomMaterial::Vertex_Definitions(const std::string& shaderPart)
{
  CustomParts.Vertex_Definitions = shaderPart;
  return *this;
}

CustomMaterial& CustomMaterial::Vertex_MainBegin(const std::string& shaderPart)
{
  CustomParts.Vertex_MainBegin = shaderPart;
  return *this;
}

CustomMaterial& CustomMaterial::Vertex_Before_PositionUpdated(const std::string& shaderPart)
{
  CustomParts.Vertex_Before_PositionUpdated
    = StringTools::replace(shaderPart, "result", "positionUpdated");
  return *this;
}

CustomMaterial& CustomMaterial::Vertex_Before_NormalUpdated(const std::string& shaderPart)
{
  CustomParts.Vertex_Before_NormalUpdated
    = StringTools::replace(shaderPart, "result", "normalUpdated");
  return *this;
}

CustomMaterial& CustomMaterial::Vertex_After_WorldPosComputed(const std::string& shaderPart)
{
  CustomParts.Vertex_After_WorldPosComputed = shaderPart;
  return *this;
}

CustomMaterial& CustomMaterial::Vertex_MainEnd(const std::string& shaderPart)
{
  CustomParts.Vertex_MainEnd = shaderPart;
  return *this;
}

void CustomMaterial::_afterBind(Mesh* mesh, Effect* effect)
{
  if (!effect) {
    return;
  }
  AttachAfterBind(mesh, effect);
  _afterBind(mesh, effect);
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
