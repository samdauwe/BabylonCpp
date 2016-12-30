#include <babylon/materials/textures/render_target_texture.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/math/matrix.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/rendering/rendering_manager.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

RenderTargetTexture::RenderTargetTexture(
  const std::string& iName, const ISize& size, Scene* scene,
  bool generateMipMaps, bool doNotChangeAspectRatio, unsigned int type,
  bool iIsCube, unsigned int samplingMode, bool generateDepthBuffer,
  bool generateStencilBuffer)

    : Texture{"", scene, !generateMipMaps}
    , renderParticles{true}
    , renderSprites{false}
    , coordinatesMode{Texture::PROJECTION_MODE}
    , _generateMipMaps{generateMipMaps}
    , _size{size}
    , _doNotChangeAspectRatio{doNotChangeAspectRatio}
    , _currentRefreshId{-1}
    , _refreshRate{1}
{
  name           = iName;
  isRenderTarget = true;
  isCube         = iIsCube;

  if (samplingMode == Texture::NEAREST_SAMPLINGMODE) {
    wrapU = Texture::CLAMP_ADDRESSMODE;
    wrapV = Texture::CLAMP_ADDRESSMODE;
  }

  if (iIsCube) {
    RenderTargetCubeTextureOptions options;
    options.generateMipMaps       = generateMipMaps;
    options.samplingMode          = samplingMode;
    options.generateDepthBuffer   = generateDepthBuffer;
    options.generateStencilBuffer = generateStencilBuffer;

    _texture = scene->getEngine()->createRenderTargetCubeTexture(size, options);
    coordinatesMode = Texture::INVCUBIC_MODE;
    _textureMatrix  = std_util::make_unique<Matrix>(Matrix::Identity());
  }
  else {
    RenderTargetTextureOptions options;
    options.generateMipMaps       = generateMipMaps;
    options.type                  = type;
    options.samplingMode          = samplingMode;
    options.generateDepthBuffer   = generateDepthBuffer;
    options.generateStencilBuffer = generateStencilBuffer;

    _texture = scene->getEngine()->createRenderTargetTexture(size, options);
  }

  // Rendering groups
  _renderingManager = std_util::make_unique<RenderingManager>(scene);
}

RenderTargetTexture::~RenderTargetTexture()
{
}

void RenderTargetTexture::setOnAfterUnbind(
  const std::function<void()>& callback)
{
  if (_onAfterUnbindObserver) {
    onAfterUnbindObservable.remove(_onAfterUnbindObserver);
  }
  _onAfterUnbindObserver = onAfterUnbindObservable.add(callback);
}

void RenderTargetTexture::setOnBeforeRender(
  const std::function<void(int faceIndex)>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
}

void RenderTargetTexture::setOnAfterRender(
  const std::function<void(int faceIndex)>& callback)
{
  if (_onAfterRenderObserver) {
    onAfterRenderObservable.remove(_onAfterRenderObserver);
  }
  _onAfterRenderObserver = onAfterRenderObservable.add(callback);
}

void RenderTargetTexture::setOnClear(
  const std::function<void(Engine* engine)>& callback)
{
  if (_onClearObserver) {
    onClearObservable.remove(_onClearObserver);
  }
  _onClearObserver = onClearObservable.add(callback);
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

  resize(newSize, _generateMipMaps);
}

Matrix* RenderTargetTexture::getReflectionTextureMatrix()
{
  if (isCube) {
    return _textureMatrix.get();
  }

  return Texture::getReflectionTextureMatrix();
}

void RenderTargetTexture::resize(const ISize& size, bool generateMipMaps)
{
  releaseInternalTexture();
  if (isCube) {
    RenderTargetCubeTextureOptions options;
    _texture
      = getScene()->getEngine()->createRenderTargetCubeTexture(size, options);
  }
  else {
    RenderTargetTextureOptions options;
    options.generateMipMaps = generateMipMaps;
    _texture
      = getScene()->getEngine()->createRenderTargetTexture(size, options);
  }
}

void RenderTargetTexture::render(bool useCameraPostProcess, bool dumpForDebug)
{
  auto scene = getScene();

  if (activeCamera && activeCamera != scene->activeCamera) {
    scene->setTransformMatrix(activeCamera->getViewMatrix(),
                              activeCamera->getProjectionMatrix(true));
  }

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

    /*for (auto& mesh : getScene()->meshes) {
      if (renderListPredicate(mesh)) {
        renderList.emplace_back(mesh);
      }
    }*/
  }

  if (renderList.empty()) {
    return;
  }

  // Prepare renderingManager
  _renderingManager->reset();

  auto& currentRenderList      = renderList;
  auto currentRenderListLength = currentRenderList.size();
  if (renderList.empty()) {
    auto& activeMeshes = scene->getActiveMeshes();
    for (auto& mesh : activeMeshes) {
      currentRenderList.emplace_back(dynamic_cast<AbstractMesh*>(mesh));
    }
  }

  for (auto& mesh : currentRenderList) {
    if (mesh) {
      if (!mesh->isReady()) {
        // Reset _currentRefreshId
        resetRefreshCounter();
        continue;
      }
      if (mesh->isEnabled() && mesh->isVisible && (!mesh->subMeshes.empty())
          && ((mesh->layerMask & scene->activeCamera->layerMask) != 0)) {
        mesh->_activate(scene->getRenderId());
        for (auto& subMesh : mesh->subMeshes) {
          scene->_activeIndices.addCount(subMesh->indexCount, false);
          //_renderingManager->dispatch(subMesh);
        }
      }
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

  scene->resetCachedMaterial();
}

void RenderTargetTexture::renderToTarget(
  unsigned int faceIndex, const std::vector<AbstractMesh*>& currentRenderList,
  size_t /*currentRenderListLength*/, bool useCameraPostProcess,
  bool dumpForDebug)

{
  auto scene  = getScene();
  auto engine = scene->getEngine();

  // Bind
  if (!useCameraPostProcess
      || !scene->postProcessManager->_prepareFrame(_texture)) {
    if (isCube) {
      engine->bindFramebuffer(_texture, faceIndex);
    }
    else {
      engine->bindFramebuffer(_texture);
    }
  }

  int _faceIndex = static_cast<int>(faceIndex);
  onBeforeRenderObservable.notifyObservers(&_faceIndex);

  // Clear
  if (onClearObservable.hasObservers()) {
    onClearObservable.notifyObservers(engine);
  }
  else {
    engine->clear(scene->clearColor, true, true, true);
  }

  if (!_doNotChangeAspectRatio) {
    scene->updateTransformMatrix(true);
  }

  // Render
  _renderingManager->render(customRenderFunction, currentRenderList,
                            renderParticles, renderSprites);

  if (useCameraPostProcess) {
    scene->postProcessManager->_finalizeFrame(false, _texture, _faceIndex);
  }

  if (!_doNotChangeAspectRatio) {
    scene->updateTransformMatrix(true);
  }

  onAfterRenderObservable.notifyObservers(&_faceIndex);

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

    engine->unBindFramebuffer(_texture, isCube);
  }
}

void RenderTargetTexture::setRenderingOrder(
  int renderingGroupId,
  const std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn,
  const std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn,
  const std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn)
{
  _renderingManager->setRenderingOrder(renderingGroupId, opaqueSortCompareFn,
                                       alphaTestSortCompareFn,
                                       transparentSortCompareFn);
}

void RenderTargetTexture::setRenderingAutoClearDepthStencil(
  int renderingGroupId, bool autoClearDepthStencil)
{
  _renderingManager->setRenderingAutoClearDepthStencil(renderingGroupId,
                                                       autoClearDepthStencil);
}

std::unique_ptr<RenderTargetTexture> RenderTargetTexture::clone() const
{
  auto textureSize = getSize();
  auto newTexture  = std_util::make_unique<RenderTargetTexture>(
    name, textureSize, getScene(), _generateMipMaps);

  // Base texture
  newTexture->hasAlpha = hasAlpha;
  newTexture->level    = level;

  // RenderTarget Texture
  newTexture->coordinatesMode = coordinatesMode;
  newTexture->renderList      = renderList;

  return newTexture;
}

Json::object RenderTargetTexture::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
