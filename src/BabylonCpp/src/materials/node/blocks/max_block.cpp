#include <babylon/materials/node/blocks/max_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

MaxBlock::MaxBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &MaxBlock::get_left}
    , right{this, &MaxBlock::get_right}
    , output{this, &MaxBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
}

MaxBlock::~MaxBlock()
{
}

const std::string MaxBlock::getClassName() const
{
  return "MaxBlock";
}

NodeMaterialConnectionPointPtr& MaxBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& MaxBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& MaxBlock::get_output()
{
  return _outputs[0];
}

MaxBlock& MaxBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& output = _outputs[0];

  state.compilationString
    += _declareOutput(output, state)
       + String::printf(" = max(%s, %s);\r\n",
                        left()->associatedVariableName().c_str(),
                        right()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
