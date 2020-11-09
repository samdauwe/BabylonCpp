#include <babylon/materials/node/blocks/fragment/screen_size_block.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ScreenSizeBlock::ScreenSizeBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , xy{this, &ScreenSizeBlock::get_xy}
    , x{this, &ScreenSizeBlock::get_x}
    , y{this, &ScreenSizeBlock::get_y}
    , _scene{nullptr}
{
  registerOutput("xy", NodeMaterialBlockConnectionPointTypes::Vector2,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("x", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("y", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
}

ScreenSizeBlock::~ScreenSizeBlock() = default;

std::string ScreenSizeBlock::getClassName() const
{
  return "ScreenSizeBlock";
}

NodeMaterialConnectionPointPtr& ScreenSizeBlock::get_xy()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& ScreenSizeBlock::get_x()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& ScreenSizeBlock::get_y()
{
  return _outputs[2];
}

void ScreenSizeBlock::bind(Effect* effect, const NodeMaterialPtr& /*nodeMaterial*/, Mesh* /*mesh*/,
                           SubMesh* /*subMesh*/)
{
  const auto engine = _scene->getEngine();

  effect->setFloat2(_varName, static_cast<float>(engine->getRenderWidth()),
                    static_cast<float>(engine->getRenderWidth()));
}

std::string ScreenSizeBlock::writeOutputs(NodeMaterialBuildState& state, const std::string& varName)
{
  std::string code;

  for (const auto& output : _outputs) {
    if (output->hasEndpoints()) {
      code += StringTools::printf("%s = %s.%s;\r\n", _declareOutput(output, state).c_str(),
                                  varName.c_str(), output->name.c_str());
    }
  }

  return code;
}

ScreenSizeBlock& ScreenSizeBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  _scene = state.sharedData->scene;

  if (state.target == NodeMaterialBlockTargets::Vertex) {
    throw std::runtime_error("ScreenSizeBlock must only be used in a fragment shader");
  }

  state.sharedData->bindableBlocks.emplace_back(shared_from_this());

  _varName = state._getFreeVariableName("screenSize");
  state._emitUniformFromString(_varName, "vec2");

  state.compilationString += writeOutputs(state, _varName);

  return *this;
}

} // end of namespace BABYLON
