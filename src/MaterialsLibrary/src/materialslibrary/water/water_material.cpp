#include <babylon/materialslibrary/water/water_material.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/random.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

WaterMaterial::WaterMaterial(const std::string& iName, Scene* scene,
                             const Vector2& renderTargetSize)
    : PushMaterial{iName, scene}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.f, 0.f, 0.f)}
    , specularPower{64.f}
    , windForce{6.f}
    , windDirection{Vector2(0.f, 1.f)}
    , waveHeight{0.4f}
    , bumpHeight{0.4f}
    , waterColor{Color3(0.1f, 0.1f, 0.6f)}
    , colorBlendFactor{0.2f}
    , waterColor2{Color3(0.1f, 0.1f, 0.6f)}
    , colorBlendFactor2{0.2f}
    , waveLength{0.1f}
    , waveSpeed{1.f}
    , _bumpTexture{nullptr}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _bumpSuperimpose{false}
    , _fresnelSeparate{false}
    , _bumpAffectsReflection{false}
    , _mesh{nullptr}
    , _refractionRTT{nullptr}
    , _reflectionRTT{nullptr}
    , _material{nullptr}
    , _reflectionTransform{Matrix::Zero()}
    , _lastTime{std::chrono::microseconds(0)}
    , _renderId{-1}
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
  _markAllSubMeshesAsMiscDirty();
}

RenderTargetTexture* WaterMaterial::refractionTexture()
{
  return _refractionRTT.get();
}

RenderTargetTexture* WaterMaterial::reflectionTexture()
{
  return _reflectionRTT.get();
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

bool WaterMaterial::isReadyForSubMesh(AbstractMesh* mesh, SubMesh* subMesh,
                                      bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_unique<WaterMaterialDefines>();
  }

  auto defines
    = *(static_cast<WaterMaterialDefines*>(subMesh->_materialDefines.get()));
  auto scene = getScene();

  if (!checkReadyOnEveryCall && subMesh->effect()) {
    if (_renderId == scene->getRenderId()) {
      return true;
    }
  }

  auto engine = scene->getEngine();

  // Textures
  if (defines._areTexturesDirty) {
    defines._needUVs = false;
    if (scene->texturesEnabled()) {
      if (_bumpTexture && StandardMaterial::BumpTextureEnabled()) {
        if (!_bumpTexture->isReady()) {
          return false;
        }
        else {
          defines._needUVs           = true;
          defines.defines[WMD::BUMP] = true;
        }
      }

      if (StandardMaterial::ReflectionTextureEnabled()) {
        defines.defines[WMD::REFLECTION] = true;
      }
    }
  }

  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances, WMD::CLIPPLANE, WMD::ALPHATEST,
    WMD::INSTANCES);

  MaterialHelper::PrepareDefinesForMisc(
    mesh, scene, _useLogarithmicDepth, pointsCloud(), fogEnabled(), defines,
    WMD::LOGARITHMICDEPTH, WMD::POINTSIZE, WMD::FOG);

  if (defines._areMiscDirty) {
    if (_fresnelSeparate) {
      defines.defines[WMD::FRESNELSEPARATE] = true;
    }

    if (_bumpSuperimpose) {
      defines.defines[WMD::BUMPSUPERIMPOSE] = true;
    }

    if (_bumpAffectsReflection) {
      defines.defines[WMD::BUMPAFFECTSREFLECTION] = true;
    }
  }

  // Lights
  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, true, _maxSimultaneousLights, _disableLighting,
    WMD::SPECULARTERM, WMD::SHADOWFULLFLOAT);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(
    mesh, defines, true, true, false, WMD::NORMAL, WMD::UV1, WMD::UV2,
    WMD::VERTEXCOLOR, WMD::VERTEXALPHA);

  _mesh = mesh;

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines[WMD::FOG]) {
      fallbacks->addFallback(1, "FOG");
    }

    if (defines[WMD::LOGARITHMICDEPTH]) {
      fallbacks->addFallback(0, "LOGARITHMICDEPTH");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              _maxSimultaneousLights);

    if (defines.NUM_BONE_INFLUENCERS > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKindChars};

    if (defines[WMD::NORMAL]) {
      attribs.emplace_back(VertexBuffer::NormalKindChars);
    }

    if (defines[WMD::UV1]) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
    }

    if (defines[WMD::UV2]) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
    }

    if (defines[WMD::VERTEXCOLOR]) {
      attribs.emplace_back(VertexBuffer::ColorKindChars);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines,
                                                  WMD::INSTANCES);

    // Legacy browser patch
    const std::string shaderName{"water"};
    auto join = defines.toString();

    const std::vector<std::string> uniforms{
      "world", "view", "viewProjection", "vEyePosition", "vLightsType",
      "vDiffuseColor", "vSpecularColor", "vFogInfos", "vFogColor", "pointSize",
      "vNormalInfos", "mBones", "vClipPlane", "normalMatrix",
      "logarithmicDepthConstant",
      // Water
      "worldReflectionViewProjection", "windDirection", "waveLength", "time",
      "windForce", "cameraPosition", "bumpHeight", "waveHeight", "waterColor",
      "waterColor2", "colorBlendFactor", "colorBlendFactor2", "waveSpeed"};

    const std::vector<std::string> samplers{"normalSampler",
                                            // Water
                                            "refractionSampler",
                                            "reflectionSampler"};
    const std::vector<std::string> uniformBuffers{};

    EffectCreationOptions options;
    options.attributes            = std::move(attribs);
    options.uniformsNames         = std::move(uniforms);
    options.uniformBuffersNames   = std::move(uniformBuffers);
    options.samplers              = std::move(samplers);
    options.materialDefines       = &defines;
    options.defines               = std::move(join);
    options.maxSimultaneousLights = _maxSimultaneousLights;
    options.fallbacks             = std::move(fallbacks);
    options.onCompiled            = onCompiled;
    options.onError               = onError;
    options.indexParameters
      = {{"maxSimultaneousLights", _maxSimultaneousLights}};

    MaterialHelper::PrepareUniformsAndSamplersList(options);
    subMesh->setEffect(
      scene->getEngine()->createEffect(shaderName, options, engine), defines);
  }

  if (!subMesh->effect()->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void WaterMaterial::bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto _defines
    = static_cast<WaterMaterialDefines*>(subMesh->_materialDefines.get());
  if (!_defines) {
    return;
  }
  auto defines = *_defines;

  auto effect   = subMesh->effect();
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(*world);
  _activeEffect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  if (_mustRebind(scene, effect)) {
    // Textures
    if (_bumpTexture && StandardMaterial::BumpTextureEnabled()) {
      _activeEffect->setTexture("normalSampler", _bumpTexture);

      _activeEffect->setFloat2("vNormalInfos", _bumpTexture->coordinatesIndex,
                               _bumpTexture->level);
      _activeEffect->setMatrix("normalMatrix",
                               *_bumpTexture->getTextureMatrix());
    }
    // Clip plane
    MaterialHelper::BindClipPlane(_activeEffect, scene);

    // Point size
    if (pointsCloud()) {
      _activeEffect->setFloat("pointSize", pointSize);
    }

    _activeEffect->setVector3("vEyePosition",
                              scene->_mirroredCameraPosition ?
                                *scene->_mirroredCameraPosition :
                                scene->activeCamera->position);
  }

  _activeEffect->setColor4("vDiffuseColor", diffuseColor,
                           alpha * mesh->visibility);

  if (defines[WMD::SPECULARTERM]) {
    _activeEffect->setColor4("vSpecularColor", specularColor, specularPower);
  }

  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, defines,
                               _maxSimultaneousLights, WMD::SPECULARTERM);
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  // Log. depth
  MaterialHelper::BindLogDepth(defines, _activeEffect, scene,
                               WMD::LOGARITHMICDEPTH);

  // Water
  if (StandardMaterial::ReflectionTextureEnabled()) {
    _activeEffect->setTexture("refractionSampler", _refractionRTT.get());
    _activeEffect->setTexture("reflectionSampler", _reflectionRTT.get());
  }

  auto wrvp = _mesh->getWorldMatrix()
                ->multiply(_reflectionTransform)
                .multiply(scene->getProjectionMatrix());
  _lastTime += scene->getEngine()->getDeltaTime();

  _activeEffect->setMatrix("worldReflectionViewProjection", wrvp);
  _activeEffect->setVector2("windDirection", windDirection);
  _activeEffect->setFloat("waveLength", waveLength);
  _activeEffect->setFloat("time", Time::fpMillisecondsDuration<float>(_lastTime)
                                    / 100000.f);
  _activeEffect->setFloat("windForce", windForce);
  _activeEffect->setFloat("waveHeight", waveHeight);
  _activeEffect->setFloat("bumpHeight", bumpHeight);
  _activeEffect->setColor4("waterColor", waterColor, 1.0);
  _activeEffect->setFloat("colorBlendFactor", colorBlendFactor);
  _activeEffect->setColor4("waterColor2", waterColor2, 1.0);
  _activeEffect->setFloat("colorBlendFactor2", colorBlendFactor2);
  _activeEffect->setFloat("waveSpeed", waveSpeed);

  _afterBind(mesh, _activeEffect);
}

void WaterMaterial::_createRenderTargets(Scene* scene,
                                         const Vector2& renderTargetSize)
{
  auto renderTargetSizeX = static_cast<int>(renderTargetSize.x);
  auto renderTargetSizeY = static_cast<int>(renderTargetSize.y);

  // Render targets
  _refractionRTT = std::make_unique<RenderTargetTexture>(
    name + "_refraction", ISize{renderTargetSizeX, renderTargetSizeY}, scene,
    false, true, Engine::TEXTURETYPE_UNSIGNED_INT, false,
    Texture::TRILINEAR_SAMPLINGMODE, true, false);
  _refractionRTT->wrapU = Texture::MIRROR_ADDRESSMODE;
  _refractionRTT->wrapV = Texture::MIRROR_ADDRESSMODE;

  _reflectionRTT = std::make_unique<RenderTargetTexture>(
    name + "_reflection", ISize{renderTargetSizeX, renderTargetSizeY}, scene,
    false, true, Engine::TEXTURETYPE_UNSIGNED_INT, false,
    Texture::TRILINEAR_SAMPLINGMODE, true, false);
  _reflectionRTT->wrapU = Texture::MIRROR_ADDRESSMODE;
  _reflectionRTT->wrapV = Texture::MIRROR_ADDRESSMODE;

  scene->customRenderTargets.emplace_back(_refractionRTT.get());
  scene->customRenderTargets.emplace_back(_reflectionRTT.get());

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

  if (_bumpTexture && _bumpTexture->animations.size() > 0) {
    results.emplace_back(_bumpTexture);
  }
  if (_reflectionRTT && _reflectionRTT->animations.size() > 0) {
    results.emplace_back(_reflectionRTT.get());
  }
  if (_refractionRTT && _refractionRTT->animations.size() > 0) {
    results.emplace_back(_refractionRTT.get());
  }

  return results;
}

void WaterMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures)
{
  if (_bumpTexture) {
    _bumpTexture->dispose();
  }

  getScene()->customRenderTargets.erase(
    std::remove(getScene()->customRenderTargets.begin(),
                getScene()->customRenderTargets.end(), _refractionRTT.get()),
    getScene()->customRenderTargets.end());

  getScene()->customRenderTargets.erase(
    std::remove(getScene()->customRenderTargets.begin(),
                getScene()->customRenderTargets.end(), _reflectionRTT.get()),
    getScene()->customRenderTargets.end());

  if (_reflectionRTT) {
    _reflectionRTT->dispose();
    _reflectionRTT.reset(nullptr);
  }
  if (_refractionRTT) {
    _refractionRTT->dispose();
    _refractionRTT.reset(nullptr);
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

Mesh* WaterMaterial::CreateDefaultMesh(const std::string& name, Scene* scene)
{
  auto mesh = Mesh::CreateGround(name, 512, 512, 32, scene, false);
  return mesh;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
