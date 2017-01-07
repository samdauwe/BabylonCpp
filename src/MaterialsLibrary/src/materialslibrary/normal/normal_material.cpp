#include <babylon/materialslibrary/normal/normal_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

NormalMaterial::NormalMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
    , diffuseTexture{nullptr}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , disableLighting{false}
    , maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
    , _cachedDefines{std_util::make_unique<NormalMaterialDefines>()}
{
  _cachedDefines->BonesPerMesh         = 0;
  _cachedDefines->NUM_BONE_INFLUENCERS = 0;
}

NormalMaterial::~NormalMaterial()
{
}

IReflect::Type NormalMaterial::type() const
{
  return IReflect::Type::NORMALMATERIAL;
}

void NormalMaterial::setDiffuseColor(const Color3& color)
{
  diffuseColor = color;
}

bool NormalMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool NormalMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* NormalMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool NormalMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                                 bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[NMD::INSTANCES] != useInstances) {
    return false;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool NormalMaterial::isReady(AbstractMesh* mesh, bool useInstances)
{
  if (checkReadyOnlyOnce) {
    if (_wasPreviouslyReady) {
      return true;
    }
  }

  auto scene = getScene();

  if (!checkReadyOnEveryCall) {
    if (_renderId == scene->getRenderId()) {
      if (_checkCache(scene, mesh, useInstances)) {
        return true;
      }
    }
  }

  auto engine      = scene->getEngine();
  auto needNormals = false;
  auto needUVs     = false;

  _defines.reset();

  // Textures
  if (scene->texturesEnabled) {
    if (diffuseTexture && StandardMaterial::DiffuseTextureEnabled) {
      if (!diffuseTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[NMD::DIFFUSE] = true;
      }
    }
  }

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[NMD::CLIPPLANE] = true;
  }

  if (engine->getAlphaTesting()) {
    _defines.defines[NMD::ALPHATEST] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud) {
    _defines.defines[NMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled && mesh && mesh->applyFog
      && scene->fogMode != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[NMD::FOG] = true;
  }

  if (scene->lightsEnabled && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, maxSimultaneousLights, NMD::SPECULARTERM,
      NMD::SHADOWS);
  }

  // Attribs
  if (mesh) {
    if (needNormals && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      _defines.defines[NMD::NORMAL] = true;
    }
    if (needUVs) {
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        _defines.defines[NMD::UV1] = true;
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        _defines.defines[NMD::UV2] = true;
      }
    }
    if (mesh->useVertexColors
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[NMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha) {
        _defines.defines[NMD::VERTEXALPHA] = true;
      }
    }
    if (mesh->useBones() && mesh->computeBonesUsingShaders) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers;
      _defines.BonesPerMesh         = mesh->skeleton()->bones.size() + 1;
    }

    // Instances
    if (useInstances) {
      _defines.defines[NMD::INSTANCES] = true;
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (_defines[NMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(_defines, fallbacks.get());

    if (_defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (_defines[NMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (_defines[NMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (_defines[NMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (_defines[NMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  NMD::INSTANCES);

    std::string shaderName = "normal";
    std::string join       = _defines.toString();
    std::vector<std::string> uniforms{"world",           "view",
                                      "viewProjection",  "vEyePosition",
                                      "vLightsType",     "vDiffuseColor",
                                      "vLightData0",     "vLightDiffuse0",
                                      "vLightSpecular0", "vLightDirection0",
                                      "vLightGround0",   "lightMatrix0",
                                      "vLightData1",     "vLightDiffuse1",
                                      "vLightSpecular1", "vLightDirection1",
                                      "vLightGround1",   "lightMatrix1",
                                      "vLightData2",     "vLightDiffuse2",
                                      "vLightSpecular2", "vLightDirection2",
                                      "vLightGround2",   "lightMatrix2",
                                      "vLightData3",     "vLightDiffuse3",
                                      "vLightSpecular3", "vLightDirection3",
                                      "vLightGround3",   "lightMatrix3",
                                      "vFogInfos",       "vFogColor",
                                      "pointSize",       "vDiffuseInfos",
                                      "mBones",          "vClipPlane",
                                      "diffuseMatrix",   "shadowsInfo0",
                                      "shadowsInfo1",    "shadowsInfo2",
                                      "shadowsInfo3",    "depthValues"};
    std::vector<std::string> samplers{"diffuseSampler", "shadowSampler0",
                                      "shadowSampler1", "shadowSampler2",
                                      "shadowSampler3"};
    std::unordered_map<std::string, unsigned int> indexParameters{
      {"maxSimultaneousLights", maxSimultaneousLights}};

    _effect = engine->createEffect(shaderName, attribs, uniforms, samplers,
                                   join, fallbacks.get(), onCompiled, onError,
                                   indexParameters);
  }
  if (!_effect->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  if (mesh) {
    if (!mesh->_materialDefines) {
      mesh->_materialDefines = std_util::make_unique<NormalMaterialDefines>();
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void NormalMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void NormalMaterial::bind(Matrix* world, Mesh* mesh)
{
  auto scene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);
  _effect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  if (scene->getCachedMaterial() != this) {
    // Textures
    if (diffuseTexture && StandardMaterial::DiffuseTextureEnabled) {
      _effect->setTexture("diffuseSampler", diffuseTexture);

      _effect->setFloat2("vDiffuseInfos",
                         static_cast<float>(diffuseTexture->coordinatesIndex),
                         diffuseTexture->level);
      _effect->setMatrix("diffuseMatrix", *diffuseTexture->getTextureMatrix());
    }

    // Clip plane
    MaterialHelper::BindClipPlane(_effect, scene);

    // Point size
    if (pointsCloud()) {
      _effect->setFloat("pointSize", pointSize);
    }

    _effect->setVector3("vEyePosition", scene->_mirroredCameraPosition ?
                                          *scene->_mirroredCameraPosition :
                                          scene->activeCamera->position);
  }

  _effect->setColor4("vDiffuseColor", diffuseColor, alpha * mesh->visibility);

  // Lights
  if (scene->lightsEnabled && !disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _effect,
                               _defines.defines[NMD::SPECULARTERM]);
  }

  // View
  if (scene->fogEnabled && mesh->applyFog
      && scene->fogMode != Scene::FOGMODE_NONE) {
    _effect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _effect);

  Material::bind(world, mesh);
}

std::vector<IAnimatable*> NormalMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (diffuseTexture && diffuseTexture->animations.size() > 0) {
    results.emplace_back(diffuseTexture);
  }

  return results;
}

void NormalMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (diffuseTexture) {
    diffuseTexture->dispose();
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* NormalMaterial::clone(const std::string& /*name*/,
                                bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object NormalMaterial::serialize() const
{
  return Json::object();
}

NormalMaterial* NormalMaterial::Parse(const Json::value& /*source*/,
                                      Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
