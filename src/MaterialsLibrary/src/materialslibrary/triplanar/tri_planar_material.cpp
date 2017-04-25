#include <babylon/materialslibrary/triplanar/tri_planar_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

TriPlanarMaterial::TriPlanarMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
    , mixTexture{nullptr}
    , diffuseTextureX{nullptr}
    , diffuseTextureY{nullptr}
    , diffuseTextureZ{nullptr}
    , normalTextureX{nullptr}
    , normalTextureY{nullptr}
    , normalTextureZ{nullptr}
    , tileSize{1.f}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.2f, 0.2f, 0.2f)}
    , specularPower{64.f}
    , disableLighting{false}
    , maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
    , _cachedDefines{std_util::make_unique<TriPlanarMaterialDefines>()}
{
  _cachedDefines->BonesPerMesh         = 0;
  _cachedDefines->NUM_BONE_INFLUENCERS = 0;
}

TriPlanarMaterial::~TriPlanarMaterial()
{
}

bool TriPlanarMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool TriPlanarMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* TriPlanarMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool TriPlanarMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                                    bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[TPMD::INSTANCES] != useInstances) {
    return false;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool TriPlanarMaterial::isReady(AbstractMesh* mesh, bool useInstances)
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

  _defines.reset();

  // Textures
  if (scene->texturesEnabled()) {
    if (StandardMaterial::DiffuseTextureEnabled) {
      std::vector<Texture*> textures{diffuseTextureX, diffuseTextureY,
                                     diffuseTextureZ};
      Uint32Array textureDefines{TPMD::DIFFUSEX, TPMD::DIFFUSEY,
                                 TPMD::DIFFUSEZ};

      for (unsigned int i = 0; i < textures.size(); ++i) {
        if (textures[i]) {
          if (!textures[i]->isReady()) {
            return false;
          }
          else {
            _defines.defines[textureDefines[i]] = true;
          }
        }
      }
    }
    if (StandardMaterial::BumpTextureEnabled) {
      std::vector<Texture*> textures{normalTextureX, normalTextureY,
                                     normalTextureZ};
      Uint32Array textureDefines{TPMD::BUMPX, TPMD::BUMPY, TPMD::BUMPZ};

      for (unsigned int i = 0; i < textures.size(); ++i) {
        if (textures[i]) {
          if (!textures[i]->isReady()) {
            return false;
          }
          else {
            _defines.defines[textureDefines[i]] = true;
          }
        }
      }
    }
  }

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[TPMD::CLIPPLANE] = true;
  }

  if (engine->getAlphaTesting()) {
    _defines.defines[TPMD::ALPHATEST] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud()) {
    _defines.defines[TPMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog
      && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[TPMD::FOG] = true;
  }

  // Lights
  if (scene->lightsEnabled() && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, maxSimultaneousLights, TPMD::SPECULARTERM,
      TPMD::SHADOWS);
  }

  // Attribs
  if (mesh) {
    if (needNormals && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      _defines.defines[TPMD::NORMAL] = true;
    }
    if (mesh->useVertexColors
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[TPMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha) {
        _defines.defines[TPMD::VERTEXALPHA] = true;
      }
    }

    if (mesh->useBones() && mesh->computeBonesUsingShaders) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers;
      _defines.BonesPerMesh         = mesh->skeleton()->bones.size() + 1;
    }

    // Instances
    if (useInstances) {
      _defines.defines[TPMD::INSTANCES] = true;
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (_defines[TPMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(_defines, fallbacks.get(),
                                              maxSimultaneousLights);

    if (_defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKindChars};

    if (_defines[TPMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (_defines[TPMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  TPMD::INSTANCES);

    // Legacy browser patch
    std::string shaderName = "triplanar";
    std::string join       = _defines.toString();
    std::vector<std::string> uniforms{
      "world",       "view",          "viewProjection", "vEyePosition",
      "vLightsType", "vDiffuseColor", "vSpecularColor", "vFogInfos",
      "vFogColor",   "pointSize",     "mBones",         "vClipPlane",
      "tileSize"};
    std::vector<std::string> samplers{"diffuseSamplerX", "diffuseSamplerY",
                                      "diffuseSamplerZ", "normalSamplerX",
                                      "normalSamplerY",  "normalSamplerZ"};
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
      mesh->_materialDefines
        = std_util::make_unique<TriPlanarMaterialDefines>();
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void TriPlanarMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void TriPlanarMaterial::bind(Matrix* world, Mesh* mesh)
{
  auto scene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);
  _effect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  _effect->setFloat("tileSize", tileSize);

  if (scene->getCachedMaterial() != this) {
    // Textures
    if (diffuseTextureX) {
      _effect->setTexture("diffuseSamplerX", diffuseTextureX);
    }
    if (diffuseTextureY) {
      _effect->setTexture("diffuseSamplerY", diffuseTextureY);
    }
    if (diffuseTextureZ) {
      _effect->setTexture("diffuseSamplerZ", diffuseTextureZ);
    }
    if (normalTextureX) {
      _effect->setTexture("normalSamplerX", normalTextureX);
    }
    if (normalTextureY) {
      _effect->setTexture("normalSamplerY", normalTextureY);
    }
    if (normalTextureZ) {
      _effect->setTexture("normalSamplerZ", normalTextureZ);
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

  if (_defines[TPMD::SPECULARTERM]) {
    _effect->setColor4("vSpecularColor", specularColor, specularPower);
  }

  if (scene->lightsEnabled() && !disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _effect,
                               _defines.defines[TPMD::SPECULARTERM],
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

std::vector<IAnimatable*> TriPlanarMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (mixTexture && mixTexture->animations.size() > 0) {
    results.emplace_back(mixTexture);
  }

  return results;
}

void TriPlanarMaterial::dispose(bool forceDisposeEffect,
                                bool forceDisposeTextures)
{

  if (mixTexture) {
    mixTexture->dispose();
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* TriPlanarMaterial::clone(const std::string& /*name*/,
                                   bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object TriPlanarMaterial::serialize() const
{
  return Json::object();
}

TriPlanarMaterial* TriPlanarMaterial::Parse(const Json::value& /*source*/,
                                            Scene* /*scene*/,
                                            const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
