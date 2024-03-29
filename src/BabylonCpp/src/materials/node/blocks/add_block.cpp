﻿#include <babylon/materials/node/blocks/add_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

AddBlock::AddBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &AddBlock::get_left}
    , right{this, &AddBlock::get_right}
    , output{this, &AddBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
}

AddBlock::~AddBlock() = default;

std::string AddBlock::getClassName() const
{
  return "AddBlock";
}

NodeMaterialConnectionPointPtr& AddBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& AddBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& AddBlock::get_output()
{
  return _outputs[0];
}

AddBlock& AddBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + StringTools::printf(" = %s + %s;\r\n", left()->associatedVariableName().c_str(),
                             right()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
