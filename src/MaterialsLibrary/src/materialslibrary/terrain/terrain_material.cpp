#include <babylon/materialslibrary/terrain/terrain_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

TerrainMaterial::TerrainMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
    , mixTexture{nullptr}
    , diffuseTexture1{nullptr}
    , diffuseTexture2{nullptr}
    , diffuseTexture3{nullptr}
    , bumpTexture1{nullptr}
    , bumpTexture2{nullptr}
    , bumpTexture3{nullptr}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.f, 0.f, 0.f)}
    , specularPower{64.f}
    , disableLighting{false}
    , maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
    , _cachedDefines{std_util::make_unique<TerrainMaterialDefines>()}
{
  _cachedDefines->BonesPerMesh         = 0;
  _cachedDefines->NUM_BONE_INFLUENCERS = 0;
}

TerrainMaterial::~TerrainMaterial()
{
}

bool TerrainMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool TerrainMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* TerrainMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool TerrainMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                                  bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[TMD::INSTANCES] != useInstances) {
    return false;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool TerrainMaterial::isReady(AbstractMesh* mesh, bool useInstances)
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

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[TMD::CLIPPLANE] = true;
  }

  if (engine->getAlphaTesting()) {
    _defines.defines[TMD::ALPHATEST] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud()) {
    _defines.defines[TMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog
      && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[TMD::FOG] = true;
  }

  // Lights
  if (scene->lightsEnabled() && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, maxSimultaneousLights, TMD::SPECULARTERM,
      TMD::SHADOWS);
  }

  // Textures
  if (scene->texturesEnabled()) {
    if (mixTexture && StandardMaterial::DiffuseTextureEnabled) {
      if (!mixTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[TMD::DIFFUSE] = true;
      }
    }
    if ((bumpTexture1 || bumpTexture2 || bumpTexture3)
        && StandardMaterial::BumpTextureEnabled) {
      needUVs                     = true;
      needNormals                 = true;
      _defines.defines[TMD::BUMP] = true;
    }
  }

  // Attribs
  if (mesh) {
    if (needNormals && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      _defines.defines[TMD::NORMAL] = true;
    }
    if (needUVs) {
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        _defines.defines[TMD::UV1] = true;
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        _defines.defines[TMD::UV2] = true;
      }
    }
    if (mesh->useVertexColors
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[TMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha) {
        _defines.defines[TMD::VERTEXALPHA] = true;
      }
    }
    if (mesh->useBones() && mesh->computeBonesUsingShaders) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers;
      _defines.BonesPerMesh         = (mesh->skeleton()->bones.size() + 1);
    }

    // Instances
    if (useInstances) {
      _defines.defines[TMD::INSTANCES] = true;
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (_defines.FOG) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(_defines, fallbacks.get(),
                                              maxSimultaneousLights);

    if (_defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (_defines[TMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (_defines[TMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (_defines[TMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (_defines[TMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  TMD::INSTANCES);

    // Legacy browser patch
    std::string shaderName = "terrain";
    std::string join       = _defines.toString();
    std::vector<std::string> uniforms{
      "world",        "view",          "viewProjection", "vEyePosition",
      "vLightsType",  "vDiffuseColor", "vSpecularColor", "vFogInfos",
      "vFogColor",    "pointSize",     "vTextureInfos",  "mBones",
      "vClipPlane",   "textureMatrix", "diffuse1Infos",  "diffuse2Infos",
      "diffuse3Infos"};
    std::vector<std::string> samplers{
      "textureSampler", "diffuse1Sampler", "diffuse2Sampler", "diffuse3Sampler",
      "bump1Sampler",   "bump2Sampler",    "bump3Sampler"};
    std::unordered_map<std::string, unsigned int> indexParameters{
      {"maxSimultaneousLights", maxSimultaneousLights}};

    MaterialHelper::PrepareUniformsAndSamplersList(uniforms, samplers, _defines,
                                                   maxSimultaneousLights);

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
      mesh->_materialDefines = std_util::make_unique<TerrainMaterialDefines>();
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void TerrainMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void TerrainMaterial::bind(Matrix* world, Mesh* mesh)
{
  auto scene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);
  _effect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  if (scene->getCachedMaterial() != this) {
    // Textures
    if (mixTexture) {
      _effect->setTexture("textureSampler", mixTexture);
      _effect->setFloat2("vTextureInfos",
                         static_cast<float>(mixTexture->coordinatesIndex),
                         mixTexture->level);
      _effect->setMatrix("textureMatrix", *mixTexture->getTextureMatrix());

      if (StandardMaterial::DiffuseTextureEnabled) {
        if (diffuseTexture1) {
          _effect->setTexture("diffuse1Sampler", diffuseTexture1);
          _effect->setFloat2("diffuse1Infos", diffuseTexture1->uScale,
                             diffuseTexture1->vScale);
        }
        if (diffuseTexture2) {
          _effect->setTexture("diffuse2Sampler", diffuseTexture2);
          _effect->setFloat2("diffuse2Infos", diffuseTexture2->uScale,
                             diffuseTexture2->vScale);
        }
        if (diffuseTexture3) {
          _effect->setTexture("diffuse3Sampler", diffuseTexture3);
          _effect->setFloat2("diffuse3Infos", diffuseTexture3->uScale,
                             diffuseTexture3->vScale);
        }
      }

      if (StandardMaterial::BumpTextureEnabled
          && scene->getEngine()->getCaps().standardDerivatives) {
        if (bumpTexture1) {
          _effect->setTexture("bump1Sampler", bumpTexture1);
        }
        if (bumpTexture2) {
          _effect->setTexture("bump2Sampler", bumpTexture2);
        }
        if (bumpTexture3) {
          _effect->setTexture("bump3Sampler", bumpTexture3);
        }
      }
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

  if (_defines[TMD::SPECULARTERM]) {
    _effect->setColor4("vSpecularColor", specularColor, specularPower);
  }

  if (scene->lightsEnabled() && !disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _effect,
                               _defines.defines[TMD::SPECULARTERM],
                               maxSimultaneousLights);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _effect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _effect);

  Material::bind(world, mesh);
}

std::vector<IAnimatable*> TerrainMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (mixTexture && mixTexture->animations.size() > 0) {
    results.emplace_back(mixTexture);
  }

  return results;
}

void TerrainMaterial::dispose(bool forceDisposeEffect,
                              bool forceDisposeTextures)
{
  if (mixTexture) {
    mixTexture->dispose();
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* TerrainMaterial::clone(const std::string& /*name*/,
                                 bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object TerrainMaterial::serialize() const
{
  return Json::object();
}

TerrainMaterial* TerrainMaterial::Parse(const Json::value& /*source*/,
                                        Scene* /*scene*/,
                                        const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
