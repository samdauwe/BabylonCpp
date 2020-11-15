#include <babylon/materials/node/blocks/nlerp_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

NLerpBlock::NLerpBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &NLerpBlock::get_left}
    , right{this, &NLerpBlock::get_right}
    , gradient{this, &NLerpBlock::get_gradient}
    , output{this, &NLerpBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("gradient", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
  _linkConnectionTypes(1, 2, true);

  _inputs[2]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Float);
}

NLerpBlock::~NLerpBlock()
{
}

std::string NLerpBlock::getClassName() const
{
  return "NLerpBlock";
}

NodeMaterialConnectionPointPtr& NLerpBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& NLerpBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& NLerpBlock::get_gradient()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& NLerpBlock::get_output()
{
  return _outputs[0];
}

NLerpBlock& NLerpBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString += _declareOutput(iOutput, state)
                             + StringTools::printf(" = normalize(mix(%s, %s, %s));\r\n",
                                                   left()->associatedVariableName().c_str(),
                                                   right()->associatedVariableName().c_str(),
                                                   gradient()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
