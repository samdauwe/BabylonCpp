#include <babylon/materials/node/blocks/min_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

MinBlock::MinBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &MinBlock::get_left}
    , right{this, &MinBlock::get_right}
    , output{this, &MinBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
}

MinBlock::~MinBlock()
{
}

std::string MinBlock::getClassName() const
{
  return "MinBlock";
}

NodeMaterialConnectionPointPtr& MinBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& MinBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& MinBlock::get_output()
{
  return _outputs[0];
}

MinBlock& MinBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(" = min(%s, %s);\r\n", left()->associatedVariableName().c_str(),
                        right()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
