#include <babylon/materials/node/blocks/subtract_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

SubtractBlock::SubtractBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &SubtractBlock::get_left}
    , right{this, &SubtractBlock::get_right}
    , output{this, &SubtractBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
}

SubtractBlock::~SubtractBlock()
{
}

std::string SubtractBlock::getClassName() const
{
  return "SubtractBlock";
}

NodeMaterialConnectionPointPtr& SubtractBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& SubtractBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& SubtractBlock::get_output()
{
  return _outputs[0];
}

SubtractBlock& SubtractBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& output = _outputs[0];

  state.compilationString
    += _declareOutput(output, state)
       + String::printf(" = %s - %s;\r\n", left()->associatedVariableName().c_str(),
                        right()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
