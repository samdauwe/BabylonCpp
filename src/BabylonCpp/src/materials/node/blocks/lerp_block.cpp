#include <babylon/materials/node/blocks/lerp_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

LerpBlock::LerpBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &LerpBlock::get_left}
    , right{this, &LerpBlock::get_right}
    , gradient{this, &LerpBlock::get_gradient}
    , output{this, &LerpBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("gradient", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
}

LerpBlock::~LerpBlock() = default;

std::string LerpBlock::getClassName() const
{
  return "LerpBlock";
}

NodeMaterialConnectionPointPtr& LerpBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& LerpBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& LerpBlock::get_gradient()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& LerpBlock::get_output()
{
  return _outputs[0];
}

LerpBlock& LerpBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + StringTools::printf(" = mix(%s, %s, %s);\r\n", left()->associatedVariableName().c_str(),
                             right()->associatedVariableName().c_str(),
                             gradient()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
