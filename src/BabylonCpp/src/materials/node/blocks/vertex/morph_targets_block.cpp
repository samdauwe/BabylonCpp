#include <babylon/materials/node/blocks/vertex/morph_targets_block.h>

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
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/morph/morph_target_manager.h>

namespace BABYLON {

MorphTargetsBlock::MorphTargetsBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Vertex}
    , position{this, &MorphTargetsBlock::get_position}
    , normal{this, &MorphTargetsBlock::get_normal}
    , tangent{this, &MorphTargetsBlock::get_tangent}
    , uv{this, &MorphTargetsBlock::get_uv}
    , positionOutput{this, &MorphTargetsBlock::get_positionOutput}
    , normalOutput{this, &MorphTargetsBlock::get_normalOutput}
    , tangentOutput{this, &MorphTargetsBlock::get_tangentOutput}
    , uvOutput{this, &MorphTargetsBlock::get_uvOutput}
    , _repeatebleContentGenerated{0}
{
  registerInput("position", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerInput("normal", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerInput("tangent", NodeMaterialBlockConnectionPointTypes::Vector3);
  registerInput("uv", NodeMaterialBlockConnectionPointTypes::Vector2);
  registerOutput("positionOutput",
                 NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("normalOutput",
                 NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("tangentOutput",
                 NodeMaterialBlockConnectionPointTypes::Vector3);
  registerOutput("uvOutput", NodeMaterialBlockConnectionPointTypes::Vector2);
}

MorphTargetsBlock::~MorphTargetsBlock()
{
}

const std::string MorphTargetsBlock::getClassName() const
{
  return "MorphTargetsBlock";
}

NodeMaterialConnectionPointPtr& MorphTargetsBlock::get_position()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& MorphTargetsBlock::get_normal()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& MorphTargetsBlock::get_tangent()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& MorphTargetsBlock::get_uv()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& MorphTargetsBlock::get_positionOutput()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& MorphTargetsBlock::get_normalOutput()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& MorphTargetsBlock::get_tangentOutput()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& MorphTargetsBlock::get_uvOutput()
{
  return _outputs[3];
}

void MorphTargetsBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("morphTargetInfluences");
}

void MorphTargetsBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!position()->isConnected()) {
    auto positionInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->isAttribute() && b->name == "position";
        });

    if (!positionInput) {
      positionInput = InputBlock::New("position");
      positionInput->setAsAttribute();
    }
    positionInput->output()->connectTo(position);
  }
  if (!normal()->isConnected()) {
    auto normalInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->isAttribute() && b->name == "normal";
        });

    if (!normalInput) {
      normalInput = InputBlock::New("normal");
      normalInput->setAsAttribute("normal");
    }
    normalInput->output()->connectTo(normal);
  }
  if (!tangent()->isConnected()) {
    auto tangentInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->isAttribute() && b->name == "tangent";
        });

    if (!tangentInput) {
      tangentInput = InputBlock::New("tangent");
      tangentInput->setAsAttribute("tangent");
    }
    tangentInput->output()->connectTo(tangent);
  }
  if (!uv()->isConnected()) {
    auto uvInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->isAttribute() && b->name == "uv";
        });

    if (!uvInput) {
      uvInput = InputBlock::New("uv");
      uvInput->setAsAttribute("uv");
    }
    uvInput->output()->connectTo(uv);
  }
}

void MorphTargetsBlock::prepareDefines(AbstractMesh* mesh,
                                       const NodeMaterialPtr& /*nodeMaterial*/,
                                       NodeMaterialDefines& defines,
                                       bool /*useInstances*/)
{
  if (!defines._areAttributesDirty) {
    return;
  }
  MaterialHelper::PrepareDefinesForMorphTargets(mesh, defines);
}

void MorphTargetsBlock::bind(const EffectPtr& effect,
                             const NodeMaterialPtr& /*nodeMaterial*/,
                             Mesh* mesh)
{
  if (mesh && _repeatebleContentGenerated > 0) {
    MaterialHelper::BindMorphTargetParameters(mesh, effect);
  }

  MaterialHelper::BindBonesParameters(mesh, effect);
}

void MorphTargetsBlock::replaceRepeatableContent(
  NodeMaterialBuildState& vertexShaderState,
  const NodeMaterialBuildState& fragmentShaderState, AbstractMesh* mesh,
  NodeMaterialDefines& defines)
{
  const auto& _position       = position();
  const auto& _normal         = normal();
  const auto& _tangent        = tangent();
  const auto& _uv             = uv();
  const auto& _positionOutput = positionOutput();
  const auto& _normalOutput   = normalOutput();
  const auto& _tangentOutput  = tangentOutput();
  const auto& _uvOutput       = uvOutput();
  auto& _state                = vertexShaderState;
  auto repeatCount            = defines.intDef["NUM_MORPH_INFLUENCERS"];
  _repeatebleContentGenerated = repeatCount;

  auto& manager   = static_cast<Mesh*>(mesh)->morphTargetManager();
  auto hasNormals = manager && manager->supportsNormals() && defines["NORMAL"];
  auto hasTangents
    = manager && manager->supportsTangents() && defines["TANGENT"];
  auto hasUVs = manager && manager->supportsUVs() && defines["UV1"];

  std::string injectionCode = "";

  for (size_t index = 0; index < repeatCount; index++) {
    injectionCode += "#ifdef MORPHTARGETS\r\n";
    injectionCode += String::printf(
      "%s += (position%zu - %s) * morphTargetInfluences[%zu];\r\n",
      _positionOutput->associatedVariableName().c_str(), index,
      _position->associatedVariableName().c_str(), index);

    if (hasNormals) {
      injectionCode += "#ifdef MORPHTARGETS_NORMAL\r\n";
      injectionCode += String::printf(
        "%s += (normal%zu - %s) * morphTargetInfluences[%zu];\r\n",
        _normalOutput->associatedVariableName().c_str(), index,
        _normal->associatedVariableName().c_str(), index);
      injectionCode += "#endif\r\n";
    }

    if (hasTangents) {
      injectionCode += "#ifdef MORPHTARGETS_TANGENT\r\n";
      injectionCode += String::printf(
        "%s.xyz += (tangent%zu - %s.xyz) * morphTargetInfluences[%zu];\r\n",
        _tangentOutput->associatedVariableName().c_str(), index,
        _tangent->associatedVariableName().c_str(), index);
      injectionCode += "#endif\r\n";
    }

    if (hasUVs) {
      injectionCode += "#ifdef MORPHTARGETS_UV\r\n";
      injectionCode += String::printf(
        "%s.xyz += (uv_%zu - %s.xyz) * morphTargetInfluences[%zu];\r\n",
        _uvOutput->associatedVariableName().c_str(), index,
        _uv->associatedVariableName().c_str(), index);
      injectionCode += "#endif\r\n";
    }

    injectionCode += "#endif\r\n";
  }

  _state.compilationString = String::replace(
    _state.compilationString, _repeatableContentAnchor, injectionCode);

  if (repeatCount > 0) {
    for (size_t index = 0; index < repeatCount; index++) {
      _state.attributes.emplace_back(
        String::printf("%s%zu", VertexBuffer::PositionKind, index));

      if (hasNormals) {
        _state.attributes.emplace_back(
          String::printf("%s%zu", VertexBuffer::NormalKind, index));
      }

      if (hasTangents) {
        _state.attributes.emplace_back(
          String::printf("%s%zu", VertexBuffer::TangentKind, index));
      }
    }
  }
}

MorphTargetsBlock& MorphTargetsBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  // Register for defines
  state.sharedData->blocksWithDefines.emplace_back(this);

  // Register for binding
  state.sharedData->bindableBlocks.emplace_back(this);

  // Register for repeatable content generation
  state.sharedData->repeatableContentBlocks.emplace_back(this);

  // Emit code
  const auto& _position       = position();
  const auto& _normal         = normal();
  const auto& _tangent        = tangent();
  const auto& _uv             = uv();
  const auto& _positionOutput = positionOutput();
  const auto& _normalOutput   = normalOutput();
  const auto& _tangentOutput  = tangentOutput();
  const auto& _uvOutput       = uvOutput;
  auto comments               = String::printf("//%s", name.c_str());

  state.uniforms.emplace_back("morphTargetInfluences");

  state._emitFunctionFromInclude("morphTargetsVertexGlobalDeclaration",
                                 comments);
  state._emitFunctionFromInclude("morphTargetsVertexDeclaration", comments,
                                 EmitFunctionFromIncludeOptions{
                                   "maxSimultaneousMorphTargets" // repeatKey
                                 });

  state.compilationString += String::printf(
    "%s = %s;\r\n", _declareOutput(_positionOutput, state).c_str(),
    _position->associatedVariableName().c_str());
  state.compilationString += "#ifdef NORMAL\r\n";
  state.compilationString += String::printf(
    "%s = %s;\r\n", _declareOutput(_normalOutput, state).c_str(),
    _normal->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";
  state.compilationString += "#ifdef TANGENT\r\n";
  state.compilationString += String::printf(
    "%s = %s;\r\n", _declareOutput(_tangentOutput, state).c_str(),
    _tangent->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";
  state.compilationString += "#ifdef UV1\r\n";
  state.compilationString
    += String::printf("%s = %s;\r\n", _declareOutput(_uvOutput, state).c_str(),
                      _uv->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";

  // Repeatable content
  _repeatableContentAnchor = state._repeatableContentAnchor;
  state.compilationString += _repeatableContentAnchor;

  return *this;
}

} // end of namespace BABYLON
