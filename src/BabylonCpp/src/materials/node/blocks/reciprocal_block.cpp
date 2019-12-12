#include <babylon/materials/node/blocks/reciprocal_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

ReciprocalBlock::ReciprocalBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , input{this, &ReciprocalBlock::get_input}
    , output{this, &ReciprocalBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _outputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
}

ReciprocalBlock::~ReciprocalBlock() = default;

std::string ReciprocalBlock::getClassName() const
{
  return "ReciprocalBlock";
}

NodeMaterialConnectionPointPtr& ReciprocalBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ReciprocalBlock::get_output()
{
  return _outputs[0];
}

ReciprocalBlock& ReciprocalBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(" = 1. / %s;\r\n", input()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
