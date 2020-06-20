#include <babylon/materials/node/blocks/mod_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ModBlock::ModBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &ModBlock::get_left}
    , right{this, &ModBlock::get_right}
    , output{this, &ModBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
}

ModBlock::~ModBlock() = default;

std::string ModBlock::getClassName() const
{
  return "ModBlock";
}

NodeMaterialConnectionPointPtr& ModBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ModBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ModBlock::get_output()
{
  return _outputs[0];
}

ModBlock& ModBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + StringTools::printf(" = mod(%s, %s);\r\n", left()->associatedVariableName().c_str(),
                             right()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
