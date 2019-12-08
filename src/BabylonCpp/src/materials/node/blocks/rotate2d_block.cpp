#include <babylon/materials/node/blocks/rotate2d_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

Rotate2dBlock::Rotate2dBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , input{this, &Rotate2dBlock::get_input}
    , angle{this, &Rotate2dBlock::get_angle}
    , output{this, &Rotate2dBlock::get_output}
{
  registerInput("input", NodeMaterialBlockConnectionPointTypes::Vector2);
  registerInput("angle", NodeMaterialBlockConnectionPointTypes::Float);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Vector2);
}

Rotate2dBlock::~Rotate2dBlock()
{
}

std::string Rotate2dBlock::getClassName() const
{
  return "Rotate2dBlock";
}

NodeMaterialConnectionPointPtr& Rotate2dBlock::get_input()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& Rotate2dBlock::get_angle()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& Rotate2dBlock::get_output()
{
  return _outputs[0];
}

void Rotate2dBlock::autoConfigure(const NodeMaterialPtr& /*material*/)
{
  if (!angle()->isConnected()) {
    auto angleInput   = InputBlock::New("angle");
    angleInput->value = std::make_shared<AnimationValue>(0.f);
    angleInput->output()->connectTo(angle());
  }
}

Rotate2dBlock& Rotate2dBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& iOutput = _outputs[0];
  const auto& iAngle  = angle();
  const auto& iInput  = input();

  state.compilationString
    += _declareOutput(iOutput, state)
       + String::printf(
         " = vec2(cos(%s) * %s.x - sin(%s) * %s.y, sin(%s) * %s.x + cos(%s) * %s.y);\r\n",
         iAngle->associatedVariableName().c_str(), iInput->associatedVariableName().c_str(),
         iAngle->associatedVariableName().c_str(), iInput->associatedVariableName().c_str(),
         iAngle->associatedVariableName().c_str(), iInput->associatedVariableName().c_str(),
         iAngle->associatedVariableName().c_str(), iInput->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
