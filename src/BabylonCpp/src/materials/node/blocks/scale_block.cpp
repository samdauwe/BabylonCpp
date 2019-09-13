#include <babylon/materials/node/blocks/scale_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

ScaleBlock::ScaleBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , input{this, &ScaleBlock::get_input}
    , factor{this, &ScaleBlock::get_factor}
    , output{this, &ScaleBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::AutoDetect);
  registerInput("factor", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::BasedOnInput);

  _outputs[0]->_typeConnectionSource = _inputs[0];
}

ScaleBlock::~ScaleBlock()
{
}

const std::string ScaleBlock::getClassName() const
{
  return "ScaleBlock";
}

NodeMaterialConnectionPointPtr& ScaleBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ScaleBlock::get_factor()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ScaleBlock::get_output()
{
  return _outputs[0];
}

ScaleBlock& ScaleBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& output = _outputs[0];

  state.compilationString
    += _declareOutput(output, state)
       + String::printf(" = %s * %s;\r\n",
                        input()->associatedVariableName().c_str(),
                        factor()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
