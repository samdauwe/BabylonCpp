#include <babylon/materials/node/blocks/wave_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

WaveBlock::WaveBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , kind{WaveBlockKind::SawTooth}
    , input{this, &WaveBlock::get_input}
    , output{this, &WaveBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];

  _inputs[0]->excludedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Matrix);
}

WaveBlock::~WaveBlock() = default;

std::string WaveBlock::getClassName() const
{
  return "WaveBlock";
}

NodeMaterialConnectionPointPtr& WaveBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& WaveBlock::get_output()
{
  return _outputs[0];
}

WaveBlock& WaveBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  switch (kind) {
    case WaveBlockKind::SawTooth: {
      state.compilationString += _declareOutput(iOutput, state)
                                 + StringTools::printf(" = %s - floor(0.5 + %s);\r\n",
                                                       input()->associatedVariableName().c_str(),
                                                       input()->associatedVariableName().c_str());
      break;
    }
    case WaveBlockKind::Square: {
      state.compilationString += _declareOutput(iOutput, state)
                                 + StringTools::printf(" = 1.0 - 2.0 * round(fract(%s));\r\n",
                                                       input()->associatedVariableName().c_str());
      break;
    }
    case WaveBlockKind::Triangle: {
      state.compilationString
        += _declareOutput(iOutput, state)
           + StringTools::printf(" = 2.0 * abs(2.0 * (%s - floor(0.5 + %s))) - 1.0;\r\n",
                                 input()->associatedVariableName().c_str(),
                                 input()->associatedVariableName().c_str());
      break;
    }
  }

  return *this;
}

json WaveBlock::serialize() const
{
  return nullptr;
}

void WaveBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                             const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
