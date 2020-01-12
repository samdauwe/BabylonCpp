#include <babylon/materials/node/blocks/posterize_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

PosterizeBlock::PosterizeBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , value{this, &PosterizeBlock::get_value}
    , steps{this, &PosterizeBlock::get_steps}
    , output{this, &PosterizeBlock::get_output}
{
  registerInput("value", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("steps", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
  _linkConnectionTypes(0, 1);

  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
  _inputs[1]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
}

PosterizeBlock::~PosterizeBlock() = default;

std::string PosterizeBlock::getClassName() const
{
  return "PosterizeBlock";
}

NodeMaterialConnectionPointPtr& PosterizeBlock::get_value()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& PosterizeBlock::get_steps()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& PosterizeBlock::get_output()
{
  return _outputs[0];
}

PosterizeBlock& PosterizeBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString += _declareOutput(iOutput, state)
                             + StringTools::printf(" = floor(%s / (1.0 / %s)) * (1.0 / %s);\r\n",
                                                   value()->associatedVariableName().c_str(),
                                                   steps()->associatedVariableName().c_str(),
                                                   steps()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
