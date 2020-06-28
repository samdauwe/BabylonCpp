#include <babylon/materials/node/blocks/pbr/ambient_occlusion_block.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_connection_point_custom_object.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

AmbientOcclusionBlock::AmbientOcclusionBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , texture{this, &AmbientOcclusionBlock::get_texture}
    , intensity{this, &AmbientOcclusionBlock::get_intensity}
    , directLightIntensity{this, &AmbientOcclusionBlock::get_directLightIntensity}
    , ambientOcclusion{this, &AmbientOcclusionBlock::get_ambientOcclusion}
    , useAmbientInGrayScale{false}
{
  _isUnique = true;
}

AmbientOcclusionBlock::~AmbientOcclusionBlock() = default;

void AmbientOcclusionBlock::RegisterConnections(
  const AmbientOcclusionBlockPtr& ambientOcclusionBlock)
{
  ambientOcclusionBlock->registerInput("texture", NodeMaterialBlockConnectionPointTypes::Color3,
                                       true, NodeMaterialBlockTargets::Fragment);
  ambientOcclusionBlock->registerInput("intensity", NodeMaterialBlockConnectionPointTypes::Float,
                                       true, NodeMaterialBlockTargets::Fragment);
  ambientOcclusionBlock->registerInput("directLightIntensity",
                                       NodeMaterialBlockConnectionPointTypes::Float, true,
                                       NodeMaterialBlockTargets::Fragment);

  ambientOcclusionBlock->registerOutput(
    "ambientOcclusion", NodeMaterialBlockConnectionPointTypes::Object,
    NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "ambientOcclusion", ambientOcclusionBlock, NodeMaterialConnectionPointDirection::Output,
      [](const std::string& iName) -> AmbientOcclusionBlockPtr {
        return AmbientOcclusionBlock::New(iName);
      },
      "AOBlock"));
}

void AmbientOcclusionBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("aoOut");
}

std::string AmbientOcclusionBlock::getClassName() const
{
  return "AmbientOcclusionBlock";
}

NodeMaterialConnectionPointPtr& AmbientOcclusionBlock::get_texture()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& AmbientOcclusionBlock::get_intensity()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& AmbientOcclusionBlock::get_directLightIntensity()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& AmbientOcclusionBlock::get_ambientOcclusion()
{
  return _outputs[0];
}

std::string AmbientOcclusionBlock::GetCode(const AmbientOcclusionBlockPtr& block)
{
  std::string code = "ambientOcclusionOutParams aoOut;\r\n";

  const std::string aoTexture
    = block->texture()->isConnected() ? block->texture()->associatedVariableName() : "vec3(0.)";
  const std::string aoIntensity
    = block->intensity()->isConnected() ? block->intensity()->associatedVariableName() : "1.";

  code += StringTools::printf(
    R"(ambientOcclusionBlock(
      #ifdef AMBIENT
          %s,
          vec4(0., 1.0, %s, 0.),
      #endif
          aoOut
      );\r\n)",
    aoTexture.c_str(), aoIntensity.c_str());

  return code;
}

void AmbientOcclusionBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                           NodeMaterialDefines& defines, bool /*useInstances*/,
                                           const SubMeshPtr& /*subMesh*/)
{
  NodeMaterialBlock::prepareDefines(mesh, nodeMaterial, defines);

  defines.setValue("AMBIENT", texture()->isConnected(), true);
  defines.setValue("AMBIENTINGRAYSCALE", useAmbientInGrayScale, true);
}

AmbientOcclusionBlock& AmbientOcclusionBlock::_buildBlock(NodeMaterialBuildState& state)
{
  if (state.target == NodeMaterialBlockTargets::Fragment) {
    state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  }

  return *this;
}

std::string AmbientOcclusionBlock::_dumpPropertiesCode()
{
  const auto codeString
    = StringTools::printf("%s.useAmbientInGrayScale = %s;\r\n", _codeVariableName.c_str(),
                          useAmbientInGrayScale ? "true" : "false");

  return codeString;
}

json AmbientOcclusionBlock::serialize() const
{
  return nullptr;
}

void AmbientOcclusionBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                         const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
