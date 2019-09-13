#include <babylon/materials/node/blocks/normalize_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

NormalizeBlock::NormalizeBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , input{this, &NormalizeBlock::get_input}
    , output{this, &NormalizeBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
}

NormalizeBlock::~NormalizeBlock()
{
}

const std::string NormalizeBlock::getClassName() const
{
  return "NormalizeBlock";
}

NodeMaterialConnectionPointPtr& NormalizeBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& NormalizeBlock::get_output()
{
  return _outputs[0];
}

NormalizeBlock& NormalizeBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& output = _outputs[0];

  state.compilationString
    += _declareOutput(output, state)
       + String::printf(" = normalize(%s);\r\n",
                        input()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
