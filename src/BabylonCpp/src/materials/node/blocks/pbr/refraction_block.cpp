#include <babylon/materials/node/blocks/pbr/refraction_block.h>

#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_connection_point_custom_object.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

RefractionBlock::RefractionBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , linkRefractionWithTransparency{false}
    , invertRefractionY{false}
    , texture{nullptr}
    , intensity{this, &RefractionBlock::get_intensity}
    , indexOfRefraction{this, &RefractionBlock::get_indexOfRefraction}
    , tintAtDistance{this, &RefractionBlock::get_tintAtDistance}
    , view{this, &RefractionBlock::get_view}
    , refraction{this, &RefractionBlock::get_refraction}
    , hasTexture{this, &RefractionBlock::get_hasTexture}
    , _scene{nullptr}
{
  _isUnique = true;
}

RefractionBlock::~RefractionBlock() = default;

void RefractionBlock::RegisterConnections(const RefractionBlockPtr& refractionBlock)
{
  refractionBlock->registerInput("intensity", NodeMaterialBlockConnectionPointTypes::Float, false,
                                 NodeMaterialBlockTargets::Fragment);
  refractionBlock->registerInput("indexOfRefraction", NodeMaterialBlockConnectionPointTypes::Float,
                                 true, NodeMaterialBlockTargets::Fragment);
  refractionBlock->registerInput("tintAtDistance", NodeMaterialBlockConnectionPointTypes::Float,
                                 true, NodeMaterialBlockTargets::Fragment);
  refractionBlock->registerInput("view", NodeMaterialBlockConnectionPointTypes::Matrix, false,
                                 NodeMaterialBlockTargets::Fragment);

  refractionBlock->registerOutput(
    "refraction", NodeMaterialBlockConnectionPointTypes::Object, NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "refraction", refractionBlock, NodeMaterialConnectionPointDirection::Output,
      [](const std::string& iName) -> RefractionBlockPtr { return RefractionBlock::New(iName); },
      "RefractionBlock"));
}

std::string RefractionBlock::getClassName() const
{
  return "RefractionBlock";
}

NodeMaterialConnectionPointPtr& RefractionBlock::get_intensity()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& RefractionBlock::get_indexOfRefraction()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& RefractionBlock::get_tintAtDistance()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& RefractionBlock::get_view()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& RefractionBlock::get_refraction()
{
  return _outputs[0];
}

bool RefractionBlock::get_hasTexture() const
{
  return _getTexture() != nullptr;
}

BaseTexturePtr RefractionBlock::_getTexture() const
{
  if (texture) {
    return texture;
  }

  return _scene->environmentTexture();
}

void RefractionBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!intensity()->isConnected()) {
    auto intensityInput
      = InputBlock::New("Refraction intensity", NodeMaterialBlockTargets::Fragment,
                        NodeMaterialBlockConnectionPointTypes::Float);
    intensityInput->value = std::make_shared<AnimationValue>(1.f);
    intensityInput->output()->connectTo(intensity);
  }

  if (!view()->isConnected()) {
    auto viewInput = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
      return b->systemValue() == NodeMaterialSystemValues::View;
    });

    if (!viewInput) {
      viewInput = InputBlock::New("view");
      viewInput->setAsSystemValue(NodeMaterialSystemValues::View);
    }
    viewInput->output()->connectTo(view);
  }
}

void RefractionBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                     NodeMaterialDefines& defines, bool /*useInstances*/,
                                     const SubMeshPtr& /*subMesh*/)
{
  NodeMaterialBlock::prepareDefines(mesh, nodeMaterial, defines);

  const auto refractionTexture = _getTexture();
  const auto iRefraction       = refractionTexture && refractionTexture->getTextureMatrix();

  defines.setValue("SS_REFRACTION", iRefraction, true);

  if (!iRefraction) {
    return;
  }

  defines.setValue(_define3DName, refractionTexture->isCube(), true);
  defines.setValue(_defineLODRefractionAlpha, refractionTexture->lodLevelInAlpha, true);
  defines.setValue(_defineLinearSpecularRefraction, refractionTexture->linearSpecularLOD(), true);
  defines.setValue(_defineOppositeZ,
                   _scene->useRightHandedSystem() ? !refractionTexture->invertZ :
                                                    refractionTexture->invertZ,
                   true);

  defines.setValue("SS_LINKREFRACTIONTOTRANSPARENCY", linkRefractionWithTransparency, true);
  defines.setValue("SS_GAMMAREFRACTION", refractionTexture->gammaSpace(), true);
  defines.setValue("SS_RGBDREFRACTION", refractionTexture->isRGBD(), true);
}

bool RefractionBlock::isReady(AbstractMesh* /*mesh*/, const NodeMaterialPtr& /*nodeMaterial*/,
                              const NodeMaterialDefines& /*defines*/, bool /*useInstances*/)
{
  const auto iTexture = _getTexture();

  if (iTexture && !iTexture->isReadyOrNotBlocking()) {
    return false;
  }

  return true;
}

void RefractionBlock::bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh,
                           SubMesh* /*subMesh*/)
{
  NodeMaterialBlock::bind(effect, nodeMaterial, mesh);

  const auto refractionTexture = _getTexture();

  if (!refractionTexture) {
    return;
  }

  if (refractionTexture->isCube()) {
    effect->setTexture(_cubeSamplerName, refractionTexture);
  }
  else {
    effect->setTexture(_2DSamplerName, refractionTexture);
  }

  effect->setMatrix(_refractionMatrixName, *refractionTexture->getReflectionTextureMatrix());

  auto depth = 1.f;
  if (!refractionTexture->isCube()) {
    /*if (refractionTexture.depth) {
        depth = (<any>refractionTexture).depth;
    }*/
  }

  auto iIndexOfRefraction = 1.f;
  (indexOfRefraction() && indexOfRefraction()->connectInputBlock()
   && indexOfRefraction()->connectInputBlock()->value()) ?
    indexOfRefraction()->connectInputBlock()->value()->get<float>() :
    1.5f;

  effect->setFloat4(_vRefractionInfosName, refractionTexture->level, 1.f / iIndexOfRefraction,
                    depth, invertRefractionY ? -1.f : 1.f);

  effect->setFloat3(
    _vRefractionMicrosurfaceInfosName, static_cast<float>(refractionTexture->getSize().width),
    refractionTexture->lodGenerationScale(), refractionTexture->lodGenerationOffset());
}

std::string RefractionBlock::getCode(NodeMaterialBuildState& state)
{
  std::string code = "";

  state.sharedData->blockingBlocks.emplace_back(shared_from_this());
  state.sharedData->textureBlocks.emplace_back(
    std::static_pointer_cast<RefractionBlock>(shared_from_this()));

  // Samplers
  _cubeSamplerName = state._getFreeVariableName(name + "CubeSampler");
  state.samplers.emplace_back(_cubeSamplerName);

  _2DSamplerName = state._getFreeVariableName(name + "2DSampler");
  state.samplers.emplace_back(_2DSamplerName);

  _define3DName = state._getFreeDefineName("SS_REFRACTIONMAP_3D");

  state._samplerDeclaration += StringTools::printf("#ifdef %s\r\n", _define3DName.c_str());
  state._samplerDeclaration
    += StringTools::printf("uniform samplerCube %s;\r\n", _cubeSamplerName.c_str());
  state._samplerDeclaration += "#else\r\n";
  state._samplerDeclaration
    += StringTools::printf("uniform sampler2D %s;\r\n", _2DSamplerName.c_str());
  state._samplerDeclaration += "#endif\r\n";

  // Fragment
  state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  state.sharedData->bindableBlocks.emplace_back(shared_from_this());

  _defineLODRefractionAlpha       = state._getFreeDefineName("SS_LODINREFRACTIONALPHA");
  _defineLinearSpecularRefraction = state._getFreeDefineName("SS_LINEARSPECULARREFRACTION");
  _defineOppositeZ                = state._getFreeDefineName("SS_REFRACTIONMAP_OPPOSITEZ");

  _refractionMatrixName = state._getFreeVariableName("refractionMatrix");

  state._emitUniformFromString(_refractionMatrixName, "mat4");

  const auto iComment = StringTools::printf("//%s", name.c_str());

  state._emitFunction("sampleRefraction",
                      StringTools::printf(
                        R"(
                           #ifdef %s
                               #define sampleRefraction(s, c) textureCube(s, c)
                           #else
                               #define sampleRefraction(s, c) texture2D(s, c)
                           #endif\r\n
                          )",
                        _define3DName.c_str()),
                      iComment.c_str());

  state._emitFunction("sampleRefractionLod",
                      StringTools::printf(
                        R"(
                           #ifdef %s
                               #define sampleRefractionLod(s, c, l) textureCubeLodEXT(s, c, l)
                           #else
                               #define sampleRefractionLod(s, c, l) texture2DLodEXT(s, c, l)
                           #endif\r\n
                          )",
                        _define3DName.c_str()),
                      iComment.c_str());

  _vRefractionMicrosurfaceInfosName = state._getFreeVariableName("vRefractionMicrosurfaceInfos");

  state._emitUniformFromString(_vRefractionMicrosurfaceInfosName, "vec3");

  _vRefractionInfosName = state._getFreeVariableName("vRefractionInfos");

  state._emitUniformFromString(_vRefractionInfosName, "vec4");

  return code;
}

RefractionBlock& RefractionBlock::_buildBlock(NodeMaterialBuildState& state)
{
  _scene = state.sharedData->scene;

  return *this;
}

std::string RefractionBlock::_dumpPropertiesCode()
{
  auto codeString = NodeMaterialBlock::_dumpPropertiesCode();

  if (texture) {
    if (texture->isCube()) {
      codeString = StringTools::printf("%s.texture = CubeTexture::New(\"%s\");\r\n",
                                       _codeVariableName.c_str(), texture->name.c_str());
    }
    else {
      codeString = StringTools::printf("%s.texture = Texture::New(\"%s\");\r\n",
                                       _codeVariableName.c_str(), texture->name.c_str());
    }
    codeString += StringTools::printf("%s.texture->coordinatesMode = %u;\r\n",
                                      _codeVariableName.c_str(), texture->coordinatesMode());
  }

  codeString
    += StringTools::printf("%s.linkRefractionWithTransparency = %s;\r\n", _codeVariableName.c_str(),
                           linkRefractionWithTransparency ? "true" : "false");
  codeString += StringTools::printf("%s.invertRefractionY = %s;\r\n", _codeVariableName.c_str(),
                                    invertRefractionY ? "true" : "false");

  return codeString;
}

json RefractionBlock::serialize() const
{
  return nullptr;
}

void RefractionBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                   const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
