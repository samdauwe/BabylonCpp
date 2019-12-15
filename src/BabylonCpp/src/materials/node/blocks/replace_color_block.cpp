#include <babylon/materials/node/blocks/replace_color_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

ReplaceColorBlock::ReplaceColorBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , value{this, &ReplaceColorBlock::get_value}
    , reference{this, &ReplaceColorBlock::get_reference}
    , distance{this, &ReplaceColorBlock::get_distance}
    , replacement{this, &ReplaceColorBlock::get_replacement}
    , output{this, &ReplaceColorBlock::get_output}
{
  registerInput("value", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("reference", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("distance", NodeMaterialBlockConnectionPointTypes::Float);
  registerInput("replacement", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);
  _linkConnectionTypes(0, 3);

  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Float);
  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
  _inputs[1]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Float);
  _inputs[1]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
  _inputs[3]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Float);
  _inputs[3]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
}

ReplaceColorBlock::~ReplaceColorBlock() = default;

std::string ReplaceColorBlock::getClassName() const
{
  return "ReplaceColorBlock";
}

NodeMaterialConnectionPointPtr& ReplaceColorBlock::get_value()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ReplaceColorBlock::get_reference()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ReplaceColorBlock::get_distance()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& ReplaceColorBlock::get_replacement()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& ReplaceColorBlock::get_output()
{
  return _outputs[0];
}

ReplaceColorBlock& ReplaceColorBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString += _declareOutput(iOutput, state) + ";\r\n";
  state.compilationString += String::printf(
    "if (length(%s - %s) < %s) {\r\n", value()->associatedVariableName().c_str(),
    reference()->associatedVariableName().c_str(), distance()->associatedVariableName().c_str());
  state.compilationString
    += String::printf("%s = %s;\r\n", iOutput->associatedVariableName().c_str(),
                      replacement()->associatedVariableName().c_str());
  state.compilationString += String::printf("} else {\r\n");
  state.compilationString
    += String::printf("%s =  %s;\r\n", iOutput->associatedVariableName().c_str(),
                      value()->associatedVariableName().c_str());
  state.compilationString += String::printf("}\r\n");

  return *this;
}

} // end of namespace BABYLON
