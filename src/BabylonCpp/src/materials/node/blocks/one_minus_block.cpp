#include <babylon/materials/node/blocks/one_minus_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

OneMinusBlock::OneMinusBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , input{this, &OneMinusBlock::get_input}
    , output{this, &OneMinusBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _outputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
}

OneMinusBlock::~OneMinusBlock() = default;

std::string OneMinusBlock::getClassName() const
{
  return "OneMinusBlock";
}

NodeMaterialConnectionPointPtr& OneMinusBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& OneMinusBlock::get_output()
{
  return _outputs[0];
}

OneMinusBlock& OneMinusBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(" = 1. - %s;\r\n", input()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
