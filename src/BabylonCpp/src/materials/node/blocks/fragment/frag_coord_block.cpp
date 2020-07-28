#include <babylon/materials/node/blocks/fragment/frag_coord_block.h>

#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

FragCoordBlock::FragCoordBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , xy{this, &FragCoordBlock::get_xy}
    , xyz{this, &FragCoordBlock::get_xyz}
    , xyzw{this, &FragCoordBlock::get_xyzw}
    , x{this, &FragCoordBlock::get_x}
    , y{this, &FragCoordBlock::get_y}
    , z{this, &FragCoordBlock::get_z}
    , output{this, &FragCoordBlock::get_output}
{
  registerOutput("xy", NodeMaterialBlockConnectionPointTypes::Vector2,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("xyz", NodeMaterialBlockConnectionPointTypes::Vector3,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("xyzw", NodeMaterialBlockConnectionPointTypes::Vector4,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("x", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("y", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("z", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("w", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
}

FragCoordBlock::~FragCoordBlock() = default;

std::string FragCoordBlock::getClassName() const
{
  return "FragCoordBlock";
}

NodeMaterialConnectionPointPtr& FragCoordBlock::get_xy()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& FragCoordBlock::get_xyz()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& FragCoordBlock::get_xyzw()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& FragCoordBlock::get_x()
{
  return _outputs[3];
}

NodeMaterialConnectionPointPtr& FragCoordBlock::get_y()
{
  return _outputs[4];
}

NodeMaterialConnectionPointPtr& FragCoordBlock::get_z()
{
  return _outputs[5];
}

NodeMaterialConnectionPointPtr& FragCoordBlock::get_output()
{
  return _outputs[6];
}

std::string FragCoordBlock::writeOutputs(NodeMaterialBuildState& state)
{
  std::string code;

  for (const auto& iOutput : _outputs) {
    if (iOutput->hasEndpoints()) {
      code += StringTools::printf("%s = gl_FragCoord.%s;\r\n",
                                  _declareOutput(iOutput, state).c_str(), iOutput->name.c_str());
    }
  }

  return code;
}

FragCoordBlock& FragCoordBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target == NodeMaterialBlockTargets::Vertex) {
    throw std::runtime_error("FragCoordBlock must only be used in a fragment shader");
  }

  state.compilationString += writeOutputs(state);

  return *this;
}

} // end of namespace BABYLON
