#include <babylon/materials/textures/render_target_texture.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/depth_texture_creation_options.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/maths/matrix.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/misc/tools.h>
#include <babylon/particles/particle_system.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/post_process_manager.h>
#include <babylon/rendering/rendering_manager.h>

namespace BABYLON {

RenderTargetTexture::RenderTargetTexture(const std::string& iName,
                                         const std::variant<int, RenderTargetSize, float>& size,
                                         Scene* scene, bool generateMipMaps,
                                         bool doNotChangeAspectRatio, unsigned int type,
                                         bool iIsCube, unsigned int iSamplingMode,
                                         bool generateDepthBuffer, bool generateStencilBuffer,
                                         bool isMulti, unsigned int format, bool delayAllocation)
    : Texture{"", scene, !generateMipMaps}
    , renderListPredicate{nullptr}
    , renderList{this, &RenderTargetTexture::get_renderList, &RenderTargetTexture::set_renderList}
    , getCustomRenderList{nullptr}
    , renderParticles{true}
    , renderSprites{false}
    , activeCamera{nullptr}
    , customRenderFunction{nullptr}
    , useCameraPostProcesses{std::nullopt}
    , ignoreCameraViewport{false}
    , clearColor{std::nullopt}
    , boundingBoxPosition{Vector3::Zero()}
    , depthStencilTexture{this, &RenderTargetTexture::get_depthStencilTexture}
    , onAfterUnbind{this, &RenderTargetTexture::set_onAfterUnbind}
    , onBeforeRender{this, &RenderTargetTexture::set_onBeforeRender}
    , onAfterRender{this, &RenderTargetTexture::set_onAfterRender}
    , onClear{this, &RenderTargetTexture::set_onClear}
    , renderTargetOptions{this, &RenderTargetTexture::get_renderTargetOptions}
    , samples{this, &RenderTargetTexture::get_samples, &RenderTargetTexture::set_samples}
    , refreshRate{this, &RenderTargetTexture::get_refreshRate,
                  &RenderTargetTexture::set_refreshRate}
    , _sizeRatio{0.f}
    , _currentRefreshId{-1}
    , _refreshRate{1}
    , _samples{1}
    , _postProcessManager{nullptr}
    , _resizeObserver{nullptr}
    , _onAfterUnbindObserver{nullptr}
    , _onBeforeRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
    , _onClearObserver{nullptr}
    , _boundingBoxSize{std::nullopt}
    , _defaultRenderListPrepared{false}
    , _nullInternalTexture{nullptr}
{
  scene = getScene();

  if (!scene) {
    return;
  }
  _engine         = scene->getEngine();
  name            = iName;
  isRenderTarget  = true;
  isCube          = iIsCube;
  coordinatesMode = TextureConstants::PROJECTION_MODE;

  _initialSizeParameter = size;

  _processSizeParameter(size);

  _resizeObserver = scene->getEngine()->onResizeObservable.add([](Engine*, EventState&) {});

  _generateMipMaps        = generateMipMaps;
  _doNotChangeAspectRatio = doNotChangeAspectRatio;

  // Rendering groups
  _renderingManager                          = std::make_unique<RenderingManager>(scene);
  _renderingManager->_useSceneAutoClearSetup = true;

  if (isMulti) {
    return;
  }

  _renderTargetOptions.generateMipMaps       = generateMipMaps;
  _renderTargetOptions.type                  = type;
  _renderTargetOptions.format                = format;
  _renderTargetOptions.samplingMode          = iSamplingMode;
  _renderTargetOptions.generateDepthBuffer   = generateDepthBuffer;
  _renderTargetOptions.generateStencilBuffer = generateStencilBuffer;

  if (iSamplingMode == TextureConstants::NEAREST_SAMPLINGMODE) {
    wrapU = TextureConstants::CLAMP_ADDRESSMODE;
    wrapV = TextureConstants::CLAMP_ADDRESSMODE;
  }

  if (!delayAllocation) {
    if (iIsCube) {
      auto renderSize = getRenderSize();
      _texture        = scene->getEngine()->createRenderTargetCubeTexture(
        ISize{renderSize.width, renderSize.height}, _renderTargetOptions);
      coordinatesMode = TextureConstants::INVCUBIC_MODE;
      _textureMatrix  = std::make_unique<Matrix>(Matrix::Identity());
    }
    else {
      _texture = scene->getEngine()->createRenderTargetTexture(size, _renderTargetOptions);
    }
  }
}

RenderTargetTexture::~RenderTargetTexture() = default;

void RenderTargetTexture::_onRatioRescale()
{
  if (_sizeRatio != 0.f) {
    resize(_initialSizeParameter);
  }
}

std::vector<AbstractMesh*>& RenderTargetTexture::get_renderList()
{
  return _renderList;
}

void RenderTargetTexture::set_renderList(const std::vector<AbstractMesh*>& value)
{
  _renderList = value;
}

void RenderTargetTexture::set_boundingBoxSize(const std::optional<Vector3>& value)
{
  if (_boundingBoxSize && (*_boundingBoxSize).equals(*value)) {
    return;
  }
  _boundingBoxSize = value;
  auto scene       = getScene();
  if (scene) {
    scene->markAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
  }
}

std::optional<Vector3>& RenderTargetTexture::get_boundingBoxSize()
{
  return _boundingBoxSize;
}

InternalTexturePtr& RenderTargetTexture::get_depthStencilTexture()
{
  return getInternalTexture() ? getInternalTexture()->_depthStencilTexture : _nullInternalTexture;
}

void RenderTargetTexture::set_onAfterUnbind(
  const std::function<void(RenderTargetTexture*, EventState&)>& callback)
{
  if (_onAfterUnbindObserver) {
    onAfterUnbindObservable.remove(_onAfterUnbindObserver);
  }
  _onAfterUnbindObserver = onAfterUnbindObservable.add(callback);
}

void RenderTargetTexture::set_onBeforeRender(
  const std::function<void(int* faceIndex, EventState&)>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
}

void RenderTargetTexture::set_onAfterRender(
  const std::function<void(int* faceIndex, EventState&)>& callback)
{
  if (_onAfterRenderObserver) {
    onAfterRenderObservable.remove(_onAfterRenderObserver);
  }
  _onAfterRenderObserver = onAfterRenderObservable.add(callback);
}

void RenderTargetTexture::set_onClear(
  const std::function<void(Engine* engine, EventState&)>& callback)
{
  if (_onClearObserver) {
    onClearObservable.remove(_onClearObserver);
  }
  _onClearObserver = onClearObservable.add(callback);
}

IRenderTargetOptions& RenderTargetTexture::get_renderTargetOptions()
{
  return _renderTargetOptions;
}

void RenderTargetTexture::createDepthStencilTexture(int comparisonFunction, bool bilinearFiltering,
                                                    bool generateStencil)
{
  auto internalTexture = getInternalTexture();
  if (!getScene() || !internalTexture) {
    return;
  }

  auto engine = getScene()->getEngine();
  DepthTextureCreationOptions options;
  options.bilinearFiltering             = bilinearFiltering;
  options.comparisonFunction            = comparisonFunction;
  options.generateStencil               = generateStencil;
  options.isCube                        = isCube;
  internalTexture->_depthStencilTexture = engine->createDepthStencilTexture(_size, options);
}

void RenderTargetTexture::_processSizeParameter(
  const std::variant<int, RenderTargetSize, float>& size)
{
  if (std::holds_alternative<float>(size)) {
    _sizeRatio = std::get<float>(size);
    _size      = RenderTargetSize{
      _bestReflectionRenderTargetDimension(_engine->getRenderWidth(),
                                           _sizeRatio), // width
      _bestReflectionRenderTargetDimension(_engine->getRenderHeight(),
                                           _sizeRatio), // height
      std::nullopt,                                     // layers
    };
  }
  else if (std::holds_alternative<RenderTargetSize>(size)) {
    _size = std::get<RenderTargetSize>(size);
  }
  else if (std::holds_alternative<int>(size)) {
    const auto sizeI = std::get<int>(size);
    _size            = RenderTargetSize{
      sizeI,       // width
      sizeI,       // height
      std::nullopt // layers
    };
  }
}

unsigned int RenderTargetTexture::get_samples() const
{
  return _samples;
}

void RenderTargetTexture::set_samples(unsigned int value)
{
  if (_samples == value) {
    return;
  }

  auto scene = getScene();

  if (!scene) {
    return;
  }

  _samples = scene->getEngine()->updateRenderTargetTextureSampleCount(_texture, value);
}

void RenderTargetTexture::resetRefreshCounter()
{
  _currentRefreshId = -1;
}

int RenderTargetTexture::get_refreshRate() const
{
  return _refreshRate;
}

void RenderTargetTexture::set_refreshRate(int value)
{
  _refreshRate = value;
  resetRefreshCounter();
}

void RenderTargetTexture::addPostProcess(const PostProcessPtr& postProcess)
{
  if (!_postProcessManager) {
    auto scene = getScene();

    if (!scene) {
      return;
    }

    _postProcessManager = std::make_unique<PostProcessManager>(scene);
    _postProcesses.clear();
  }

  _postProcesses.emplace_back(postProcess);
  _postProcesses[0]->autoClear = false;
}

void RenderTargetTexture::clearPostProcesses(bool dispose)
{
  if (_postProcesses.empty()) {
    return;
  }

  if (dispose) {
    for (auto& postProcess : _postProcesses) {
      postProcess->dispose();
      postProcess = nullptr;
    }
  }

  _postProcesses.clear();
}

void RenderTargetTexture::removePostProcess(const PostProcessPtr& postProcess)
{
  if (_postProcesses.empty()) {
    return;
  }

  auto it = std::find(_postProcesses.begin(), _postProcesses.end(), postProcess);

  if (it == _postProcesses.end()) {
    return;
  }

  _postProcesses.erase(it);

  if (!_postProcesses.empty()) {
    _postProcesses[0]->autoClear = false;
  }
}

bool RenderTargetTexture::_shouldRender()
{
  if (_currentRefreshId == -1) { // At least render once
    _currentRefreshId = 1;
    return true;
  }

  if (_refreshRate == _currentRefreshId) {
    _currentRefreshId = 1;
    return true;
  }

  ++_currentRefreshId;
  return false;
}

bool RenderTargetTexture::isReady()
{
  if (!getScene()->renderTargetsEnabled) {
    return false;
  }
  return Texture::isReady();
}

RenderTargetSize& RenderTargetTexture::getRenderSize()
{
  return _size;
}

int RenderTargetTexture::getRenderWidth() const
{
  return _size.width;
}

int RenderTargetTexture::getRenderHeight() const
{
  return _size.height;
}

int RenderTargetTexture::getRenderLayers() const
{
  const auto& layers = _size.layers;
  if (layers) {
    return *layers;
  }

  return 0;
}

bool RenderTargetTexture::canRescale() const
{
  return true;
}

void RenderTargetTexture::scale(float ratio)
{
  const auto& currentSize = getRenderSize();
  RenderTargetSize newSize;
  newSize.width  = std::max(1, static_cast<int>(currentSize.width * ratio));
  newSize.height = std::max(1, static_cast<int>(currentSize.height * ratio));
  resize(newSize);
}

Matrix* RenderTargetTexture::getReflectionTextureMatrix()
{
  if (isCube) {
    return _textureMatrix.get();
  }

  return Texture::getReflectionTextureMatrix();
}

void RenderTargetTexture::resize(const std::variant<int, RenderTargetSize, float>& size)
{
  const auto wasCube = isCube();

  releaseInternalTexture();
  auto scene = getScene();

  if (!scene) {
    return;
  }

  _processSizeParameter(size);

  if (wasCube) {
    auto renderSize = getRenderSize();
    _texture        = scene->getEngine()->createRenderTargetCubeTexture(
      ISize{renderSize.width, renderSize.height}, _renderTargetOptions);
  }
  else {
    _texture = scene->getEngine()->createRenderTargetTexture(_size, _renderTargetOptions);
  }

  if (onResizeObservable.hasObservers()) {
    onResizeObservable.notifyObservers(this);
  }
}

void RenderTargetTexture::render(bool iUseCameraPostProcess, bool dumpForDebug)
{
  auto scene = getScene();

  if (!scene) {
    return;
  }

  auto engine = scene->getEngine();

  if (useCameraPostProcesses) {
    iUseCameraPostProcess = *useCameraPostProcesses;
  }

  if (!_waitingRenderList.empty()) {
    renderList().clear();
    for (const auto& id : _waitingRenderList) {
      auto mesh = scene->getMeshByID(id);
      if (mesh) {
        renderList().emplace_back(mesh.get());
      }
    }

    _waitingRenderList.clear();
  }

  // Is predicate defined?
  if (renderListPredicate) {
    renderList().clear(); // Clear previous renderList

    auto iScene = getScene();

    if (!iScene) {
      return;
    }

    const auto& sceneMeshes = iScene->meshes;

    for (const auto& mesh : sceneMeshes) {
      if (renderListPredicate(mesh.get())) {
        renderList().emplace_back(mesh.get());
      }
    }
  }

  onBeforeBindObservable.notifyObservers(this);

  // Set custom projection.
  // Needs to be before binding to prevent changing the aspect ratio.
  CameraPtr camera = nullptr;
  if (activeCamera) {
    camera = activeCamera;
    engine->setViewport(activeCamera->viewport, getRenderWidth(), getRenderHeight());

    if (activeCamera != scene->activeCamera()) {
      scene->setTransformMatrix(activeCamera->getViewMatrix(),
                                activeCamera->getProjectionMatrix(true));
    }
  }
  else {
    camera = scene->activeCamera();
    if (camera) {
      engine->setViewport(scene->activeCamera()->viewport, getRenderWidth(), getRenderHeight());
    }
  }

  _defaultRenderListPrepared = false;

  if (is2DArray) {
    for (unsigned int layer = 0; layer < static_cast<unsigned int>(getRenderLayers()); layer++) {
      renderToTarget(0, iUseCameraPostProcess, dumpForDebug, layer, camera);
      scene->incrementRenderId();
      scene->resetCachedMaterial();
    }
  }
  else if (isCube) {
    for (unsigned int face = 0; face < 6; ++face) {
      renderToTarget(face, iUseCameraPostProcess, dumpForDebug, 0, camera);
      scene->incrementRenderId();
      scene->resetCachedMaterial();
    }
  }
  else {
    renderToTarget(0, iUseCameraPostProcess, dumpForDebug, 0, camera);
  }

  onAfterUnbindObservable.notifyObservers(this);

  if (scene->activeCamera()) {
    // Do not avoid setting uniforms when multiple scenes are active as another camera may have
    // overwrite these
    if (scene->getEngine()->scenes.size() > 1
        || (activeCamera && activeCamera != scene->activeCamera())) {
      scene->setTransformMatrix(scene->activeCamera()->getViewMatrix(),
                                scene->activeCamera()->getProjectionMatrix(true));
    }
    engine->setViewport(scene->activeCamera()->viewport);
  }

  scene->resetCachedMaterial();
}

int RenderTargetTexture::_bestReflectionRenderTargetDimension(int renderDimension,
                                                              float scale) const
{
  const int minimum = 128;
  const float x     = renderDimension * scale;
  const int curved  = Engine::NearestPOT(static_cast<int>(x + (minimum * minimum / (minimum + x))));

  // Ensure we don't exceed the render dimension (while staying POT)
  return std::min(Engine::FloorPOT(renderDimension), curved);
}

void RenderTargetTexture::_prepareRenderingManager(
  const std::vector<AbstractMesh*>& currentRenderList, size_t currentRenderListLength,
  const CameraPtr& camera, bool checkLayerMask)
{
  auto scene = getScene();

  if (!scene) {
    return;
  }

  _renderingManager->reset();

  auto sceneRenderId = scene->getRenderId();
  for (size_t meshIndex = 0; meshIndex < currentRenderListLength; ++meshIndex) {
    auto mesh = currentRenderList[meshIndex];

    if (mesh) {
      if (!mesh->isReady(refreshRate() == 0)) {
        resetRefreshCounter();
        continue;
      }

      mesh->_preActivateForIntermediateRendering(sceneRenderId);

      auto isMasked = false;

      if (checkLayerMask && camera) {
        isMasked = ((mesh->layerMask() & camera->layerMask) == 0);
      }
      else {
        isMasked = false;
      }

      if (mesh->isEnabled() && mesh->isVisible && (!mesh->subMeshes.empty()) && !isMasked) {
        if (mesh->_activate(sceneRenderId, true) && !mesh->subMeshes.empty()) {
          if (!mesh->isAnInstance()) {
            mesh->_internalAbstractMeshDataInfo._onlyForInstancesIntermediate = false;
          }
          else {
            mesh = static_cast<InstancedMesh*>(mesh);
          }
          mesh->_internalAbstractMeshDataInfo._isActiveIntermediate = true;

          for (const auto& subMesh : mesh->subMeshes) {
            _renderingManager->dispatch(subMesh.get(), mesh);
          }
        }
      }
    }
  }

  for (const auto& particleSystem : scene->particleSystems) {
    if (!particleSystem->isStarted() || !particleSystem->hasEmitter()
        || !(std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)
             && std::get<AbstractMeshPtr>(particleSystem->emitter)->isEnabled())) {
      continue;
    }
    if (stl_util::index_of(currentRenderList,
                           std::get<AbstractMeshPtr>(particleSystem->emitter).get())
        >= 0) {
      _renderingManager->dispatchParticles(particleSystem.get());
    }
  }
}

void RenderTargetTexture::_bindFrameBuffer(unsigned int faceIndex, unsigned int layer)
{
  auto scene = getScene();
  if (!scene) {
    return;
  }

  auto engine = scene->getEngine();
  if (_texture) {
    engine->bindFramebuffer(_texture, isCube() ? faceIndex : 0, std::nullopt, std::nullopt,
                            ignoreCameraViewport, 0, static_cast<int>(layer));
  }
}

void RenderTargetTexture::unbindFrameBuffer(Engine* engine, unsigned int faceIndex)
{
  if (!_texture) {
    return;
  }
  engine->unBindFramebuffer(_texture, isCube, [&, faceIndex]() {
    auto i_faceIndex = static_cast<int>(faceIndex);
    onAfterRenderObservable.notifyObservers(&i_faceIndex);
  });
}

void RenderTargetTexture::renderToTarget(unsigned int faceIndex, bool useCameraPostProcess,
                                         bool dumpForDebug, unsigned int layer,
                                         const CameraPtr& camera)
{
  auto scene = getScene();

  if (!scene) {
    return;
  }

  auto engine = scene->getEngine();

  if (!_texture) {
    return;
  }

  // Bind
  if (_postProcessManager) {
    _postProcessManager->_prepareFrame(_texture, _postProcesses);
  }
  else if (!useCameraPostProcess || !scene->postProcessManager->_prepareFrame(_texture)) {
    _bindFrameBuffer(faceIndex, layer);
  }

  _faceIndex = static_cast<int>(faceIndex);

  if (is2DArray) {
    auto _layer = static_cast<int>(layer);
    onBeforeRenderObservable.notifyObservers(&_layer);
  }
  else {
    onBeforeRenderObservable.notifyObservers(&_faceIndex);
  }

  // Get the list of meshes to render
  std::vector<AbstractMesh*> currentRenderList;
  auto defaultRenderList = !renderList().empty() ? renderList() : scene->getActiveMeshes();
  auto defaultRenderListLength
    = !renderList().empty() ? renderList().size() : scene->getActiveMeshes().size();

  if (getCustomRenderList) {
    currentRenderList = getCustomRenderList(is2DArray ? layer : faceIndex, defaultRenderList,
                                            defaultRenderListLength);
  }

  if (currentRenderList.empty()) {
    // No custom render list provided, we prepare the rendering for the default list, but check
    // first if we did not already performed the preparation before so as to avoid re-doing it
    // several times
    if (!_defaultRenderListPrepared) {
      _prepareRenderingManager(defaultRenderList, defaultRenderListLength, camera,
                               renderList().empty());
      _defaultRenderListPrepared = true;
    }
    currentRenderList = defaultRenderList;
  }
  else {
    // Prepare the rendering for the custom render list provided
    _prepareRenderingManager(currentRenderList, defaultRenderListLength, camera, false);
  }

  // Clear
  if (onClearObservable.hasObservers()) {
    onClearObservable.notifyObservers(engine);
  }
  else {
    engine->clear(clearColor.has_value() ? *clearColor : scene->clearColor, true, true, true);
  }

  if (!_doNotChangeAspectRatio) {
    scene->updateTransformMatrix(true);
  }

  // Before Camera Draw
  for (const auto& step : scene->_beforeRenderTargetDrawStage) {
    step.action(std::static_pointer_cast<RenderTargetTexture>(shared_from_this()));
  }

  // Render
  _renderingManager->render(customRenderFunction, currentRenderList, renderParticles,
                            renderSprites);

  // After Camera Draw
  for (const auto& step : scene->_afterRenderTargetDrawStage) {
    step.action(std::static_pointer_cast<RenderTargetTexture>(shared_from_this()));
  }

  if (_postProcessManager) {
    _postProcessManager->_finalizeFrame(false, _texture, faceIndex, _postProcesses,
                                        ignoreCameraViewport);
  }
  else if (useCameraPostProcess) {
    scene->postProcessManager->_finalizeFrame(false, _texture, static_cast<unsigned>(_faceIndex));
  }

  if (!_doNotChangeAspectRatio) {
    scene->updateTransformMatrix(true);
  }

  // Dump ?
  if (dumpForDebug) {
    Tools::DumpFramebuffer(getRenderWidth(), getRenderHeight(), engine);
  }

  // Unbind
  if (!isCube || faceIndex == 5) {
    if (isCube) {
      if (faceIndex == 5) {
        engine->generateMipMapsForCubemap(_texture);
      }
    }

    unbindFrameBuffer(engine, faceIndex);
  }
  else {
    onAfterRenderObservable.notifyObservers(&_faceIndex);
  }
}

void RenderTargetTexture::setRenderingOrder(
  unsigned int renderingGroupId,
  const std::function<int(const SubMesh* a, const SubMesh* b)>& opaqueSortCompareFn,
  const std::function<int(const SubMesh* a, const SubMesh* b)>& alphaTestSortCompareFn,
  const std::function<int(const SubMesh* a, const SubMesh* b)>& transparentSortCompareFn)
{
  _renderingManager->setRenderingOrder(renderingGroupId, opaqueSortCompareFn,
                                       alphaTestSortCompareFn, transparentSortCompareFn);
}

void RenderTargetTexture::setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                                            bool autoClearDepthStencil)
{
  _renderingManager->setRenderingAutoClearDepthStencil(renderingGroupId, autoClearDepthStencil);
  _renderingManager->_useSceneAutoClearSetup = false;
}

RenderTargetTexturePtr RenderTargetTexture::clone()
{
  auto textureSize = getSize();
  auto newTexture  = RenderTargetTexture::New(
    name,                                                                //
    RenderTargetSize{textureSize.width, textureSize.height}, getScene(), //
    _renderTargetOptions.generateMipMaps.value(),                        //
    _doNotChangeAspectRatio,                                             //
    _renderTargetOptions.type.value(),                                   //
    isCube,                                                              //
    _renderTargetOptions.samplingMode.value(),                           //
    _renderTargetOptions.generateDepthBuffer.value(),                    //
    _renderTargetOptions.generateStencilBuffer.value()                   //
  );

  // Base texture
  newTexture->hasAlpha = hasAlpha();
  newTexture->level    = level;

  // RenderTarget Texture
  newTexture->coordinatesMode = coordinatesMode();
  if (!renderList().empty()) {
    newTexture->renderList = renderList();
  }

  return newTexture;
}

json RenderTargetTexture::serialize() const
{
  return nullptr;
}

void RenderTargetTexture::disposeFramebufferObjects()
{
  auto objBuffer = getInternalTexture();
  auto scene     = getScene();
  if (objBuffer && scene) {
    scene->getEngine()->_releaseFramebufferObjects(objBuffer);
  }
}

void RenderTargetTexture::dispose()
{
  onResizeObservable.clear();
  onClearObservable.clear();
  onAfterRenderObservable.clear();
  onAfterUnbindObservable.clear();
  onBeforeBindObservable.clear();
  onBeforeRenderObservable.clear();

  if (_postProcessManager) {
    _postProcessManager->dispose();
    _postProcessManager.reset(nullptr);
  }

  clearPostProcesses(true);

  if (_resizeObserver) {
    getScene()->getEngine()->onResizeObservable.remove(_resizeObserver);
    _resizeObserver = nullptr;
  }

  renderList().clear();

  // Remove from custom render targets
  auto scene = getScene();

  if (!scene) {
    return;
  }

  stl_util::remove_vector_elements_equal_sharedptr(scene->customRenderTargets, this);

  for (const auto& camera : scene->cameras) {
    stl_util::remove_vector_elements_equal_sharedptr(camera->customRenderTargets, this);
  }

  if (depthStencilTexture()) {
    getScene()->getEngine()->_releaseTexture(depthStencilTexture);
  }

  Texture::dispose();
}

void RenderTargetTexture::_rebuild()
{
  if (refreshRate() == RenderTargetTexture::REFRESHRATE_RENDER_ONCE) {
    refreshRate = RenderTargetTexture::REFRESHRATE_RENDER_ONCE;
  }

  if (_postProcessManager) {
    _postProcessManager->_rebuild();
  }
}

void RenderTargetTexture::freeRenderingGroups()
{
  if (_renderingManager) {
    _renderingManager->freeRenderingGroups();
  }
}

unsigned int RenderTargetTexture::getViewCount() const
{
  return 1;
}

} // end of namespace BABYLON
