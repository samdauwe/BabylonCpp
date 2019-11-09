#include <babylon/materials/node/blocks/dual/light_block.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

LightBlock::LightBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::VertexAndFragment}
    , light{nullptr}
    , worldPosition{this, &LightBlock::get_worldPosition}
    , worldNormal{this, &LightBlock::get_worldNormal}
    , cameraPosition{this, &LightBlock::get_cameraPosition}
    , diffuseOutput{this, &LightBlock::get_diffuseOutput}
    , specularOutput{this, &LightBlock::get_specularOutput}
{
  registerInput("worldPosition", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Vertex);
  registerInput("worldNormal", NodeMaterialBlockConnectionPointTypes::Vector4, false,
                NodeMaterialBlockTargets::Fragment);

  registerInput("cameraPosition", NodeMaterialBlockConnectionPointTypes::Vector3, false,
                NodeMaterialBlockTargets::Fragment);
  registerOutput("diffuseOutput", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Fragment);
  registerOutput("specularOutput", NodeMaterialBlockConnectionPointTypes::Color3,
                 NodeMaterialBlockTargets::Fragment);
}

LightBlock::~LightBlock()
{
}

const std::string LightBlock::getClassName() const
{
  return "LightBlock";
}

NodeMaterialConnectionPointPtr& LightBlock::get_worldPosition()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& LightBlock::get_worldNormal()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& LightBlock::get_cameraPosition()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& LightBlock::get_diffuseOutput()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& LightBlock::get_specularOutput()
{
  return _outputs[1];
}

void LightBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!cameraPosition()->isConnected) {
    auto cameraPositionInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->systemValue() == NodeMaterialSystemValues::CameraPosition;
        });

    if (!cameraPositionInput) {
      cameraPositionInput = InputBlock::New("cameraPosition");
      cameraPositionInput->setAsSystemValue(NodeMaterialSystemValues::CameraPosition);
    }
    cameraPositionInput->output()->connectTo(cameraPosition());
  }
}

void LightBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                NodeMaterialDefines& defines, bool /*useInstances*/)
{
  if (!defines._areLightsDirty) {
    return;
  }

  auto scene = mesh->getScene();

  if (!light) {
    MaterialHelper::PrepareDefinesForLights(scene, mesh, defines, true,
                                            nodeMaterial->maxSimultaneousLights);
  }
  else {
    PrepareDefinesForLightsState state{
      false, // needNormals
      false, // needRebuild
      false, // shadowEnabled
      false, // specularEnabled
      false  // lightmapMode
    };

    MaterialHelper::PrepareDefinesForLight(scene, mesh, light, _lightId, defines, true, state);

    if (state.needRebuild) {
      defines.rebuild();
    }
  }
}

void LightBlock::updateUniformsAndSamples(NodeMaterialBuildState& state,
                                          const NodeMaterialPtr& nodeMaterial,
                                          const NodeMaterialDefines& defines)
{
  for (auto lightIndex = 0u; lightIndex < nodeMaterial->maxSimultaneousLights; ++lightIndex) {
    if (!defines["LIGHT" + std::to_string(lightIndex)]) {
      break;
    }
    MaterialHelper::PrepareUniformsAndSamplersForLight(lightIndex, state.uniforms, state.samplers,
                                                       state.uniformBuffers, false);
  }
}

void LightBlock::bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh)
{
  if (!mesh) {
    return;
  }

  auto scene = mesh->getScene();

  if (!light) {
    MaterialHelper::BindLights(scene, mesh, effect, true, nodeMaterial->maxSimultaneousLights,
                               false);
  }
  else {
    MaterialHelper::BindLight(light, _lightId, scene, mesh, effect, true, false);
  }
}

void LightBlock::_injectVertexCode(NodeMaterialBuildState& state)
{
  const auto& worldPos = worldPosition();
  auto comments        = String::printf("//%s", name.c_str());

  // Declaration
  if (!light) { // Emit for all lights
    state._emitFunctionFromInclude(
      state.supportUniformBuffers ? "lightUboDeclaration" : "lightFragmentDeclaration", comments,
      EmitFunctionFromIncludeOptions{
        "maxSimultaneousLights" // repeatKey
      });
    _lightId = 0;

    state.sharedData->dynamicUniformBlocks.emplace_back(this);
  }
  else {

    _lightId = static_cast<uint32_t>((stl_util::contains(state.counters, "lightCounter") ?
                                        static_cast<int>(state.counters["lightCounter"]) :
                                        -1)
                                     + 1);
    state.counters["lightCounter"] = _lightId;

    EmitFunctionFromIncludeOptions options;
    options.replaceStrings = {StringsReplacement{
      "{X}",                   // search
      std::to_string(_lightId) // replace
    }};
    state._emitFunctionFromInclude(state.supportUniformBuffers ? "lightUboDeclaration" :
                                                                 "lightFragmentDeclaration",
                                   comments, options, std::to_string(_lightId));
  }

  // Inject code in vertex
  auto worldPosVaryingName = String::printf("v_%s", worldPos->associatedVariableName().c_str());
  if (state._emitVaryingFromString(worldPosVaryingName, "vec4")) {
    state.compilationString += String::printf("%s = %s;\r\n", worldPosVaryingName.c_str(),
                                              worldPos->associatedVariableName().c_str());
  }

  if (light) {
    EmitCodeFromIncludeOptions options;
    options.replaceStrings = {StringsReplacement{
                                "{X}",                   // search
                                std::to_string(_lightId) // replace
                              },
                              StringsReplacement{
                                "worldPos",                        // search
                                worldPos->associatedVariableName() // replace
                              }};
    state.compilationString += state._emitCodeFromInclude("shadowsVertex", comments, options);
  }
  else {
    state.compilationString
      += String::printf("vec4 worldPos = %s;\r\n", worldPos->associatedVariableName().c_str());
    EmitCodeFromIncludeOptions options;
    options.repeatKey = "maxSimultaneousLights";
    state.compilationString += state._emitCodeFromInclude("shadowsVertex", comments, options);
  }
}

LightBlock& LightBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  if (state.target != NodeMaterialBlockTargets::Fragment) {
    // Vertex
    _injectVertexCode(state);

    return *this;
  }

  // Fragment
  state.sharedData->bindableBlocks.emplace_back(this);
  state.sharedData->blocksWithDefines.emplace_back(this);

  auto comments        = String::printf("//%s", name.c_str());
  const auto& worldPos = worldPosition();

  state._emitFunctionFromInclude("helperFunctions", comments);

  EmitFunctionFromIncludeOptions options;
  options.replaceStrings = {StringsReplacement{
    "vPositionW",                                                          // search
    String::printf("v_%s.xyz", worldPos->associatedVariableName().c_str()) // replace
  }};
  state._emitFunctionFromInclude("lightsFragmentFunctions", comments, options);

  state._emitFunctionFromInclude("shadowsFragmentFunctions", comments, options);

  if (!light) { // Emit for all lights
    state._emitFunctionFromInclude(
      state.supportUniformBuffers ? "lightUboDeclaration" : "lightFragmentDeclaration", comments,
      EmitFunctionFromIncludeOptions{
        "maxSimultaneousLights" // repeatKey
      });
  }
  else {
    options.replaceStrings = {StringsReplacement{
      "{X}",                   // search
      std::to_string(_lightId) // replace
    }};

    state._emitFunctionFromInclude(state.supportUniformBuffers ? "lightUboDeclaration" :
                                                                 "lightFragmentDeclaration",
                                   comments, options, std::to_string(_lightId));

    // Uniforms and samplers
    MaterialHelper::PrepareUniformsAndSamplersForLight(_lightId, state.uniforms, state.samplers,
                                                       state.uniformBuffers, true, false);
  }

  // Code
  if (_lightId == 0) {
    if (state._registerTempVariable("viewDirectionW")) {
      state.compilationString += String::printf(
        "vec3 viewDirectionW = normalize(%s - %s);\r\n",
        cameraPosition()->associatedVariableName().c_str(),
        String::printf("v_%s.xyz", worldPos->associatedVariableName().c_str()).c_str());
    }
    state.compilationString += "lightingInfo info;\r\n";
    state.compilationString += "float shadow = 1.;\r\n";
    state.compilationString += "float glossiness = 0.;\r\n";
    state.compilationString += "vec3 diffuseBase = vec3(0., 0., 0.);\r\n";
    state.compilationString += "vec3 specularBase = vec3(0., 0., 0.);\r\n";
    state.compilationString += String::printf("vec3 normalW = %s.xyz;\r\n",
                                              worldNormal()->associatedVariableName().c_str());
  }

  if (light) {
    options.replaceStrings = {StringsReplacement{
      "{X}",                   // search
      std::to_string(_lightId) // replace
    }};
  }
  else {
    EmitCodeFromIncludeOptions includeOptions;
    includeOptions.repeatKey = "maxSimultaneousLights";
    state.compilationString
      += state._emitCodeFromInclude("lightFragment", comments, includeOptions);
  }

  const auto& _diffuseOutput  = diffuseOutput();
  const auto& _specularOutput = specularOutput();

  state.compilationString
    += String::printf("%s = diffuseBase;\r\n", _declareOutput(_diffuseOutput, state).c_str());
  if (_specularOutput->hasEndpoints()) {
    state.compilationString
      += String::printf("%s = specularBase;\r\n", _declareOutput(_specularOutput, state).c_str());
  }

  return *this;
}

json LightBlock::serialize() const
{
  return nullptr;
}

void LightBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                              const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
