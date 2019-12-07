#include <babylon/materials/node/blocks/remap_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

RemapBlock::RemapBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , sourceRange{Vector2(-1.f, 1.f)}
    , targetRange{Vector2(0.f, 1.f)}
    , input{this, &RemapBlock::get_input}
    , output{this, &RemapBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
}

RemapBlock::~RemapBlock()
{
}

std::string RemapBlock::getClassName() const
{
  return "RemapBlock";
}

NodeMaterialConnectionPointPtr& RemapBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& RemapBlock::get_output()
{
  return _outputs[0];
}

RemapBlock& RemapBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& output = _outputs[0];

  state.compilationString
    += _declareOutput(output, state)
       + String::printf(
         " = %s + (%s - %s) * (%s - %s) / (%s - %s);\r\n", _writeFloat(targetRange.x).c_str(),
         _inputs[0]->associatedVariableName().c_str(), _writeFloat(sourceRange.x).c_str(),
         _writeFloat(targetRange.y).c_str(), _writeFloat(targetRange.x).c_str(),
         _writeFloat(sourceRange.y).c_str(), _writeFloat(sourceRange.x).c_str());

  return *this;
}

std::string RemapBlock::_dumpPropertiesCode()
{
  auto codeString = String::printf("%s.sourceRange = Vector2(%f, %f);\r\n",
                                   _codeVariableName.c_str(), sourceRange.x, sourceRange.y);

  codeString += String::printf("%s.targetRange = Vector2(%f, %f);\r\n", _codeVariableName.c_str(),
                               targetRange.x, targetRange.y);

  return codeString;
}

json RemapBlock::serialize() const
{
  return nullptr;
}

void RemapBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                              const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
