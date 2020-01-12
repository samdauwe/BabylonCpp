#include <babylon/materials/node/blocks/refract_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

RefractBlock::RefractBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , incident{this, &RefractBlock::get_incident}
    , normal{this, &RefractBlock::get_normal}
    , ior{this, &RefractBlock::get_ior}
    , output{this, &RefractBlock::get_output}
{
  registerInput("incident", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerInput("normal", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerInput("ior", NodeMaterialBlockConnectionPointTypes::Float);
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

RefractBlock::~RefractBlock() = default;

std::string RefractBlock::getClassName() const
{
  return "RefractBlock";
}

NodeMaterialConnectionPointPtr& RefractBlock::get_incident()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& RefractBlock::get_normal()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& RefractBlock::get_ior()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& RefractBlock::get_output()
{
  return _outputs[0];
}

RefractBlock& RefractBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString += _declareOutput(iOutput, state)
                             + StringTools::printf(" = refract(%s.xyz, %s.xyz, %s);\r\n",
                                                   incident()->associatedVariableName().c_str(),
                                                   normal()->associatedVariableName().c_str(),
                                                   ior()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
