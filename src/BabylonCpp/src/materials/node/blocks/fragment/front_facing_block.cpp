#include <babylon/materials/node/blocks/fragment/front_facing_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

FrontFacingBlock::FrontFacingBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , output{this, &FrontFacingBlock::get_output}
{
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
}

FrontFacingBlock::~FrontFacingBlock() = default;

std::string FrontFacingBlock::getClassName() const
{
  return "FrontFacingBlock";
}

NodeMaterialConnectionPointPtr& FrontFacingBlock::get_output()
{
  return _outputs[0];
}

FrontFacingBlock& FrontFacingBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target == NodeMaterialBlockTargets::Vertex) {
    throw std::runtime_error("FrontFacingBlock must only be used in a fragment shader");
  }

  const auto& iOutput = _outputs[0];

  state.compilationString += _declareOutput(iOutput, state) + " = gl_FrontFacing ? 1.0 : 0.0;\r\n";

  return *this;
}

} // end of namespace BABYLON
