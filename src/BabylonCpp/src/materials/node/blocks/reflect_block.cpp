#include <babylon/materials/node/blocks/reflect_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

ReflectBlock::ReflectBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , incident{this, &ReflectBlock::get_incident}
    , normal{this, &ReflectBlock::get_normal}
    , output{this, &ReflectBlock::get_output}
{
  registerInput("incident", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerInput("normal", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Vector3);

  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color3);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color4);
  _inputs[1]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);
  _inputs[1]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color3);
  _inputs[1]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Color4);
}

ReflectBlock::~ReflectBlock() = default;

std::string ReflectBlock::getClassName() const
{
  return "ReflectBlock";
}

NodeMaterialConnectionPointPtr& ReflectBlock::get_incident()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ReflectBlock::get_normal()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ReflectBlock::get_output()
{
  return _outputs[0];
}

ReflectBlock& ReflectBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString += _declareOutput(iOutput, state)
                             + String::printf(" = reflect(%s.xyz, %s.xyz);\r\n",
                                              incident()->associatedVariableName().c_str(),
                                              normal()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
