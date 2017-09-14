#include <babylon/materials/textures/render_target_texture.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
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
  const string_t& iName, const ISize& size, Scene* scene, bool generateMipMaps,
  bool doNotChangeAspectRatio, unsigned int type, bool iIsCube,
  unsigned int samplingMode, bool generateDepthBuffer,
  bool generateStencilBuffer, bool isMulti)

    : Texture{"", scene, !generateMipMaps}
    , renderParticles{true}
    , renderSprites{false}
    , coordinatesMode{TextureConstants::PROJECTION_MODE}
    , activeCamera{nullptr}
    , ignoreCameraViewport{false}
    , _generateMipMaps{generateMipMaps}
    , _size{size}
    , _doNotChangeAspectRatio{doNotChangeAspectRatio}
    , _currentRefreshId{-1}
    , _refreshRate{1}
    , _samples{1}
    , _postProcessManager{nullptr}
{
  name           = iName;
  isRenderTarget = true;
  isCube         = iIsCube;

  // Rendering groups
  _renderingManager = ::std::make_unique<RenderingManager>(scene);

  if (isMulti) {
    return;
  }

  _renderTargetOptions.generateMipMaps       = generateMipMaps;
  _renderTargetOptions.type                  = type;
  _renderTargetOptions.samplingMode          = samplingMode;
  _renderTargetOptions.generateDepthBuffer   = generateDepthBuffer;
  _renderTargetOptions.generateStencilBuffer = generateStencilBuffer;

  if (samplingMode == TextureConstants::NEAREST_SAMPLINGMODE) {
    wrapU = TextureConstants::CLAMP_ADDRESSMODE;
    wrapV = TextureConstants::CLAMP_ADDRESSMODE;
  }

  if (iIsCube) {
    _texture = scene->getEngine()->createRenderTargetCubeTexture(
      size, _renderTargetOptions);
    coordinatesMode = TextureConstants::INVCUBIC_MODE;
    _textureMatrix  = ::std::make_unique<Matrix>(Matrix::Identity());
  }
  else {
    _texture = scene->getEngine()->createRenderTargetTexture(
      size, _renderTargetOptions);
  }
}

RenderTargetTexture::~RenderTargetTexture()
{
}

void RenderTargetTexture::setOnAfterUnbind(
  const ::std::function<void()>& callback)
{
  if (_onAfterUnbindObserver) {
    onAfterUnbindObservable.remove(_onAfterUnbindObserver);
  }
  _onAfterUnbindObserver = onAfterUnbindObservable.add(callback);
}

void RenderTargetTexture::setOnBeforeRender(
  const ::std::function<void(int faceIndex)>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
}

void RenderTargetTexture::setOnAfterRender(
  const ::std::function<void(int faceIndex)>& callback)
{
  if (_onAfterRenderObserver) {
    onAfterRenderObservable.remove(_onAfterRenderObserver);
  }
  _onAfterRenderObserver = onAfterRenderObservable.add(callback);
}

void RenderTargetTexture::setOnClear(
  const ::std::function<void(Engine* engine)>& callback)
{
  if (_onClearObserver) {
    onClearObservable.remove(_onClearObserver);
  }
  _onClearObserver = onClearObservable.add(callback);
}

IRenderTargetOptions& RenderTargetTexture::renderTargetOptions()
{
  return _renderTargetOptions;
}

const IRenderTargetOptions& RenderTargetTexture::renderTargetOptions() const
{
  return _renderTargetOptions;
}

unsigned int RenderTargetTexture::samples() const
{
  return _samples;
}

void RenderTargetTexture::setSamples(unsigned int value)
{
  if (_samples == value) {
    return;
  }

  _samples = getScene()->getEngine()->updateRenderTargetTextureSampleCount(
    _texture, value);
}

void RenderTargetTexture::resetRefreshCounter()
{
  _currentRefreshId = -1;
}

int RenderTargetTexture::refreshRate() const
{
  return _refreshRate;
}

void RenderTargetTexture::setRefreshRate(int value)
{
  _refreshRate = value;
  resetRefreshCounter();
}

void RenderTargetTexture::addPostProcess(PostProcess* postProcess)
{
  if (!_postProcessManager) {
    _postProcessManager = ::std::make_unique<PostProcessManager>(getScene());
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

void RenderTargetTexture::removePostProcess(PostProcess* postProcess)
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

bool RenderTargetTexture::canRescale() const
{
  return true;
}

void RenderTargetTexture::scale(float ratio)
{
  ISize newSize = {static_cast<int>(static_cast<float>(_size.width) * ratio),
                   static_cast<int>(static_cast<float>(_size.height) * ratio)};

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
  if (isCube) {
    _texture = getScene()->getEngine()->createRenderTargetCubeTexture(
      size, _renderTargetOptions);
  }
  else {
    _texture = getScene()->getEngine()->createRenderTargetTexture(
      size, _renderTargetOptions);
  }
}

void RenderTargetTexture::render(bool useCameraPostProcess, bool dumpForDebug)
{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  if (!_waitingRenderList.empty()) {
    renderList.clear();
    for (auto& id : _waitingRenderList) {
      renderList.emplace_back(scene->getMeshByID(id));
    }

    _waitingRenderList.clear();
  }

  // Is predicate defined?
  if (renderListPredicate) {
    renderList.clear(); // Clear previous renderList

    for (auto& mesh : getScene()->meshes) {
      if (renderListPredicate(mesh.get())) {
        renderList.emplace_back(mesh.get());
      }
    }
  }

  if (renderList.empty()) {
    return;
  }

  onBeforeBindObservable.notifyObservers(this);

  // Set custom projection.
  // Needs to be before binding to prevent changing the aspect ratio.
  Camera* camera = nullptr;
  if (activeCamera) {
    camera = activeCamera;
    engine->setViewport(activeCamera->viewport, _size.width, _size.height);

    if (activeCamera != scene->activeCamera) {
      scene->setTransformMatrix(activeCamera->getViewMatrix(),
                                activeCamera->getProjectionMatrix(true));
    }
  }
  else {
    camera = scene->activeCamera;
    engine->setViewport(scene->activeCamera->viewport, _size.width,
                        _size.height);
  }

  // Prepare renderingManager
  _renderingManager->reset();

  auto& currentRenderList = renderList;
  if (renderList.empty()) {
    auto& activeMeshes = scene->getActiveMeshes();
    for (auto& mesh : activeMeshes) {
      currentRenderList.emplace_back(dynamic_cast<AbstractMesh*>(mesh));
    }
  }
  auto currentRenderListLength = currentRenderList.size();

  for (auto& mesh : currentRenderList) {
    if (mesh) {
      if (!mesh->isReady()) {
        // Reset _currentRefreshId
        resetRefreshCounter();
        continue;
      }

      bool isMasked;
      if (renderList.empty()) {
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
          _renderingManager->dispatch(subMesh.get());
        }
      }
    }
  }

  for (auto& particleSystem : scene->particleSystems) {
    if (!particleSystem->isStarted() || !particleSystem->hasEmitter()
        || !(particleSystem->emitter.is<AbstractMesh*>()
             && particleSystem->emitter.get<AbstractMesh*>()->isEnabled())) {
      continue;
    }
    if (stl_util::index_of(currentRenderList,
                           particleSystem->emitter.get<AbstractMesh*>())
        >= 0) {
      _renderingManager->dispatchParticles(particleSystem.get());
    }
  }

  if (isCube) {
    for (unsigned int face = 0; face < 6; ++face) {
      renderToTarget(face, currentRenderList, currentRenderListLength,
                     useCameraPostProcess, dumpForDebug);
      scene->incrementRenderId();
      scene->resetCachedMaterial();
    }
  }
  else {
    renderToTarget(0, currentRenderList, currentRenderListLength,
                   useCameraPostProcess, dumpForDebug);
  }

  onAfterUnbindObservable.notifyObservers(this);

  if (activeCamera && activeCamera != scene->activeCamera) {
    scene->setTransformMatrix(scene->activeCamera->getViewMatrix(),
                              scene->activeCamera->getProjectionMatrix(true));
  }

  engine->setViewport(scene->activeCamera->viewport);

  scene->resetCachedMaterial();
}

void RenderTargetTexture::renderToTarget(
  unsigned int faceIndex, const vector_t<AbstractMesh*>& currentRenderList,
  size_t /*currentRenderListLength*/, bool useCameraPostProcess,
  bool dumpForDebug)

{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  // Bind
  if (_postProcessManager) {
    _postProcessManager->_prepareFrame(_texture, _postProcesses);
  }
  else if (!useCameraPostProcess
           || !scene->postProcessManager->_prepareFrame(_texture)) {
    engine->bindFramebuffer(_texture, isCube ? faceIndex : 0, 0, 0,
                            ignoreCameraViewport);
  }

  _faceIndex = static_cast<int>(faceIndex);
  onBeforeRenderObservable.notifyObservers(&_faceIndex);

  // Clear
  if (onClearObservable.hasObservers()) {
    onClearObservable.notifyObservers(engine);
  }
  else {
    engine->clear(!clearColor.isNull() ? *clearColor : scene->clearColor, true,
                  true, true);
  }

  if (!_doNotChangeAspectRatio) {
    scene->updateTransformMatrix(true);
  }

  // Render
  _renderingManager->render(customRenderFunction, currentRenderList,
                            renderParticles, renderSprites);

  if (_postProcessManager) {
    _postProcessManager->_finalizeFrame(false, _texture, faceIndex,
                                        _postProcesses);
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
    Tools::DumpFramebuffer(_size.width, _size.height, engine);
  }

  // Unbind
  if (!isCube || faceIndex == 5) {
    if (isCube) {
      if (faceIndex == 5) {
        engine->generateMipMapsForCubemap(_texture);
      }
    }

    engine->unBindFramebuffer(_texture, isCube, [this]() {
      onAfterRenderObservable.notifyObservers(&_faceIndex);
    });
  }
  else {
    onAfterRenderObservable.notifyObservers(&_faceIndex);
  }
}

void RenderTargetTexture::setRenderingOrder(
  unsigned int renderingGroupId,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn)
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
}

std::unique_ptr<RenderTargetTexture> RenderTargetTexture::clone() const
{
  auto textureSize = getSize();
  auto newTexture  = ::std::make_unique<RenderTargetTexture>(
    name,                                      //
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
  newTexture->setHasAlpha(hasAlpha());
  newTexture->level = level;

  // RenderTarget Texture
  newTexture->coordinatesMode = coordinatesMode;
  newTexture->renderList      = renderList;

  return newTexture;
}

Json::object RenderTargetTexture::serialize() const
{
  return Json::object();
}

void RenderTargetTexture::disposeFramebufferObjects()
{
  getScene()->getEngine()->_releaseFramebufferObjects(getInternalTexture());
}

void RenderTargetTexture::dispose(bool doNotRecurse)
{
  if (_postProcessManager) {
    _postProcessManager->dispose();
    _postProcessManager.reset(nullptr);
  }

  clearPostProcesses(true);

  renderList.clear();

  // Remove from custom render targets
  auto scene = getScene();

  stl_util::erase(scene->customRenderTargets, this);

  for (auto& camera : scene->cameras) {
    stl_util::erase(camera->customRenderTargets, this);
  }

  Texture::dispose(doNotRecurse);
}

void RenderTargetTexture::_rebuild()
{
  if (refreshRate() == RenderTargetTexture::REFRESHRATE_RENDER_ONCE) {
    setRefreshRate(RenderTargetTexture::REFRESHRATE_RENDER_ONCE);
  }
}

} // end of namespace BABYLON
