#include <babylon/materials/node/blocks/matrix_builder_block.h>

#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

MatrixBuilderBlock::MatrixBuilderBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , row0{this, &MatrixBuilderBlock::get_row0}
    , row1{this, &MatrixBuilderBlock::get_row1}
    , row2{this, &MatrixBuilderBlock::get_row2}
    , row3{this, &MatrixBuilderBlock::get_row3}
    , output{this, &MatrixBuilderBlock::get_output}
{
  registerInput("row0", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("row1", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("row2", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("row3", NodeMaterialBlockConnectionPointTypes::Vector4);
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Matrix);
}

MatrixBuilderBlock::~MatrixBuilderBlock() = default;

std::string MatrixBuilderBlock::getClassName() const
{
  return "MatrixBuilder";
}

NodeMaterialConnectionPointPtr& MatrixBuilderBlock::get_row0()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& MatrixBuilderBlock::get_row1()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& MatrixBuilderBlock::get_row2()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& MatrixBuilderBlock::get_row3()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& MatrixBuilderBlock::get_output()
{
  return _outputs[0];
}

void MatrixBuilderBlock::autoConfigure(const NodeMaterialPtr& /*nodeMaterial*/)
{
  if (!row0()->isConnected()) {
    const auto row0Input = InputBlock::New("row0");
    row0Input->value     = std::make_shared<AnimationValue>(Vector4(1.f, 0.f, 0.f, 0.f));
    row0Input->output()->connectTo(row0());
  }

  if (!row1()->isConnected) {
    const auto row1Input = InputBlock::New("row1");
    row1Input->value     = std::make_shared<AnimationValue>(Vector4(0.f, 1.f, 0.f, 0.f));
    row1Input->output()->connectTo(row1());
  }

  if (!row2()->isConnected()) {
    const auto row2Input = InputBlock::New("row2");
    row2Input->value     = std::make_shared<AnimationValue>(Vector4(0.f, 0.f, 1.f, 0.f));
    row2Input->output()->connectTo(row2());
  }

  if (!row3()->isConnected()) {
    const auto row3Input = InputBlock::New("row3");
    row3Input->value     = std::make_shared<AnimationValue>(Vector4(0.f, 0.f, 0.f, 1.f));
    row3Input->output()->connectTo(row3());
  }
}

MatrixBuilderBlock& MatrixBuilderBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& output = _outputs[0];
  const auto& iRow0  = row0();
  const auto& iRow1  = row1();
  const auto& iRow2  = row2();
  const auto& iRow3  = row3();

  state.compilationString += _declareOutput(output, state)
                             + StringTools::printf(" = mat4(%s, %s, %s, %s);\r\n",
                                                   iRow0->associatedVariableName().c_str(),
                                                   iRow1->associatedVariableName().c_str(),
                                                   iRow2->associatedVariableName().c_str(),
                                                   iRow3->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
