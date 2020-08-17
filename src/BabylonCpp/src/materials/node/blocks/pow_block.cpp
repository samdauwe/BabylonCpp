#include <babylon/materials/node/blocks/pow_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

PowBlock::PowBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , value{this, &PowBlock::get_value}
    , power{this, &PowBlock::get_power}
    , output{this, &PowBlock::get_output}
{
  registerInput("value", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("power", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
}

PowBlock::~PowBlock() = default;

std::string PowBlock::getClassName() const
{
  return "PowBlock";
}

NodeMaterialConnectionPointPtr& PowBlock::get_value()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& PowBlock::get_power()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& PowBlock::get_output()
{
  return _outputs[0];
}

PowBlock& PowBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + StringTools::printf(" = pow(%s, %s);\r\n", value()->associatedVariableName().c_str(),
                             power()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
