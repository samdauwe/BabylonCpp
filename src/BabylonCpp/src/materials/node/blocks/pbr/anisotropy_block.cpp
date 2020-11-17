#include <babylon/materials/node/blocks/pbr/anisotropy_block.h>

#include <babylon/core/logging.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_connection_point_custom_object.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

AnisotropyBlock::AnisotropyBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , worldPositionConnectionPoint{nullptr}
    , worldNormalConnectionPoint{nullptr}
    , intensity{this, &AnisotropyBlock::get_intensity}
    , direction{this, &AnisotropyBlock::get_direction}
    , uv{this, &AnisotropyBlock::get_uv}
    , worldTangent{this, &AnisotropyBlock::get_worldTangent}
    , anisotropy{this, &AnisotropyBlock::get_anisotropy}
{
}

AnisotropyBlock::~AnisotropyBlock() = default;

void AnisotropyBlock::RegisterConnections(const AnisotropyBlockPtr& anisotropyBlock)
{
  anisotropyBlock->registerInput("intensity", NodeMaterialBlockConnectionPointTypes::Float, true,
                                 NodeMaterialBlockTargets::Fragment);
  anisotropyBlock->registerInput("direction", NodeMaterialBlockConnectionPointTypes::Vector2, true,
                                 NodeMaterialBlockTargets::Fragment);
  anisotropyBlock->registerInput("uv", NodeMaterialBlockConnectionPointTypes::Vector2,
                                 true); // need this property and the next one in case there's no
                                        // PerturbNormal block connected to the main PBR block
  anisotropyBlock->registerInput("worldTangent", NodeMaterialBlockConnectionPointTypes::Vector4,
                                 true);

  anisotropyBlock->registerOutput(
    "anisotropy", NodeMaterialBlockConnectionPointTypes::Object, NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "anisotropy", anisotropyBlock, NodeMaterialConnectionPointDirection::Output,
      [](const std::string& iName) -> AnisotropyBlockPtr { return AnisotropyBlock::New(iName); },
      "AnisotropyBlock"));
}

void AnisotropyBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("anisotropicOut");
  state._excludeVariableName("TBN");
}

std::string AnisotropyBlock::getClassName() const
{
  return "AnisotropyBlock";
}

NodeMaterialConnectionPointPtr& AnisotropyBlock::get_intensity()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& AnisotropyBlock::get_direction()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& AnisotropyBlock::get_uv()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& AnisotropyBlock::get_worldTangent()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& AnisotropyBlock::get_anisotropy()
{
  return _outputs[0];
}

std::string AnisotropyBlock::_generateTBNSpace(NodeMaterialBuildState& state) const
{
  std::string code = "";

  auto iComments     = StringTools::printf("//%s", name().c_str());
  auto iUv           = uv();
  auto worldPosition = worldPositionConnectionPoint;
  auto worldNormal   = worldNormalConnectionPoint;
  auto iWorldTangent = worldTangent();

  if (!iUv->isConnected()) {
    // we must set the uv input as optional because we may not end up in this method (in case a
    // PerturbNormal block is linked to the PBR material) in which case uv is not required. But if
    // we do come here, we do need the uv, so we have to raise an error but not with throw, else it
    // will stop the building of the node material and will lead to errors in the editor!
    BABYLON_LOG_ERROR("AnisotropyBlock", "You must connect the 'uv' input of the Anisotropy block!")
  }

  state._emitExtension("derivatives", "#extension GL_OES_standard_derivatives : enable");

  EmitFunctionFromIncludeOptions tangentReplaceStringOptions;
  tangentReplaceStringOptions.replaceStrings
    = {{R"(defined\(TANGENT\)/)",
        iWorldTangent->isConnected() ? "defined(TANGENT)" : "defined(IGNORE)"}};

  if (iWorldTangent->isConnected()) {
    code += StringTools::printf("vec3 tbnNormal = normalize(%s.xyz);\r\n",
                                worldNormal->associatedVariableName().c_str());
    code += StringTools::printf("vec3 tbnTangent = normalize(%s.xyz);\r\n",
                                iWorldTangent->associatedVariableName().c_str());
    code += "vec3 tbnBitangent = cross(tbnNormal, tbnTangent);\r\n";
    code += "mat3 vTBN = mat3(tbnTangent, tbnBitangent, tbnNormal);\r\n";
  }

  code += StringTools::printf(
    R"(#if defined(${worldTangent.isConnected ? "TANGENT" : "IGNORE"}) && defined(NORMAL)
            mat3 TBN = vTBN;
        #else
            mat3 TBN = cotangent_frame(%s, %s, %s, vec2(1., 1.));
        #endif\r\n
      )",
    StringTools::printf("%s.xyz", worldNormal->associatedVariableName().c_str()).c_str(),
    StringTools::printf("v_%s.xyz", worldPosition->associatedVariableName().c_str()).c_str(),
    StringTools::printf("%s", iUv->isConnected ? iUv->associatedVariableName().c_str() : "vec2(0.)")
      .c_str());

  state._emitFunctionFromInclude("bumpFragmentMainFunctions", iComments,
                                 tangentReplaceStringOptions);

  return code;
}

std::string AnisotropyBlock::getCode(NodeMaterialBuildState& state, bool generateTBNSpace) const
{
  std::string code = "";

  if (generateTBNSpace) {
    code += _generateTBNSpace(state);
  }

  const auto iIntensity
    = intensity()->isConnected() ? intensity()->associatedVariableName() : "1.0";
  const auto iDirection
    = direction()->isConnected() ? direction()->associatedVariableName() : "vec2(1., 0.)";

  code += StringTools::printf(
    R"(anisotropicOutParams anisotropicOut;
        anisotropicBlock(
            vec3(%s, %s),
        #ifdef ANISOTROPIC_TEXTURE
            vec3(0.),
        #endif
            TBN,
            normalW,
            viewDirectionW,
            anisotropicOut
        );\r\n
      )",
    iDirection.c_str(), iIntensity.c_str());

  return code;
}

void AnisotropyBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                     NodeMaterialDefines& defines, bool /*useInstances*/,
                                     SubMesh* /*subMesh*/)
{
  NodeMaterialBlock::prepareDefines(mesh, nodeMaterial, defines);

  defines.setValue("ANISOTROPIC", true);
  defines.setValue("ANISOTROPIC_TEXTURE", false, true);
}

AnisotropyBlock& AnisotropyBlock::_buildBlock(NodeMaterialBuildState& state)
{
  if (state.target == NodeMaterialBlockTargets::Fragment) {
    state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  }

  return *this;
}

} // end of namespace BABYLON
