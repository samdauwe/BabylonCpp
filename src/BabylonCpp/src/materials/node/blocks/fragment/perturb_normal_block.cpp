#include <babylon/materials/node/blocks/fragment/perturb_normal_block.h>

#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>

namespace BABYLON {

PerturbNormalBlock::PerturbNormalBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Neutral}
    , invertX{false}
    , invertY{false}
    , worldPosition{this, &PerturbNormalBlock::get_worldPosition}
    , worldNormal{this, &PerturbNormalBlock::get_worldNormal}
    , uv{this, &PerturbNormalBlock::get_uv}
    , normalMapColor{this, &PerturbNormalBlock::get_normalMapColor}
    , strength{this, &PerturbNormalBlock::get_strength}
    , output{this, &PerturbNormalBlock::get_output}
{
  // Vertex
  registerInput("worldPosition", NodeMaterialBlockConnectionPointTypes::Vector4, false);
  registerInput("worldNormal", NodeMaterialBlockConnectionPointTypes::Vector4, false);
  registerInput("uv", NodeMaterialBlockConnectionPointTypes::Vector2, false);
  registerInput("normalMapColor", NodeMaterialBlockConnectionPointTypes::Color3, false);
  registerInput("strength", NodeMaterialBlockConnectionPointTypes::Float, false);

  // Fragment
  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Vector4);
}

PerturbNormalBlock::~PerturbNormalBlock() = default;

std::string PerturbNormalBlock::getClassName() const
{
  return "PerturbNormalBlock";
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_worldPosition()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_worldNormal()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_uv()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_normalMapColor()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_strength()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& PerturbNormalBlock::get_output()
{
  return _outputs[0];
}

void PerturbNormalBlock::prepareDefines(AbstractMesh* /*mesh*/,
                                        const NodeMaterialPtr& /*nodeMaterial*/,
                                        NodeMaterialDefines& defines, bool /*useInstances*/)
{
  defines.setValue("BUMP", true);
}

void PerturbNormalBlock::bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial,
                              Mesh* /*mesh*/)
{
  if (nodeMaterial->getScene()->_mirroredCameraPosition) {
    effect->setFloat2(_tangentSpaceParameterName, invertX ? 1.f : -1.f, invertY ? 1.f : -1.f);
  }
  else {
    effect->setFloat2(_tangentSpaceParameterName, invertX ? -1.f : 1.f, invertY ? -1.f : 1.f);
  }
}

void PerturbNormalBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!uv()->isConnected()) {
    auto uvInput = material->getInputBlockByPredicate(
      [](const InputBlockPtr& b) -> bool { return b->isAttribute() && b->name == "uv"; });

    if (!uvInput) {
      uvInput = InputBlock::New("uv");
      uvInput->setAsAttribute();
    }
    uvInput->output()->connectTo(uv);
  }

  if (!strength()->isConnected()) {
    auto strengthInput   = InputBlock::New("strength");
    strengthInput->value = std::make_shared<AnimationValue>(1.f);
    strengthInput->output()->connectTo(strength);
  }
}

PerturbNormalBlock& PerturbNormalBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  const auto comments        = String::printf("//%s", name.c_str());
  const auto& _uv            = uv();
  const auto& _worldPosition = worldPosition();
  const auto& _worldNormal   = worldNormal();

  state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  state.sharedData->bindableBlocks.emplace_back(shared_from_this());

  _tangentSpaceParameterName = state._getFreeDefineName("tangentSpaceParameter");

  state._emitUniformFromString(_tangentSpaceParameterName, "vec2");

  const auto replaceForBumpInfos
    = strength()->isConnectedToInputBlock() && strength()->connectInputBlock()->isConstant ?
        state._emitFloat(1.f / strength()->connectInputBlock()->value()->get<float>()).c_str() :
        String::printf("1.0 / %s", strength()->associatedVariableName().c_str());

  state._emitExtension("derivatives", "#extension GL_OES_standard_derivatives : enable");
  EmitFunctionFromIncludeOptions emitFunctionFromIncludeOptions;
  emitFunctionFromIncludeOptions.replaceStrings
    = {{"vBumpInfos.y", replaceForBumpInfos},
       {"vTangentSpaceParams", _tangentSpaceParameterName},
       {"vPositionW", _worldPosition->associatedVariableName() + ".xyz"},
       {"defined\\(TANGENT\\)", "defined(IGNORE)"}};
  state._emitFunctionFromInclude("bumpFragmentFunctions", comments, emitFunctionFromIncludeOptions);
  state.compilationString += _declareOutput(output, state) + " = vec4(0.);\r\n";
  EmitCodeFromIncludeOptions emitCodeFromIncludeOptions;
  emitCodeFromIncludeOptions.replaceStrings = {
    {"perturbNormal\\(TBN,vBumpUV\\+uvOffset\\)",
     String::printf("perturbNormal(TBN, %s)", normalMapColor()->associatedVariableName().c_str())},
    {"vBumpInfos.y", replaceForBumpInfos},
    {"vBumpUV", _uv->associatedVariableName()},
    {"vPositionW", _worldPosition->associatedVariableName() + ".xyz"},
    {"normalW=", output()->associatedVariableName() + ".xyz = "},
    {"normalW", _worldNormal->associatedVariableName() + ".xyz"},
    {"defined\\(TANGENT\\)", "defined(IGNORE)"}};
  state.compilationString
    += state._emitCodeFromInclude("bumpFragment", comments, emitCodeFromIncludeOptions);

  return *this;
}

std::string PerturbNormalBlock::_dumpPropertiesCode()
{
  auto codeString
    = String::printf("%s.invertX = %s;\r\n", _codeVariableName.c_str(), invertX ? "true" : "false");

  codeString += String::printf("%s.invertY = %s;\r\n", _codeVariableName.c_str(),
                               invertY ? "true" : "false");

  return codeString;
}

json PerturbNormalBlock::serialize() const
{
  return nullptr;
}

void PerturbNormalBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
