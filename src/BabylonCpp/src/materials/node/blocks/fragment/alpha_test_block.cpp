#include <babylon/materials/node/blocks/fragment/alpha_test_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

AlphaTestBlock::AlphaTestBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , alphaCutOff{0.4f}
    , color{this, &AlphaTestBlock::get_color}
    , alpha{this, &AlphaTestBlock::get_alpha}
{
  registerInput("color", NodeMaterialBlockConnectionPointTypes::Color4, true);
  registerInput("alpha", NodeMaterialBlockConnectionPointTypes::Float, true);
}

AlphaTestBlock::~AlphaTestBlock() = default;

std::string AlphaTestBlock::getClassName() const
{
  return "AlphaTestBlock";
}

NodeMaterialConnectionPointPtr& AlphaTestBlock::get_color()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& AlphaTestBlock::get_alpha()
{
  return _inputs[1];
}

AlphaTestBlock& AlphaTestBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  state.sharedData->hints.needAlphaTesting = true;

  if (color()->connectedPoint()) {
    state.compilationString += String::printf(
      "if (%s.a < %f) discard;\r\n", color()->associatedVariableName().c_str(), alphaCutOff);
  }
  else {
    state.compilationString += String::printf(
      "if (%s.a < %f) discard;\r\n", alpha()->associatedVariableName().c_str(), alphaCutOff);
  }

  return *this;
}

std::string AlphaTestBlock::_dumpPropertiesCode()
{
  auto codeString
    = String::printf("%s.alphaCutOff = %f;\r\n", _codeVariableName.c_str(), alphaCutOff);

  return codeString;
}

json AlphaTestBlock::serialize() const
{
  return nullptr;
}

void AlphaTestBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
