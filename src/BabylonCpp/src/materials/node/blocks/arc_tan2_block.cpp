#include <babylon/materials/node/blocks/arc_tan2_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

ArcTan2Block::ArcTan2Block(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , x{this, &ArcTan2Block::get_x}
    , y{this, &ArcTan2Block::get_y}
    , output{this, &ArcTan2Block::get_output}
{
  registerInput("x", NodeMaterialBlockConnectionPointTypes::Float);
  registerInput("y", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Float);
}

ArcTan2Block::~ArcTan2Block()
{
}

std::string ArcTan2Block::getClassName() const
{
  return "ArcTan2Block";
}

NodeMaterialConnectionPointPtr& ArcTan2Block::get_x()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ArcTan2Block::get_y()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ArcTan2Block::get_output()
{
  return _outputs[0];
}

ArcTan2Block& ArcTan2Block::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(" = atan(%s, %s);\r\n", x()->associatedVariableName().c_str(),
                        y()->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
