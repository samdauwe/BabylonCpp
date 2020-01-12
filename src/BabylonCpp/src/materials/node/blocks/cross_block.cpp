#include <babylon/materials/node/blocks/cross_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

CrossBlock::CrossBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &CrossBlock::get_left}
    , right{this, &CrossBlock::get_right}
    , output{this, &CrossBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Vector3);

  _linkConnectionTypes(0, 1);

  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Float);
  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector2);
  _inputs[1]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Float);
  _inputs[1]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
  _inputs[1]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector2);
}

CrossBlock::~CrossBlock() = default;

std::string CrossBlock::getClassName() const
{
  return "CrossBlock";
}

NodeMaterialConnectionPointPtr& CrossBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& CrossBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& CrossBlock::get_output()
{
  return _outputs[1];
}

CrossBlock& CrossBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString += _declareOutput(iOutput, state)
                             + StringTools::printf(" = cross(%s.xyz, %s.xyz);\r\n",
                                                   left()->associatedVariableName().c_str(),
                                                   right()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
