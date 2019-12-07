#include <babylon/materials/node/blocks/clamp_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

ClampBlock::ClampBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , minimum{0.f}
    , maximum{1.f}
    , value{this, &ClampBlock::get_value}
    , output{this, &ClampBlock::get_output}
{
  registerInput("value", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Float);
}

ClampBlock::~ClampBlock()
{
}

std::string ClampBlock::getClassName() const
{
  return "ClampBlock";
}

NodeMaterialConnectionPointPtr& ClampBlock::get_value()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ClampBlock::get_output()
{
  return _outputs[0];
}

ClampBlock& ClampBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(" = clamp(%s , %s, %s);\r\n", value()->associatedVariableName().c_str(),
                        _writeFloat(minimum).c_str(), _writeFloat(maximum).c_str());

  return *this;
}

std::string ClampBlock::_dumpPropertiesCode()
{
  auto codeString = String::printf("%s.minimum = %f;\r\n", _codeVariableName.c_str(), minimum);

  codeString += String::printf("%s.maximum = %f;\r\n", _codeVariableName.c_str(), maximum);

  return codeString;
}

json ClampBlock::serialize() const
{
  return nullptr;
}

void ClampBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                              const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
