#include <babylon/materials/node/blocks/particle/particle_texture_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/enums/node_material_block_connection_point_types.h>
#include <babylon/materials/node/enums/node_material_block_targets.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ParticleTextureBlock::ParticleTextureBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , texture{nullptr}
    , convertToGammaSpace{false}
    , convertToLinearSpace{false}
    , uv{this, &ParticleTextureBlock::get_uv}
    , rgba{this, &ParticleTextureBlock::get_rgba}
    , rgb{this, &ParticleTextureBlock::get_rgb}
    , r{this, &ParticleTextureBlock::get_r}
    , g{this, &ParticleTextureBlock::get_g}
    , b{this, &ParticleTextureBlock::get_b}
    , a{this, &ParticleTextureBlock::get_a}
    , _samplerName{"diffuseSampler"}
{
  _isUnique = true;

  registerInput("uv", NodeMaterialBlockConnectionPointTypes::Vector2, false,
                NodeMaterialBlockTargets::VertexAndFragment);

  registerOutput("rgba", NodeMaterialBlockConnectionPointTypes::Color4,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("rgb", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("r", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("g", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("b", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Neutral);
  registerOutput("a", NodeMaterialBlockConnectionPointTypes::Float,
                 NodeMaterialBlockTargets::Neutral);

  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector3);
  _inputs[0]->acceptedConnectionPointTypes.emplace_back(
    NodeMaterialBlockConnectionPointTypes::Vector4);
}

ParticleTextureBlock::~ParticleTextureBlock() = default;

std::string ParticleTextureBlock::getClassName() const
{
  return "ParticleTextureBlock";
}

NodeMaterialConnectionPointPtr& ParticleTextureBlock::ParticleTextureBlock::get_uv()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ParticleTextureBlock::ParticleTextureBlock::get_rgba()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& ParticleTextureBlock::ParticleTextureBlock::get_rgb()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& ParticleTextureBlock::ParticleTextureBlock::get_r()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& ParticleTextureBlock::ParticleTextureBlock::get_g()
{
  return _outputs[3];
}

NodeMaterialConnectionPointPtr& ParticleTextureBlock::ParticleTextureBlock::get_b()
{
  return _outputs[4];
}

NodeMaterialConnectionPointPtr& ParticleTextureBlock::ParticleTextureBlock::get_a()
{
  return _outputs[5];
}

void ParticleTextureBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("diffuseSampler");
}

void ParticleTextureBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!uv()->isConnected()) {
    auto uvInput = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
      return b->isAttribute() && b->name() == "particle_uv";
    });

    if (!uvInput) {
      uvInput = InputBlock::New("uv");
      uvInput->setAsAttribute("particle_uv");
    }
    uvInput->output()->connectTo(uv);
  }
}

void ParticleTextureBlock::prepareDefines(AbstractMesh* /*mesh*/,
                                          const NodeMaterialPtr& /*nodeMaterial*/,
                                          NodeMaterialDefines& defines, bool /*useInstances*/,
                                          SubMesh* /*subMesh*/)
{
  defines.setValue(_linearDefineName, convertToGammaSpace, true);
  defines.setValue(_gammaDefineName, convertToLinearSpace, true);
}

bool ParticleTextureBlock::isReady(AbstractMesh* /*mesh*/, const NodeMaterialPtr& /*nodeMaterial*/,
                                   const NodeMaterialDefines& /*defines*/, bool /*useInstances*/)
{
  if (texture && !texture->isReadyOrNotBlocking()) {
    return false;
  }

  return true;
}

void ParticleTextureBlock::_writeOutput(NodeMaterialBuildState& state,
                                        const NodeMaterialConnectionPointPtr& output,
                                        const std::string& swizzle)
{
  state.compilationString
    += StringTools::printf("%s = %s.%s;\r\n", _declareOutput(output, state).c_str(),
                           _tempTextureRead.c_str(), swizzle.c_str());

  state.compilationString += StringTools::printf("#ifdef %s\r\n", _linearDefineName.c_str());
  state.compilationString
    += StringTools::printf("%s = toGammaSpace(%s);\r\n", output->associatedVariableName().c_str(),
                           output->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";

  state.compilationString += StringTools::printf("#ifdef %s\r\n", _gammaDefineName.c_str());
  state.compilationString
    += StringTools::printf("%s = toLinearSpace(%s);\r\n", output->associatedVariableName().c_str(),
                           output->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";
}

ParticleTextureBlock& ParticleTextureBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target == NodeMaterialBlockTargets::Vertex) {
    return *this;
  }

  _tempTextureRead = state._getFreeVariableName("tempTextureRead");

  state._emit2DSampler(_samplerName);

  state.sharedData->blockingBlocks.emplace_back(shared_from_this());
  state.sharedData->textureBlocks.emplace_back(
    std::static_pointer_cast<ParticleTextureBlock>(shared_from_this()));
  state.sharedData->blocksWithDefines.emplace_back(shared_from_this());

  _linearDefineName = state._getFreeDefineName("ISLINEAR");
  _gammaDefineName  = state._getFreeDefineName("ISGAMMA");

  const auto iComments = StringTools::printf("//%s", name().c_str());
  state._emitFunctionFromInclude("helperFunctions", iComments);

  state.compilationString
    += StringTools::printf("vec4 %s = texture2D(%s, %s);\r\n", _tempTextureRead.c_str(),
                           _samplerName.c_str(), uv()->associatedVariableName().c_str());

  for (const auto& output : _outputs) {
    if (output->hasEndpoints()) {
      _writeOutput(state, output, output->name);
    }
  }

  return *this;
}

json ParticleTextureBlock::serialize() const
{
  return nullptr;
}

void ParticleTextureBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                        const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
