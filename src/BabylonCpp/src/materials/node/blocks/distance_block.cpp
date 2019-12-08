#include <babylon/materials/node/blocks/distance_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

DistanceBlock::DistanceBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , left{this, &DistanceBlock::get_left}
    , right{this, &DistanceBlock::get_right}
    , output{this, &DistanceBlock::get_output}
{
  registerInput("left", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("right", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Float);

  _linkConnectionTypes(0, 1);

  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Float);
  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
  _inputs[1]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Float);
  _inputs[1]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
}

DistanceBlock::~DistanceBlock()
{
}

std::string DistanceBlock::getClassName() const
{
  return "DistanceBlock";
}

NodeMaterialConnectionPointPtr& DistanceBlock::get_left()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& DistanceBlock::get_right()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& DistanceBlock::get_output()
{
  return _outputs[0];
}

DistanceBlock& DistanceBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(" = length(%s - %s);\r\n", left()->associatedVariableName().c_str(),
                        right()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
