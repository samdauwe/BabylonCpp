#include <babylon/materials/node/blocks/multiply_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

MultiplyBlock::MultiplyBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &MultiplyBlock::get_left}
    , right{this, &MultiplyBlock::get_right}
    , output{this, &MultiplyBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
}

MultiplyBlock::~MultiplyBlock()
{
}

const std::string MultiplyBlock::getClassName() const
{
  return "MultiplyBlock";
}

NodeMaterialConnectionPointPtr& MultiplyBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& MultiplyBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& MultiplyBlock::get_output()
{
  return _outputs[0];
}

MultiplyBlock& MultiplyBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& output = _outputs[0];

  state.compilationString
    += _declareOutput(output, state)
       + String::printf(" = %s * %s;\r\n",
                        left()->associatedVariableName().c_str(),
                        right()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
