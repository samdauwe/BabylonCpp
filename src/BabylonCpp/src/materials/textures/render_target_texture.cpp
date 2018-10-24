#include <babylon/materials/textures/render_target_texture.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/depth_texture_creation_options.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/math/matrix.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/particles/particle_system.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/rendering/rendering_manager.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

RenderTargetTexture::RenderTargetTexture(
  const std::string& iName, const ISize& size, Scene* scene,
  bool generateMipMaps, bool doNotChangeAspectRatio, unsigned int type,
  bool iIsCube, unsigned int samplingMode, bool generateDepthBuffer,
  bool generateStencilBuffer, bool isMulti, unsigned int format)
    : Texture{"", scene, !generateMipMaps}
    , renderListPredicate{nullptr}
    , renderList{this, &RenderTargetTexture::get_renderList,
                 &RenderTargetTexture::set_renderList}
    , renderParticles{true}
    , renderSprites{false}
    , activeCamera{nullptr}
    , ignoreCameraViewport{false}
    , _generateMipMaps{generateMipMaps ? true : false}
    , boundingBoxPosition{Vector3::Zero()}
    , depthStencilTexture{nullptr}
    , onAfterUnbind{this, &RenderTargetTexture::set_onAfterUnbind}
    , onBeforeRender{this, &RenderTargetTexture::set_onBeforeRender}
    , onAfterRender{this, &RenderTargetTexture::set_onAfterRender}
    , onClear{this, &RenderTargetTexture::set_onClear}
    , samples{this, &RenderTargetTexture::get_samples,
              &RenderTargetTexture::set_samples}
    , refreshRate{this, &RenderTargetTexture::get_refreshRate,
                  &RenderTargetTexture::set_refreshRate}
    , _sizeRatio{0.f}
    , _doNotChangeAspectRatio{doNotChangeAspectRatio}
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
{
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

  _resizeObserver
    = scene->getEngine()->onResizeObservable.add([](Engine*, EventState&) {});

  // Rendering groups
  _renderingManager = std::make_unique<RenderingManager>(scene);
  _renderingManager->_useSceneAutoClearSetup = true;

  if (isMulti) {
    return;
  }

  _renderTargetOptions.generateMipMaps       = generateMipMaps;
  _renderTargetOptions.type                  = type;
  _renderTargetOptions.format                = format;
  _renderTargetOptions.samplingMode          = samplingMode;
  _renderTargetOptions.generateDepthBuffer   = generateDepthBuffer;
  _renderTargetOptions.generateStencilBuffer = generateStencilBuffer;

  if (samplingMode == TextureConstants::NEAREST_SAMPLINGMODE) {
    wrapU = TextureConstants::CLAMP_ADDRESSMODE;
    wrapV = TextureConstants::CLAMP_ADDRESSMODE;
  }

  if (iIsCube) {
    _texture = scene->getEngine()->createRenderTargetCubeTexture(
      getRenderSize(), _renderTargetOptions);
    coordinatesMode = TextureConstants::INVCUBIC_MODE;
    _textureMatrix  = std::make_unique<Matrix>(Matrix::Identity());
  }
  else {
    _texture = scene->getEngine()->createRenderTargetTexture(
      size, _renderTargetOptions);
  }
}

RenderTargetTexture::~RenderTargetTexture()
{
}

void RenderTargetTexture::_onRatioRescale()
{
  if (_sizeRatio != 0.f) {
    resize(_initialSizeParameter);
  }
}

std::vector<AbstractMeshPtr>& RenderTargetTexture::get_renderList()
{
  return _renderList;
}

void RenderTargetTexture::set_renderList(
  const std::vector<AbstractMeshPtr>& value)
{
  _renderList = value;
}

void RenderTargetTexture::set_boundingBoxSize(
  const std::optional<Vector3>& value)
{
  if (_boundingBoxSize && (*_boundingBoxSize).equals(*value)) {
    return;
  }
  _boundingBoxSize = value;
  auto scene       = getScene();
  if (scene) {
    scene->markAllMaterialsAsDirty(Material::TextureDirtyFlag());
  }
}

std::optional<Vector3>& RenderTargetTexture::get_boundingBoxSize()
{
  return _boundingBoxSize;
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

RenderTargetCreationOptions& RenderTargetTexture::renderTargetOptions()
{
  return _renderTargetOptions;
}

const RenderTargetCreationOptions&
RenderTargetTexture::renderTargetOptions() const
{
  return _renderTargetOptions;
}

void RenderTargetTexture::createDepthStencilTexture(int comparisonFunction,
                                                    bool bilinearFiltering,
                                                    bool generateStencil)
{
  if (!getScene()) {
    return;
  }

  auto engine = getScene()->getEngine();
  DepthTextureCreationOptions options;
  options.bilinearFiltering  = bilinearFiltering;
  options.comparisonFunction = comparisonFunction;
  options.generateStencil    = generateStencil;
  options.isCube             = isCube;
  depthStencilTexture = engine->createDepthStencilTexture(_size, options);

  engine->setFrameBufferDepthStencilTexture(this);
}

void RenderTargetTexture::_processSizeParameter(const ISize& size)
{
  _size = size;
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

  _samples
    = scene->getEngine()->updateRenderTargetTextureSampleCount(_texture, value);
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

  auto it
    = std::find(_postProcesses.begin(), _postProcesses.end(), postProcess);

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

ISize& RenderTargetTexture::getRenderSize()
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

bool RenderTargetTexture::canRescale() const
{
  return true;
}

void RenderTargetTexture::scale(float ratio)
{
  ISize newSize = _size * ratio;
  resize(newSize);
}

Matrix* RenderTargetTexture::getReflectionTextureMatrix()
{
  if (isCube) {
    return _textureMatrix.get();
  }

  return Texture::getReflectionTextureMatrix();
}

void RenderTargetTexture::resize(const ISize& size)
{
  releaseInternalTexture();

  auto scene = getScene();
  if (!scene) {
    return;
  }

  _processSizeParameter(size);

  if (isCube) {
    _texture = scene->getEngine()->createRenderTargetCubeTexture(
      getRenderSize(), _renderTargetOptions);
  }
  else {
    _texture = scene->getEngine()->createRenderTargetTexture(
      _size, _renderTargetOptions);
  }
}

void RenderTargetTexture::render(bool useCameraPostProcess, bool dumpForDebug)
{
  auto scene = getScene();
  if (!scene) {
    return;
  }

  auto engine = scene->getEngine();

  if (!_waitingRenderList.empty()) {
    renderList().clear();
    for (auto& id : _waitingRenderList) {
      auto mesh = scene->getMeshByID(id);
      if (mesh) {
        renderList().emplace_back(mesh);
      }
    }

    _waitingRenderList.clear();
  }

  // Is predicate defined?
  if (renderListPredicate) {
    renderList().clear(); // Clear previous renderList

    auto scene = getScene();
    if (!scene) {
      return;
    }

    auto& sceneMeshes = scene->meshes;

    for (auto& mesh : sceneMeshes) {
      if (renderListPredicate(mesh.get())) {
        renderList().emplace_back(mesh);
      }
    }
  }

  onBeforeBindObservable.notifyObservers(this);

  // Set custom projection.
  // Needs to be before binding to prevent changing the aspect ratio.
  CameraPtr camera = nullptr;
  if (activeCamera) {
    camera = activeCamera;
    engine->setViewport(activeCamera->viewport, getRenderWidth(),
                        getRenderHeight());

    if (activeCamera != scene->activeCamera) {
      scene->setTransformMatrix(activeCamera->getViewMatrix(),
                                activeCamera->getProjectionMatrix(true));
    }
  }
  else {
    camera = scene->activeCamera;
    if (camera) {
      engine->setViewport(scene->activeCamera->viewport, getRenderWidth(),
                          getRenderHeight());
    }
  }

  // Prepare renderingManager
  _renderingManager->reset();

  auto& currentRenderList = renderList;
  if (renderList().empty()) {
    auto& activeMeshes = scene->getActiveMeshes();
    for (auto& mesh : activeMeshes) {
      currentRenderList().emplace_back(
        std::static_pointer_cast<AbstractMesh>(mesh));
    }
  }
  auto currentRenderListLength = currentRenderList().size();
  auto sceneRenderId           = scene->getRenderId();
  for (auto& mesh : currentRenderList()) {
    if (mesh) {
      if (!mesh->isReady(refreshRate() == 0)) {
        resetRefreshCounter();
        continue;
      }

      mesh->_preActivateForIntermediateRendering(sceneRenderId);

      bool isMasked;
      if (renderList().empty() && camera) {
        isMasked = ((mesh->layerMask() & camera->layerMask) == 0);
      }
      else {
        isMasked = false;
      }

      if (mesh->isEnabled() && mesh->isVisible && (!mesh->subMeshes.empty())
          && !isMasked) {
        mesh->_activate(scene->getRenderId());
        for (auto& subMesh : mesh->subMeshes) {
          scene->_activeIndices.addCount(subMesh->indexCount, false);
          _renderingManager->dispatch(subMesh, mesh.get());
        }
      }
    }
  }

  for (auto& particleSystem : scene->particleSystems) {
    if (!particleSystem->isStarted() || !particleSystem->hasEmitter()
        || !(std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)
             && std::get<AbstractMeshPtr>(particleSystem->emitter)
                  ->isEnabled())) {
      continue;
    }
    if (stl_util::index_of(currentRenderList(),
                           std::get<AbstractMeshPtr>(particleSystem->emitter))
        >= 0) {
      _renderingManager->dispatchParticles(particleSystem.get());
    }
  }

  if (isCube) {
    for (unsigned int face = 0; face < 6; ++face) {
      renderToTarget(face, currentRenderList(), currentRenderListLength,
                     useCameraPostProcess, dumpForDebug);
      scene->incrementRenderId();
      scene->resetCachedMaterial();
    }
  }
  else {
    renderToTarget(0, currentRenderList(), currentRenderListLength,
                   useCameraPostProcess, dumpForDebug);
  }

  onAfterUnbindObservable.notifyObservers(this);

  if (scene->activeCamera) {
    if (activeCamera && activeCamera != scene->activeCamera) {
      scene->setTransformMatrix(scene->activeCamera->getViewMatrix(),
                                scene->activeCamera->getProjectionMatrix(true));
    }
    engine->setViewport(scene->activeCamera->viewport);
  }

  scene->resetCachedMaterial();
}

int RenderTargetTexture::_bestReflectionRenderTargetDimension(
  int renderDimension, float scale) const
{
  const int minimum = 128;
  const float x     = renderDimension * scale;
  const int curved  = Tools::NearestPOT(
    static_cast<int>(x + (minimum * minimum / (minimum + x))));

  // Ensure we don't exceed the render dimension (while staying POT)
  return std::min(Tools::FloorPOT(renderDimension), curved);
}

void RenderTargetTexture::unbindFrameBuffer(Engine* engine,
                                            unsigned int faceIndex)
{
  if (!_texture) {
    return;
  }
  engine->unBindFramebuffer(_texture, isCube, [&, faceIndex]() {
    auto _faceIndex = static_cast<int>(faceIndex);
    onAfterRenderObservable.notifyObservers(&_faceIndex);
  });
}

void RenderTargetTexture::renderToTarget(
  unsigned int faceIndex, const std::vector<AbstractMeshPtr>& currentRenderList,
  size_t /*currentRenderListLength*/, bool useCameraPostProcess,
  bool dumpForDebug)

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
  else if (!useCameraPostProcess
           || !scene->postProcessManager->_prepareFrame(_texture)) {
    if (_texture) {
      std::optional<unsigned int> faceIndexVal = std::nullopt;
      if (isCube) {
        faceIndexVal = faceIndex;
      }
      engine->bindFramebuffer(_texture, faceIndexVal, std::nullopt,
                              std::nullopt, ignoreCameraViewport,
                              depthStencilTexture ? depthStencilTexture.get() :
                                                    nullptr);
    }
  }

  _faceIndex = static_cast<int>(faceIndex);
  onBeforeRenderObservable.notifyObservers(&_faceIndex);

  // Clear
  if (onClearObservable.hasObservers()) {
    onClearObservable.notifyObservers(engine);
  }
  else {
    engine->clear(clearColor.has_value() ? *clearColor : scene->clearColor,
                  true, true, true);
  }

  if (!_doNotChangeAspectRatio) {
    scene->updateTransformMatrix(true);
  }

  // Render
  _renderingManager->render(customRenderFunction, currentRenderList,
                            renderParticles, renderSprites);

  if (_postProcessManager) {
    _postProcessManager->_finalizeFrame(false, _texture, faceIndex,
                                        _postProcesses, ignoreCameraViewport);
  }
  else if (useCameraPostProcess) {
    scene->postProcessManager->_finalizeFrame(
      false, _texture, static_cast<unsigned>(_faceIndex));
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
  const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
    opaqueSortCompareFn,
  const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
    alphaTestSortCompareFn,
  const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
    transparentSortCompareFn)
{
  _renderingManager->setRenderingOrder(renderingGroupId, opaqueSortCompareFn,
                                       alphaTestSortCompareFn,
                                       transparentSortCompareFn);
}

void RenderTargetTexture::setRenderingAutoClearDepthStencil(
  unsigned int renderingGroupId, bool autoClearDepthStencil)
{
  _renderingManager->setRenderingAutoClearDepthStencil(renderingGroupId,
                                                       autoClearDepthStencil);
  _renderingManager->_useSceneAutoClearSetup = false;
}

RenderTargetTexturePtr RenderTargetTexture::clone()
{
  auto textureSize = getSize();
  auto newTexture
    = RenderTargetTexture::New(name,                                      //
                               textureSize, getScene(),                   //
                               _renderTargetOptions.generateMipMaps,      //
                               _doNotChangeAspectRatio,                   //
                               _renderTargetOptions.type,                 //
                               isCube,                                    //
                               _renderTargetOptions.samplingMode,         //
                               _renderTargetOptions.generateDepthBuffer,  //
                               _renderTargetOptions.generateStencilBuffer //
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

Json::object RenderTargetTexture::serialize() const
{
  return Json::object();
}

void RenderTargetTexture::disposeFramebufferObjects()
{
  auto objBuffer = getInternalTexture();
  auto scene     = getScene();
  if (objBuffer && scene) {
    scene->getEngine()->_releaseFramebufferObjects(objBuffer.get());
  }
}

void RenderTargetTexture::dispose()
{
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

  scene->customRenderTargets.erase(
    std::remove_if(
      scene->customRenderTargets.begin(), scene->customRenderTargets.end(),
      [this](const RenderTargetTexturePtr& renderTargetTexturePtr) {
        return renderTargetTexturePtr.get() == this;
      }),
    scene->customRenderTargets.end());

  for (auto& camera : scene->cameras) {
    camera->customRenderTargets.erase(
      std::remove_if(
        camera->customRenderTargets.begin(), camera->customRenderTargets.end(),
        [this](const RenderTargetTexturePtr& renderTargetTexturePtr) {
          return renderTargetTexturePtr.get() == this;
        }),
      camera->customRenderTargets.end());
  }

  Texture::dispose();
}

void RenderTargetTexture::_rebuild()
{
  if (refreshRate() == RenderTargetTexture::REFRESHRATE_RENDER_ONCE()) {
    refreshRate = RenderTargetTexture::REFRESHRATE_RENDER_ONCE();
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

} // end of namespace BABYLON
