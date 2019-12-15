#include <babylon/materials/node/blocks/normal_blend_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

NormalBlendBlock::NormalBlendBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , normalMap0{this, &NormalBlendBlock::get_normalMap0}
    , normalMap1{this, &NormalBlendBlock::get_normalMap1}
    , output{this, &NormalBlendBlock::get_output}
{
  registerInput("normalMap0", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerInput("normalMap1", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Vector3);

  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color3);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color4);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);

  _inputs[1]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color3);
  _inputs[1]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color4);
  _inputs[1]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);
}

NormalBlendBlock::~NormalBlendBlock() = default;

std::string NormalBlendBlock::getClassName() const
{
  return "NormalBlendBlock";
}

NodeMaterialConnectionPointPtr& NormalBlendBlock::get_normalMap0()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& NormalBlendBlock::get_normalMap1()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& NormalBlendBlock::get_output()
{
  return _outputs[0];
}

NormalBlendBlock& NormalBlendBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];
  const auto& input0  = _inputs[0];
  const auto& input1  = _inputs[1];
  const auto stepR    = state._getFreeVariableName("stepR");
  const auto stepG    = state._getFreeVariableName("stepG");

  state.compilationString += String::printf("float %s = step(0.5, %s.r);\r\n", stepR.c_str(),
                                            input0->associatedVariableName().c_str());
  state.compilationString += String::printf("float %s = step(0.5, %s.g);\r\n", stepG.c_str(),
                                            input0->associatedVariableName().c_str());
  state.compilationString += _declareOutput(iOutput, state) + ";\r\n";
  state.compilationString += String::printf(
    "%s.r = (1.0 - %s) * %s.r * %s.r * 2.0 + %s * (1.0 - %s.r) * (1.0 - %s.r) * 2.0;\r\n",
    iOutput->associatedVariableName().c_str(), stepR.c_str(),
    input0->associatedVariableName().c_str(), input1->associatedVariableName().c_str(),
    stepR.c_str(), input0->associatedVariableName().c_str(),
    input1->associatedVariableName().c_str());
  state.compilationString += String::printf(
    "%s.g = (1.0 - %s) * %s.g * %s.g * 2.0 + %s * (1.0 - %s.g) * (1.0 - %s.g) * 2.0;\r\n",
    iOutput->associatedVariableName().c_str(), stepG.c_str(),
    input0->associatedVariableName().c_str(), input1->associatedVariableName().c_str(),
    stepG.c_str(), input0->associatedVariableName().c_str(),
    input1->associatedVariableName().c_str());
  state.compilationString += String::printf(
    "%s.b = %s.b * %s.b;\r\n", iOutput->associatedVariableName().c_str(),
    input0->associatedVariableName().c_str(), input1->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
