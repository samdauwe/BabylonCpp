#include <babylon/materials/material.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/misc/guid.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

const Material::MaterialDefinesCallback Material::_AllDirtyCallBack
  = [](MaterialDefines& defines) -> void { defines.markAllAsDirty(); };
const Material::MaterialDefinesCallback Material::_ImageProcessingDirtyCallBack
  = [](MaterialDefines& defines) -> void { defines.markAsImageProcessingDirty(); };
const Material::MaterialDefinesCallback Material::_TextureDirtyCallBack
  = [](MaterialDefines& defines) -> void { defines.markAsTexturesDirty(); };
const Material::MaterialDefinesCallback Material::_FresnelDirtyCallBack
  = [](MaterialDefines& defines) -> void { defines.markAsFresnelDirty(); };
const Material::MaterialDefinesCallback Material::_MiscDirtyCallBack
  = [](MaterialDefines& defines) -> void { defines.markAsMiscDirty(); };
const Material::MaterialDefinesCallback Material::_PrePassDirtyCallBack
  = [](MaterialDefines& defines) -> void { defines.markAsPrePassDirty(); };
const Material::MaterialDefinesCallback Material::_LightsDirtyCallBack
  = [](MaterialDefines& defines) -> void { defines.markAsLightDirty(); };
const Material::MaterialDefinesCallback Material::_AttributeDirtyCallBack
  = [](MaterialDefines& defines) -> void { defines.markAsAttributesDirty(); };

const Material::MaterialDefinesCallback Material::_FresnelAndMiscDirtyCallBack
  = [](MaterialDefines& defines) -> void {
  Material::_FresnelDirtyCallBack(defines);
  Material::_MiscDirtyCallBack(defines);
};
const Material::MaterialDefinesCallback Material::_TextureAndMiscDirtyCallBack
  = [](MaterialDefines& defines) -> void {
  Material::_TextureDirtyCallBack(defines);
  Material::_MiscDirtyCallBack(defines);
};

std::vector<Material::MaterialDefinesCallback> Material::_DirtyCallbackArray{};
const Material::MaterialDefinesCallback Material::_RunDirtyCallBacks
  = [](MaterialDefines& defines) -> void {
  for (const auto& cb : Material::_DirtyCallbackArray) {
    cb(defines);
  }
};

Material::Material(const std::string& iName, Scene* scene, bool doNotAdd)
    : customShaderNameResolve{nullptr}
    , shadowDepthWrapper{nullptr}
    , allowShaderHotSwapping{true}
    , checkReadyOnEveryCall{false}
    , checkReadyOnlyOnce{false}
    , canRenderToMRT{this, &Material::get_canRenderToMRT}
    , alpha{this, &Material::get_alpha, &Material::set_alpha}
    , backFaceCulling{this, &Material::get_backFaceCulling, &Material::set_backFaceCulling}
    , hasRenderTargetTextures{this, &Material::get_hasRenderTargetTextures}
    , onCompiled{nullptr}
    , onError{nullptr}
    , getRenderTargetTextures{nullptr}
    , doNotSerialize{false}
    , _storeEffectOnSubMeshes{false}
    , onDispose{this, &Material::set_onDispose}
    , onBindObservable{this, &Material::get_onBindObservable}
    , onBind{this, &Material::set_onBind}
    , onUnBindObservable{this, &Material::get_onUnBindObservable}
    , onEffectCreatedObservable{this, &Material::get_onEffectCreatedObservable}
    , alphaMode{this, &Material::get_alphaMode, &Material::set_alphaMode}
    , needDepthPrePass{this, &Material::get_needDepthPrePass, &Material::set_needDepthPrePass}
    , disableDepthWrite{false}
    , disableColorWrite{false}
    , forceDepthWrite{false}
    , depthFunction{0}
    , separateCullingPass{false}
    , fogEnabled{this, &Material::get_fogEnabled, &Material::set_fogEnabled}
    , pointSize{1.f}
    , zOffset{0.f}
    , wireframe{this, &Material::get_wireframe, &Material::set_wireframe}
    , pointsCloud{this, &Material::get_pointsCloud, &Material::set_pointsCloud}
    , fillMode{this, &Material::get_fillMode, &Material::set_fillMode}
    , _effect{nullptr}
    , _indexInSceneMaterialArray{-1}
    , transparencyMode{this, &Material::get_transparencyMode, &Material::set_transparencyMode}
    , useLogarithmicDepth{this, &Material::get_useLogarithmicDepth,
                          &Material::set_useLogarithmicDepth}
    , _alpha{1.f}
    , _backFaceCulling{true}
    , _uniformBuffer{std::make_unique<UniformBuffer>(scene->getEngine())}
    , _forceAlphaTest{false}
    , _transparencyMode{std::nullopt}
    , _disableAlphaBlending{this, &Material::get__disableAlphaBlending}
    , _onDisposeObserver{nullptr}
    , _onBindObserver{nullptr}
    , _alphaMode{Constants::ALPHA_COMBINE}
    , _needDepthPrePass{false}
    , _fogEnabled{true}
    , _useUBO{false}
    , _needToBindSceneUbo{false}
    , _fillMode{Material::TriangleFillMode}
    , _cachedDepthWriteState{false}
    , _cachedColorWriteState{false}
    , _cachedDepthFunctionState{0}
{
  name             = iName;
  auto idSubscript = 1u;
  _scene           = scene ? scene : Engine::LastCreatedScene();

  id = !iName.empty() ? iName : GUID::RandomId();
  while (_scene->getMaterialByID(id)) {
    id = name + " " + std::to_string(idSubscript++);
  }

  uniqueId = _scene->getUniqueId();

  if (_scene->useRightHandedSystem()) {
    sideOrientation = Material::ClockWiseSideOrientation;
  }
  else {
    sideOrientation = Material::CounterClockWiseSideOrientation;
  }

  _useUBO = getScene()->getEngine()->supportsUniformBuffers();

  if (!doNotAdd) {
    // _scene->materials.emplace_back(this);
  }

  if (_scene->useMaterialMeshMap) {
    meshMap = {};
  }
}

Material::~Material() = default;

bool Material::get_canRenderToMRT() const
{
  // By default, shaders are not compatible with MRTs
  // Base classes should override that if their shader supports MRT
  return false;
}

void Material::set_alpha(float value)
{
  if (stl_util::almost_equal(_alpha, value)) {
    return;
  }
  _alpha = value;
  markAsDirty(Material::MiscDirtyFlag);
}

float Material::get_alpha() const
{
  return _alpha;
}

void Material::set_backFaceCulling(bool value)
{
  if (_backFaceCulling == value) {
    return;
  }
  _backFaceCulling = value;
  markAsDirty(Material::TextureDirtyFlag);
}

bool Material::get_backFaceCulling() const
{
  return _backFaceCulling;
}

bool Material::get_hasRenderTargetTextures() const
{
  return false;
}

Type Material::type() const
{
  return Type::MATERIAL;
}

AnimationValue Material::getProperty(const std::vector<std::string>& /*targetPropertyPath*/)
{
  return AnimationValue();
}

void Material::setProperty(const std::vector<std::string>& /*targetPropertyPath*/,
                           const AnimationValue& /*value*/)
{
}

void Material::addMaterialToScene(const MaterialPtr& newMaterial)
{
  _scene->materials.emplace_back(newMaterial);
  _scene->onNewMaterialAddedObservable.notifyObservers(newMaterial.get());
}

void Material::addMultiMaterialToScene(const MultiMaterialPtr& newMultiMaterial)
{
  _scene->multiMaterials.emplace_back(newMultiMaterial);
}

unsigned int Material::get_alphaMode() const
{
  return _alphaMode;
}

void Material::set_alphaMode(unsigned int value)
{
  if (_alphaMode == value) {
    return;
  }
  _alphaMode = value;
  markAsDirty(Material::TextureDirtyFlag);
}

bool Material::get_needDepthPrePass() const
{
  return _needDepthPrePass;
}

void Material::set_needDepthPrePass(bool value)
{
  if (_needDepthPrePass == value) {
    return;
  }
  _needDepthPrePass = value;
  if (_needDepthPrePass) {
    checkReadyOnEveryCall = true;
  }
}

bool Material::get_fogEnabled() const
{
  return _fogEnabled;
}

void Material::set_fogEnabled(bool value)
{
  if (_fogEnabled == value) {
    return;
  }
  _fogEnabled = value;
  markAsDirty(Material::MiscDirtyFlag);
}

bool Material::get_useLogarithmicDepth() const
{
  return false;
}

void Material::set_useLogarithmicDepth(bool /*value*/)
{
}

// Methods
std::vector<AnimationPtr> Material::getAnimations()
{
  return animations;
}

// Events
void Material::set_onDispose(const std::function<void(Material*, EventState&)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

Observable<AbstractMesh>& Material::get_onBindObservable()
{
  return _onBindObservable;
}

void Material::set_onBind(const std::function<void(AbstractMesh*, EventState&)>& callback)
{
  if (_onBindObserver) {
    onBindObservable().remove(_onBindObserver);
  }
  _onBindObserver = onBindObservable().add(callback);
}

Observable<Material>& Material::get_onUnBindObservable()
{
  return _onUnBindObservable;
}

Observable<OnCreatedEffectParameters>& Material::get_onEffectCreatedObservable()
{
  return _onEffectCreatedObservable;
}

void Material::set_wireframe(bool value)
{
  fillMode = value ? Material::WireFrameFillMode : Material::TriangleFillMode;
}

bool Material::get_wireframe() const
{
  switch (_fillMode) {
    case Material::WireFrameFillMode:
    case Material::LineListDrawMode:
    case Material::LineLoopDrawMode:
    case Material::LineStripDrawMode:
      return true;
  }

  return _scene->forceWireframe();
}

void Material::set_pointsCloud(bool value)
{
  fillMode = value ? Material::PointFillMode : Material::TriangleFillMode;
}

bool Material::get_pointsCloud() const
{
  switch (_fillMode) {
    case Material::PointFillMode:
    case Material::PointListDrawMode:
      return true;
  }

  return _scene->forcePointsCloud();
}

void Material::set_fillMode(unsigned int value)
{
  if (_fillMode == value) {
    return;
  }

  _fillMode = value;
  markAsDirty(Material::MiscDirtyFlag);
}

unsigned int Material::get_fillMode() const
{
  return _fillMode;
}

std::string Material::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name;
  if (fullDetails) {
  }
  return oss.str();
}

std::string Material::getClassName() const
{
  return "Material";
}

bool Material::isFrozen() const
{
  return checkReadyOnlyOnce;
}

void Material::freeze()
{
  markDirty();
  checkReadyOnlyOnce = true;
}

void Material::unfreeze()
{
  markDirty();
  checkReadyOnlyOnce = false;
}

bool Material::isReady(AbstractMesh* /*mesh*/, bool /*useInstances*/)
{
  return true;
}

bool Material::isReadyForSubMesh(AbstractMesh* /*mesh*/, SubMesh* /*subMesh*/,
                                 bool /*useInstances*/)
{
  return false;
}

EffectPtr& Material::getEffect()
{
  return _effect;
}

Scene* Material::getScene() const
{
  return _scene;
}

std::optional<unsigned int>& Material::get_transparencyMode()
{
  return _transparencyMode;
}

void Material::set_transparencyMode(const std::optional<unsigned int>& value)
{
  if (_transparencyMode == value) {
    return;
  }

  _transparencyMode = value;

  _forceAlphaTest = value && (*value == Material::MATERIAL_ALPHATESTANDBLEND);

  _markAllSubMeshesAsTexturesAndMiscDirty();
}

bool Material::get__disableAlphaBlending() const
{
  return (_transparencyMode
          && (*_transparencyMode == Material::MATERIAL_OPAQUE
              || *_transparencyMode == Material::MATERIAL_ALPHATEST));
}

bool Material::needAlphaBlending() const
{
  if (_disableAlphaBlending) {
    return false;
  }

  return (_alpha < 1.f);
}

bool Material::needAlphaBlendingForMesh(const AbstractMesh& mesh) const
{
  if (_disableAlphaBlending && mesh.visibility >= 1.f) {
    return false;
  }

  return needAlphaBlending() || (mesh.visibility() < 1.f) || mesh.hasVertexAlpha();
}

bool Material::needAlphaTesting() const
{
  if (_forceAlphaTest) {
    return true;
  }

  return false;
}

bool Material::_shouldTurnAlphaTestOn(AbstractMesh* mesh) const
{
  return (!needAlphaBlendingForMesh(*mesh) && needAlphaTesting());
}

BaseTexturePtr Material::getAlphaTestTexture()
{
  return nullptr;
}

void Material::trackCreation(
  const std::function<void(const EffectPtr& effect)>& /*onCompiled*/,
  const std::function<void(const EffectPtr& effect, const std::string& errors)>&
  /*onError*/)
{
}

void Material::markDirty()
{
  const auto& meshes = getScene()->meshes;
  for (const auto& mesh : meshes) {
    if (mesh->subMeshes.empty()) {
      continue;
    }
    for (const auto& subMesh : mesh->subMeshes) {
      if (subMesh->getMaterial().get() != this) {
        continue;
      }

      if (!subMesh->effect()) {
        continue;
      }

      subMesh->effect()->_wasPreviouslyReady = false;
    }
  }
}

bool Material::_preBind(const EffectPtr& effect, std::optional<unsigned int> overrideOrientation)
{
  auto engine = _scene->getEngine();

  const auto orientation = overrideOrientation.value_or(static_cast<unsigned>(sideOrientation));
  const auto reverse     = orientation == Material::ClockWiseSideOrientation;

  engine->enableEffect(effect ? effect : _effect);
  engine->setState(backFaceCulling(), zOffset, false, reverse);

  return reverse;
}

void Material::bind(Matrix& /*world*/, Mesh* /*mesh*/, const EffectPtr& /*effectOverride*/)
{
}

void Material::bindForSubMesh(Matrix& /*world*/, Mesh* /*mesh*/, SubMesh* /*subMesh*/)
{
}

void Material::bindOnlyWorldMatrix(Matrix& /*world*/, const EffectPtr& /*effectOverride*/)
{
}

void Material::bindSceneUniformBuffer(Effect* effect, UniformBuffer* sceneUbo)
{
  sceneUbo->bindToEffect(effect, "Scene");
}

void Material::bindView(Effect* effect)
{
  if (!_useUBO) {
    effect->setMatrix("view", getScene()->getViewMatrix());
  }
  else {
    bindSceneUniformBuffer(effect, getScene()->getSceneUniformBuffer());
  }
}

void Material::bindViewProjection(const EffectPtr& effect)
{
  if (!_useUBO) {
    effect->setMatrix("viewProjection", getScene()->getTransformMatrix());
  }
  else {
    bindSceneUniformBuffer(effect.get(), getScene()->getSceneUniformBuffer());
  }
}

void Material::bindEyePosition(Effect* effect, const std::string& variableName)
{
  if (!_useUBO) {
    MaterialHelper::BindEyePosition(effect, _scene, variableName);
  }
  else {
    _needToBindSceneUbo = true;
  }
}

void Material::_afterBind(Mesh* mesh, const EffectPtr& /*effect*/)
{
  _scene->_cachedMaterial = this;
  if (mesh) {
    _scene->_cachedVisibility = mesh->visibility();
  }
  else {
    _scene->_cachedVisibility = 1.f;
  }

  if (mesh) {
    onBindObservable().notifyObservers(mesh);
  }

  if (disableDepthWrite) {
    auto engine            = _scene->getEngine();
    _cachedDepthWriteState = engine->getDepthWrite();
    engine->setDepthWrite(false);
  }

  if (disableColorWrite) {
    auto engine            = _scene->getEngine();
    _cachedColorWriteState = engine->getColorWrite();
    engine->setColorWrite(false);
  }

  if (depthFunction != 0) {
    auto engine               = _scene->getEngine();
    _cachedDepthFunctionState = engine->getDepthFunction();
    engine->setDepthFunction(depthFunction);
  }
}

void Material::unbind()
{
  onUnBindObservable().notifyObservers(this);

  if (depthFunction != 0) {
    auto engine = _scene->getEngine();
    engine->setDepthFunction(_cachedDepthFunctionState);
  }

  if (disableDepthWrite) {
    auto engine = _scene->getEngine();
    engine->setDepthWrite(_cachedDepthWriteState);
  }

  if (disableColorWrite) {
    auto engine = _scene->getEngine();
    engine->setColorWrite(_cachedColorWriteState);
  }
}

std::vector<BaseTexturePtr> Material::getActiveTextures() const
{
  return {};
}

bool Material::hasTexture(const BaseTexturePtr& /*texture*/) const
{
  return false;
}

MaterialPtr Material::clone(const std::string& /*name*/, bool /*cloneChildren*/) const
{
  return nullptr;
}

std::vector<AbstractMeshPtr> Material::getBindedMeshes()
{
  std::vector<AbstractMeshPtr> result;

  if (!meshMap.empty()) {
    for (const auto& meshItem : meshMap) {
      const auto& mesh = meshItem.second;
      if (mesh) {
        result.emplace_back(mesh);
      }
    }
  }
  else {
    for (const auto& mesh : _scene->meshes) {
      if (mesh->material().get() == this) {
        result.emplace_back(mesh);
      }
    }
  }

  return result;
}

void Material::forceCompilation(const AbstractMeshPtr& mesh,
                                const std::function<void(Material* material)>& iOnCompiled,
                                const std::optional<IMaterialCompilationOptions>& options,
                                const std::function<void(const std::string& reason)>& iOnError)
{
  auto localOptions = IMaterialCompilationOptions{
    false, // clipPlane
    false, // useInstances
  };
  if (options) {
    localOptions.clipPlane    = options->clipPlane;
    localOptions.useInstances = options->useInstances;
  }

  auto scene                        = getScene();
  const auto currentHotSwapingState = allowShaderHotSwapping;
  allowShaderHotSwapping = false; // Turned off to let us evaluate the real compilation state

  const auto checkReady = [=]() {
    if (!_scene || !_scene->getEngine()) {
      return;
    }

    auto clipPlaneState = scene->clipPlane;

    if (localOptions.clipPlane) {
      scene->clipPlane = Plane(0.f, 0.f, 0.f, 1.f);
    }

    if (_storeEffectOnSubMeshes) {
      bool allDone = true;
      std::string lastError;
      if (!mesh->subMeshes.empty()) {
        const auto tempSubMesh = SubMesh::New(0, 0, 0, 0, 0, mesh, nullptr, false, false);
        if (tempSubMesh->_materialDefines) {
          tempSubMesh->_materialDefines->_renderId = -1;
        }
        if (!isReadyForSubMesh(mesh.get(), tempSubMesh.get(), localOptions.useInstances)) {
          if (tempSubMesh->effect() && !tempSubMesh->effect()->getCompilationError().empty()
              && tempSubMesh->effect()->allFallbacksProcessed()) {
            lastError = tempSubMesh->effect()->getCompilationError();
          }
          else {
            allDone = false;
            // setTimeout(checkReady, 16);
          }
        }
      }
      if (allDone) {
        allowShaderHotSwapping = currentHotSwapingState;
        if (!lastError.empty()) {
          if (onError) {
            iOnError(lastError);
          }
        }
        if (onCompiled) {
          iOnCompiled(this);
        }
      }
    }
    else {
      if (isReady()) {
        allowShaderHotSwapping = currentHotSwapingState;
        if (iOnCompiled) {
          iOnCompiled(this);
        }
      }
      else {
        // setTimeout(checkReady, 16);
      }
    }

    if (localOptions.clipPlane) {
      scene->clipPlane = clipPlaneState;
    }
  };

  checkReady();
}

void Material::markAsDirty(unsigned int flag)
{
  if (getScene()->blockMaterialDirtyMechanism()) {
    return;
  }

  Material::_DirtyCallbackArray.clear();

  if (flag & Material::TextureDirtyFlag) {
    Material::_DirtyCallbackArray.emplace_back(Material::_TextureDirtyCallBack);
  }

  if (flag & Material::LightDirtyFlag) {
    Material::_DirtyCallbackArray.emplace_back(Material::_LightsDirtyCallBack);
  }

  if (flag & Material::FresnelDirtyFlag) {
    Material::_DirtyCallbackArray.emplace_back(Material::_FresnelDirtyCallBack);
  }

  if (flag & Material::AttributesDirtyFlag) {
    Material::_DirtyCallbackArray.emplace_back(Material::_AttributeDirtyCallBack);
  }

  if (flag & Material::MiscDirtyFlag) {
    Material::_DirtyCallbackArray.emplace_back(Material::_MiscDirtyCallBack);
  }

  if (flag & Material::PrePassDirtyFlag) {
    Material::_DirtyCallbackArray.emplace_back(Material::_PrePassDirtyCallBack);
  }

  if (!Material::_DirtyCallbackArray.empty()) {
    _markAllSubMeshesAsDirty(Material::_RunDirtyCallBacks);
  }

  getScene()->resetCachedMaterial();
}

void Material::_markAllSubMeshesAsDirty(const Material::MaterialDefinesCallback& func)
{
  if (getScene()->blockMaterialDirtyMechanism()) {
    return;
  }

  const auto& meshes = getScene()->meshes;
  for (const auto& mesh : meshes) {
    if (mesh->subMeshes.empty()) {
      continue;
    }
    for (auto& subMesh : mesh->subMeshes) {
      if (subMesh->getMaterial().get() != this) {
        continue;
      }

      if (!subMesh->_materialDefines) {
        continue;
      }

      func(*subMesh->_materialDefines);
    }
  }
}

void Material::_markScenePrePassDirty()
{
  if (getScene()->blockMaterialDirtyMechanism()) {
    return;
  }

  const auto prePassRenderer = getScene()->enablePrePassRenderer();
  if (prePassRenderer) {
    prePassRenderer->markAsDirty();
  }
}

void Material::_markAllSubMeshesAsAllDirty()
{
  _markAllSubMeshesAsDirty(Material::_AllDirtyCallBack);
}

void Material::_markAllSubMeshesAsImageProcessingDirty()
{
  _markAllSubMeshesAsDirty(Material::_ImageProcessingDirtyCallBack);
}

void Material::_markAllSubMeshesAsTexturesDirty()
{
  _markAllSubMeshesAsDirty(Material::_TextureDirtyCallBack);
}

void Material::_markAllSubMeshesAsFresnelDirty()
{
  _markAllSubMeshesAsDirty(Material::_FresnelDirtyCallBack);
}

void Material::_markAllSubMeshesAsFresnelAndMiscDirty()
{
  _markAllSubMeshesAsDirty(Material::_FresnelAndMiscDirtyCallBack);
}

void Material::_markAllSubMeshesAsLightsDirty()
{
  _markAllSubMeshesAsDirty(Material::_LightsDirtyCallBack);
}

void Material::_markAllSubMeshesAsAttributesDirty()
{
  _markAllSubMeshesAsDirty(Material::_AttributeDirtyCallBack);
}

void Material::_markAllSubMeshesAsMiscDirty()
{
  _markAllSubMeshesAsDirty(Material::_MiscDirtyCallBack);
}

void Material::_markAllSubMeshesAsPrePassDirty()
{
  _markAllSubMeshesAsDirty(Material::_MiscDirtyCallBack);
}

void Material::_markAllSubMeshesAsTexturesAndMiscDirty()
{
  _markAllSubMeshesAsDirty(Material::_TextureAndMiscDirtyCallBack);
}

bool Material::setPrePassRenderer(const PrePassRendererPtr& /*prePassRenderer*/)
{
  // Do Nothing by default
  return false;
}

void Material::dispose(bool forceDisposeEffect, bool /*forceDisposeTextures*/, bool notBoundToMesh)
{
  auto& scene = *getScene();
  // Animations
  scene.stopAnimation(this);
  scene.freeProcessedMaterials();

  // Remove from scene
  scene.removeMaterial(this);

  if (!notBoundToMesh) {
    // Remove from meshes
    if (meshMap.empty()) {
      for (const auto& meshItem : meshMap) {
        const auto& mesh = meshItem.second;
        if (mesh) {
          mesh->material = nullptr; // will set the entry in the map to undefined
          releaseVertexArrayObject(mesh, forceDisposeEffect);
        }
      }
    }
    else {
      const auto& meshes = scene.meshes;
      for (const auto& mesh : meshes) {
        if (mesh->material().get() == this) {
        }
        auto instancedMesh = std::static_pointer_cast<InstancedMesh>(mesh);
        if (instancedMesh && !instancedMesh->sourceMesh()) {
          mesh->material = nullptr;
          releaseVertexArrayObject(mesh, forceDisposeEffect);
        }
      }
    }
  }

  _uniformBuffer->dispose();

  // Shader are kept in cache for further use but we can get rid of this by
  // using forceDisposeEffect
  if (forceDisposeEffect && _effect) {
    if (!_storeEffectOnSubMeshes) {
      _effect->dispose();
    }

    _effect = nullptr;
  }

  // Callback
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  _onBindObservable.clear();
  _onUnBindObservable.clear();
  _onEffectCreatedObservable.clear();
}

void Material::releaseVertexArrayObject(const AbstractMeshPtr& iMesh, bool forceDisposeEffect)
{
  auto mesh = std::static_pointer_cast<Mesh>(iMesh);
  if (mesh && mesh->geometry()) {
    const auto& geometry = mesh->geometry();
    if (_storeEffectOnSubMeshes) {
      for (const auto& subMesh : mesh->subMeshes) {
        geometry->_releaseVertexArrayObject(subMesh->_materialEffect);
        if (forceDisposeEffect && subMesh->_materialEffect) {
          subMesh->_materialEffect->dispose();
        }
      }
    }
    else {
      geometry->_releaseVertexArrayObject(_effect);
    }
  }
}

void Material::copyTo(Material* other) const
{
  other->checkReadyOnlyOnce    = checkReadyOnlyOnce;
  other->checkReadyOnEveryCall = checkReadyOnEveryCall;
  other->alpha                 = _alpha;
  other->fillMode              = fillMode();
  other->backFaceCulling       = backFaceCulling();
  other->wireframe             = wireframe();
  other->fogEnabled            = fogEnabled();
  other->zOffset               = zOffset;
  other->alphaMode             = alphaMode();
  other->sideOrientation       = sideOrientation;
  other->disableDepthWrite     = disableDepthWrite;
  other->pointSize             = pointSize;
  other->pointsCloud           = pointsCloud();
}

json Material::serialize() const
{
  return nullptr;
}

MultiMaterialPtr Material::ParseMultiMaterial(const json& parsedMultiMaterial, Scene* scene)
{
  auto multiMaterial
    = MultiMaterial::New(json_util::get_string(parsedMultiMaterial, "name"), scene);

  multiMaterial->id = json_util::get_string(parsedMultiMaterial, "id");

  for (const auto& subMatId : json_util::get_array<json>(parsedMultiMaterial, "materials")) {
    if (!subMatId.is_null()) {
      multiMaterial->subMaterials().emplace_back(scene->getMaterialByID(subMatId));
    }
    else {
      multiMaterial->subMaterials().emplace_back(nullptr);
    }
  }

  return multiMaterial;
}

MaterialPtr Material::Parse(const json& parsedMaterial, Scene* scene, const std::string& rootUrl)
{
  if (!json_util::has_key(parsedMaterial, "customType")
      || json_util::get_string(parsedMaterial, "customType") == "BABYLON.StandardMaterial") {
    return StandardMaterial::Parse(parsedMaterial, scene, rootUrl);
  }

  return nullptr;
}

} // end of namespace BABYLON
