#include <babylon/materials/node/blocks/dual/reflection_texture_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ReflectionTextureBlock::ReflectionTextureBlock(const std::string& iName)
    : ReflectionTextureBaseBlock{iName}
    , rgb{this, &ReflectionTextureBlock::get_rgb}
    , r{this, &ReflectionTextureBlock::get_r}
    , g{this, &ReflectionTextureBlock::get_g}
    , b{this, &ReflectionTextureBlock::get_b}
{
  registerInput("position", NodeMaterialBlockConnectionPointTypes::Vector3, false,
                NodeMaterialBlockTargets::Vertex);
  registerInput("worldPosition", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Vertex);
  registerInput("worldNormal", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Fragment); // Flagging as fragment as the normal can be
                                                     // changed by fragment code
  registerInput("world", NodeMaterialBlockConnectionPointTypes::Matrix, false,
                NodeMaterialBlockTargets::Vertex);

  registerInput("cameraPosition", NodeMaterialBlockConnectionPointTypes::Vector3, false,
                NodeMaterialBlockTargets::Fragment);
  registerInput("view", NodeMaterialBlockConnectionPointTypes::Matrix, false,
                NodeMaterialBlockTargets::Fragment);

  registerOutput("rgb", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("r", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("g", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("b", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Fragment);

  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);
}

ReflectionTextureBlock::~ReflectionTextureBlock() = default;

std::string ReflectionTextureBlock::getClassName() const
{
  return "ReflectionTextureBlock";
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_position()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_worldPosition()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_worldNormal()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_world()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_cameraPosition()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_view()
{
  return _inputs[5];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_rgb()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_r()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_g()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& ReflectionTextureBlock::get_b()
{
  return _outputs[3];
}

void ReflectionTextureBlock::autoConfigure(const NodeMaterialPtr& material)
{
  ReflectionTextureBaseBlock::autoConfigure(material);

  if (!cameraPosition()->isConnected()) {
    auto cameraPositionInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& inputBlock) -> bool {
          return inputBlock->systemValue() == NodeMaterialSystemValues::CameraPosition;
        });

    if (!cameraPositionInput) {
      cameraPositionInput = InputBlock::New("cameraPosition");
      cameraPositionInput->setAsSystemValue(NodeMaterialSystemValues::CameraPosition);
    }
    cameraPositionInput->output()->connectTo(cameraPosition());
  }
}

ReflectionTextureBlock& ReflectionTextureBlock::_buildBlock(NodeMaterialBuildState& state)
{
  ReflectionTextureBaseBlock::_buildBlock(state);

  if (!texture) {
    state.compilationString += writeOutputs(state, "vec3(0.)");
    return *this;
  }

  if (state.target != NodeMaterialBlockTargets::Fragment) {
    state.compilationString += handleVertexSide(state);
    return *this;
  }

  handleFragmentSideInits(state);

  const auto normalWUnit = state._getFreeVariableName("normalWUnit");

  state.compilationString
    += StringTools::printf("vec4 %s = normalize(%s);\r\n", normalWUnit.c_str(),
                           worldNormal()->associatedVariableName().c_str());

  state.compilationString += handleFragmentSideCodeReflectionCoords(normalWUnit);

  state.compilationString += handleFragmentSideCodeReflectionColor();

  state.compilationString += writeOutputs(state, _reflectionColorName);

  return *this;
}

} // end of namespace BABYLON
