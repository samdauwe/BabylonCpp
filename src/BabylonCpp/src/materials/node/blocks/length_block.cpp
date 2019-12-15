#include <babylon/materials/node/blocks/length_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

LengthBlock::LengthBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , value{this, &LengthBlock::get_value}
    , output{this, &LengthBlock::get_output}
{
  registerInput("value", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Float);

  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Float);
  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
}

LengthBlock::~LengthBlock() = default;

std::string LengthBlock::getClassName() const
{
  return "LengthBlock";
}

NodeMaterialConnectionPointPtr& LengthBlock::get_value()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& LengthBlock::get_output()
{
  return _outputs[0];
}

LengthBlock& LengthBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(" = length(%s);\r\n", value()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
