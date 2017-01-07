#include <babylon/materialslibrary/fire/fire_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/time.h>
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

unsigned int FireMaterial::maxSimultaneousLights = 4;

FireMaterial::FireMaterial(const std::string& iName, Scene* scene)
    : Material{iName, scene}
    , diffuseTexture{nullptr}
    , distortionTexture{nullptr}
    , opacityTexture{nullptr}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , speed{1.f}
    , _renderId{-1}
    , _cachedDefines{std_util::make_unique<FireMaterialDefines>()}
    , _lastTime{std::chrono::microseconds(0)}
{
  _cachedDefines->BonesPerMesh         = 0;
  _cachedDefines->NUM_BONE_INFLUENCERS = 0;
}

FireMaterial::~FireMaterial()
{
}

bool FireMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool FireMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* FireMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool FireMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                               bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[FMD::INSTANCES] != useInstances) {
    return false;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool FireMaterial::isReady(AbstractMesh* mesh, bool useInstances)
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

  auto engine  = scene->getEngine();
  auto needUVs = false;

  _defines.reset();

  // Textures
  if (scene->texturesEnabled) {
    if (diffuseTexture && StandardMaterial::DiffuseTextureEnabled) {
      if (!diffuseTexture->isReady()) {
        return false;
      }
      else {
        needUVs                        = true;
        _defines.defines[FMD::DIFFUSE] = true;
      }
    }
  }

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[FMD::CLIPPLANE] = true;
  }

  _defines.defines[FMD::ALPHATEST] = true;

  // Point size
  if (pointsCloud() || scene->forcePointsCloud) {
    _defines.defines[FMD::POINTSIZE] = true;
  }

  // Fog
  if (scene->fogEnabled && mesh && mesh->applyFog
      && scene->fogMode != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[FMD::FOG] = true;
  }

  // Attribs
  if (mesh) {
    if (needUVs) {
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        _defines.defines[FMD::UV1] = true;
      }
    }
    if (mesh->useVertexColors
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[FMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha) {
        _defines.defines[FMD::VERTEXALPHA] = true;
      }
    }
    if (mesh->useBones() && mesh->computeBonesUsingShaders) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers;
      _defines.BonesPerMesh         = mesh->skeleton()->bones.size() + 1;
    }

    // Instances
    if (useInstances) {
      _defines.defines[FMD::INSTANCES] = true;
    }
  }

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (_defines[FMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    if (_defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs{VertexBuffer::PositionKindChars};

    if (_defines[FMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (_defines[FMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  FMD::INSTANCES);

    // Legacy browser patch
    std::string shaderName = "fire";

    std::string join = _defines.toString();
    std::vector<std::string> uniforms{"world", "view", "viewProjection",
                                      "vEyePosition", "vFogInfos", "vFogColor",
                                      "pointSize", "vDiffuseInfos", "mBones",
                                      "vClipPlane", "diffuseMatrix",
                                      // Fire
                                      "time", "speed"};
    std::vector<std::string> samplers{"diffuseSampler",
                                      // Fire
                                      "distortionSampler", "opacitySampler"};

    _effect = engine->createEffect(shaderName, attribs, uniforms, samplers,
                                   join, fallbacks.get(), onCompiled, onError);
  }

  if (!_effect->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  if (mesh) {
    if (!mesh->_materialDefines) {
      mesh->_materialDefines = std_util::make_unique<FireMaterialDefines>();
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void FireMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void FireMaterial::bind(Matrix* world, Mesh* mesh)
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

      _effect->setTexture("distortionSampler", distortionTexture);
      _effect->setTexture("opacitySampler", opacityTexture);
    }

    // Clip plane
    if (scene->clipPlane()) {
      auto clipPlane = scene->clipPlane();
      _effect->setFloat4("vClipPlane", clipPlane->normal.x, clipPlane->normal.y,
                         clipPlane->normal.z, clipPlane->d);
    }

    // Point size
    if (pointsCloud()) {
      _effect->setFloat("pointSize", pointSize);
    }

    _effect->setVector3("vEyePosition", scene->_mirroredCameraPosition ?
                                          *scene->_mirroredCameraPosition :
                                          scene->activeCamera->position);
  }

  _effect->setColor4("vDiffuseColor", _scaledDiffuse, alpha * mesh->visibility);

  // View
  if (scene->fogEnabled && mesh->applyFog
      && scene->fogMode != Scene::FOGMODE_NONE) {
    _effect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _effect);

  // Time
  _lastTime += scene->getEngine()->getDeltaTime();
  _effect->setFloat("time", Time::fpMillisecondsDuration<float>(_lastTime));

  // Speed
  _effect->setFloat("speed", speed);

  Material::bind(world, mesh);
}

std::vector<IAnimatable*> FireMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (diffuseTexture && diffuseTexture->animations.size() > 0) {
    results.emplace_back(diffuseTexture);
  }

  if (distortionTexture && distortionTexture->animations.size() > 0) {
    results.emplace_back(distortionTexture);
  }

  if (opacityTexture && opacityTexture->animations.size() > 0) {
    results.emplace_back(opacityTexture);
  }

  return results;
}

void FireMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (diffuseTexture) {
    diffuseTexture->dispose();
  }

  if (distortionTexture) {
    distortionTexture->dispose();
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* FireMaterial::clone(const std::string& /*name*/,
                              bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object FireMaterial::serialize() const
{
  return Json::object();
}

FireMaterial* FireMaterial::Parse(const Json::value& /*source*/,
                                  Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
