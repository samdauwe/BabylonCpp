#include <babylon/materials/node/blocks/vertex/vertex_output_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>

namespace BABYLON {

VertexOutputBlock::VertexOutputBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Vertex, true}
    , vector{this, &VertexOutputBlock::get_vector}
{
  registerInput("vector", NodeMaterialBlockConnectionPointTypes::Vector4);
}

VertexOutputBlock::~VertexOutputBlock() = default;

std::string VertexOutputBlock::getClassName() const
{
  return "VertexOutputBlock";
}

NodeMaterialConnectionPointPtr& VertexOutputBlock::get_vector()
{
  return _inputs[0];
}

VertexOutputBlock& VertexOutputBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto& input = vector();

  state.compilationString
    += String::printf("gl_Position = %s;\r\n", input->associatedVariableName().c_str());

  return *this;
}

} // end of namespace BABYLON
