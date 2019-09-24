#include <babylon/materials/node/blocks/vertex/bones_block.h>

#include <babylon/core/string.h>
#include <babylon/materials/effect_fallbacks.h>
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

BonesBlock::BonesBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Vertex}
    , matricesIndices{this, &BonesBlock::get_matricesIndices}
    , matricesWeights{this, &BonesBlock::get_matricesWeights}
    , matricesIndicesExtra{this, &BonesBlock::get_matricesIndicesExtra}
    , matricesWeightsExtra{this, &BonesBlock::get_matricesWeightsExtra}
    , world{this, &BonesBlock::get_world}
    , output{this, &BonesBlock::get_output}
{
  registerInput("matricesIndices",
                NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("matricesWeights",
                NodeMaterialBlockConnectionPointTypes::Vector4);
  registerInput("matricesIndicesExtra",
                NodeMaterialBlockConnectionPointTypes::Vector4, true);
  registerInput("matricesWeightsExtra",
                NodeMaterialBlockConnectionPointTypes::Vector4, true);
  registerInput("world", NodeMaterialBlockConnectionPointTypes::Matrix);

  registerOutput("output", NodeMaterialBlockConnectionPointTypes::Matrix);
}

BonesBlock::~BonesBlock()
{
}

void BonesBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("boneSampler");
  state._excludeVariableName("boneTextureWidth");
  state._excludeVariableName("mBones");
  state._excludeVariableName("BonesPerMesh");
}

const std::string BonesBlock::getClassName() const
{
  return "BonesBlock";
}

NodeMaterialConnectionPointPtr& BonesBlock::get_matricesIndices()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& BonesBlock::get_matricesWeights()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& BonesBlock::get_matricesIndicesExtra()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& BonesBlock::get_matricesWeightsExtra()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& BonesBlock::get_world()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& BonesBlock::get_output()
{
  return _outputs[0];
}

void BonesBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!matricesIndices()->isConnected()) {
    auto matricesIndicesInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->isAttribute() && b->name == "matricesIndices";
        });

    if (!matricesIndicesInput) {
      matricesIndicesInput = InputBlock::New("matricesIndices");
      matricesIndicesInput->setAsAttribute("matricesIndices");
    }
    matricesIndicesInput->output()->connectTo(matricesIndices);
  }
  if (!matricesWeights()->isConnected()) {
    auto matricesWeightsInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->isAttribute() && b->name == "matricesWeights";
        });

    if (!matricesWeightsInput) {
      matricesWeightsInput = InputBlock::New("matricesWeights");
      matricesWeightsInput->setAsAttribute("matricesWeights");
    }
    matricesWeightsInput->output()->connectTo(matricesWeights);
  }
  if (!world()->isConnected()) {
    auto worldInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->systemValue().has_value()
                 && b->systemValue().value() == NodeMaterialSystemValues::World;
        });

    if (!worldInput) {
      worldInput = InputBlock::New("world");
      worldInput->setAsSystemValue(NodeMaterialSystemValues::World);
    }
    worldInput->output()->connectTo(world);
  }
}

void BonesBlock::provideFallbacks(AbstractMesh* mesh,
                                  EffectFallbacks* fallbacks)
{
  if (mesh && mesh->useBones() && mesh->computeBonesUsingShaders()
      && mesh->skeleton()) {
    fallbacks->addCPUSkinningFallback(0, mesh);
  }
}

void BonesBlock::bind(const EffectPtr& effect,
                      const NodeMaterialPtr& /*nodeMaterial*/, Mesh* mesh)
{
  MaterialHelper::BindBonesParameters(mesh, effect);
}

void BonesBlock::prepareDefines(AbstractMesh* mesh,
                                const NodeMaterialPtr& /*nodeMaterial*/,
                                NodeMaterialDefines& defines,
                                bool /*useInstances*/)
{
  if (!defines._areAttributesDirty) {
    return;
  }
  MaterialHelper::PrepareDefinesForBones(mesh, defines);
}

BonesBlock& BonesBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  // Register for compilation fallbacks
  state.sharedData->blocksWithFallbacks.emplace_back(this);

  // Register for binding
  state.sharedData->bindableBlocks.emplace_back(this);

  // Register for defines
  state.sharedData->blocksWithDefines.emplace_back(this);

  // Register internal uniforms and samplers
  state.uniforms.emplace_back("boneTextureWidth");
  state.uniforms.emplace_back("mBones");

  state.samplers.emplace_back("boneSampler");

  // Emit code
  auto comments = String::printf("//%s", name.c_str());
  state._emitFunctionFromInclude("bonesDeclaration", comments,
                                 EmitFunctionFromIncludeOptions{
                                   "",    // repeatKey
                                   true,  // removeAttributes
                                   false, // removeUniforms
                                   true,  // removeVaryings
                                   false, // removeIfDef
                                   {}     // replaceStrings
                                 });

  auto influenceVariablename = state._getFreeVariableName("influence");

  state.compilationString += state._emitCodeFromInclude(
    "bonesVertex", comments,
    EmitCodeFromIncludeOptions{
      {StringsReplacement{
         "finalWorld=finalWorld\\*influence;", // search
         ""                                    // repeatKey
       },
       StringsReplacement{
         "influence",          // search
         influenceVariablename // repeatKey
       }}                      // replaceStrings
    });

  const auto& output     = _outputs[0];
  const auto& worldInput = world();

  state.compilationString += "#if NUM_BONE_INFLUENCERS>0\r\n";
  state.compilationString
    += _declareOutput(output, state)
       + String::printf(" = %s * %s;\r\n",
                        worldInput->associatedVariableName().c_str(),
                        influenceVariablename.c_str());
  state.compilationString += "#else\r\n";
  state.compilationString
    += _declareOutput(output, state)
       + String::printf(" = %s;\r\n",
                        worldInput->associatedVariableName().c_str());
  state.compilationString += "#endif\r\n";

  return *this;
}

} // end of namespace BABYLON
