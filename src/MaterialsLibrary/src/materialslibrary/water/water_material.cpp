#include <babylon/materialslibrary/water/water_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/random.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

WaterMaterial::WaterMaterial(const std::string& iName, Scene* scene,
                             const Vector2& renderTargetSize)
    : Material{iName, scene}
    , bumpTexture{nullptr}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.f, 0.f, 0.f)}
    , specularPower{64.f}
    , disableLighting{false}
    , maxSimultaneousLights{4}
    , windForce{6.f}
    , windDirection{Vector2(0.f, 1.f)}
    , waveHeight{0.4f}
    , bumpHeight{0.4f}
    , bumpSuperimpose{false}
    , fresnelSeparate{false}
    , bumpAffectsReflection{false}
    , waterColor{Color3(0.1f, 0.1f, 0.6f)}
    , colorBlendFactor{0.2f}
    , waterColor2{Color3(0.1f, 0.1f, 0.6f)}
    , colorBlendFactor2{0.2f}
    , waveLength{0.1f}
    , waveSpeed{1.f}
    , _mesh{nullptr}
    , _refractionRTT{nullptr}
    , _reflectionRTT{nullptr}
    , _material{nullptr}
    , _reflectionTransform{Matrix::Zero()}
    , _lastTime{std::chrono::microseconds(0)}
    , _renderId{-1}
    , _cachedDefines{std_util::make_unique<WaterMaterialDefines>()}
    , _isVisible{false}
    , _clipPlane{nullptr}
    , _savedViewMatrix{Matrix::Zero()}
    , _mirrorMatrix{Matrix::Zero()}
    , _useLogarithmicDepth{false}
{
  // Create render targets
  _createRenderTargets(scene, renderTargetSize);
}

WaterMaterial::~WaterMaterial()
{
}

bool WaterMaterial::useLogarithmicDepth() const
{
  return _useLogarithmicDepth;
}

void WaterMaterial::setUseLogarithmicDepth(bool value)
{
  _useLogarithmicDepth
    = value && getScene()->getEngine()->getCaps().fragmentDepthSupported;
}

RenderTargetTexture* WaterMaterial::refractionTexture()
{
  return _refractionRTT;
}

RenderTargetTexture* WaterMaterial::reflectionTexture()
{
  return _reflectionRTT;
}

void WaterMaterial::addToRenderList(AbstractMesh* node)
{
  _refractionRTT->renderList.emplace_back(node);
  _reflectionRTT->renderList.emplace_back(node);
}

void WaterMaterial::enableRenderTargets(bool enable)
{
  int refreshRate = enable ? 1 : 0;

  _refractionRTT->setRefreshRate(refreshRate);
  _reflectionRTT->setRefreshRate(refreshRate);
}

std::vector<AbstractMesh*>& WaterMaterial::getRenderList()
{
  return _refractionRTT->renderList;
}

bool WaterMaterial::renderTargetsEnabled() const
{
  return !(_refractionRTT->refreshRate() == 0);
}

bool WaterMaterial::needAlphaBlending()
{
  return (alpha < 1.f);
}

bool WaterMaterial::needAlphaTesting()
{
  return false;
}

BaseTexture* WaterMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool WaterMaterial::_checkCache(Scene* /*scene*/, AbstractMesh* mesh,
                                bool useInstances)
{
  if (!mesh) {
    return true;
  }

  if (_defines[WMD::INSTANCES] != useInstances) {
    return false;
  }

  if (mesh->_materialDefines && mesh->_materialDefines->isEqual(_defines)) {
    return true;
  }

  return false;
}

bool WaterMaterial::isReady(AbstractMesh* mesh, bool useInstances)
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
    if (bumpTexture && StandardMaterial::BumpTextureEnabled) {
      if (!bumpTexture->isReady()) {
        return false;
      }
      else {
        needUVs                     = true;
        _defines.defines[WMD::BUMP] = true;
      }
    }

    if (StandardMaterial::ReflectionTextureEnabled) {
      _defines.defines[WMD::REFLECTION] = true;
    }
  }

  // Effect
  if (scene->clipPlane()) {
    _defines.defines[WMD::CLIPPLANE] = true;
  }

  if (engine->getAlphaTesting()) {
    _defines.defines[WMD::ALPHATEST] = true;
  }

  // Point size
  if (pointsCloud() || scene->forcePointsCloud()) {
    _defines.defines[WMD::POINTSIZE] = true;
  }

  if (useLogarithmicDepth()) {
    _defines.defines[WMD::LOGARITHMICDEPTH] = true;
  }

  if (fresnelSeparate) {
    _defines.defines[WMD::FRESNELSEPARATE] = true;
  }

  if (bumpSuperimpose) {
    _defines.defines[WMD::BUMPSUPERIMPOSE] = true;
  }

  if (bumpAffectsReflection) {
    _defines.defines[WMD::BUMPAFFECTSREFLECTION] = true;
  }

  // Fog
  if (scene->fogEnabled() && mesh && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE && fogEnabled) {
    _defines.defines[WMD::FOG] = true;
  }

  // Lights
  if (scene->lightsEnabled() && !disableLighting) {
    needNormals = MaterialHelper::PrepareDefinesForLights(
      scene, mesh, _defines, maxSimultaneousLights, WMD::SPECULARTERM,
      WMD::SHADOWS);
  }

  // Attribs
  if (mesh) {
    if (needNormals && mesh->isVerticesDataPresent(VertexBuffer::NormalKind)) {
      _defines.defines[WMD::NORMAL] = true;
    }
    if (needUVs) {
      if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        _defines.defines[WMD::UV1] = true;
      }
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
        _defines.defines[WMD::UV2] = true;
      }
    }
    if (mesh->useVertexColors()
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind)) {
      _defines.defines[WMD::VERTEXCOLOR] = true;

      if (mesh->hasVertexAlpha()) {
        _defines.defines[WMD::VERTEXALPHA] = true;
      }
    }

    if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
      _defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers();
      _defines.BonesPerMesh         = mesh->skeleton()->bones.size() + 1;
    }

    // Instances
    if (useInstances) {
      _defines.defines[WMD::INSTANCES] = true;
    }
  }

  _mesh = mesh;

  // Get correct effect
  if (!_defines.isEqual(*_cachedDefines)) {
    _defines.cloneTo(*_cachedDefines);

    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std_util::make_unique<EffectFallbacks>();
    if (_defines[WMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    if (_defines[WMD::LOGARITHMICDEPTH]) {
      fallbacks->addFallback(0, "LOGARITHMICDEPTH");
    }

    MaterialHelper::HandleFallbacksForShadows(_defines, fallbacks.get(),
                                              maxSimultaneousLights);

    if (_defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKindChars};

    if (_defines[WMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (_defines[WMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (_defines[WMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (_defines[WMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(
      attribs, mesh, _defines.NUM_BONE_INFLUENCERS, fallbacks.get());
    MaterialHelper::PrepareAttributesForInstances(attribs, _defines,
                                                  WMD::INSTANCES);

    // Legacy browser patch
    std::string shaderName = "water";
    std::string join       = _defines.toString();
    std::vector<std::string> uniforms{
      "world", "view", "viewProjection", "vEyePosition", "vLightsType",
      "vDiffuseColor", "vSpecularColor", "vFogInfos", "vFogColor", "pointSize",
      "vNormalInfos", "mBones", "vClipPlane", "normalMatrix",
      "logarithmicDepthConstant",
      // Water
      "worldReflectionViewProjection", "windDirection", "waveLength", "time",
      "windForce", "cameraPosition", "bumpHeight", "waveHeight", "waterColor",
      "waterColor2", "colorBlendFactor", "colorBlendFactor2", "waveSpeed"};
    std::vector<std::string> samplers{"normalSampler",
                                      // Water
                                      "refractionSampler", "reflectionSampler"};

    MaterialHelper::PrepareUniformsAndSamplersList(uniforms, samplers, _defines,
                                                   maxSimultaneousLights);

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
      mesh->_materialDefines = std_util::make_unique<WaterMaterialDefines>();
    }

    _defines.cloneTo(*mesh->_materialDefines);
  }

  return true;
}

void WaterMaterial::bindOnlyWorldMatrix(Matrix& world)
{
  _effect->setMatrix("world", world);
}

void WaterMaterial::bind(Matrix* world, Mesh* mesh)
{
  auto scene = getScene();

  // Matrices
  bindOnlyWorldMatrix(*world);
  _effect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _effect);

  if (scene->getCachedMaterial() != this) {
    // Textures
    if (bumpTexture && StandardMaterial::BumpTextureEnabled) {
      _effect->setTexture("normalSampler", bumpTexture);

      _effect->setFloat2("vNormalInfos",
                         static_cast<float>(bumpTexture->coordinatesIndex),
                         bumpTexture->level);
      _effect->setMatrix("normalMatrix", *bumpTexture->getTextureMatrix());
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

  if (_defines[WMD::SPECULARTERM]) {
    _effect->setColor4("vSpecularColor", specularColor, specularPower);
  }

  if (scene->lightsEnabled() && !disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _effect,
                               _defines.defines[WMD::SPECULARTERM],
                               maxSimultaneousLights);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _effect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _effect);

  // Log. depth
  MaterialHelper::BindLogDepth(_defines[WMD::LOGARITHMICDEPTH], _effect, scene);

  // Water
  if (StandardMaterial::ReflectionTextureEnabled) {
    _effect->setTexture("refractionSampler", _refractionRTT);
    _effect->setTexture("reflectionSampler", _reflectionRTT);
  }

  auto wrvp = _mesh->getWorldMatrix()
                ->multiply(_reflectionTransform)
                .multiply(scene->getProjectionMatrix());
  _lastTime += scene->getEngine()->getDeltaTime();

  _effect->setMatrix("worldReflectionViewProjection", wrvp);
  _effect->setVector2("windDirection", windDirection);
  _effect->setFloat("waveLength", waveLength);
  _effect->setFloat("time",
                    Time::fpMillisecondsDuration<float>(_lastTime) / 100000.f);
  _effect->setFloat("windForce", windForce);
  _effect->setFloat("waveHeight", waveHeight);
  _effect->setFloat("bumpHeight", bumpHeight);
  _effect->setColor4("waterColor", waterColor, 1.f);
  _effect->setFloat("colorBlendFactor", colorBlendFactor);
  _effect->setColor4("waterColor2", waterColor2, 1.0);
  _effect->setFloat("colorBlendFactor2", colorBlendFactor2);
  _effect->setFloat("waveSpeed", waveSpeed);

  Material::bind(world, mesh);
}

void WaterMaterial::_createRenderTargets(Scene* scene,
                                         const Vector2& renderTargetSize)
{
  auto renderTargetSizeX = static_cast<int>(renderTargetSize.x);
  auto renderTargetSizeY = static_cast<int>(renderTargetSize.y);

  // Render targets
  _refractionRTT = new RenderTargetTexture(
    name + "_refraction", {renderTargetSizeX, renderTargetSizeY}, scene, false,
    true);
  _refractionRTT->wrapU = Texture::MIRROR_ADDRESSMODE;
  _refractionRTT->wrapV = Texture::MIRROR_ADDRESSMODE;

  _reflectionRTT = new RenderTargetTexture(
    name + "_reflection", {renderTargetSizeX, renderTargetSizeY}, scene, false,
    true);
  _reflectionRTT->wrapU = Texture::MIRROR_ADDRESSMODE;
  _reflectionRTT->wrapV = Texture::MIRROR_ADDRESSMODE;

  scene->customRenderTargets.emplace_back(_refractionRTT);
  scene->customRenderTargets.emplace_back(_reflectionRTT);

  _refractionRTT->onBeforeRender = [this, &scene]() {
    if (_mesh) {
      _isVisible       = _mesh->isVisible;
      _mesh->isVisible = false;
    }
    // Clip plane
    _clipPlane = scene->clipPlane();

    float positiony = _mesh ? _mesh->position().y : 0.f;
    Vector3 normal(0.f, 1.f, 0.f);
    scene->setClipPlane(Plane::FromPositionAndNormal(
      Vector3(0.f, positiony + 0.05f, 0.f), normal));
  };

  _refractionRTT->onAfterRender = [this, &scene]() {
    if (_mesh) {
      _mesh->isVisible = _isVisible;
    }

    // Clip plane
    scene->setClipPlane(*_clipPlane);
  };

  _reflectionRTT->onBeforeRender = [this, &scene]() {
    if (_mesh) {
      _isVisible       = _mesh->isVisible;
      _mesh->isVisible = false;
    }

    // Clip plane
    _clipPlane = scene->clipPlane();

    float positiony = _mesh ? _mesh->position().y : 0.f;
    Vector3 normal(0.f, -1.f, 0.f);
    scene->setClipPlane(Plane::FromPositionAndNormal(
      Vector3(0.f, positiony - 0.05f, 0.f), normal));

    // Transform
    Matrix::ReflectionToRef(*scene->clipPlane(), _mirrorMatrix);
    _savedViewMatrix = scene->getViewMatrix();

    _mirrorMatrix.multiplyToRef(_savedViewMatrix, _reflectionTransform);
    Matrix projectionMatrix = scene->getProjectionMatrix();
    scene->setTransformMatrix(_reflectionTransform, projectionMatrix);
    scene->getEngine()->cullBackFaces = false;
    scene->setMirroredCameraPosition(Vector3::TransformCoordinates(
      scene->activeCamera->position, _mirrorMatrix));
  };

  _reflectionRTT->onAfterRender = [this, &scene]() {
    if (_mesh) {
      _mesh->isVisible = _isVisible;
    }

    // Clip plane
    scene->setClipPlane(*_clipPlane);

    // Transform
    auto projectionMatrix = scene->getProjectionMatrix();
    scene->setTransformMatrix(_savedViewMatrix, projectionMatrix);
    scene->getEngine()->cullBackFaces = true;
    scene->_mirroredCameraPosition    = nullptr;
  };
}

std::vector<IAnimatable*> WaterMaterial::getAnimatables()
{
  std::vector<IAnimatable*> results;

  if (bumpTexture && bumpTexture->animations.size() > 0) {
    results.emplace_back(bumpTexture);
  }
  if (_reflectionRTT && _reflectionRTT->animations.size() > 0) {
    results.emplace_back(_reflectionRTT);
  }
  if (_refractionRTT && _refractionRTT->animations.size() > 0) {
    results.emplace_back(_refractionRTT);
  }

  return results;
}

void WaterMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (bumpTexture) {
    bumpTexture->dispose();
  }

  getScene()->customRenderTargets.erase(
    std::remove(getScene()->customRenderTargets.begin(),
                getScene()->customRenderTargets.end(), _refractionRTT),
    getScene()->customRenderTargets.end());

  getScene()->customRenderTargets.erase(
    std::remove(getScene()->customRenderTargets.begin(),
                getScene()->customRenderTargets.end(), _reflectionRTT),
    getScene()->customRenderTargets.end());

  if (_reflectionRTT) {
    _reflectionRTT->dispose();
  }
  if (_refractionRTT) {
    _refractionRTT->dispose();
  }

  Material::dispose(forceDisposeEffect, forceDisposeTextures);
}

Material* WaterMaterial::clone(const std::string& /*name*/,
                               bool /*cloneChildren*/) const
{
  return nullptr;
}

Json::object WaterMaterial::serialize() const
{
  return Json::object();
}

WaterMaterial* WaterMaterial::Parse(const Json::value& /*source*/,
                                    Scene* /*scene*/,
                                    const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
