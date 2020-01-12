#include <babylon/materials/node/blocks/negate_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

NegateBlock::NegateBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , value{this, &NegateBlock::get_value}
    , output{this, &NegateBlock::get_output}
{
  registerInput("value", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
}

NegateBlock::~NegateBlock() = default;

std::string NegateBlock::getClassName() const
{
  return "NegateBlock";
}

NodeMaterialConnectionPointPtr& NegateBlock::get_value()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& NegateBlock::get_output()
{
  return _outputs[0];
}

NegateBlock& NegateBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + StringTools::printf(" = -1.0 * %s;\r\n", value()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
