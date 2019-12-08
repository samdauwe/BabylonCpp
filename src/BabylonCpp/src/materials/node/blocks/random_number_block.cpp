#include <babylon/materials/node/blocks/random_number_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

RandomNumberBlock::RandomNumberBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , seed{this, &RandomNumberBlock::get_seed}
    , output{this, &RandomNumberBlock::get_output}
{
  registerInput("seed", NodeMaterialBlockConnectionPointTypes::Vector2);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Float);

  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector3);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color3);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color4);
}
RandomNumberBlock::~RandomNumberBlock()
{
}

std::string RandomNumberBlock::getClassName() const
{
  return "RandomNumberBlock";
}

NodeMaterialConnectionPointPtr& RandomNumberBlock::get_seed()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& RandomNumberBlock::get_output()
{
  return _outputs[0];
}

RandomNumberBlock& RandomNumberBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  const auto comments = String::printf("//%s", name.c_str());
  state._emitFunctionFromInclude("helperFunctions", comments);

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(" = getRand(%s.xy);\r\n", seed()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
