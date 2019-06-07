#include <babylon/materialslibrary/water/water_material.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materialslibrary/water/water_fragment_fx.h>
#include <babylon/materialslibrary/water/water_vertex_fx.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace MaterialsLibrary {

WaterMaterial::WaterMaterial(const std::string& iName, Scene* scene,
                             const Vector2& renderTargetSize)
    : PushMaterial{iName, scene}
    , bumpTexture{this, &WaterMaterial::get_bumpTexture,
                  &WaterMaterial::set_bumpTexture}
    , diffuseColor{Color3(1.f, 1.f, 1.f)}
    , specularColor{Color3(0.f, 0.f, 0.f)}
    , specularPower{64.f}
    , disableLighting{this, &WaterMaterial::get_disableLighting,
                      &WaterMaterial::set_disableLighting}
    , maxSimultaneousLights{this, &WaterMaterial::get_maxSimultaneousLights,
                            &WaterMaterial::set_maxSimultaneousLights}
    , windForce{6.f}
    , windDirection{Vector2(0.f, 1.f)}
    , waveHeight{0.4f}
    , bumpHeight{0.4f}
    , bumpSuperimpose{this, &WaterMaterial::get_bumpSuperimpose,
                      &WaterMaterial::set_bumpSuperimpose}
    , fresnelSeparate{this, &WaterMaterial::get_fresnelSeparate,
                      &WaterMaterial::set_fresnelSeparate}
    , bumpAffectsReflection{this, &WaterMaterial::get_bumpAffectsReflection,
                            &WaterMaterial::set_bumpAffectsReflection}
    , waterColor{Color3(0.1f, 0.1f, 0.6f)}
    , colorBlendFactor{0.2f}
    , waterColor2{Color3(0.1f, 0.1f, 0.6f)}
    , colorBlendFactor2{0.2f}
    , waveLength{0.1f}
    , waveSpeed{1.f}
    , refractionTexture{this, &WaterMaterial::get_refractionTexture}
    , reflectionTexture{this, &WaterMaterial::get_reflectionTexture}
    , renderTargetsEnabled{this, &WaterMaterial::get_renderTargetsEnabled}
    , _bumpTexture{nullptr}
    , _disableLighting{false}
    , _maxSimultaneousLights{4}
    , _bumpSuperimpose{false}
    , _fresnelSeparate{false}
    , _bumpAffectsReflection{false}
    , _mesh{nullptr}
    , _refractionRTT{nullptr}
    , _reflectionRTT{nullptr}
    , _reflectionTransform{Matrix::Zero()}
    , _lastTime{0.f}
    , _lastDeltaTime{0.f}
    , _renderId{-1}
    , _isVisible{false}
    , _clipPlane{std::nullopt}
    , _savedViewMatrix{Matrix::Zero()}
    , _mirrorMatrix{Matrix::Zero()}
    , _useLogarithmicDepth{false}
{
  // Vertex shader
  Effect::ShadersStore()["waterVertexShader"] = waterVertexShader;

  // Fragment shader
  Effect::ShadersStore()["waterPixelShader"] = waterPixelShader;

  // Create render targets
  _createRenderTargets(scene, renderTargetSize);

  getRenderTargetTextures = [this]() -> std::vector<RenderTargetTexturePtr> {
    _renderTargets.clear();
    _renderTargets.emplace_back(_reflectionRTT);
    _renderTargets.emplace_back(_refractionRTT);

    return _renderTargets;
  };
}

WaterMaterial::~WaterMaterial()
{
}

BaseTexturePtr& WaterMaterial::get_bumpTexture()
{
  return _bumpTexture;
}

void WaterMaterial::set_bumpTexture(const BaseTexturePtr& value)
{
  if (_bumpTexture != value) {
    _bumpTexture = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool WaterMaterial::get_disableLighting() const
{
  return _disableLighting;
}

void WaterMaterial::set_disableLighting(bool value)
{
  if (_disableLighting != value) {
    _disableLighting = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

unsigned int WaterMaterial::get_maxSimultaneousLights() const
{
  return _maxSimultaneousLights;
}

void WaterMaterial::set_maxSimultaneousLights(unsigned int value)
{
  if (_maxSimultaneousLights != value) {
    _maxSimultaneousLights = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool WaterMaterial::get_bumpSuperimpose() const
{
  return _bumpSuperimpose;
}

void WaterMaterial::set_bumpSuperimpose(bool value)
{
  if (_bumpSuperimpose != value) {
    _bumpSuperimpose = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool WaterMaterial::get_fresnelSeparate() const
{
  return _fresnelSeparate;
}

void WaterMaterial::set_fresnelSeparate(bool value)
{
  if (_fresnelSeparate != value) {
    _fresnelSeparate = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool WaterMaterial::get_bumpAffectsReflection() const
{
  return _bumpAffectsReflection;
}

void WaterMaterial::set_bumpAffectsReflection(bool value)
{
  if (_bumpAffectsReflection != value) {
    _bumpAffectsReflection = value;
    _markAllSubMeshesAsLightsDirty();
  }
}

bool WaterMaterial::get_useLogarithmicDepth() const
{
  return _useLogarithmicDepth;
}

void WaterMaterial::set_useLogarithmicDepth(bool value)
{
  _useLogarithmicDepth
    = value && getScene()->getEngine()->getCaps().fragmentDepthSupported;
  _markAllSubMeshesAsMiscDirty();
}

bool WaterMaterial::get_hasRenderTargetTextures() const
{
  return true;
}

RenderTargetTexturePtr& WaterMaterial::get_refractionTexture()
{
  return _refractionRTT;
}

RenderTargetTexturePtr& WaterMaterial::get_reflectionTexture()
{
  return _reflectionRTT;
}

void WaterMaterial::addToRenderList(const AbstractMeshPtr& node)
{
  if (_refractionRTT) {
    _refractionRTT->renderList().emplace_back(node.get());
  }

  if (_reflectionRTT) {
    _reflectionRTT->renderList().emplace_back(node.get());
  }
}

void WaterMaterial::enableRenderTargets(bool enable)
{
  int refreshRate = enable ? 1 : 0;

  if (_refractionRTT) {
    _refractionRTT->refreshRate = refreshRate;
  }

  if (_reflectionRTT) {
    _reflectionRTT->refreshRate = refreshRate;
  }
}

std::vector<AbstractMesh*>& WaterMaterial::getRenderList()
{
  return _refractionRTT->renderList();
}

bool WaterMaterial::get_renderTargetsEnabled() const
{
  return !(_refractionRTT && _refractionRTT->refreshRate() == 0);
}

bool WaterMaterial::needAlphaBlending() const
{
  return (alpha < 1.f);
}

bool WaterMaterial::needAlphaTesting() const
{
  return false;
}

BaseTexturePtr WaterMaterial::getAlphaTestTexture()
{
  return nullptr;
}

bool WaterMaterial::isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                                      bool useInstances)
{
  if (isFrozen()) {
    if (_wasPreviouslyReady && subMesh->effect()) {
      return true;
    }
  }

  if (!subMesh->_materialDefines) {
    subMesh->_materialDefines = std::make_shared<WaterMaterialDefines>();
  }

  auto definesPtr
    = std::static_pointer_cast<WaterMaterialDefines>(subMesh->_materialDefines);
  auto& defines = *definesPtr.get();
  auto scene    = getScene();

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
          defines._needUVs        = true;
          defines.boolDef["BUMP"] = true;
        }
      }

      if (StandardMaterial::ReflectionTextureEnabled()) {
        defines.boolDef["REFLECTION"] = true;
      }
    }
  }

  MaterialHelper::PrepareDefinesForFrameBoundValues(
    scene, engine, defines, useInstances ? true : false);

  MaterialHelper::PrepareDefinesForMisc(mesh, scene, _useLogarithmicDepth,
                                        pointsCloud(), fogEnabled(),
                                        _shouldTurnAlphaTestOn(mesh), defines);

  if (defines._areMiscDirty) {
    if (_fresnelSeparate) {
      defines.boolDef["FRESNELSEPARATE"] = true;
    }

    if (_bumpSuperimpose) {
      defines.boolDef["BUMPSUPERIMPOSE"] = true;
    }

    if (_bumpAffectsReflection) {
      defines.boolDef["BUMPAFFECTSREFLECTION"] = true;
    }
  }

  // Lights
  defines._needNormals = MaterialHelper::PrepareDefinesForLights(
    scene, mesh, defines, true, _maxSimultaneousLights, _disableLighting);

  // Attribs
  MaterialHelper::PrepareDefinesForAttributes(mesh, defines, true, true);

  // Configure this
  _mesh = mesh;

  // Get correct effect
  if (defines.isDirty()) {
    defines.markAsProcessed();
    scene->resetCachedMaterial();

    // Fallbacks
    auto fallbacks = std::make_unique<EffectFallbacks>();
    if (defines["FOG"]) {
      fallbacks->addFallback(1, "FOG");
    }

    if (defines["LOGARITHMICDEPTH"]) {
      fallbacks->addFallback(0, "LOGARITHMICDEPTH");
    }

    MaterialHelper::HandleFallbacksForShadows(defines, *fallbacks,
                                              maxSimultaneousLights());

    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh);
    }

    // Attributes
    std::vector<std::string> attribs = {VertexBuffer::PositionKind};

    if (defines["NORMAL"]) {
      attribs.emplace_back(VertexBuffer::NormalKind);
    }

    if (defines["UV1"]) {
      attribs.emplace_back(VertexBuffer::UVKind);
    }

    if (defines["UV2"]) {
      attribs.emplace_back(VertexBuffer::UV2Kind);
    }

    if (defines["VERTEXCOLOR"]) {
      attribs.emplace_back(VertexBuffer::ColorKind);
    }

    MaterialHelper::PrepareAttributesForBones(attribs, mesh, defines,
                                              *fallbacks);
    MaterialHelper::PrepareAttributesForInstances(attribs, defines);

    // Legacy browser patch
    const std::string shaderName{"water"};
    auto join = defines.toString();

    const std::vector<std::string> uniforms{
      "world", "view", "viewProjection", "vEyePosition", "vLightsType",
      "vDiffuseColor", "vSpecularColor", "vFogInfos", "vFogColor", "pointSize",
      "vNormalInfos", "mBones", "vClipPlane", "vClipPlane2", "vClipPlane3",
      "vClipPlane4", "normalMatrix", "logarithmicDepthConstant",
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
      scene->getEngine()->createEffect(shaderName, options, engine),
      definesPtr);
  }

  if (!subMesh->effect() || !subMesh->effect()->isReady()) {
    return false;
  }

  _renderId           = scene->getRenderId();
  _wasPreviouslyReady = true;

  return true;
}

void WaterMaterial::bindForSubMesh(Matrix& world, Mesh* mesh, SubMesh* subMesh)
{
  auto scene = getScene();

  auto _defines
    = static_cast<WaterMaterialDefines*>(subMesh->_materialDefines.get());
  if (!_defines) {
    return;
  }
  auto defines = *_defines;

  auto effect = subMesh->effect();
  if (!effect || !_mesh) {
    return;
  }
  _activeEffect = effect;

  // Matrices
  bindOnlyWorldMatrix(world);
  _activeEffect->setMatrix("viewProjection", scene->getTransformMatrix());

  // Bones
  MaterialHelper::BindBonesParameters(mesh, _activeEffect);

  if (_mustRebind(scene, effect)) {
    // Textures
    if (_bumpTexture && StandardMaterial::BumpTextureEnabled()) {
      _activeEffect->setTexture("normalSampler", _bumpTexture);

      _activeEffect->setFloat2(
        "vNormalInfos", static_cast<float>(_bumpTexture->coordinatesIndex),
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

    MaterialHelper::BindEyePosition(effect, scene);
  }

  _activeEffect->setColor4("vDiffuseColor", diffuseColor,
                           alpha * mesh->visibility);

  if (defines["SPECULARTERM"]) {
    _activeEffect->setColor4("vSpecularColor", specularColor, specularPower);
  }

  if (scene->lightsEnabled() && !_disableLighting) {
    MaterialHelper::BindLights(scene, mesh, _activeEffect, defines,
                               maxSimultaneousLights());
  }

  // View
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    _activeEffect->setMatrix("view", scene->getViewMatrix());
  }

  // Fog
  MaterialHelper::BindFogParameters(scene, mesh, _activeEffect);

  // Log. depth
  MaterialHelper::BindLogDepth(defines, _activeEffect, scene);

  // Water
  if (StandardMaterial::ReflectionTextureEnabled()) {
    _activeEffect->setTexture("refractionSampler", _refractionRTT);
    _activeEffect->setTexture("reflectionSampler", _reflectionRTT);
  }

  auto wrvp = _mesh->getWorldMatrix()
                .multiply(_reflectionTransform)
                .multiply(scene->getProjectionMatrix());

  // Add delta time. Prevent adding delta time if it hasn't changed.
  auto deltaTime = scene->getEngine()->getDeltaTime();
  if (!stl_util::almost_equal(deltaTime, _lastDeltaTime)) {
    _lastDeltaTime = deltaTime;
    _lastTime += _lastDeltaTime;
  }

  _activeEffect->setMatrix("worldReflectionViewProjection", wrvp);
  _activeEffect->setVector2("windDirection", windDirection);
  _activeEffect->setFloat("waveLength", waveLength);
  _activeEffect->setFloat("time", _lastTime / 100000.f);
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
  _refractionRTT = RenderTargetTexture::New(
    name + "_refraction", ISize{renderTargetSizeX, renderTargetSizeY}, scene,
    false, true, EngineConstants::TEXTURETYPE_UNSIGNED_INT, false,
    TextureConstants::TRILINEAR_SAMPLINGMODE, true, false);
  _refractionRTT->wrapU                = TextureConstants::MIRROR_ADDRESSMODE;
  _refractionRTT->wrapV                = TextureConstants::MIRROR_ADDRESSMODE;
  _refractionRTT->ignoreCameraViewport = true;

  _reflectionRTT = RenderTargetTexture::New(
    name + "_reflection", ISize{renderTargetSizeX, renderTargetSizeY}, scene,
    false, true, EngineConstants::TEXTURETYPE_UNSIGNED_INT, false,
    TextureConstants::TRILINEAR_SAMPLINGMODE, true, false);
  _reflectionRTT->wrapU                = TextureConstants::MIRROR_ADDRESSMODE;
  _reflectionRTT->wrapV                = TextureConstants::MIRROR_ADDRESSMODE;
  _reflectionRTT->ignoreCameraViewport = true;

  _refractionRTT->onBeforeRender
    = [this](int* /*faceIndex*/, EventState& /*es*/) {
        if (_mesh) {
          _isVisible       = _mesh->isVisible;
          _mesh->isVisible = false;
        }
        // Clip plane
        _clipPlane = getScene()->clipPlane;

        auto positiony        = _mesh ? _mesh->position().y : 0.f;
        getScene()->clipPlane = Plane::FromPositionAndNormal(
          Vector3(0.f, positiony + 0.05f, 0.f), Vector3(0.f, 1.f, 0.f));
      };

  _refractionRTT->onAfterRender
    = [this](int* /*faceIndex*/, EventState& /*es*/) {
        if (_mesh) {
          _mesh->isVisible = _isVisible;
        }

        // Clip plane
        getScene()->clipPlane = _clipPlane;
      };

  _reflectionRTT->onBeforeRender
    = [this](int* /*faceIndex*/, EventState& /*es*/) {
        if (_mesh) {
          _isVisible       = _mesh->isVisible;
          _mesh->isVisible = false;
        }

        auto scene = getScene();

        // Clip plane
        _clipPlane = scene->clipPlane;

        float positiony  = _mesh ? _mesh->position().y : 0.f;
        scene->clipPlane = Plane::FromPositionAndNormal(
          Vector3(0.f, positiony - 0.05f, 0.f), Vector3(0.f, -1.f, 0.f));

        // Transform
        Matrix::ReflectionToRef(*scene->clipPlane, _mirrorMatrix);
        _savedViewMatrix = scene->getViewMatrix();

        _mirrorMatrix.multiplyToRef(_savedViewMatrix, _reflectionTransform);
        Matrix projectionMatrix = scene->getProjectionMatrix();
        scene->setTransformMatrix(_reflectionTransform, projectionMatrix);
        scene->getEngine()->cullBackFaces = false;
        scene->setMirroredCameraPosition(Vector3::TransformCoordinates(
          scene->activeCamera->position, _mirrorMatrix));
      };

  _reflectionRTT->onAfterRender
    = [this](int* /*faceIndex*/, EventState& /*es*/) {
        if (_mesh) {
          _mesh->isVisible = _isVisible;
        }

        auto scene = getScene();

        // Clip plane
        scene->clipPlane = _clipPlane;

        // Transform
        auto projectionMatrix = scene->getProjectionMatrix();
        scene->setTransformMatrix(_savedViewMatrix, projectionMatrix);
        scene->getEngine()->cullBackFaces = true;
        scene->_mirroredCameraPosition    = nullptr;
      };
}

std::vector<IAnimatablePtr> WaterMaterial::getAnimatables()
{
  std::vector<IAnimatablePtr> results;

  if (_bumpTexture && _bumpTexture->animations.size() > 0) {
    results.emplace_back(_bumpTexture);
  }
  if (_reflectionRTT && _reflectionRTT->animations.size() > 0) {
    results.emplace_back(_reflectionRTT);
  }
  if (_refractionRTT && _refractionRTT->animations.size() > 0) {
    results.emplace_back(_refractionRTT);
  }

  return results;
}

std::vector<BaseTexturePtr> WaterMaterial::getActiveTextures() const
{
  auto activeTextures = PushMaterial::getActiveTextures();

  if (_bumpTexture) {
    activeTextures.emplace_back(_bumpTexture);
  }

  return activeTextures;
}

bool WaterMaterial::hasTexture(const BaseTexturePtr& texture) const
{
  if (PushMaterial::hasTexture(texture)) {
    return true;
  }

  if (_bumpTexture == texture) {
    return true;
  }

  return false;
}

void WaterMaterial::dispose(bool forceDisposeEffect, bool forceDisposeTextures,
                            bool /*notBoundToMesh*/)
{
  if (_bumpTexture) {
    _bumpTexture->dispose();
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
    _reflectionRTT = nullptr;
  }
  if (_refractionRTT) {
    _refractionRTT->dispose();
    _refractionRTT = nullptr;
  }

  PushMaterial::dispose(forceDisposeEffect, forceDisposeTextures);
}

MaterialPtr WaterMaterial::clone(const std::string& /*name*/,
                                 bool /*cloneChildren*/) const
{
  return nullptr;
}

json WaterMaterial::serialize() const
{
  return nullptr;
}

const std::string WaterMaterial::getClassName() const
{
  return "WaterMaterial";
}

WaterMaterial* WaterMaterial::Parse(const json& /*source*/, Scene* /*scene*/,
                                    const std::string& /*rootUrl*/)
{
  return nullptr;
}

MeshPtr WaterMaterial::CreateDefaultMesh(const std::string& name, Scene* scene)
{
  auto mesh = Mesh::CreateGround(name, 512, 512, 32, scene, false);
  return mesh;
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
