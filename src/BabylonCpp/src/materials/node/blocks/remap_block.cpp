#include <babylon/materials/node/blocks/remap_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

RemapBlock::RemapBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , sourceRange{Vector2(-1.f, 1.f)}
    , targetRange{Vector2(0.f, 1.f)}
    , input{this, &RemapBlock::get_input}
    , sourceMin{this, &RemapBlock::get_sourceMin}
    , sourceMax{this, &RemapBlock::get_sourceMax}
    , targetMin{this, &RemapBlock::get_targetMin}
    , targetMax{this, &RemapBlock::get_targetMax}
    , output{this, &RemapBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("sourceMin", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("sourceMax", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("targetMin", NodeMaterialBlockConnectionPointTypes::Float, true);
  registerInput("targetMax", NodeMaterialBlockConnectionPointTypes::Float, true);

  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
}

RemapBlock::~RemapBlock() = default;

std::string RemapBlock::getClassName() const
{
  return "RemapBlock";
}

NodeMaterialConnectionPointPtr& RemapBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& RemapBlock::get_sourceMin()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& RemapBlock::get_sourceMax()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& RemapBlock::get_targetMin()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& RemapBlock::get_targetMax()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& RemapBlock::get_output()
{
  return _outputs[0];
}

RemapBlock& RemapBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  const auto iSourceMin = sourceMin()->isConnected() ? sourceMin()->associatedVariableName() :
                                                       _writeFloat(sourceRange.x);
  const auto iSourceMax = sourceMax()->isConnected() ? sourceMax()->associatedVariableName() :
                                                       _writeFloat(sourceRange.y);

  const auto iTargetMin = targetMin()->isConnected() ? targetMin()->associatedVariableName() :
                                                       _writeFloat(targetRange.x);
  const auto iTargetMax = targetMax()->isConnected() ? targetMax()->associatedVariableName() :
                                                       _writeFloat(targetRange.y);

  state.compilationString
    += _declareOutput(iOutput, state)
       + StringTools::printf(" = %s + (%s - %s) * (%s - %s) / (%s - %s);\r\n", iTargetMin.c_str(),
                             _inputs[0]->associatedVariableName().c_str(), iSourceMin.c_str(),
                             iTargetMax.c_str(), iTargetMin.c_str(), iSourceMax.c_str(),
                             iSourceMin.c_str());

  return *this;
}

std::string RemapBlock::_dumpPropertiesCode()
{
  auto codeString = StringTools::printf("%s.sourceRange = Vector2(%f, %f);\r\n",
                                        _codeVariableName.c_str(), sourceRange.x, sourceRange.y);

  codeString += StringTools::printf("%s.targetRange = Vector2(%f, %f);\r\n",
                                    _codeVariableName.c_str(), targetRange.x, targetRange.y);

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
