#include <babylon/materials/node/node_material_build_state.h>

#include <sstream>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>

namespace BABYLON {

NodeMaterialBuildState::NodeMaterialBuildState()
    : supportUniformBuffers{false}, _repeatableContentAnchorIndex{0}
{
}

std::string NodeMaterialBuildState::_repeatableContentAnchor()
{
  return get__repeatableContentAnchor();
}

void NodeMaterialBuildState::finalize(const NodeMaterialBuildState& state)
{
  auto emitComments   = state.sharedData->emitComments;
  auto isFragmentMode = (target == NodeMaterialBlockTargets::Fragment);

  compilationString
    = String::printf("\r\n%svoid main(void) {\r\n%s", emitComments ? "//Entry point\r\n" : "",
                     compilationString.c_str());

  std::string functionCode = "";
  for (const auto& functionItem : functions) {
    functionCode += functionItem.second + "\r\n";
  }
  compilationString = String::printf("\r\n\r\n%s", functionCode.c_str(), compilationString.c_str());

  if (!isFragmentMode && !_varyingTransfer.empty()) {
    compilationString
      = String::printf("%s\r\n%s", compilationString.c_str(), _varyingTransfer.c_str());
  }

  compilationString = String::printf("%s\r\n}", compilationString.c_str());

  if (!sharedData->varyingDeclaration.empty()) {
    compilationString
      = String::printf("\r\n%s%s\r\n%s", emitComments ? "//Varyings\r\n" : "",
                       sharedData->varyingDeclaration.c_str(), compilationString.c_str());
  }

  if (!_samplerDeclaration.empty()) {
    compilationString = String::printf("\r\n%s%s\r\n%s)", emitComments ? "//Samplers\r\n" : "",
                                       _samplerDeclaration.c_str(), compilationString.c_str());
  }

  if (!_uniformDeclaration.empty()) {
    compilationString = String::printf("\r\n%s%s\r\n%s)", emitComments ? "//Uniforms\r\n" : "",
                                       _uniformDeclaration.c_str(), compilationString.c_str());
  }

  if (!_attributeDeclaration.empty() && !isFragmentMode) {
    compilationString = String::printf("\r\n%s%s\r\n%s)", emitComments ? "//Attributes\r\n" : "",
                                       _attributeDeclaration.c_str(), compilationString.c_str());
  }

  for (const auto& extensionsItem : extensions) {
    const auto& extension = extensionsItem.second;
    compilationString
      = String::printf("\r\n%s\r\n%s", extension.c_str(), compilationString.c_str());
  }

  _builtCompilationString = compilationString;
}

std::string NodeMaterialBuildState::get__repeatableContentAnchor()
{
  return String::printf("###___ANCHOR$%d___###", _repeatableContentAnchorIndex++);
}

std::string NodeMaterialBuildState::_getFreeVariableName(std::string prefix)
{
  prefix = String::regexReplace(prefix, "[^a-zA-Z_]+", "");

  if (!stl_util::contains(sharedData->variableNames, prefix)) {
    sharedData->variableNames[prefix] = 0;

    // Check reserved words
    if (prefix == "output" || prefix == "texture") {
      return prefix + std::to_string(sharedData->variableNames[prefix]);
    }

    return prefix;
  }
  else {
    ++sharedData->variableNames[prefix];
  }

  return prefix + std::to_string(sharedData->variableNames[prefix]);
}

std::string NodeMaterialBuildState::_getFreeDefineName(const std::string& prefix)
{
  if (!stl_util::contains(sharedData->defineNames, prefix)) {
    sharedData->defineNames[prefix] = 0;
  }
  else {
    ++sharedData->defineNames[prefix];
  }

  return prefix + std::to_string(sharedData->defineNames[prefix]);
}

void NodeMaterialBuildState::_excludeVariableName(const std::string& name)
{
  sharedData->variableNames[name] = 0;
}

std::string NodeMaterialBuildState::_getGLType(NodeMaterialBlockConnectionPointTypes type) const
{
  switch (type) {
    case NodeMaterialBlockConnectionPointTypes::Float:
      return "float";
    case NodeMaterialBlockConnectionPointTypes::Int:
      return "int";
    case NodeMaterialBlockConnectionPointTypes::Vector2:
      return "vec2";
    case NodeMaterialBlockConnectionPointTypes::Color3:
    case NodeMaterialBlockConnectionPointTypes::Vector3:
      return "vec3";
    case NodeMaterialBlockConnectionPointTypes::Color4:
    case NodeMaterialBlockConnectionPointTypes::Vector4:
      return "vec4";
    case NodeMaterialBlockConnectionPointTypes::Matrix:
      return "mat4";
    default:
      break;
  }

  return "";
}

void NodeMaterialBuildState::_emitExtension(const std::string& name, const std::string& extension)
{
  if (stl_util::contains(extensions, name) && !extensions[name].empty()) {
    return;
  }

  extensions[name] = extension;
}

void NodeMaterialBuildState::_emitFunction(const std::string& name, std::string code,
                                           const std::string& comments)
{
  if (stl_util::contains(functions, name) && !functions[name].empty()) {
    return;
  }

  if (sharedData->emitComments) {
    code = comments + "\r\n" + code;
  }

  functions[name] = code;
}

std::string NodeMaterialBuildState::_emitCodeFromInclude(
  const std::string& includeName, const std::string& comments,
  const std::optional<EmitCodeFromIncludeOptions>& options)
{
  if (options && !options->repeatKey.empty()) {
    return String::printf("#include<%s>[0..%s]\r\n", includeName.c_str(),
                          options->repeatKey.c_str());
  }

  auto code = (stl_util::contains(Effect::IncludesShadersStore(), includeName) ?
                 Effect::IncludesShadersStore()[includeName] :
                 "")
              + "\r\n";

  if (sharedData->emitComments) {
    code = comments + "\r\n" + code;
  }

  if (!options) {
    return code;
  }

  if (!options->replaceStrings.empty()) {
    for (const auto& replaceString : options->replaceStrings) {
      code = String::regexReplace(code, replaceString.search, replaceString.replace);
    }
  }

  return code;
}

void NodeMaterialBuildState::_emitFunctionFromInclude(
  const std::string& includeName, const std::string& comments,
  const std::optional<EmitFunctionFromIncludeOptions>& options, const std::string& storeKey)
{
  auto key = includeName + storeKey;
  if (stl_util::contains(functions, key) && !functions[key].empty()) {
    return;
  }

  if (!options
      || (!options->removeAttributes && !options->removeUniforms && !options->removeVaryings
          && !options->removeIfDef && options->replaceStrings.empty())) {

    if (options && !options->repeatKey.empty()) {
      functions[key] = String::printf("#include<%s>[0..%s]\r\n", includeName.c_str(),
                                      options->repeatKey.c_str());
    }
    else {
      functions[key] = String::printf("#include<%s>\r\n", includeName.c_str());
    }

    if (sharedData->emitComments) {
      functions[key] = comments + "\r\n" + functions[key];
    }

    return;
  }

  functions[key] = stl_util::contains(Effect::IncludesShadersStore(), includeName) ?
                     Effect::IncludesShadersStore()[includeName] :
                     "";

  if (sharedData->emitComments) {
    functions[key] = comments + "\r\n" + functions[key];
  }

  if (options->removeIfDef) {
    functions[key] = String::regexReplace(functions[key], "^\\s*?#ifdef.+$", "");
    functions[key] = String::regexReplace(functions[key], "^\\s*?#endif.*$", "");
    functions[key] = String::regexReplace(functions[key], "^\\s*?#else.*$", "");
    functions[key] = String::regexReplace(functions[key], "/^\\s*?#elif.*$", "");
  }

  if (options->removeAttributes) {
    functions[key] = String::regexReplace(functions[key], "^\\s*?attribute.+$", "");
  }

  if (options->removeUniforms) {
    functions[key] = String::regexReplace(functions[key], "^\\s*?uniform.+$", "");
  }

  if (options->removeVaryings) {
    functions[key] = String::regexReplace(functions[key], "^\\s*?varying.+$", "");
  }

  if (!options->replaceStrings.empty()) {
    for (const auto& replaceString : options->replaceStrings) {
      functions[key]
        = String::regexReplace(functions[key], replaceString.search, replaceString.replace);
    }
  }
}

bool NodeMaterialBuildState::_registerTempVariable(const std::string& name)
{
  if (stl_util::contains(sharedData->temps, name)) {
    return false;
  }

  sharedData->temps.emplace_back(name);
  return true;
}

bool NodeMaterialBuildState::_emitVaryingFromString(const std::string& name,
                                                    const std::string& type,
                                                    const std::string& define, bool notDefine)
{
  if (stl_util::contains(sharedData->varyings, name)) {
    return false;
  }

  sharedData->varyings.emplace_back(name);

  if (!define.empty()) {
    if (String::startsWith(define, "defined(")) {
      sharedData->varyingDeclaration += String::printf("#if %s\r\n", define.c_str());
    }
    else {
      sharedData->varyingDeclaration
        += String::printf("%s %s\r\n", notDefine ? "#ifndef" : "#ifdef", define.c_str());
    }
  }
  sharedData->varyingDeclaration
    += String::printf("varying %s %s;\r\n", type.c_str(), name.c_str());
  if (!define.empty()) {
    sharedData->varyingDeclaration += "#endif\r\n";
  }

  return true;
}

void NodeMaterialBuildState::_emitUniformFromString(const std::string& name,
                                                    const std::string& type,
                                                    const std::string& define, bool notDefine)
{
  if (stl_util::contains(uniforms, name)) {
    return;
  }

  uniforms.emplace_back(name);

  if (!define.empty()) {
    _uniformDeclaration
      += String::printf("%s %s\r\n", notDefine ? "#ifndef" : "#ifdef", define.c_str());
  }
  _uniformDeclaration += String::printf("uniform %s %s;\r\n", type.c_str(), name.c_str());
  if (!define.empty()) {
    _uniformDeclaration += "#endif\r\n";
  }
}

std::string NodeMaterialBuildState::_emitFloat(float value)
{
  if (fabsf(roundf(value) - value) <= 0.00001f) {
    return String::printf("%d.0", static_cast<int>(value));
  }

  return std::to_string(value);
}

} // end of namespace BABYLON
