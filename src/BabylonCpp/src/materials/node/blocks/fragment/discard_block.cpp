#include <babylon/materials/node/blocks/fragment/discard_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

DiscardBlock::DiscardBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment, true}
    , value{this, &DiscardBlock::get_value}
    , cutoff{this, &DiscardBlock::get_cutoff}
{
  registerInput("value", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("cutoff", NodeMaterialBlockConnectionPointTypes::Float, true);
}

DiscardBlock::~DiscardBlock() = default;

std::string DiscardBlock::getClassName() const
{
  return "DiscardBlock";
}

NodeMaterialConnectionPointPtr& DiscardBlock::get_value()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& DiscardBlock::get_cutoff()
{
  return _inputs[1];
}

DiscardBlock& DiscardBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  state.sharedData->hints.needAlphaTesting = true;

  state.compilationString
    += String::printf("if (%s < %s) discard;\r\n", value()->associatedVariableName().c_str(),
                      cutoff()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
