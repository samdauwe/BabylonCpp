#include <babylon/materials/node/blocks/gradient_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

GradientBlock::GradientBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , colorSteps{GradientBlockColorStep(0.f, Color3::White()),
                 GradientBlockColorStep(1.f, Color3::Black())}
    , gradient{this, &GradientBlock::get_gradient}
    , output{this, &GradientBlock::get_output}
{
  registerInput("gradient", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Color3);

  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector2);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector3);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color3);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color4);
}

GradientBlock::~GradientBlock()
{
}

std::string GradientBlock::getClassName() const
{
  return "GradientBlock";
}

NodeMaterialConnectionPointPtr& GradientBlock::get_gradient()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& GradientBlock::get_output()
{
  return _outputs[0];
}

std::string GradientBlock::_writeColorConstant(unsigned int index)
{
  const auto& step = colorSteps[index];
  return StringTools::printf("vec3(%f, %f, %f)", step.color.r, step.color.g, step.color.b);
}

GradientBlock& GradientBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  if (colorSteps.empty() || !gradient()->connectedPoint()) {
    state.compilationString += _declareOutput(iOutput, state) + " = vec3(0., 0., 0.);\r\n";
    return *this;
  }

  auto tempColor    = state._getFreeVariableName("gradientTempColor");
  auto tempPosition = state._getFreeVariableName("gradientTempPosition");

  state.compilationString
    += StringTools::printf("vec3 %s = %s;\r\n", tempColor.c_str(), _writeColorConstant(0).c_str());
  state.compilationString += StringTools::printf("float %s;\r\n", tempPosition.c_str());

  auto gradientSource = gradient()->associatedVariableName();

  if (gradient()->connectedPoint()->type() != NodeMaterialBlockConnectionPointTypes::Float) {
    gradientSource += ".x";
  }

  for (unsigned int index = 1; index < colorSteps.size(); ++index) {
    const auto& step         = colorSteps[index];
    const auto& previousStep = colorSteps[index - 1];
    state.compilationString += StringTools::printf(
      "%s = clamp((%s - %s) / (%s -  %s), 0.0, 1.0) * step(%s, %s);\r\n", tempPosition.c_str(),
      gradientSource.c_str(), state._emitFloat(previousStep.step).c_str(),
      state._emitFloat(step.step).c_str(), state._emitFloat(previousStep.step).c_str(),
      state._emitFloat(static_cast<float>(index)).c_str(),
      state._emitFloat(static_cast<float>(colorSteps.size() - 1)).c_str());
    state.compilationString
      += StringTools::printf("%s = mix(%s, %s, %s);\r\n", tempColor.c_str(), tempColor.c_str(),
                             _writeColorConstant(index).c_str(), tempPosition.c_str());
  }
  state.compilationString
    += _declareOutput(iOutput, state) + StringTools::printf(" = %s;\r\n", tempColor.c_str());

  return *this;
}

json GradientBlock::serialize() const
{
  return nullptr;
}

void GradientBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                 const std::string& /*rootUrl*/)
{
}

std::string GradientBlock::_dumpPropertiesCode()
{
  std::string codeString = "";

  for (const auto& colorStep : colorSteps) {
    codeString += StringTools::printf(
      "%s.colorSteps.emplace_back(GradientBlockColorStep(%s, Color3(%f, %f, %f)));\r\n",
      _codeVariableName.c_str(), colorStep.step, colorStep.color.r, colorStep.color.g,
      colorStep.color.b);
  }

  return codeString;
}

} // end of namespace BABYLON
