#include <babylon/materials/node/blocks/step_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

StepBlock::StepBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , value{this, &StepBlock::get_value}
    , edge{this, &StepBlock::get_edge}
    , output{this, &StepBlock::get_output}
{
  registerInput("value", NodeMaterialBlockConnectionPointTypes::Float);
  registerInput("edge", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Float);
}

StepBlock::~StepBlock()
{
}

std::string StepBlock::getClassName() const
{
  return "StepBlock";
}

NodeMaterialConnectionPointPtr& StepBlock::get_value()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& StepBlock::get_edge()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& StepBlock::get_output()
{
  return _outputs[0];
}

StepBlock& StepBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(" = step(%s, %s);\r\n", edge()->associatedVariableName().c_str(),
                        value()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
