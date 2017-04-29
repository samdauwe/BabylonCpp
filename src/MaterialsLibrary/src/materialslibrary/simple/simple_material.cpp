#include <babylon/materialslibrary/simple/simple_material.h>

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

SimpleMaterial::SimpleMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
    , diffuseTexture{nullptr}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , disableLighting{false}
    , maxSimultaneousLights{4}
    , _worldViewProjectionMatrix{Matrix::Zero()}
    , _renderId{-1}
    , _cachedDefines{std_util::make_unique<SimpleMaterialDefines>()}
{
  _cachedDefines->BonesPerMesh         = 0;
  _cachedDefines->NUM_BONE_INFLUENCERS = 0;
}

SimpleMaterial::~SimpleMaterial()
{
}

bool SimpleMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool SimpleMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* SimpleMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool SimpleMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                                 bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[SMD::INSTANCES] != useInstances) {
    return false;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool SimpleMaterial::isReady(AbstractMesh* mesh, bool useInstances)
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
  if (scene->texturesEnabled()) {
    if (diffuseTexture && StandardMaterial::DiffuseTextureEnabled) {
      if (!diffuseTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[SMD::DIFFUSE] = true;
      }
    }
  }

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[SMD::CLIPPLANE] = true;
  }

  if (engine->getAlphaTesting()) {
    _defines.defines[SMD::ALPHATEST] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud()) {
    _defines.defines[SMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[SMD::FOG] = true;
  }

  if (scene->lightsEnabled() && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, maxSimultaneousLights, SMD::SPECULARTERM,
      SMD::SHADOWS);
  }

  // Attribs
  if (mesh) {
    if (needNormals && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      _defines.defines[SMD::NORMAL] = true;
    }
    if (needUVs) {
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        _defines.defines[SMD::UV1] = true;
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        _defines.defines[SMD::UV2] = true;
      }
    }
    if (mesh->useVertexColors()
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[SMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha()) {
        _defines.defines[SMD::VERTEXALPHA] = true;
      }
    }
    if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers();
      _defines.BonesPerMesh         = mesh->skeleton()->bones.size() + 1;
    }

    // Instances
    if (useInstances) {
      _defines.defines[SMD::INSTANCES] = true;
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (_defines[SMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    MaterialHelper::HandleFallbacksForShadows(_defines, fallbacks.get(),
                                              maxSimultaneousLights);

    if (_defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKindChars};

    if (_defines[SMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (_defines[SMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (_defines[SMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (_defines[SMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  SMD::INSTANCES);

    std::string shaderName = "simple";
    std::string join       = _defines.toString();
    std::vector<std::string> uniforms{
      "world",         "view",          "viewProjection", "vEyePosition",
      "vLightsType",   "vDiffuseColor", "vFogInfos",      "vFogColor",
      "pointSize",     "vDiffuseInfos", "mBones",         "vClipPlane",
      "diffuseMatrix", "depthValues"};
    std::vector<std::string> samplers{"diffuseSampler"};
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
      mesh->_materialDefines = std_util::make_unique<SimpleMaterialDefines>();
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void SimpleMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void SimpleMaterial::bind(Matrix* world, Mesh* mesh)
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
  if (scene->lightsEnabled() && !disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _effect,
                               _defines.defines[SMD::SPECULARTERM],
                               maxSimultaneousLights);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _effect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _effect);

  Material::bind(world, mesh);
}

std::vector<IAnimatable*> SimpleMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (diffuseTexture && diffuseTexture->animations.size() > 0) {
    results.emplace_back(diffuseTexture);
  }

  return results;
}

void SimpleMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (diffuseTexture) {
    diffuseTexture->dispose();
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* SimpleMaterial::clone(const std::string& /*name*/,
                                bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object SimpleMaterial::serialize() const
{
  return Json::object();
}

SimpleMaterial* SimpleMaterial::Parse(const Json::value& /*source*/,
                                      Scene* /*scene*/,
                                      const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
