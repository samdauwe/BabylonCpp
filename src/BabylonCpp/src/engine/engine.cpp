#include <babylon/engine/engine.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/babylon_version.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/core/time.h>
#include <babylon/engine/instancing_attribute_info.h>
#include <babylon/instrumentation/_time_token.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/interfaces/iloading_screen.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/textures/imulti_render_target_options.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/states/_alpha_state.h>
#include <babylon/states/_depth_culling_state.h>
#include <babylon/states/_stencil_state.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

string_t Engine::Version()
{
  return BABYLONCPP_VERSION;
}

float Engine::CollisionsEpsilon    = 0.001f;
string_t Engine::CodeRepository    = "src/";
string_t Engine::ShadersRepository = "src/shaders/";
vector_t<Engine*> Engine::Instances{};

Engine::Engine(ICanvas* canvas, const EngineOptions& options)
    : forcePOTTextures{false}
    , isFullscreen{false}
    , isPointerLock{false}
    , cullBackFaces{true}
    , renderEvenInBackground{true}
    , preventCacheWipeBetweenFrames{false}
    , enableOfflineSupport{false}
    , _vrDisplayEnabled{false}
    , disableUniformBuffers{false}
    , disablePerformanceMonitorInBackground{false}
    , _depthCullingState{::std::make_unique<Internals::_DepthCullingState>()}
    , _stencilState{::std::make_unique<Internals::_StencilState>()}
    , _alphaState{::std::make_unique<Internals::_AlphaState>()}
    , _alphaMode{EngineConstants::ALPHA_DISABLE}
    , _currentEffect{nullptr}
    , _currentProgram{nullptr}
    , _cachedViewport{nullptr}
    , _cachedVertexBuffers{nullptr}
    , _cachedIndexBuffer{nullptr}
    , _cachedEffectForVertexBuffers{nullptr}
    , _currentRenderTarget{nullptr}
    , _currentFramebuffer{nullptr}
    , _vrExclusivePointerMode{false}
    , _gl{nullptr}
    , _renderingCanvas{canvas}
    , _windowIsBackground{false}
    , _webGLVersion{1.f}
    , _badOS{false}
    , _alphaTest{false}
    , _colorWrite{true}
    , _videoTextureSupported{false}
    , _renderingQueueLaunched{false}
    , _deterministicLockstep{false}
    , _lockstepMaxSteps{4}
    , _contextWasLost{false}
    , _doNotHandleContextLost{options.doNotHandleContextLost ? true : false}
    , _performanceMonitor{::std::make_unique<PerformanceMonitor>()}
    , _fps{60.f}
    , _deltaTime{0.f}
    , _cachedVertexArrayObject{nullptr}
    , _uintIndicesCurrentlySet{false}
    , _workingCanvas{nullptr}
    , _workingContext{nullptr}
    , _rescalePostProcess{nullptr}
    , _dummyFramebuffer{nullptr}
    , _vaoRecordInProgress{false}
    , _mustWipeVertexAttributes{false}
    , _emptyTexture{nullptr}
    , _emptyCubeTexture{nullptr}
    , _emptyTexture3D{nullptr}
{
  Engine::Instances.emplace_back(this);

  // Checks if some of the format renders first to allow the use of webgl
  // inspector.
  // auto renderToFullFloat = _canRenderToFloatTexture();
  // auto renderToHalfFloat = _canRenderToHalfFloatTexture();

  if (!canvas) {
    return;
  }

  // GL
  if (!options.disableWebGL2Support) {
    if (_gl) {
      _webGLVersion = 2.f;
    }
  }

  if (!_gl) {
    if (!canvas) {
      BABYLON_LOG_ERROR("Engine", "The provided canvas is null or undefined");
      return;
    }
    _gl = canvas->getContext3d(options);
  }

  if (!_gl) {
    BABYLON_LOG_ERROR("Engine", "GL not supported");
    return;
  }

  _onCanvasFocus = [this]() { onCanvasFocusObservable.notifyObservers(this); };

  _onCanvasBlur = [this]() { onCanvasBlurObservable.notifyObservers(this); };

  _onBlur = [this]() {
    if (disablePerformanceMonitorInBackground) {
      _performanceMonitor->disable();
    }
    _windowIsBackground = true;
  };

  _onFocus = [this]() {
    if (disablePerformanceMonitorInBackground) {
      _performanceMonitor->enable();
    }
    _windowIsBackground = false;
  };

  _onCanvasPointerOut
    = [this]() { onCanvasPointerOutObservable.notifyObservers(this); };

  // Viewport
  _hardwareScalingLevel = options.adaptToDeviceRatio ? 1 : 1;
  resize();

  _isStencilEnable = options.stencil ? true : false;
  _initGLContext();

  // if (!Engine.audioEngine) {
  //  Engine::audioEngine = new AudioEngine();
  //}

  // Default loading screen
  //_loadingScreen
  //  = ::std::make_unique<DefaultLoadingScreen>(_renderingCanvas);

  // Load WebVR Devices
  // if (options.autoEnableWebVR) {
  //  initWebVR();
  //}

  BABYLON_LOGF_INFO("Engine", "BabylonCpp engine (v%s) launched",
                    Engine::Version().c_str());
}

Engine::~Engine()
{
}

Engine* Engine::LastCreatedEngine()
{
  if (Engine::Instances.empty()) {
    return nullptr;
  }

  return Engine::Instances.back();
}

Scene* Engine::LastCreatedScene()
{
  auto lastCreatedEngine = Engine::LastCreatedEngine();
  if (!lastCreatedEngine) {
    return nullptr;
  }

  if (lastCreatedEngine->scenes.empty()) {
    return nullptr;
  }

  return lastCreatedEngine->scenes.back();
}

void Engine::MarkAllMaterialsAsDirty(
  unsigned int flag, const ::std::function<bool(Material* mat)>& predicate)
{
  for (auto& engine : Engine::Instances) {
    for (auto& scene : engine->scenes) {
      scene->markAllMaterialsAsDirty(flag, predicate);
    }
  }
}

vector_t<string_t>& Engine::texturesSupported()
{
  return _texturesSupported;
}

string_t Engine::textureFormatInUse() const
{
  return _textureFormatInUse;
}

Viewport* Engine::currentViewport() const
{
  return _cachedViewport;
}

InternalTexture* Engine::emptyTexture()
{
  if (!_emptyTexture) {
    _emptyTexture = createRawTexture(
      Uint8Array(4), 1, 1, EngineConstants::TEXTUREFORMAT_RGBA, false, false,
      TextureConstants::NEAREST_SAMPLINGMODE);
  }

  return _emptyTexture;
}

InternalTexture* Engine::emptyTexture3D()
{
  if (!_emptyTexture3D) {
    _emptyTexture3D = createRawTexture3D(
      Uint8Array(4), 1, 1, 1, EngineConstants::TEXTUREFORMAT_RGBA, false, false,
      TextureConstants::NEAREST_SAMPLINGMODE);
  }

  return _emptyTexture3D;
}

InternalTexture* Engine::emptyCubeTexture()
{
  if (!_emptyCubeTexture) {
#if 0
    Uint8Array faceData(4);
    vector_t<Uint8Array> cubeData{faceData, faceData, faceData,
                                  faceData, faceData, faceData};
    _emptyCubeTexture
      = createRawCubeTexture(cubeData, 1, EngineConstants::TEXTUREFORMAT_RGBA,
                             EngineConstants::TEXTURETYPE_UNSIGNED_INT, false,
                             false, TextureConstants::NEAREST_SAMPLINGMODE);
#endif
  }

  return _emptyCubeTexture.get();
}

void Engine::_rebuildInternalTextures()
{
  for (auto& internalTexture : _internalTexturesCache) {
    internalTexture->_rebuild();
  }
}

void Engine::_rebuildEffects()
{
  for (auto& item : _compiledEffects) {
    item.second->_prepareEffect();
  }

  Effect::ResetCache();
}

void Engine::_rebuildBuffers()
{
  // Index / Vertex
  for (auto& scene : scenes) {
    scene->resetCachedMaterial();
    scene->_rebuildGeometries();
    scene->_rebuildTextures();
  }

  // Uniforms
  for (auto& uniformBuffer : _uniformBuffers) {
    uniformBuffer->_rebuild();
  }
}

void Engine::_initGLContext()
{
  // Caps
  _caps                       = EngineCapabilities();
  _caps.maxTexturesImageUnits = _gl->getParameteri(GL::MAX_TEXTURE_IMAGE_UNITS);
  _caps.maxTextureSize        = _gl->getParameteri(GL::MAX_TEXTURE_SIZE);
  _caps.maxCubemapTextureSize
    = _gl->getParameteri(GL::MAX_CUBE_MAP_TEXTURE_SIZE);
  _caps.maxRenderTextureSize = _gl->getParameteri(GL::MAX_RENDERBUFFER_SIZE);
  _caps.maxVertexAttribs     = _gl->getParameteri(GL::MAX_VERTEX_ATTRIBS);

  // Infos
  _glVersion  = _gl->getString(GL::VERSION);
  _glRenderer = _gl->getString(GL::RENDERER);
  _glVendor   = _gl->getString(GL::VENDOR);

  if (_glVendor.empty()) {
    _glVendor = "Unknown vendor";
  }

  if (_glRenderer.empty()) {
    _glRenderer = "Unknown renderer";
  }

  // Constants
  // Half floating-point type (16-bit).
  _gl->HALF_FLOAT_OES = 0x8D61;
  if (_gl->RGBA16F != 0x881A) {
    // RGBA 16-bit floating-point color-renderable internal sized format.
    _gl->RGBA16F = 0x881A;
  }
  if (_gl->RGBA32F != 0x8814) {
    // RGBA 32-bit floating-point color-renderable internal sized format.
    _gl->RGBA32F = 0x8814;
  }
  if (_gl->DEPTH24_STENCIL8 != 35056) {
    _gl->DEPTH24_STENCIL8 = 35056;
  }

  // Extensions
  auto extensionList = String::split(_gl->getString(GL::EXTENSIONS), ' ');
  std::set<string_t> extensions;
  for (auto& extension : extensionList) {
    extensions.insert(extension);
  }

  _caps.standardDerivatives = true;
  _caps.textureFloat  = stl_util::contains(extensions, "GL_ARB_texture_float");
  _caps.maxAnisotropy = _caps.textureAnisotropicFilterExtension ?
                          static_cast<unsigned>(_gl->getParameteri(
                            GL::MAX_TEXTURE_MAX_ANISOTROPY_EXT)) :
                          0;
  _caps.instancedArrays              = false;
  _caps.uintIndices                  = true;
  _caps.fragmentDepthSupported       = true;
  _caps.highPrecisionShaderSupported = true;
  _caps.drawBuffersExtension
    = stl_util::contains(extensions, "GL_ARB_draw_buffers");
  _caps.textureFloatLinearFiltering = true;
  _caps.textureLOD
    = stl_util::contains(extensions, "GL_ARB_shader_texture_lod");
  // _caps.textureFloatRender = renderToFullFloat;

  _caps.textureHalfFloat
    = stl_util::contains(extensions, "OES_texture_half_float");
  _caps.textureHalfFloatLinearFiltering
    = stl_util::contains(extensions, "OES_texture_half_float_linear");
  if (_webGLVersion > 1) {
    _gl->HALF_FLOAT_OES = 0x140B;
  }
  // _caps.textureHalfFloatRender = renderToHalfFloat;

  auto highp
    = _gl->getShaderPrecisionFormat(GL::FRAGMENT_SHADER, GL::HIGH_FLOAT);
  if (highp) {
    _caps.highPrecisionShaderSupported = highp ? highp->precision != 0 : false;
  }

  // Depth buffer
  setDepthBuffer(true);
  setDepthFunctionToLessOrEqual();
  setDepthWrite(true);
}

float Engine::webGLVersion() const
{
  return _webGLVersion;
}

bool Engine::isInVRExclusivePointerMode() const
{
  return _vrExclusivePointerMode;
}

bool Engine::supportsUniformBuffers() const
{
  return webGLVersion() > 1.f && !disableUniformBuffers;
}

bool Engine::needPOTTextures() const
{
  return _webGLVersion < 2.f || forcePOTTextures;
}

bool Engine::badOS() const
{
  return _badOS;
}

bool Engine::badDesktopOS() const
{
  return _badDesktopOS;
}

PerformanceMonitor* Engine::performanceMonitor() const
{
  return _performanceMonitor.get();
}

bool Engine::isStencilEnable() const
{
  return _isStencilEnable;
}

void Engine::resetTextureCache()
{
  for (auto& boundTextureItem : _boundTexturesCache) {
    boundTextureItem.second = nullptr;
  }
}

bool Engine::isDeterministicLockStep() const
{
  return _deterministicLockstep;
}

unsigned int Engine::getLockstepMaxSteps() const
{
  return _lockstepMaxSteps;
}

GL::GLInfo Engine::getGlInfo()
{
  return {
    _glVendor,   // vendor
    _glRenderer, // renderer
    _glVersion   // version
  };
}

float Engine::getAspectRatio(Camera* camera, bool useScreen)
{
  const auto& viewport = camera->viewport;
  return static_cast<float>(getRenderWidth(useScreen) * viewport.width)
         / static_cast<float>(getRenderHeight(useScreen) * viewport.height);
}

int Engine::getRenderWidth(bool useScreen)
{
  if (!useScreen && _currentRenderTarget) {
    return _currentRenderTarget->width;
  }

  return _gl->drawingBufferWidth;
}

int Engine::getRenderHeight(bool useScreen)
{
  if (!useScreen && _currentRenderTarget) {
    return _currentRenderTarget->height;
  }

  return _gl->drawingBufferHeight;
}

ICanvas* Engine::getRenderingCanvas()
{
  return _renderingCanvas;
}

Nullable<ClientRect> Engine::getRenderingCanvasClientRect()
{
  if (!_renderingCanvas) {
    return nullptr;
  }

  return _renderingCanvas->getBoundingClientRect();
}

void Engine::setHardwareScalingLevel(int level)
{
  _hardwareScalingLevel = level;
  resize();
}

int Engine::getHardwareScalingLevel() const
{
  return _hardwareScalingLevel;
}

vector_t<unique_ptr_t<InternalTexture>>& Engine::getLoadedTexturesCache()
{
  return _internalTexturesCache;
}

EngineCapabilities& Engine::getCaps()
{
  return _caps;
}

size_t Engine::drawCalls() const
{
  BABYLON_LOG_WARN(
    "Engine", "drawCalls is deprecated. Please use SceneInstrumentation class");
  return 0;
}

Nullable<PerfCounter> Engine::drawCallsPerfCounter()
{
  BABYLON_LOG_WARN("Engine",
                   "drawCallsPerfCounter is deprecated. Please use "
                   "SceneInstrumentation class");
  return nullptr;
}

// Methods
void Engine::backupGLState()
{
  _gl->backupGLState();
}

void Engine::restoreGLState()
{
  _gl->restoreGLState();
}

int Engine::getDepthFunction() const
{
  return _depthCullingState->depthFunc();
}

void Engine::setDepthFunction(int depthFunc)
{
  _depthCullingState->setDepthFunc(depthFunc);
}

void Engine::setDepthFunctionToGreater()
{
  _depthCullingState->setDepthFunc(GL::GREATER);
}

void Engine::setDepthFunctionToGreaterOrEqual()
{
  _depthCullingState->setDepthFunc(GL::GEQUAL);
}

void Engine::setDepthFunctionToLess()
{
  _depthCullingState->setDepthFunc(GL::LESS);
}

void Engine::setDepthFunctionToLessOrEqual()
{
  _depthCullingState->setDepthFunc(GL::LEQUAL);
}

bool Engine::getStencilBuffer() const
{
  return _stencilState->stencilTest();
}

void Engine::setStencilBuffer(bool enable)
{
  _stencilState->setStencilTest(enable);
}

unsigned int Engine::getStencilMask() const
{
  return _stencilState->stencilMask();
}

void Engine::setStencilMask(unsigned int mask)
{
  _stencilState->setStencilMask(mask);
}

unsigned int Engine::getStencilFunction() const
{
  return _stencilState->stencilFunc();
}

int Engine::getStencilFunctionReference() const
{
  return _stencilState->stencilFuncRef();
}

unsigned int Engine::getStencilFunctionMask() const
{
  return _stencilState->stencilFuncMask();
}

void Engine::setStencilFunction(unsigned int stencilFunc)
{
  _stencilState->setStencilFunc(stencilFunc);
}

void Engine::setStencilFunctionReference(int reference)
{
  _stencilState->setStencilFuncRef(reference);
}

void Engine::setStencilFunctionMask(unsigned int mask)
{
  _stencilState->setStencilFuncMask(mask);
}

unsigned int Engine::getStencilOperationFail() const
{
  return _stencilState->stencilOpStencilFail();
}

unsigned int Engine::getStencilOperationDepthFail() const
{
  return _stencilState->stencilOpDepthFail();
}

unsigned int Engine::getStencilOperationPass() const
{
  return _stencilState->stencilOpStencilDepthPass();
}

void Engine::setStencilOperationFail(unsigned int operation)
{
  _stencilState->setStencilOpStencilFail(operation);
}

void Engine::setStencilOperationDepthFail(unsigned int operation)
{
  _stencilState->setStencilOpDepthFail(operation);
}

void Engine::setStencilOperationPass(unsigned int operation)
{
  _stencilState->setStencilOpStencilDepthPass(operation);
}

void Engine::setDitheringState(bool value)
{
  if (value) {
    _gl->enable(GL::DITHER);
  }
  else {
    _gl->disable(GL::DITHER);
  }
}

void Engine::setRasterizerState(bool value)
{
  if (value) {
    _gl->enable(GL::RASTERIZER_DISCARD);
  }
  else {
    _gl->disable(GL::RASTERIZER_DISCARD);
  }
}

void Engine::stopRenderLoop()
{
  _activeRenderLoops.clear();
}

void Engine::stopRenderLoop(const delegate_t<void()>& renderFunction)
{
  _activeRenderLoops.erase(::std::remove(_activeRenderLoops.begin(),
                                         _activeRenderLoops.end(),
                                         renderFunction),
                           _activeRenderLoops.end());
}

void Engine::_renderLoop()
{
  if (!_contextWasLost) {
    auto shouldRender = true;
    if (!renderEvenInBackground && _windowIsBackground) {
      shouldRender = false;
    }

    if (shouldRender) {
      // Start new frame
      beginFrame();

      for (auto& renderFunction : _activeRenderLoops) {
        renderFunction();
      }

      // Present
      endFrame();
    }
  }

  if (_activeRenderLoops.size() > 0) {
    // Register new frame
  }
  else {
    _renderingQueueLaunched = false;
  }
}

void Engine::runRenderLoop(const ::std::function<void()>& renderFunction)
{
  if (::std::find(_activeRenderLoops.begin(), _activeRenderLoops.end(),
                  renderFunction)
      != _activeRenderLoops.end()) {
    return;
  }

  _activeRenderLoops.emplace_back(renderFunction);

  if (!_renderingQueueLaunched) {
    _renderingQueueLaunched = true;
  }
}

void Engine::renderFunction(const ::std::function<void()>& renderFunction)
{
  bool shouldRender = true;
  if (!renderEvenInBackground && _windowIsBackground) {
    shouldRender = false;
  }

  if (shouldRender) {
    // Start new frame
    beginFrame();
    // Render
    renderFunction();
    // Present
    endFrame();
  }
}

void Engine::switchFullscreen(bool requestPointerLock)
{
  if (isFullscreen) {
    Tools::ExitFullscreen();
  }
  else {
    _pointerLockRequested = requestPointerLock;
    if (_renderingCanvas) {
      Tools::RequestFullscreen(_renderingCanvas);
    }
  }
}

void Engine::clear(bool depth, bool stencil)
{
  applyStates();

  unsigned int mode = 0;
  if (depth) {
    _gl->clearDepth(1.f);
    mode |= GL::DEPTH_BUFFER_BIT;
  }

  if (stencil) {
    _gl->clearStencil(0);
    mode |= GL::STENCIL_BUFFER_BIT;
  }

  _gl->clear(mode);
}

void Engine::clear(const Color4& color, bool backBuffer, bool depth,
                   bool stencil)
{
  applyStates();

  unsigned int mode = 0;
  if (backBuffer) {
    _gl->clearColor(color.r, color.g, color.b, color.a);
    mode |= GL::COLOR_BUFFER_BIT;
  }

  if (depth) {
    _gl->clearDepth(1.f);
    mode |= GL::DEPTH_BUFFER_BIT;
  }

  if (stencil) {
    _gl->clearStencil(0);
    mode |= GL::STENCIL_BUFFER_BIT;
  }

  _gl->clear(mode);
}

void Engine::scissorClear(int x, int y, int width, int height,
                          const Color4& clearColor)
{
  // Save state
  auto curScissor    = _gl->getParameteri(GL::SCISSOR_TEST);
  auto curScissorBox = _gl->getScissorBoxParameter();

  // Change state
  _gl->enable(GL::SCISSOR_TEST);
  _gl->scissor(x, y, width, height);

  // Clear
  clear(clearColor, true, true, true);

  // Restore state
  _gl->scissor(curScissorBox[0], curScissorBox[1], curScissorBox[2],
               curScissorBox[3]);

  if (curScissor == 1) {
    _gl->enable(GL::SCISSOR_TEST);
  }
  else {
    _gl->disable(GL::SCISSOR_TEST);
  }
}

void Engine::setViewport(Viewport& viewport, int requiredWidth,
                         int requiredHeight)
{

  if (_renderingCanvas->onlyRenderBoundingClientRect()) {
    const auto& rec = _renderingCanvas->getBoundingClientRect();
    _gl->viewport(rec.left, rec.bottom, rec.width, rec.height);
  }
  else {
    int width  = requiredWidth != 0 ? requiredWidth : getRenderWidth();
    int height = requiredHeight != 0 ? requiredHeight : getRenderHeight();
    int x      = viewport.x;
    int y      = viewport.y;

    _gl->viewport(x * width, y * height, width * viewport.width,
                  height * viewport.height);
  }

  _cachedViewport = &viewport;
}

Viewport& Engine::setDirectViewport(int x, int y, int width, int height)
{
  auto currentViewport = _cachedViewport;
  _cachedViewport      = nullptr;

  _gl->viewport(x, y, width, height);

  return *currentViewport;
}

void Engine::beginFrame()
{
  onBeginFrameObservable.notifyObservers(this);
  _measureFps();
}

void Engine::endFrame()
{
  // Force a flush in case we are using a bad OS.
  if (_badOS) {
    flushFramebuffer();
  }

  // submit frame to the vr device, if enabled
  // if (_vrDisplayEnabled && _vrDisplayEnabled.isPresenting) {
  //  _vrDisplayEnabled.submitFrame()
  //}

  onEndFrameObservable.notifyObservers(this);
}

void Engine::resize()
{
  // We're not resizing the size of the canvas while in VR mode & presenting
  if (!_vrDisplayEnabled) {
    const int width  = _renderingCanvas ? _renderingCanvas->clientWidth : 0;
    const int height = _renderingCanvas ? _renderingCanvas->clientHeight : 0;
    setSize(width / _hardwareScalingLevel, height / _hardwareScalingLevel);
  }
}

void Engine::setSize(int width, int height)
{
  if (!_renderingCanvas) {
    return;
  }

  if (_renderingCanvas->width == width && _renderingCanvas->height == height) {
    return;
  }

  _renderingCanvas->width  = width;
  _renderingCanvas->height = height;

  for (auto& scene : scenes) {
    for (auto& cam : scene->cameras) {
      cam->_currentRenderId = 0;
    }
  }

  if (onResizeObservable.hasObservers()) {
    onResizeObservable.notifyObservers(this);
  }
}

void Engine::_onVRFullScreenTriggered()
{
}

void Engine::_getVRDisplays()
{
}

void Engine::bindFramebuffer(InternalTexture* texture, unsigned int faceIndex,
                             int requiredWidth, int requiredHeight,
                             bool forceFullscreenViewport)
{
  if (_currentRenderTarget) {
    unBindFramebuffer(_currentRenderTarget);
  }
  _currentRenderTarget = texture;
  bindUnboundFramebuffer(texture->_MSAAFramebuffer ?
                           texture->_MSAAFramebuffer.get() :
                           texture->_framebuffer.get());

  if (texture->isCube) {
    _gl->framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                              GL::TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex,
                              texture->_webGLTexture.get(), 0);
  }

  if (_cachedViewport && !forceFullscreenViewport) {
    setViewport(*_cachedViewport, requiredWidth, requiredHeight);
  }
  else {
    _gl->viewport(0, 0, (requiredWidth == 0) ? texture->width : requiredWidth,
                  (requiredHeight == 0) ? texture->height : requiredHeight);
  }

  wipeCaches();
}

void Engine::bindUnboundFramebuffer(GL::IGLFramebuffer* framebuffer)
{
  if (_currentFramebuffer != framebuffer) {
    _gl->bindFramebuffer(GL::FRAMEBUFFER, framebuffer);
    _currentFramebuffer = framebuffer;
  }
}

void Engine::unBindFramebuffer(InternalTexture* texture,
                               bool disableGenerateMipMaps,
                               const ::std::function<void()>& onBeforeUnbind)
{
  _currentRenderTarget = nullptr;

  // If MSAA, we need to bitblt back to main texture
  if (texture->_MSAAFramebuffer) {
    _gl->bindFramebuffer(GL::READ_FRAMEBUFFER, texture->_MSAAFramebuffer.get());
    _gl->bindFramebuffer(GL::DRAW_FRAMEBUFFER, texture->_framebuffer.get());
    _gl->blitFramebuffer(0, 0, texture->width, texture->height, 0, 0,
                         texture->width, texture->height, GL::COLOR_BUFFER_BIT,
                         GL::NEAREST);
  }

  if (texture->generateMipMaps && !disableGenerateMipMaps && !texture->isCube) {
    _bindTextureDirectly(GL::TEXTURE_2D, texture);
    _gl->generateMipmap(GL::TEXTURE_2D);
    _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  }

  if (onBeforeUnbind) {
    if (texture->_MSAAFramebuffer) {
      // Bind the correct framebuffer
      bindUnboundFramebuffer(texture->_framebuffer.get());
    }
    onBeforeUnbind();
  }

  bindUnboundFramebuffer(nullptr);
}

void Engine::generateMipMapsForCubemap(InternalTexture* texture)
{
  if (texture->generateMipMaps) {
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture);
    _gl->generateMipmap(GL::TEXTURE_CUBE_MAP);
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
  }
}

void Engine::flushFramebuffer()
{
  _gl->flush();
}

void Engine::restoreDefaultFramebuffer()
{
  if (_currentRenderTarget) {
    unBindFramebuffer(_currentRenderTarget);
  }
  else {
    bindUnboundFramebuffer(nullptr);
  }

  if (_cachedViewport) {
    setViewport(*_cachedViewport);
  }

  wipeCaches();
}

// UBOs
unique_ptr_t<GL::IGLBuffer>
Engine::createUniformBuffer(const Float32Array& elements)
{
  auto ubo = _gl->createBuffer();
  if (!ubo) {
    BABYLON_LOG_ERROR("Engine", "Unable to create uniform buffer");
    return nullptr;
  }

  bindUniformBuffer(ubo.get());

  _gl->bufferData(GL::UNIFORM_BUFFER, elements, GL::STATIC_DRAW);

  bindUniformBuffer(nullptr);

  ubo->references = 1;
  return ubo;
}

unique_ptr_t<GL::IGLBuffer>
Engine::createDynamicUniformBuffer(const Float32Array& elements)
{
  auto ubo = _gl->createBuffer();
  if (!ubo) {
    BABYLON_LOG_ERROR("Engine", "Unable to create dynamic uniform buffer");
    return nullptr;
  }

  bindUniformBuffer(ubo.get());

  _gl->bufferData(GL::UNIFORM_BUFFER, elements, GL::DYNAMIC_DRAW);

  bindUniformBuffer(nullptr);

  ubo->references = 1;
  return ubo;
}

void Engine::updateUniformBuffer(GL::IGLBuffer* uniformBuffer,
                                 const Float32Array& elements, int offset,
                                 int count)
{
  bindUniformBuffer(uniformBuffer);

  if (offset == -1) {
    offset = 0;
  }

  if (count == -1) {
    _gl->bufferSubData(GL::UNIFORM_BUFFER, offset, elements);
  }
  else {
    Float32Array subvector;
    ::std::copy(elements.begin() + offset, elements.begin() + offset + count,
                ::std::back_inserter(subvector));
    _gl->bufferSubData(GL::UNIFORM_BUFFER, 0, subvector);
  }

  bindUniformBuffer(nullptr);
}

// VBOs
void Engine::_resetVertexBufferBinding()
{
  bindArrayBuffer(nullptr);
  _cachedVertexBuffers = nullptr;
}

Engine::GLBufferPtr Engine::createVertexBuffer(const Float32Array& vertices)
{
  auto vbo = _gl->createBuffer();
  if (!vbo) {
    BABYLON_LOG_ERROR("Engine", "Unable to create vertex buffer");
    return nullptr;
  }

  bindArrayBuffer(vbo.get());

  _gl->bufferData(GL::ARRAY_BUFFER, vertices, GL::STATIC_DRAW);

  _resetVertexBufferBinding();
  vbo->references = 1;
  return vbo;
}

Engine::GLBufferPtr
Engine::createDynamicVertexBuffer(const Float32Array& vertices)
{
  auto vbo = _gl->createBuffer();
  if (!vbo) {
    BABYLON_LOG_ERROR("Engine", "Unable to create dynamic vertex buffer");
    return nullptr;
  }

  bindArrayBuffer(vbo.get());

  _gl->bufferData(GL::ARRAY_BUFFER, vertices, GL::DYNAMIC_DRAW);

  _resetVertexBufferBinding();
  vbo->references = 1;
  return vbo;
}

void Engine::updateDynamicIndexBuffer(const GLBufferPtr& indexBuffer,
                                      const IndicesArray& indices,
                                      int /*offset*/)
{
  // Force cache update
  _currentBoundBuffer[GL::ELEMENT_ARRAY_BUFFER] = nullptr;
  bindIndexBuffer(indexBuffer.get());
  auto arrayBuffer = indices;

  _gl->bufferData(GL::ELEMENT_ARRAY_BUFFER, arrayBuffer, GL::DYNAMIC_DRAW);

  _resetIndexBufferBinding();
}

void Engine::updateDynamicVertexBuffer(const Engine::GLBufferPtr& vertexBuffer,
                                       const Float32Array& vertices, int offset,
                                       int count)
{
  bindArrayBuffer(vertexBuffer.get());

  if (offset == -1) {
    offset = 0;
  }

  if (count == -1) {
    _gl->bufferSubData(GL::ARRAY_BUFFER, offset, vertices);
  }
  else {
    Float32Array subvector;
    ::std::copy(vertices.begin() + offset, vertices.begin() + offset + count,
                ::std::back_inserter(subvector));
    _gl->bufferSubData(GL::ARRAY_BUFFER, 0, subvector);
  }

  _resetVertexBufferBinding();
}

void Engine::_resetIndexBufferBinding()
{
  bindIndexBuffer(nullptr);
  _cachedIndexBuffer = nullptr;
}

Engine::GLBufferPtr Engine::createIndexBuffer(const IndicesArray& indices,
                                              bool updatable)
{
  auto vbo = _gl->createBuffer();
  if (!vbo) {
    BABYLON_LOG_ERROR("Engine", "Unable to create index buffer");
    return nullptr;
  }

  bindIndexBuffer(vbo.get());

  // Check for 32 bits indices
  auto need32Bits = false;

  if (_caps.uintIndices) {
    // check 32 bit support
    auto it = ::std::find_if(indices.begin(), indices.end(),
                             ::std::bind2nd(::std::greater<uint32_t>(), 65535));
    if (it != indices.end()) {
      need32Bits = true;
    }
  }

  if (need32Bits) {
    Uint32Array arrayBuffer;
    arrayBuffer.assign(indices.begin(), indices.end());
    _gl->bufferData(GL::ELEMENT_ARRAY_BUFFER, arrayBuffer,
                    updatable ? GL::DYNAMIC_DRAW : GL::STATIC_DRAW);
  }
  else {
    Uint16Array arrayBuffer;
    arrayBuffer.assign(indices.begin(), indices.end());
    _gl->bufferData(GL::ELEMENT_ARRAY_BUFFER, arrayBuffer,
                    updatable ? GL::DYNAMIC_DRAW : GL::STATIC_DRAW);
  }

  _resetIndexBufferBinding();
  vbo->references = 1;
  vbo->is32Bits   = need32Bits;
  return vbo;
}

void Engine::bindArrayBuffer(GL::IGLBuffer* buffer)
{
  if (!_vaoRecordInProgress) {
    _unbindVertexArrayObject();
  }
  bindBuffer(buffer, GL::ARRAY_BUFFER);
}

void Engine::bindUniformBuffer(GL::IGLBuffer* buffer)
{
  _gl->bindBuffer(GL::UNIFORM_BUFFER, buffer);
}

void Engine::bindUniformBufferBase(GL::IGLBuffer* buffer, unsigned int location)
{
  _gl->bindBufferBase(GL::UNIFORM_BUFFER, location, buffer);
}

void Engine::bindUniformBlock(GL::IGLProgram* shaderProgram,
                              const string_t blockName, unsigned int index)
{
  auto uniformLocation = _gl->getUniformBlockIndex(shaderProgram, blockName);
  _gl->uniformBlockBinding(shaderProgram, uniformLocation, index);
}

void Engine::bindIndexBuffer(GL::IGLBuffer* buffer)
{
  if (!_vaoRecordInProgress) {
    _unbindVertexArrayObject();
  }
  bindBuffer(buffer, GL::ELEMENT_ARRAY_BUFFER);
}

void Engine::bindBuffer(GL::IGLBuffer* buffer, int target)
{
  if (_vaoRecordInProgress
      || (_currentBoundBuffer.find(target) == _currentBoundBuffer.end())
      || (_currentBoundBuffer[target] != buffer)) {
    _gl->bindBuffer(static_cast<unsigned int>(target), buffer);
    _currentBoundBuffer[target] = buffer;
  }
}

void Engine::updateArrayBuffer(const Float32Array& data)
{
  _gl->bufferSubData(GL::ARRAY_BUFFER, 0, data);
}

void Engine::vertexAttribPointer(GL::IGLBuffer* buffer, unsigned int indx,
                                 int size, unsigned int type, bool normalized,
                                 int stride, int offset)
{
  bool changed = false;
  if (_currentBufferPointers.find(indx) == _currentBufferPointers.end()) {
    changed                      = true;
    _currentBufferPointers[indx] = BufferPointer(
      true, indx, size, type, normalized, stride, offset, buffer);
  }
  else {
    auto& pointer = _currentBufferPointers[indx];
    if (pointer.buffer != buffer) {
      pointer.buffer = buffer;
      changed        = true;
    }
    if (pointer.size != size) {
      pointer.size = size;
      changed      = true;
    }
    if (pointer.type != type) {
      pointer.type = type;
      changed      = true;
    }
    if (pointer.normalized != normalized) {
      pointer.normalized = normalized;
      changed            = true;
    }
    if (pointer.stride != stride) {
      pointer.stride = stride;
      changed        = true;
    }
    if (pointer.offset != offset) {
      pointer.offset = offset;
      changed        = true;
    }
  }

  if (changed || _vaoRecordInProgress) {
    bindArrayBuffer(buffer);
    _gl->vertexAttribPointer(indx, size, type, normalized, stride, offset);
  }
}

void Engine::_bindIndexBufferWithCache(GL::IGLBuffer* indexBuffer)
{
  if (indexBuffer == nullptr) {
    return;
  }
  if (_cachedIndexBuffer != indexBuffer) {
    _cachedIndexBuffer = indexBuffer;
    bindIndexBuffer(indexBuffer);
    _uintIndicesCurrentlySet = indexBuffer->is32Bits;
  }
}

void Engine::_bindVertexBuffersAttributes(
  const unordered_map_t<string_t, VertexBuffer*>& vertexBuffers, Effect* effect)
{
  auto attributes = effect->getAttributesNames();

  if (!_vaoRecordInProgress) {
    _unbindVertexArrayObject();
  }

  unbindAllAttributes();

  unsigned int _order = 0;
  for (unsigned int index = 0; index < attributes.size(); ++index) {
    auto order = effect->getAttributeLocation(index);

    if (order >= 0) {
      _order             = static_cast<unsigned int>(order);
      auto& vertexBuffer = vertexBuffers.at(attributes[index]);

      if (!vertexBuffer) {
        continue;
      }

      _gl->enableVertexAttribArray(_order);
      if (!_vaoRecordInProgress) {
        if (_order >= _vertexAttribArraysEnabled.size()) {
          _vertexAttribArraysEnabled.resize(_order + 1);
        }
        _vertexAttribArraysEnabled[_order] = true;
      }

      auto buffer = vertexBuffer->getBuffer();
      if (buffer) {
        vertexAttribPointer(buffer, _order, vertexBuffer->getSize(), GL::FLOAT,
                            false, vertexBuffer->getStrideSize() * 4,
                            static_cast<int>(vertexBuffer->getOffset() * 4));

        if (vertexBuffer->getIsInstanced()) {
          _gl->vertexAttribDivisor(_order, vertexBuffer->getInstanceDivisor());
          if (!_vaoRecordInProgress) {
            _currentInstanceLocations.emplace_back(order);
            _currentInstanceBuffers.emplace_back(buffer);
          }
        }
      }
    }
  }
}

unique_ptr_t<GL::IGLVertexArrayObject> Engine::recordVertexArrayObject(
  const unordered_map_t<string_t, VertexBuffer*>& vertexBuffers,
  GL::IGLBuffer* indexBuffer, Effect* effect)
{
  auto vao = _gl->createVertexArray();

  _vaoRecordInProgress = true;

  _gl->bindVertexArray(vao.get());

  _mustWipeVertexAttributes = true;
  _bindVertexBuffersAttributes(vertexBuffers, effect);

  bindIndexBuffer(indexBuffer);

  _vaoRecordInProgress = false;
  _gl->bindVertexArray(nullptr);

  return vao;
}

void Engine::bindVertexArrayObject(GL::IGLVertexArrayObject* vertexArrayObject,
                                   GL::IGLBuffer* indexBuffer)
{
  if (_cachedVertexArrayObject != vertexArrayObject) {
    _cachedVertexArrayObject = vertexArrayObject;

    _gl->bindVertexArray(vertexArrayObject);
    _cachedVertexBuffers = nullptr;
    _cachedIndexBuffer   = nullptr;

    _uintIndicesCurrentlySet  = indexBuffer != nullptr && indexBuffer->is32Bits;
    _mustWipeVertexAttributes = true;
  }
}

void Engine::bindBuffersDirectly(GL::IGLBuffer* vertexBuffer,
                                 GL::IGLBuffer* indexBuffer,
                                 const Float32Array& vertexDeclaration,
                                 int vertexStrideSize, Effect* effect)
{
  if (_cachedVertexBuffers != vertexBuffer
      || _cachedEffectForVertexBuffers != effect) {
    _cachedVertexBuffers          = vertexBuffer;
    _cachedEffectForVertexBuffers = effect;

    auto attributesCount = effect->getAttributesCount();

    _unbindVertexArrayObject();
    unbindAllAttributes();

    int offset = 0;
    for (unsigned int index = 0; index < attributesCount; ++index) {
      if (index < vertexDeclaration.size()) {
        auto vertexDeclarationi = static_cast<int>(vertexDeclaration[index]);
        auto order              = effect->getAttributeLocation(index);

        if (order >= 0) {
          auto _order = static_cast<unsigned int>(order);
          if (_order + 1 > _vertexAttribArraysEnabled.size()) {
            _gl->enableVertexAttribArray(_order);
            _vertexAttribArraysEnabled.resize(_order + 1);
            _vertexAttribArraysEnabled[_order] = true;
          }

          _gl->vertexAttribPointer(_order, vertexDeclarationi, GL::FLOAT, false,
                                   vertexStrideSize, offset);
        }
        offset += vertexDeclarationi * 4;
      }
    }
  }

  _bindIndexBufferWithCache(indexBuffer);
}

void Engine::_unbindVertexArrayObject()
{
  if (!_cachedVertexArrayObject) {
    return;
  }

  _cachedVertexArrayObject = nullptr;
  _gl->bindVertexArray(nullptr);
}

void Engine::bindBuffers(
  const unordered_map_t<string_t, VertexBuffer*>& vertexBuffers,
  GL::IGLBuffer* indexBuffer, Effect* effect)
{
  if (_cachedVertexBuffersMap != vertexBuffers
      || _cachedEffectForVertexBuffers != effect) {
    _cachedVertexBuffersMap       = vertexBuffers;
    _cachedEffectForVertexBuffers = effect;

    _bindVertexBuffersAttributes(vertexBuffers, effect);
  }

  _bindIndexBufferWithCache(indexBuffer);
}

void Engine::unbindInstanceAttributes()
{
  GL::IGLBuffer* boundBuffer = nullptr;
  for (size_t i = 0, ul = _currentInstanceLocations.size(); i < ul; ++i) {
    auto instancesBuffer = _currentInstanceBuffers[i];
    if (boundBuffer != instancesBuffer && instancesBuffer->references) {
      boundBuffer = instancesBuffer;
      bindArrayBuffer(instancesBuffer);
    }
    auto offsetLocation
      = static_cast<unsigned int>(_currentInstanceLocations[i]);
    _gl->vertexAttribDivisor(offsetLocation, 0);
  }
  _currentInstanceBuffers.clear();
  _currentInstanceLocations.clear();
}

void Engine::releaseVertexArrayObject(GL::IGLVertexArrayObject* vao)
{
  _gl->deleteVertexArray(vao);
}

bool Engine::_releaseBuffer(GL::IGLBuffer* buffer)
{
  --buffer->references;

  if (buffer->references == 0) {
    _gl->deleteBuffer(buffer);
    return true;
  }

  return false;
}

Engine::GLBufferPtr Engine::createInstancesBuffer(unsigned int capacity)
{
  auto buffer = _gl->createBuffer();
  if (!buffer) {
    BABYLON_LOG_ERROR("Engine", "Unable to create instance buffer");
    return nullptr;
  }

  buffer->capacity = capacity;

  bindArrayBuffer(buffer.get());
  _gl->bufferData(GL::ARRAY_BUFFER, capacity, GL::DYNAMIC_DRAW);
  return buffer;
}

void Engine::deleteInstancesBuffer(GL::IGLBuffer* buffer)
{
  _gl->deleteBuffer(buffer);
}

void Engine::updateAndBindInstancesBuffer(GL::IGLBuffer* instancesBuffer,
                                          const Float32Array& data,
                                          const Uint32Array& offsetLocations)
{
  _gl->bindBuffer(GL::ARRAY_BUFFER, instancesBuffer);
  if (!data.empty()) {
    _gl->bufferSubData(GL::ARRAY_BUFFER, 0, data);
  }

  for (unsigned int index = 0; index < 4; ++index) {
    auto& offsetLocation = offsetLocations[index];
    _gl->enableVertexAttribArray(offsetLocation);

    if (offsetLocation > _vertexAttribArraysEnabled.size()) {
      _gl->enableVertexAttribArray(offsetLocation);
      _vertexAttribArraysEnabled.resize(offsetLocation + 1);
      _vertexAttribArraysEnabled[offsetLocation] = true;
    }

    vertexAttribPointer(instancesBuffer, offsetLocation, 4, GL::FLOAT, false,
                        64, static_cast<int>(index * 16));
    _gl->vertexAttribDivisor(offsetLocation, 1);
    _currentInstanceLocations.emplace_back(offsetLocation);
    _currentInstanceBuffers.emplace_back(instancesBuffer);
  }
}

void Engine::updateAndBindInstancesBuffer(
  GL::IGLBuffer* instancesBuffer, const Float32Array& data,
  const vector_t<InstancingAttributeInfo>& offsetLocations)
{
  _gl->bindBuffer(GL::ARRAY_BUFFER, instancesBuffer);
  if (!data.empty()) {
    _gl->bufferSubData(GL::ARRAY_BUFFER, 0, data);
  }

  int stride = 0;
  for (unsigned int i = 0; i < offsetLocations.size(); ++i) {
    stride += offsetLocations[i].attributeSize * 4;
  }

  for (size_t i = 0; i < offsetLocations.size(); ++i) {
    const InstancingAttributeInfo& ai = offsetLocations[i];

    if (ai.index > _vertexAttribArraysEnabled.size()) {
      _gl->enableVertexAttribArray(ai.index);
      _vertexAttribArraysEnabled.resize(ai.index + 1);
      _vertexAttribArraysEnabled[ai.index] = true;
    }

    _gl->enableVertexAttribArray(ai.index);
    vertexAttribPointer(instancesBuffer, ai.index, ai.attributeSize,
                        ai.attribyteType, ai.normalized, stride, ai.offset);
    _gl->vertexAttribDivisor(ai.index, 1);
    _currentInstanceLocations.emplace_back(ai.index);
    _currentInstanceBuffers.emplace_back(instancesBuffer);
  }
}

void Engine::applyStates()
{
  _depthCullingState->apply(*_gl);
  _stencilState->apply(*_gl);
  _alphaState->apply(*_gl);
}

void Engine::draw(bool useTriangles, unsigned int indexStart, int indexCount,
                  int instancesCount)
{
  // Apply states
  applyStates();
  _drawCalls.addCount(1, false);

  // Render
  auto indexFormat
    = _uintIndicesCurrentlySet ? GL::UNSIGNED_INT : GL::UNSIGNED_SHORT;
  auto mult = _uintIndicesCurrentlySet ? 4u : 2u;

  if (instancesCount) {
    _gl->drawElementsInstanced(useTriangles ? GL::TRIANGLES : GL::LINES,
                               indexCount, indexFormat, indexStart * mult,
                               instancesCount);
    return;
  }

  _gl->drawElements(useTriangles ? GL::TRIANGLES : GL::LINES,
                    static_cast<int>(indexCount), indexFormat,
                    indexStart * mult);
}

void Engine::drawPointClouds(int verticesStart, int verticesCount,
                             int instancesCount)
{
  // Apply states
  applyStates();
  _drawCalls.addCount(1, false);

  if (instancesCount > 0) {
    _gl->drawArraysInstanced(GL::POINTS, verticesStart, verticesCount,
                             instancesCount);
    return;
  }

  _gl->drawArrays(GL::POINTS, verticesStart, static_cast<int>(verticesCount));
}

void Engine::drawUnIndexed(bool useTriangles, int verticesStart,
                           int verticesCount, int instancesCount)
{
  // Apply states
  applyStates();
  _drawCalls.addCount(1, false);

  if (instancesCount > 0) {
    _gl->drawArraysInstanced(useTriangles ? GL::TRIANGLES : GL::LINES,
                             verticesStart, verticesCount, instancesCount);
    return;
  }

  _gl->drawArrays(useTriangles ? GL::TRIANGLES : GL::LINES, verticesStart,
                  static_cast<int>(verticesCount));
}

// Shaders
void Engine::_releaseEffect(Effect* effect)
{
  if (stl_util::contains(_compiledEffects, effect->_key)) {
    _deleteProgram(effect->getProgram());
    _compiledEffects.erase(effect->_key);
  }
}

void Engine::_deleteProgram(GL::IGLProgram* program)
{
  if (program) {
    program->__SPECTOR_rebuildProgram = nullptr;

    if (program->transformFeedback) {
      deleteTransformFeedback(program->transformFeedback);
      program->transformFeedback = nullptr;
    }

    _gl->deleteProgram(program);
  }
}

Effect*
Engine::createEffect(const string_t& baseName, EffectCreationOptions& options,
                     Engine* engine,
                     const ::std::function<void(Effect* effect)>& onCompiled)
{
  string_t name = baseName + "+" + baseName + "@" + options.defines;
  if (stl_util::contains(_compiledEffects, name)) {
    auto compiledEffect = _compiledEffects[name].get();
    if (onCompiled && compiledEffect->isReady()) {
      onCompiled(compiledEffect);
    }
    return compiledEffect;
  }

  auto effect  = ::std::make_unique<Effect>(baseName, options, engine);
  effect->_key = name;
  _compiledEffects[name] = ::std::move(effect);

  return _compiledEffects[name].get();
}

Effect* Engine::createEffect(unordered_map_t<string_t, string_t>& baseName,
                             EffectCreationOptions& options, Engine* engine)
{
  string_t vertex
    = stl_util::contains(baseName, "vertexElement") ?
        baseName["vertexElement"] :
        stl_util::contains(baseName, "vertex") ? baseName["vertex"] : "vertex";
  string_t fragment = stl_util::contains(baseName, "fragmentElement") ?
                        baseName["fragmentElement"] :
                        stl_util::contains(baseName, "fragment") ?
                        baseName["fragment"] :
                        "fragment";

  string_t name = vertex + "+" + fragment + "@" + options.defines;
  if (stl_util::contains(_compiledEffects, name)) {
    return _compiledEffects[name].get();
  }

  auto effect  = ::std::make_unique<Effect>(baseName, options, engine);
  effect->_key = name;
  _compiledEffects[name] = ::std::move(effect);

  return _compiledEffects[name].get();
}

Effect* Engine::createEffectForParticles(
  const string_t& fragmentName, const vector_t<string_t>& uniformsNames,
  const vector_t<string_t>& samplers, const string_t& defines,
  EffectFallbacks* fallbacks,
  const ::std::function<void(const Effect* effect)>& onCompiled,
  const ::std::function<void(const Effect* effect, const string_t& errors)>&
    onError)
{
  unordered_map_t<string_t, string_t> baseName{
    {"vertex", "particles"}, {"fragmentElement", fragmentName}};
  vector_t<string_t> attributesNames{"position", "color", "options"};
  vector_t<string_t> _uniformsNames = {"view", "projection"};
  stl_util::concat(_uniformsNames, uniformsNames);
  vector_t<string_t> _samplers = {"diffuseSampler"};
  stl_util::concat(_samplers, samplers);

  EffectCreationOptions options;
  options.attributes    = ::std::move(attributesNames);
  options.uniformsNames = ::std::move(_uniformsNames);
  options.samplers      = ::std::move(_samplers);
  options.defines       = defines;
  options.fallbacks     = ::std::make_unique<EffectFallbacks>(*fallbacks);
  options.onCompiled    = onCompiled;
  options.onError       = onError;

  return createEffect(baseName, options, this);
}

unique_ptr_t<GL::IGLProgram> Engine::createRawShaderProgram(
  const string_t& vertexCode, const string_t& fragmentCode,
  GL::IGLRenderingContext* context,
  const vector_t<string_t>& transformFeedbackVaryings)
{
  context = context ? context : _gl;

  auto vertexShader   = CompileRawShader(context, vertexCode, "vertex");
  auto fragmentShader = CompileRawShader(context, fragmentCode, "fragment");

  return _createShaderProgram(vertexShader, fragmentShader, context,
                              transformFeedbackVaryings);
}

unique_ptr_t<GL::IGLProgram> Engine::createShaderProgram(
  const string_t& vertexCode, const string_t& fragmentCode,
  const string_t& defines, GL::IGLRenderingContext* context,
  const vector_t<string_t>& transformFeedbackVaryings)
{
  context = context ? context : _gl;

  onBeforeShaderCompilationObservable.notifyObservers(this);

  const string_t shaderVersion
    = (_webGLVersion > 1.f) ? "#version 300 es\n" : "";
  auto vertexShader   = Engine::CompileShader(context, vertexCode, "vertex",
                                            defines, shaderVersion);
  auto fragmentShader = Engine::CompileShader(context, fragmentCode, "fragment",
                                              defines, shaderVersion);

  auto program = _createShaderProgram(vertexShader, fragmentShader, context,
                                      transformFeedbackVaryings);

  onAfterShaderCompilationObservable.notifyObservers(this);

  return program;
}

unique_ptr_t<GL::IGLProgram> Engine::_createShaderProgram(
  const unique_ptr_t<GL::IGLShader>& vertexShader,
  const unique_ptr_t<GL::IGLShader>& fragmentShader,
  GL::IGLRenderingContext* context,
  const vector_t<string_t>& transformFeedbackVaryings)
{
  auto shaderProgram = context->createProgram();
  if (!shaderProgram) {
    BABYLON_LOG_ERROR("Engine", "Unable to create program");
    return nullptr;
  }

  context->attachShader(shaderProgram, vertexShader);
  context->attachShader(shaderProgram, fragmentShader);

  if (webGLVersion() > 1.f && !transformFeedbackVaryings.empty()) {
    auto transformFeedback = createTransformFeedback();

    bindTransformFeedback(transformFeedback.get());
    setTranformFeedbackVaryings(shaderProgram.get(), transformFeedbackVaryings);
    shaderProgram->transformFeedback = transformFeedback.get();
  }

  bool linked = context->linkProgram(shaderProgram);

  if (webGLVersion() > 1.f && !transformFeedbackVaryings.empty()) {
    bindTransformFeedback(nullptr);
  }

  if (!linked) {
    context->validateProgram(shaderProgram.get());
    const auto error = context->getProgramInfoLog(shaderProgram);
    if (!error.empty()) {
      BABYLON_LOG_ERROR("Engine", error);
      return nullptr;
    }
  }

  context->deleteShader(vertexShader);
  context->deleteShader(fragmentShader);

  return shaderProgram;
}

unordered_map_t<string_t, unique_ptr_t<GL::IGLUniformLocation>>
Engine::getUniforms(GL::IGLProgram* shaderProgram,
                    const vector_t<string_t>& uniformsNames)
{
  unordered_map_t<string_t, unique_ptr_t<GL::IGLUniformLocation>> results;

  for (auto& name : uniformsNames) {
    auto uniform = _gl->getUniformLocation(shaderProgram, name);
    if (uniform) {
      results[name] = ::std::move(uniform);
    }
  }

  return results;
}

Int32Array Engine::getAttributes(GL::IGLProgram* shaderProgram,
                                 const vector_t<string_t>& attributesNames)
{
  Int32Array results;

  for (auto& attributesName : attributesNames) {
    results.emplace_back(_gl->getAttribLocation(shaderProgram, attributesName));
  }

  return results;
}

void Engine::enableEffect(Effect* effect)
{
  if (!effect) {
    return;
  }

  // Use program
  setProgram(effect->getProgram());

  _currentEffect = effect;

  if (effect->onBind) {
    effect->onBind(effect);
  }
  effect->onBindObservable.notifyObservers(effect);
}

void Engine::setIntArray(GL::IGLUniformLocation* uniform,
                         const Int32Array& array)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1iv(uniform, array);
}

void Engine::setIntArray2(GL::IGLUniformLocation* uniform,
                          const Int32Array& array)
{
  if (!uniform || array.size() % 2 != 0) {
    return;
  }

  _gl->uniform2iv(uniform, array);
}

void Engine::setIntArray3(GL::IGLUniformLocation* uniform,
                          const Int32Array& array)
{
  if (!uniform || array.size() % 3 != 0) {
    return;
  }

  _gl->uniform3iv(uniform, array);
}

void Engine::setIntArray4(GL::IGLUniformLocation* uniform,
                          const Int32Array& array)
{
  if (!uniform || array.size() % 4 != 0) {
    return;
  }

  _gl->uniform4iv(uniform, array);
}

void Engine::setFloatArray(GL::IGLUniformLocation* uniform,
                           const Float32Array& array)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1fv(uniform, array);
}

void Engine::setFloatArray2(GL::IGLUniformLocation* uniform,
                            const Float32Array& array)
{
  if (!uniform || array.size() % 2 != 0) {
    return;
  }

  _gl->uniform2fv(uniform, array);
}

void Engine::setFloatArray3(GL::IGLUniformLocation* uniform,
                            const Float32Array& array)
{
  if (!uniform || array.size() % 3 != 0) {
    return;
  }

  _gl->uniform3fv(uniform, array);
}

void Engine::setFloatArray4(GL::IGLUniformLocation* uniform,
                            const Float32Array& array)
{
  if (!uniform || array.size() % 4 != 0) {
    return;
  }

  _gl->uniform4fv(uniform, array);
}

void Engine::setArray(GL::IGLUniformLocation* uniform,
                      const Float32Array& array)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1fv(uniform, array);
}

void Engine::setArray2(GL::IGLUniformLocation* uniform,
                       const Float32Array& array)
{
  if (!uniform || array.size() % 2 != 0) {
    return;
  }

  _gl->uniform2fv(uniform, array);
}

void Engine::setArray3(GL::IGLUniformLocation* uniform,
                       const Float32Array& array)
{
  if (!uniform || array.size() % 3 != 0) {
    return;
  }

  _gl->uniform3fv(uniform, array);
}

void Engine::setArray4(GL::IGLUniformLocation* uniform,
                       const Float32Array& array)
{
  if (!uniform || array.size() % 4 != 0) {
    return;
  }

  _gl->uniform4fv(uniform, array);
}

void Engine::setMatrices(GL::IGLUniformLocation* uniform,
                         const Float32Array& matrices)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix4fv(uniform, false, matrices);
}

void Engine::setMatrix(GL::IGLUniformLocation* uniform, const Matrix& matrix)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix4fv(uniform, false, matrix.toArray());
}

void Engine::setMatrix3x3(GL::IGLUniformLocation* uniform,
                          const Float32Array& matrix)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix3fv(uniform, false, matrix);
}

void Engine::setMatrix2x2(GL::IGLUniformLocation* uniform,
                          const Float32Array& matrix)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix2fv(uniform, false, matrix);
}

void Engine::setFloat(GL::IGLUniformLocation* uniform, float value)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1f(uniform, value);
}

void Engine::setFloat2(GL::IGLUniformLocation* uniform, float x, float y)
{
  if (!uniform) {
    return;
  }

  _gl->uniform2f(uniform, x, y);
}

void Engine::setFloat3(GL::IGLUniformLocation* uniform, float x, float y,
                       float z)
{
  if (!uniform) {
    return;
  }

  _gl->uniform3f(uniform, x, y, z);
}

void Engine::setBool(GL::IGLUniformLocation* uniform, int value)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1i(uniform, value);
}

void Engine::setFloat4(GL::IGLUniformLocation* uniform, float x, float y,
                       float z, float w)
{
  if (!uniform) {
    return;
  }

  _gl->uniform4f(uniform, x, y, z, w);
}

void Engine::setColor3(GL::IGLUniformLocation* uniform, const Color3& color3)
{
  if (!uniform) {
    return;
  }

  _gl->uniform3f(uniform, color3.r, color3.g, color3.b);
}

void Engine::setColor4(GL::IGLUniformLocation* uniform, const Color3& color3,
                       float alpha)
{
  if (!uniform) {
    return;
  }

  _gl->uniform4f(uniform, color3.r, color3.g, color3.b, alpha);
}

/** States **/
void Engine::setState(bool culling, float zOffset, bool force, bool reverseSide)
{
  // Culling
  auto showSide = reverseSide ? GL::FRONT : GL::BACK;
  auto hideSide = reverseSide ? GL::BACK : GL::FRONT;
  auto cullFace = cullBackFaces ? showSide : hideSide;

  if (_depthCullingState->cull() != culling || force
      || _depthCullingState->cullFace() != static_cast<int>(cullFace)) {
    if (culling) {
      _depthCullingState->setCullFace(static_cast<int>(cullFace));
      _depthCullingState->setCull(true);
    }
    else {
      _depthCullingState->setCull(false);
    }
  }

  // Z offset
  setZOffset(zOffset);
}

void Engine::setZOffset(float value)
{
  _depthCullingState->setZOffset(value);
}

float Engine::getZOffset() const
{
  return _depthCullingState->zOffset();
}

void Engine::setDepthBuffer(bool enable)
{
  _depthCullingState->setDepthTest(enable);
}

bool Engine::getDepthWrite() const
{
  return _depthCullingState->depthMask();
}

void Engine::setDepthWrite(bool enable)
{
  _depthCullingState->setDepthMask(enable);
}

void Engine::setColorWrite(bool enable)
{
  _gl->colorMask(enable, enable, enable, enable);
  _colorWrite = enable;
}

bool Engine::getColorWrite() const
{
  return _colorWrite;
}

void Engine::setAlphaConstants(float r, float g, float b, float a)
{
  _alphaState->setAlphaBlendConstants(r, g, b, a);
}

void Engine::setAlphaMode(unsigned int mode, bool noDepthWriteChange)
{
  if (_alphaMode == mode) {
    return;
  }

  switch (mode) {
    case EngineConstants::ALPHA_DISABLE:
      _alphaState->setAlphaBlend(false);
      break;
    case EngineConstants::ALPHA_PREMULTIPLIED:
      _alphaState->setAlphaBlendFunctionParameters(
        GL::ONE, GL::ONE_MINUS_SRC_ALPHA, GL::ONE, GL::ONE);
      _alphaState->setAlphaBlend(true);
      break;
    case EngineConstants::ALPHA_PREMULTIPLIED_PORTERDUFF:
      _alphaState->setAlphaBlendFunctionParameters(
        GL::ONE, GL::ONE_MINUS_SRC_ALPHA, GL::ONE, GL::ONE_MINUS_SRC_ALPHA);
      _alphaState->setAlphaBlend(true);
      break;
    case EngineConstants::ALPHA_COMBINE:
      _alphaState->setAlphaBlendFunctionParameters(
        GL::SRC_ALPHA, GL::ONE_MINUS_SRC_ALPHA, GL::ONE, GL::ONE);
      _alphaState->setAlphaBlend(true);
      break;
    case EngineConstants::ALPHA_ONEONE:
      _alphaState->setAlphaBlendFunctionParameters(GL::ONE, GL::ONE, GL::ZERO,
                                                   GL::ONE);
      _alphaState->setAlphaBlend(true);
      break;
    case EngineConstants::ALPHA_ADD:
      _alphaState->setAlphaBlendFunctionParameters(GL::SRC_ALPHA, GL::ONE,
                                                   GL::ZERO, GL::ONE);
      _alphaState->setAlphaBlend(true);
      break;
    case EngineConstants::ALPHA_SUBTRACT:
      _alphaState->setAlphaBlendFunctionParameters(
        GL::ZERO, GL::ONE_MINUS_SRC_COLOR, GL::ONE, GL::ONE);
      _alphaState->setAlphaBlend(true);
      break;
    case EngineConstants::ALPHA_MULTIPLY:
      _alphaState->setAlphaBlendFunctionParameters(GL::DST_COLOR, GL::ZERO,
                                                   GL::ONE, GL::ONE);
      _alphaState->setAlphaBlend(true);
      break;
    case EngineConstants::ALPHA_MAXIMIZED:
      _alphaState->setAlphaBlendFunctionParameters(
        GL::SRC_ALPHA, GL::ONE_MINUS_SRC_COLOR, GL::ONE, GL::ONE);
      _alphaState->setAlphaBlend(true);
      break;
    case EngineConstants::ALPHA_INTERPOLATE:
      _alphaState->setAlphaBlendFunctionParameters(
        GL::CONSTANT_COLOR, GL::ONE_MINUS_CONSTANT_COLOR, GL::CONSTANT_ALPHA,
        GL::ONE_MINUS_CONSTANT_ALPHA);
      _alphaState->setAlphaBlend(true);
      break;
    case EngineConstants::ALPHA_SCREENMODE:
      _alphaState->setAlphaBlendFunctionParameters(
        GL::ONE, GL::ONE_MINUS_SRC_COLOR, GL::ONE, GL::ONE_MINUS_SRC_ALPHA);
      _alphaState->setAlphaBlend(true);
      break;
    default:
      break;
  }
  if (!noDepthWriteChange) {
    setDepthWrite(mode == EngineConstants::ALPHA_DISABLE);
  }
  _alphaMode = mode;
}

unsigned int Engine::getAlphaMode() const
{
  return _alphaMode;
}

void Engine::setAlphaTesting(bool enable)
{
  _alphaTest = enable;
}

bool Engine::getAlphaTesting() const
{
  return _alphaTest;
}

Internals::_StencilState* Engine::stencilState()
{
  return _stencilState.get();
}

// Textures
void Engine::wipeCaches(bool bruteForce)
{
  if (preventCacheWipeBetweenFrames) {
    return;
  }
  resetTextureCache();
  _currentEffect = nullptr;

  if (bruteForce) {
    _currentProgram = nullptr;

    _stencilState->reset();
    _depthCullingState->reset();
    setDepthFunctionToLessOrEqual();
    _alphaState->reset();
  }

  _cachedVertexBuffers          = nullptr;
  _cachedIndexBuffer            = nullptr;
  _cachedEffectForVertexBuffers = nullptr;
  _unbindVertexArrayObject();
  bindIndexBuffer(nullptr);
  bindArrayBuffer(nullptr);
}

string_t&
Engine::setTextureFormatToUse(const vector_t<string_t>& formatsAvailable)
{
  for (size_t i = 0; i < _texturesSupported.size(); ++i) {
    for (const auto& formatAvailable : formatsAvailable) {
      if (_texturesSupported[i] == String::toLowerCase(formatAvailable)) {
        _textureFormatInUse = _texturesSupported[i];
        return _textureFormatInUse;
      }
    }
  }
  // actively set format to nothing, to allow this to be called more than once
  // and possibly fail the 2nd time
  _textureFormatInUse.clear();
  return _textureFormatInUse;
}

unique_ptr_t<GL::IGLTexture> Engine::_createTexture()
{
  auto texture = _gl->createTexture();

  if (!texture) {
    BABYLON_LOG_ERROR("Engine", "Unable to create texture");
  }

  return texture;
}

InternalTexture* Engine::createTexture(
  const vector_t<string_t>& list, bool noMipmap, bool invertY, Scene* scene,
  unsigned int samplingMode,
  const ::std::function<void(InternalTexture*, EventState&)>& onLoad,
  const ::std::function<void()>& onError,
  const Variant<ArrayBuffer, Image>& buffer)
{
  if (list.empty()) {
    return nullptr;
  }

  return createTexture(list[0], noMipmap, invertY, scene, samplingMode, onLoad,
                       onError, buffer);
}

InternalTexture* Engine::createTexture(
  const string_t& urlArg, bool noMipmap, bool invertY, Scene* scene,
  unsigned int samplingMode,
  const ::std::function<void(InternalTexture*, EventState&)>& onLoad,
  const ::std::function<void()>& onError,
  const Variant<ArrayBuffer, Image>& buffer, InternalTexture* fallBack,
  unsigned int format)
{
  // assign a new string, so that the original is still available in case of
  // fallback
  auto url      = urlArg;
  auto fromData = url.substr(0, 5) == "data:";
  auto fromBlob = url.substr(0, 5) == "blob:";
  auto isBase64 = fromData && String::contains(url, "base64");

  auto texture = fallBack ?
                   fallBack :
                   new InternalTexture(this, InternalTexture::DATASOURCE_URL);

  // establish the file extension, if possible
  auto lastDotTmp = String::lastIndexOf(url, ".");
  size_t lastDot  = lastDotTmp >= 0 ? static_cast<size_t>(lastDotTmp) : 0;
  auto extension
    = (lastDot > 0) ? String::toLowerCase(url.substr(lastDot, url.size())) : "";
  auto isDDS = getCaps().s3tc && (extension == ".dds");
  auto isTGA = (extension == ".tga");

  // determine if a ktx file should be substituted
  auto isKTX = false;
  if (!_textureFormatInUse.empty() && !isBase64 && !fallBack) {
    url   = url.substr(0, lastDot) + _textureFormatInUse;
    isKTX = true;
  }

  if (scene) {
    scene->_addPendingData(texture);
  }
  texture->url             = url;
  texture->generateMipMaps = !noMipmap;
  texture->samplingMode    = samplingMode;
  texture->invertY         = invertY;

  if (!_doNotHandleContextLost) {
    // Keep a link to the buffer only if we plan to handle context lost
    texture->_buffer = buffer;
  }

  if (onLoad && !fallBack) {
    texture->onLoadedObservable.add(onLoad);
  }

  if (!fallBack) {
    _internalTexturesCache.emplace_back(texture);
  }

  const auto _onerror = [&](const string_t& /*msg*/) {
    if (scene) {
      scene->_removePendingData(texture);
    }

    // fallback for when compressed file not found to try again.  For instance,
    // etc1 does not have an alpha capable type
    if (isKTX) {
      createTexture(urlArg, noMipmap, invertY, scene, samplingMode, nullptr,
                    onError, buffer, texture);
    }
    else if (onError) {
      onError();
    }
  };

#if 0
  ::std::function<void(ArrayBufferView & arrayBuffer)> callback;
#endif

  // processing for non-image formats
  if (isKTX || isTGA || isDDS) {
    // Not implemented yet
  }
  else {
    auto onload = [&](const Image& img) {
      if (fromBlob && !_doNotHandleContextLost) {
        // We need to store the image if we need to rebuild the texture
        // in case of a webgl context lost
        texture->_buffer.set<Image>(img);
      }

      const auto processFunction =
        [&](int potWidth, int potHeight,
            const ::std::function<void()>& continuationCallback) {
          auto isPot = (img.width == potWidth && img.height == potHeight);
          auto internalFormat = format ?
                                  _getInternalFormat(format) :
                                  ((extension == ".jpg") ? GL::RGB : GL::RGBA);

          if (isPot) {
            _gl->texImage2D(GL::TEXTURE_2D, 0, GL::RGBA, img.width, img.height,
                            0, GL::RGBA, GL::UNSIGNED_BYTE, img.data);
            return false;
          }

          // Using shaders to rescale because canvas.drawImage is lossy
          auto source
            = new InternalTexture(this, InternalTexture::DATASOURCE_TEMP);
          _bindTextureDirectly(GL::TEXTURE_2D, source);
          _gl->texImage2D(GL::TEXTURE_2D, 0, GL::RGBA, img.width, img.height, 0,
                          GL::RGBA, GL::UNSIGNED_BYTE, img.data);

          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER,
                             GL::LINEAR);
          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER,
                             GL::LINEAR);
          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S,
                             GL::CLAMP_TO_EDGE);
          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T,
                             GL::CLAMP_TO_EDGE);

          _rescaleTexture(source, texture, scene, internalFormat, [&]() {
            _releaseTexture(source);
            _bindTextureDirectly(GL::TEXTURE_2D, texture);

            continuationCallback();
          });

          return true;
        };

      _prepareWebGLTexture(texture, scene, img.width, img.height, invertY,
                           noMipmap, false, processFunction, samplingMode);
    };

    if (!fromData || isBase64) {
      // Not implemented yet
    }
    else {
      Tools::LoadImage(url, onload, _onerror);
    }
  }

  return texture;
}

void Engine::_rescaleTexture(InternalTexture* source,
                             InternalTexture* destination, Scene* scene,
                             unsigned int internalFormat,
                             const ::std::function<void()>& onComplete)
{
  IRenderTargetOptions options;
  options.generateMipMaps       = false;
  options.type                  = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
  options.samplingMode          = TextureConstants::BILINEAR_SAMPLINGMODE;
  options.generateDepthBuffer   = false;
  options.generateStencilBuffer = false;

  auto rtt = createRenderTargetTexture(
    ISize(destination->width, destination->height), options);

  if (!_rescalePostProcess) {
    _rescalePostProcess = ::std::make_unique<PassPostProcess>(
      "rescale", 1.f, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, this,
      false, EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  }

  _rescalePostProcess->getEffect()->executeWhenCompiled(
    [&](Effect* /*effect*/) {
      _rescalePostProcess->setOnApply([&](Effect* effect, EventState&) {
        effect->_bindTexture("textureSampler", source);
      });

      auto hostingScene = scene;

      if (!hostingScene) {
        hostingScene = scenes.back();
      }
      hostingScene->postProcessManager->directRender(
        {_rescalePostProcess.get()}, rtt, true);

      _bindTextureDirectly(GL::TEXTURE_2D, destination);
      _gl->copyTexImage2D(GL::TEXTURE_2D, 0, internalFormat, 0, 0,
                          destination->width, destination->height, 0);

      unBindFramebuffer(rtt);
      _releaseTexture(rtt);

      if (onComplete) {
        onComplete();
      }
    });
}

GL::GLenum Engine::_getInternalFormat(unsigned int format) const
{
  GL::GLenum internalFormat = GL::RGBA;
  switch (format) {
    case EngineConstants::TEXTUREFORMAT_ALPHA:
      internalFormat = GL::ALPHA;
      break;
    case EngineConstants::TEXTUREFORMAT_LUMINANCE:
      internalFormat = GL::LUMINANCE;
      break;
    case EngineConstants::TEXTUREFORMAT_LUMINANCE_ALPHA:
      internalFormat = GL::LUMINANCE_ALPHA;
      break;
    case EngineConstants::TEXTUREFORMAT_RGB:
      internalFormat = GL::RGB;
      break;
    case EngineConstants::TEXTUREFORMAT_RGBA:
      internalFormat = GL::RGBA;
      break;
    default:
      break;
  }

  return internalFormat;
}

void Engine::updateRawTexture(InternalTexture* texture, const Uint8Array& data,
                              unsigned int format, bool invertY,
                              const string_t& compression, unsigned int type)
{
  if (!texture) {
    return;
  }

  auto internalFormat     = _getInternalFormat(format);
  auto internalSizedFomat = _getRGBABufferInternalSizedFormat(type);
  auto textureType        = _getWebGLTextureType(type);
  _bindTextureDirectly(GL::TEXTURE_2D, texture);
  _gl->pixelStorei(GL::UNPACK_FLIP_Y_WEBGL, invertY ? 1 : 0);

  if (!_doNotHandleContextLost) {
    texture->_bufferView  = data;
    texture->format       = format;
    texture->type         = type;
    texture->invertY      = invertY;
    texture->_compression = compression;
  }

  if (texture->width % 4 != 0) {
    _gl->pixelStorei(GL::UNPACK_ALIGNMENT, 1);
  }

  if (!compression.empty() && !data.empty()) {
    //_gl->compressedTexImage2D(GL::TEXTURE_2D, 0, getCaps().s3tc[compression],
    //                          texture->_width, texture->_height, 0, data);
  }
  else {
    auto _internalSizedFomat = static_cast<GL::GLint>(internalSizedFomat);
    _gl->texImage2D(GL::TEXTURE_2D, 0, _internalSizedFomat, texture->width,
                    texture->height, 0, internalFormat, textureType, data);
  }

  if (texture->generateMipMaps) {
    _gl->generateMipmap(GL::TEXTURE_2D);
  }
  _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  resetTextureCache();
  texture->isReady = true;
}

InternalTexture* Engine::createRawTexture(const Uint8Array& data, int width,
                                          int height, unsigned int format,
                                          bool generateMipMaps, bool invertY,
                                          unsigned int samplingMode,
                                          const string_t& compression,
                                          unsigned int type)
{
  auto texture = ::std::make_unique<InternalTexture>(
    this, InternalTexture::DATASOURCE_RAW);
  auto _texture             = texture.get();
  _texture->baseWidth       = width;
  _texture->baseHeight      = height;
  _texture->width           = width;
  _texture->height          = height;
  _texture->format          = format;
  _texture->generateMipMaps = generateMipMaps;
  _texture->samplingMode    = samplingMode;
  _texture->invertY         = invertY;
  _texture->_compression    = compression;
  texture->type             = type;

  if (!_doNotHandleContextLost) {
    _texture->_bufferView = data;
  }

  updateRawTexture(_texture, data, format, invertY, compression, type);
  _bindTextureDirectly(GL::TEXTURE_2D, _texture);

  // Filters
  auto filters = GetSamplingParameters(samplingMode, generateMipMaps);

  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, filters.mag);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, filters.min);

  if (generateMipMaps) {
    _gl->generateMipmap(GL::TEXTURE_2D);
  }

  _bindTextureDirectly(GL::TEXTURE_2D, nullptr);

  _texture->samplingMode = samplingMode;

  _internalTexturesCache.emplace_back(::std::move(texture));

  return _texture;
}

InternalTexture* Engine::createDynamicTexture(int width, int height,
                                              bool generateMipMaps,
                                              unsigned int samplingMode)
{
  auto texture = ::std::make_unique<InternalTexture>(
    this, InternalTexture::DATASOURCE_DYNAMIC);
  auto _texture       = texture.get();
  texture->baseWidth  = width;
  texture->baseHeight = height;

  if (generateMipMaps) {
    width = needPOTTextures() ?
              Tools::GetExponentOfTwo(width, _caps.maxTextureSize) :
              width;
    height = needPOTTextures() ?
               Tools::GetExponentOfTwo(height, _caps.maxTextureSize) :
               height;
  }

  resetTextureCache();
  texture->width           = width;
  texture->height          = height;
  texture->isReady         = false;
  texture->generateMipMaps = generateMipMaps;
  texture->samplingMode    = samplingMode;

  updateTextureSamplingMode(samplingMode, _texture);

  _internalTexturesCache.emplace_back(::std::move(texture));

  return _texture;
}

void Engine::updateTextureSamplingMode(unsigned int samplingMode,
                                       InternalTexture* texture)
{
  auto filters = GetSamplingParameters(samplingMode, texture->generateMipMaps);

  if (texture->isCube) {
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture);

    _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER,
                       filters.mag);
    _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER,
                       filters.min);
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
  }
  else if (texture->is3D) {
    _bindTextureDirectly(GL::TEXTURE_3D, texture);

    _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_MAG_FILTER, filters.mag);
    _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_MIN_FILTER, filters.min);
    _bindTextureDirectly(GL::TEXTURE_3D, nullptr);
  }
  else {
    _bindTextureDirectly(GL::TEXTURE_2D, texture);

    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, filters.mag);
    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, filters.min);
    _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  }

  texture->samplingMode = samplingMode;
}

void Engine::updateDynamicTexture(InternalTexture* texture, ICanvas* canvas,
                                  bool invertY, bool premulAlpha,
                                  unsigned int format)
{
  if (!texture) {
    return;
  }

  _bindTextureDirectly(GL::TEXTURE_2D, texture);
  _gl->pixelStorei(GL::UNPACK_FLIP_Y_WEBGL, invertY ? 1 : 0);
  if (premulAlpha) {
    _gl->pixelStorei(GL::UNPACK_PREMULTIPLY_ALPHA_WEBGL, 1);
  }
  auto internalFormat = format ? _getInternalFormat(format) : GL::RGBA;
  _gl->texImage2D(GL::TEXTURE_2D, 0, internalFormat, internalFormat,
                  GL::UNSIGNED_BYTE, canvas);
  if (texture->generateMipMaps) {
    _gl->generateMipmap(GL::TEXTURE_2D);
  }
  _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  if (premulAlpha) {
    _gl->pixelStorei(GL::UNPACK_PREMULTIPLY_ALPHA_WEBGL, 0);
  }
  resetTextureCache();
  texture->isReady = true;
}

InternalTexture*
Engine::createRenderTargetTexture(ISize size,
                                  const IRenderTargetOptions& options)
{
  // old version had a "generateMipMaps" arg instead of options.
  // if options.generateMipMaps is undefined, consider that options itself if
  // the generateMipmaps value in the same way, generateDepthBuffer is defaulted
  // to true
  bool generateMipMaps     = options.generateMipMaps;
  bool generateDepthBuffer = options.generateDepthBuffer;
  bool generateStencilBuffer
    = generateDepthBuffer && options.generateStencilBuffer;

  unsigned int type         = options.type;
  unsigned int samplingMode = options.samplingMode;

  if (type == EngineConstants::TEXTURETYPE_FLOAT
      && !_caps.textureFloatLinearFiltering) {
    // if floating point linear (GL::FLOAT) then force to NEAREST_SAMPLINGMODE
    samplingMode = TextureConstants::NEAREST_SAMPLINGMODE;
  }
  else if (type == EngineConstants::TEXTURETYPE_HALF_FLOAT
           && !_caps.textureHalfFloatLinearFiltering) {
    // if floating point linear (HALF_FLOAT) then force to NEAREST_SAMPLINGMODE
    samplingMode = TextureConstants::NEAREST_SAMPLINGMODE;
  }

  auto texture = ::std::make_unique<InternalTexture>(
    this, InternalTexture::DATASOURCE_RENDERTARGET);
  auto _texture = texture.get();
  _bindTextureDirectly(GL::TEXTURE_2D, _texture);

  int width  = size.width;
  int height = size.height;

  auto filters = GetSamplingParameters(samplingMode, generateMipMaps);

  if (type == EngineConstants::TEXTURETYPE_FLOAT && !_caps.textureFloat) {
    type = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
    BABYLON_LOG_WARN(
      "Engine",
      "Float textures are not supported. Render target forced to "
      "TEXTURETYPE_UNSIGNED_BYTE type");
  }

  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, filters.mag);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, filters.min);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

  _gl->texImage2D(GL::TEXTURE_2D, 0,
                  static_cast<int>(_getRGBABufferInternalSizedFormat(type)),
                  width, height, 0, GL::RGBA, _getWebGLTextureType(type),
                  Uint8Array());

  // Create the framebuffer
  auto framebuffer = _gl->createFramebuffer();
  bindUnboundFramebuffer(framebuffer.get());
  _gl->framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                            GL::TEXTURE_2D, texture->_webGLTexture.get(), 0);

  _texture->_depthStencilBuffer = _setupFramebufferDepthAttachments(
    generateStencilBuffer ? true : false, generateDepthBuffer, width, height);

  if (generateMipMaps) {
    _gl->generateMipmap(GL::TEXTURE_2D);
  }

  // Unbind
  _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  _gl->bindRenderbuffer(GL::RENDERBUFFER, nullptr);
  bindUnboundFramebuffer(nullptr);

  _texture->_framebuffer           = ::std::move(framebuffer);
  _texture->baseWidth              = width;
  _texture->baseHeight             = height;
  _texture->width                  = width;
  _texture->height                 = height;
  _texture->isReady                = true;
  _texture->samples                = 1;
  _texture->generateMipMaps        = generateMipMaps ? true : false;
  _texture->samplingMode           = samplingMode;
  _texture->type                   = type;
  _texture->_generateDepthBuffer   = generateDepthBuffer;
  _texture->_generateStencilBuffer = generateStencilBuffer ? true : false;

  resetTextureCache();

  _internalTexturesCache.emplace_back(::std::move(texture));

  return _texture;
}

vector_t<InternalTexture*>
Engine::createMultipleRenderTarget(ISize size,
                                   const IMultiRenderTargetOptions& options)
{
  auto generateMipMaps       = options.generateMipMaps;
  auto generateDepthBuffer   = options.generateDepthBuffer;
  auto generateStencilBuffer = options.generateStencilBuffer;
  auto generateDepthTexture  = options.generateDepthTexture;
  auto textureCount          = options.textureCount;

  auto defaultType         = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
  auto defaultSamplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE;

  const auto& types         = options.types;
  const auto& samplingModes = options.samplingModes;

  auto width  = size.width;
  auto height = size.height;

  // Create the framebuffer
  auto framebuffer = _gl->createFramebuffer();
  bindUnboundFramebuffer(framebuffer.get());

  vector_t<InternalTexture*> textures;
  vector_t<GL::GLenum> attachments;

  auto depthStencilBuffer = _setupFramebufferDepthAttachments(
    generateStencilBuffer, generateDepthBuffer, width, height);

  for (unsigned int i = 0; i < textureCount; ++i) {
    const auto iStr = ::std::to_string(i);
    auto samplingMode
      = (i < samplingModes.size()) ? samplingModes[i] : defaultSamplingMode;
    auto type = (i < types.size()) ? types[i] : defaultType;

    if (type == EngineConstants::TEXTURETYPE_FLOAT
        && !_caps.textureFloatLinearFiltering) {
      // if floating point linear (gl.FLOAT) then force to NEAREST_SAMPLINGMODE
      samplingMode = TextureConstants::NEAREST_SAMPLINGMODE;
    }
    else if (type == EngineConstants::TEXTURETYPE_HALF_FLOAT
             && !_caps.textureHalfFloatLinearFiltering) {
      // if floating point linear (HALF_FLOAT) then force to
      // NEAREST_SAMPLINGMODE
      samplingMode = TextureConstants::NEAREST_SAMPLINGMODE;
    }

    auto filters = GetSamplingParameters(samplingMode, generateMipMaps);
    if (type == EngineConstants::TEXTURETYPE_FLOAT && !_caps.textureFloat) {
      type = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
      BABYLON_LOG_WARN("Engine",
                       "Float textures are not supported. Render target forced "
                       "to TEXTURETYPE_UNSIGNED_BYTE type");
    }

    auto texture = ::std::make_unique<InternalTexture>(
      this, InternalTexture::DATASOURCE_MULTIRENDERTARGET);
    auto attachment
      = (*_gl)[webGLVersion() > 1 ? "COLOR_ATTACHMENT" + iStr :
                                    "COLOR_ATTACHMENT" + iStr + "_WEBGL"];
    textures.emplace_back(texture.get());
    attachments.emplace_back(attachment);

    _gl->activeTexture((*_gl)["TEXTURE" + iStr]);
    _gl->bindTexture(GL::TEXTURE_2D, texture->_webGLTexture.get());

    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, filters.mag);
    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, filters.min);
    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

    _gl->texImage2D(GL::TEXTURE_2D, 0, _getRGBABufferInternalSizedFormat(type),
                    width, height, 0, GL::RGBA, _getWebGLTextureType(type),
                    nullptr);

    _gl->framebufferTexture2D(GL::DRAW_FRAMEBUFFER, attachment, GL::TEXTURE_2D,
                              texture->_webGLTexture.get(), 0);

    if (generateMipMaps) {
      _gl->generateMipmap(GL::TEXTURE_2D);
    }

    // Unbind
    _bindTextureDirectly(GL::TEXTURE_2D, nullptr);

    texture->_framebuffer           = ::std::move(framebuffer);        // FIXME
    texture->_depthStencilBuffer    = ::std::move(depthStencilBuffer); // FIXME
    texture->baseWidth              = width;
    texture->baseHeight             = height;
    texture->width                  = width;
    texture->height                 = height;
    texture->isReady                = true;
    texture->samples                = 1;
    texture->generateMipMaps        = generateMipMaps;
    texture->samplingMode           = samplingMode;
    texture->type                   = type;
    texture->_generateDepthBuffer   = generateDepthBuffer;
    texture->_generateStencilBuffer = generateStencilBuffer;

    _internalTexturesCache.emplace_back(texture.get());
  }

  if (generateDepthTexture && _caps.depthTextureExtension) {
    // Depth texture
    auto depthTexture = ::std::make_unique<InternalTexture>(
      this, InternalTexture::DATASOURCE_MULTIRENDERTARGET);

    _gl->activeTexture(GL::TEXTURE0);
    _gl->bindTexture(GL::TEXTURE_2D, depthTexture->_webGLTexture.get());
    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, GL::NEAREST);
    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, GL::NEAREST);
    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
    _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);
    _gl->texImage2D(GL::TEXTURE_2D, //
                    0,              //
                    webGLVersion() < 2.f ? GL::DEPTH_COMPONENT :
                                           GL::DEPTH_COMPONENT16, //
                    width, height,                                //
                    0,                                            //
                    GL::DEPTH_COMPONENT,                          //
                    GL::UNSIGNED_SHORT,                           //
                    nullptr                                       //
    );

    _gl->framebufferTexture2D(GL::FRAMEBUFFER,                   //
                              GL::DEPTH_ATTACHMENT,              //
                              GL::TEXTURE_2D,                    //
                              depthTexture->_webGLTexture.get(), //
                              0                                  //
    );

    depthTexture->_framebuffer           = ::std::move(framebuffer); // FIXME
    depthTexture->baseWidth              = width;
    depthTexture->baseHeight             = height;
    depthTexture->width                  = width;
    depthTexture->height                 = height;
    depthTexture->isReady                = true;
    depthTexture->samples                = 1;
    depthTexture->generateMipMaps        = generateMipMaps;
    depthTexture->samplingMode           = GL::NEAREST;
    depthTexture->_generateDepthBuffer   = generateDepthBuffer;
    depthTexture->_generateStencilBuffer = generateStencilBuffer;

    textures.emplace_back(depthTexture.get());
    _internalTexturesCache.emplace_back(::std::move(depthTexture));
  }

  _gl->drawBuffers(attachments);
  _gl->bindRenderbuffer(GL::RENDERBUFFER, nullptr);
  bindUnboundFramebuffer(nullptr);

  resetTextureCache();

  return textures;
}

unique_ptr_t<GL::IGLRenderbuffer>
Engine::_setupFramebufferDepthAttachments(bool generateStencilBuffer,
                                          bool generateDepthBuffer, int width,
                                          int height, int samples)
{
  GLRenderBufferPtr depthStencilBuffer = nullptr;

  // Create the depth/stencil buffer
  if (generateStencilBuffer) {
    depthStencilBuffer = _gl->createRenderbuffer();
    _gl->bindRenderbuffer(GL::RENDERBUFFER, depthStencilBuffer);

    if (samples > 1) {
      _gl->renderbufferStorageMultisample(GL::RENDERBUFFER, samples,
                                          GL::DEPTH24_STENCIL8, width, height);
    }
    else {
      _gl->renderbufferStorage(GL::RENDERBUFFER, GL::DEPTH_STENCIL, width,
                               height);
    }

    _gl->framebufferRenderbuffer(GL::FRAMEBUFFER, GL::DEPTH_STENCIL_ATTACHMENT,
                                 GL::RENDERBUFFER, depthStencilBuffer);
  }
  else if (generateDepthBuffer) {
    depthStencilBuffer = _gl->createRenderbuffer();
    _gl->bindRenderbuffer(GL::RENDERBUFFER, depthStencilBuffer);

    if (samples > 1) {
      _gl->renderbufferStorageMultisample(GL::RENDERBUFFER, samples,
                                          GL::DEPTH_COMPONENT16, width, height);
    }
    else {
      _gl->renderbufferStorage(GL::RENDERBUFFER, GL::DEPTH_COMPONENT16, width,
                               height);
    }

    _gl->framebufferRenderbuffer(GL::FRAMEBUFFER, GL::DEPTH_ATTACHMENT,
                                 GL::RENDERBUFFER, depthStencilBuffer);
  }

  return depthStencilBuffer;
}

unsigned int
Engine::updateRenderTargetTextureSampleCount(InternalTexture* texture,
                                             unsigned int samples)
{
  if (webGLVersion() < 2.f || !texture) {
    return 1;
  }

  if (texture->samples == samples) {
    return samples;
  }

  samples = ::std::min(
    samples, static_cast<unsigned int>(_gl->getParameteri(GL::MAX_SAMPLES)));

  // Dispose previous render buffers
  if (texture->_depthStencilBuffer) {
    _gl->deleteRenderbuffer(texture->_depthStencilBuffer.get());
  }

  if (texture->_MSAAFramebuffer) {
    _gl->deleteFramebuffer(texture->_MSAAFramebuffer.get());
  }

  if (texture->_MSAARenderBuffer) {
    _gl->deleteRenderbuffer(texture->_MSAARenderBuffer.get());
  }

  if (samples > 1) {
    auto framebuffer = _gl->createFramebuffer();

    if (!framebuffer) {
      BABYLON_LOG_ERROR("Engine", "Unable to create multi sampled framebuffer");
      return 0;
    }

    texture->_MSAAFramebuffer = ::std::move(framebuffer);
    bindUnboundFramebuffer(texture->_MSAAFramebuffer.get());

    auto colorRenderbuffer = _gl->createRenderbuffer();

    if (!colorRenderbuffer) {
      BABYLON_LOG_ERROR("Engine", "Unable to create multi sampled framebuffer");
      return 0;
    }

    _gl->bindRenderbuffer(GL::RENDERBUFFER, colorRenderbuffer);
    _gl->renderbufferStorageMultisample(GL::RENDERBUFFER,
                                        static_cast<int>(samples), GL::RGBA8,
                                        texture->width, texture->height);

    _gl->framebufferRenderbuffer(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                                 GL::RENDERBUFFER, colorRenderbuffer);

    texture->_MSAARenderBuffer = ::std::move(colorRenderbuffer);
  }
  else {
    bindUnboundFramebuffer(texture->_framebuffer.get());
  }

  texture->samples             = samples;
  texture->_depthStencilBuffer = _setupFramebufferDepthAttachments(
    texture->_generateStencilBuffer, texture->_generateDepthBuffer,
    texture->width, texture->height, static_cast<int>(samples));

  _gl->bindRenderbuffer(GL::RENDERBUFFER, nullptr);
  bindUnboundFramebuffer(nullptr);

  return samples;
}

void Engine::_uploadDataToTexture(unsigned int target, int lod,
                                  int internalFormat, int width, int height,
                                  unsigned int format, unsigned int type,
                                  const Uint8Array& data)
{
  _gl->texImage2D(target, lod, internalFormat, width, height, 0, format, type,
                  data);
}

void Engine::_uploadCompressedDataToTexture(unsigned int target, int lod,
                                            unsigned int internalFormat,
                                            int width, int height,
                                            const Uint8Array& data)
{
  _gl->compressedTexImage2D(target, lod, internalFormat, width, height, 0,
                            data);
}

InternalTexture* Engine::createRenderTargetCubeTexture(
  const ISize& size, const RenderTargetCreationOptions& options)
{
  auto texture = ::std::make_unique<InternalTexture>(
    this, InternalTexture::DATASOURCE_RENDERTARGET);
  auto _texture = texture.get();

  bool generateMipMaps     = options.generateMipMaps;
  bool generateDepthBuffer = options.generateDepthBuffer;
  bool generateStencilBuffer
    = (generateDepthBuffer && options.generateStencilBuffer) ? true : false;

  unsigned int samplingMode = options.samplingMode;

  _texture->isCube          = true;
  _texture->generateMipMaps = generateMipMaps;
  _texture->samples         = 1;
  _texture->samplingMode    = samplingMode;

  auto filters = Engine::GetSamplingParameters(samplingMode, generateMipMaps);

  _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, _texture);

  for (unsigned int face = 0; face < 6; ++face) {
    _gl->texImage2D((GL::TEXTURE_CUBE_MAP_POSITIVE_X + face), 0, GL::RGBA,
                    size.width, size.height, 0, GL::RGBA, GL::UNSIGNED_BYTE,
                    Uint8Array());
  }

  _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER, filters.mag);
  _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER, filters.min);
  _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_S,
                     GL::CLAMP_TO_EDGE);
  _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_T,
                     GL::CLAMP_TO_EDGE);

  // Create the depth buffer
  GLRenderBufferPtr depthStencilBuffer = nullptr;

  // Create the framebuffer
  GLFrameBufferPtr framebuffer = _gl->createFramebuffer();
  bindUnboundFramebuffer(framebuffer.get());

  texture->_depthStencilBuffer = _setupFramebufferDepthAttachments(
    generateStencilBuffer, generateDepthBuffer, size.width, size.height);

  // Mipmaps
  if (_texture->generateMipMaps) {
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, _texture);
    _gl->generateMipmap(GL::TEXTURE_CUBE_MAP);
  }

  // Unbind
  _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
  _gl->bindRenderbuffer(GL::RENDERBUFFER, nullptr);
  bindUnboundFramebuffer(nullptr);

  _texture->_framebuffer = ::std::move(framebuffer);
  _texture->width        = size.width;
  _texture->height       = size.height;
  _texture->isReady      = true;

  resetTextureCache();

  _internalTexturesCache.emplace_back(::std::move(texture));

  return _texture;
}

InternalTexture* Engine::createPrefilteredCubeTexture(
  const string_t& /*rootUrl*/, Scene* /*scene*/, float /*scale*/,
  float /*offset*/,
  const ::std::function<void(InternalTexture*, EventState&)>& /*onLoad*/,
  const ::std::function<void()>& /*onError*/, unsigned int /*format*/,
  const string_t& /*forcedExtension*/)
{
  return nullptr;
}

InternalTexture* Engine::createCubeTexture(
  const string_t& /*rootUrl*/, Scene* /*scene*/,
  const vector_t<string_t>& /*extensions*/, bool /*noMipmap*/,
  const ::std::function<void(InternalTexture*, EventState&)>& /*onLoad*/,
  const ::std::function<void()>& /*onError*/, unsigned int /*format*/,
  const string_t& /*forcedExtension*/)
{
  return nullptr;
}

void Engine::setCubeMapTextureParams(GL::IGLRenderingContext* gl,
                                     bool loadMipmap)
{
  gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER, GL::LINEAR);
  gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER,
                    loadMipmap ? GL::LINEAR_MIPMAP_LINEAR : GL::LINEAR);
  gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_S,
                    GL::CLAMP_TO_EDGE);
  gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_T,
                    GL::CLAMP_TO_EDGE);

  _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);

  resetTextureCache();
}

void Engine::updateRawCubeTexture(InternalTexture* texture,
                                  const vector_t<Uint8Array>& data,
                                  unsigned int format, unsigned int type,
                                  bool invertY, const string_t& compression,
                                  unsigned int level)
{
  texture->_bufferViewArray = data;
  texture->format           = format;
  texture->type             = type;
  texture->invertY          = invertY;
  texture->_compression     = compression;

  // auto textureType        = _getWebGLTextureType(type);
  auto internalFormat = _getInternalFormat(format);
  // auto internalSizedFomat = _getRGBABufferInternalSizedFormat(type);

  bool needConversion = false;
  if (internalFormat == GL::RGB) {
    internalFormat = GL::RGBA;
    needConversion = true;
  }

  _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture);
  _gl->pixelStorei(GL::UNPACK_FLIP_Y_WEBGL, invertY ? 1 : 0);

  if (texture->width % 4 != 0) {
    _gl->pixelStorei(GL::UNPACK_ALIGNMENT, 1);
  }

  const vector_t<unsigned int> facesIndex
    = {GL::TEXTURE_CUBE_MAP_POSITIVE_X, GL::TEXTURE_CUBE_MAP_POSITIVE_Y,
       GL::TEXTURE_CUBE_MAP_POSITIVE_Z, GL::TEXTURE_CUBE_MAP_NEGATIVE_X,
       GL::TEXTURE_CUBE_MAP_NEGATIVE_Y, GL::TEXTURE_CUBE_MAP_NEGATIVE_Z};

  // Data are known to be in +X +Y +Z -X -Y -Z
  for (std::size_t index = 0; index < facesIndex.size(); ++index) {
    // const auto& faceData = data[index];

    if (!compression.empty()) {
      // _gl->compressedTexImage2D(facesIndex[index], level,
      // getCaps().s3tc[compression], texture->_width, texture->_height, 0,
      // faceData);
    }
    else {
      if (needConversion) {
        // faceData = _convertRGBtoRGBATextureData(faceData, texture->_width,
        // texture->_height, type);
      }
      // _gl->texImage2D(facesIndex[index], level, internalSizedFomat,
      // texture->_width, texture->_height, 0, internalFormat, textureType,
      // faceData);
    }
  }

  auto isPot = !needPOTTextures()
               || (Tools::IsExponentOfTwo(texture->width)
                   && Tools::IsExponentOfTwo(texture->height));
  if (isPot && texture->generateMipMaps && level == 0) {
    _gl->generateMipmap(GL::TEXTURE_CUBE_MAP);
  }
  _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);

  resetTextureCache();
  texture->isReady = true;
}

InternalTexture* Engine::createRawCubeTexture(
  const vector_t<Uint8Array> data, int size, unsigned int format,
  unsigned int type, bool generateMipMaps, bool invertY,
  unsigned int samplingMode, const string_t& compression)
{
  auto texture = new InternalTexture(this, InternalTexture::DATASOURCE_CUBERAW);
  texture->isCube          = true;
  texture->generateMipMaps = generateMipMaps;
  texture->format          = format;
  texture->type            = type;
  if (!_doNotHandleContextLost) {
    texture->_bufferViewArray = data;
  }

  auto textureType    = _getWebGLTextureType(type);
  auto internalFormat = _getInternalFormat(format);

  if (internalFormat == GL::RGB) {
    internalFormat = GL::RGBA;
  }

  int width  = size;
  int height = width;

  texture->width  = width;
  texture->height = height;

  // Double check on POT to generate Mips.
  auto isPot = !needPOTTextures()
               || (Tools::IsExponentOfTwo(texture->width)
                   && Tools::IsExponentOfTwo(texture->height));
  if (!isPot) {
    generateMipMaps = false;
  }
  texture->generateMipMaps = generateMipMaps;

  // Upload data if needed. The texture won't be ready until then.
  if (!data.empty()) {
    updateRawCubeTexture(texture, data, format, type, invertY, compression);
  }

  _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture);

  // Filters
  if (!data.empty() && generateMipMaps) {
    _gl->generateMipmap(GL::TEXTURE_CUBE_MAP);
  }

  if (textureType == GL::FLOAT && !_caps.textureFloatLinearFiltering) {
    _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER,
                       GL::NEAREST);
    _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER,
                       GL::NEAREST);
  }
  else if (textureType == _gl->HALF_FLOAT_OES
           && !_caps.textureHalfFloatLinearFiltering) {
    _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER,
                       GL::NEAREST);
    _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER,
                       GL::NEAREST);
  }
  else {
    auto filters = GetSamplingParameters(samplingMode, generateMipMaps);
    _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER,
                       filters.mag);
    _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER,
                       filters.min);
  }

  _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_S,
                     GL::CLAMP_TO_EDGE);
  _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_T,
                     GL::CLAMP_TO_EDGE);
  _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);

  return texture;
}

InternalTexture* Engine::createRawCubeTextureFromUrl(
  const string_t& /*url*/, Scene* /*scene*/, int /*size*/,
  unsigned int /*format*/, unsigned int /*type*/, bool /*noMipmap*/,
  const ::std::function<ArrayBufferViewArray(const Uint8Array& arrayBuffer)>&
  /*callback*/,
  const ::std::function<vector_t<ArrayBufferViewArray>(
    const ArrayBufferViewArray& faces)>& /*mipmmapGenerator*/,
  const ::std::function<void()>& /*onLoad*/,
  const ::std::function<void()>& /*onError*/, unsigned int /*samplingMode*/,
  bool /*invertY*/)
{
  return nullptr;
}

void Engine::updateRawTexture3D(InternalTexture* texture,
                                const ArrayBuffer& data, unsigned int format,
                                bool invertY, const string_t& compression)
{
  auto internalFormat = _getInternalFormat(format);
  _bindTextureDirectly(GL::TEXTURE_3D, texture);
  _gl->pixelStorei(GL::UNPACK_FLIP_Y_WEBGL, invertY ? 1 : 0);

  if (!_doNotHandleContextLost) {
    texture->_bufferView  = data;
    texture->format       = format;
    texture->invertY      = invertY;
    texture->_compression = compression;
  }

  if (texture->width % 4 != 0) {
    _gl->pixelStorei(GL::UNPACK_ALIGNMENT, 1);
  }

  if (!compression.empty() && !data.empty()) {
    // _gl.compressedTexImage3D(GL::TEXTURE_3D, 0,
    // (<any>getCaps().s3tc)[compression], texture->width, texture->height,
    // texture->depth, 0, data);
  }
  else {
    _gl->texImage3D(GL::TEXTURE_3D, 0, static_cast<int>(internalFormat),
                    texture->width, texture->height, texture->depth, 0,
                    internalFormat, GL::UNSIGNED_BYTE, data);
  }

  if (texture->generateMipMaps) {
    _gl->generateMipmap(GL::TEXTURE_3D);
  }
  _bindTextureDirectly(GL::TEXTURE_3D, nullptr);
  resetTextureCache();
  texture->isReady = true;
}

InternalTexture* Engine::createRawTexture3D(const ArrayBuffer& data, int width,
                                            int height, int depth,
                                            unsigned int format,
                                            bool generateMipMaps, bool invertY,
                                            unsigned int samplingMode,
                                            const string_t& compression)
{
  auto texture = new InternalTexture(this, InternalTexture::DATASOURCE_RAW3D);
  texture->baseWidth       = width;
  texture->baseHeight      = height;
  texture->baseDepth       = depth;
  texture->width           = width;
  texture->height          = height;
  texture->depth           = depth;
  texture->format          = format;
  texture->generateMipMaps = generateMipMaps;
  texture->samplingMode    = samplingMode;
  texture->is3D            = true;

  if (!_doNotHandleContextLost) {
    texture->_bufferView = data;
  }

  updateRawTexture3D(texture, data, format, invertY, compression);
  _bindTextureDirectly(GL::TEXTURE_3D, texture);

  // Filters
  auto filters = GetSamplingParameters(samplingMode, generateMipMaps);

  _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_MAG_FILTER, filters.mag);
  _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_MIN_FILTER, filters.min);

  if (generateMipMaps) {
    _gl->generateMipmap(GL::TEXTURE_3D);
  }

  _bindTextureDirectly(GL::TEXTURE_3D, nullptr);

  _internalTexturesCache.emplace_back(texture);

  return texture;
}

void Engine::_prepareWebGLTextureContinuation(InternalTexture* texture,
                                              Scene* scene, bool noMipmap,
                                              bool isCompressed,
                                              unsigned int samplingMode)
{
  if (!_gl) {
    return;
  }

  auto filters = GetSamplingParameters(samplingMode, !noMipmap);

  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, filters.mag);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, filters.min);

  if (!noMipmap && !isCompressed) {
    _gl->generateMipmap(GL::TEXTURE_2D);
  }

  _bindTextureDirectly(GL::TEXTURE_2D, nullptr);

  resetTextureCache();
  if (scene) {
    scene->_removePendingData(texture);
  }

  texture->onLoadedObservable.notifyObservers(texture);
  texture->onLoadedObservable.clear();
}

void Engine::_prepareWebGLTexture(
  InternalTexture* texture, Scene* scene, int width, int height,
  Nullable<bool> invertY, bool noMipmap, bool isCompressed,
  const ::std::function<
    bool(int width, int height,
         const ::std::function<void()>& continuationCallback)>& processFunction,
  unsigned int samplingMode)
{
  auto potWidth = needPOTTextures() ?
                    Tools::GetExponentOfTwo(width, getCaps().maxTextureSize) :
                    width;
  auto potHeight = needPOTTextures() ?
                     Tools::GetExponentOfTwo(height, getCaps().maxTextureSize) :
                     height;

  if (!_gl) {
    return;
  }

  if (!texture->_webGLTexture) {
    resetTextureCache();
    if (scene) {
      scene->_removePendingData(texture);
    }

    return;
  }

  _bindTextureDirectly(GL::TEXTURE_2D, texture);
  _gl->pixelStorei(GL::UNPACK_FLIP_Y_WEBGL,
                   invertY.isNull() ? 1 : (*invertY ? 1 : 0));

  texture->baseWidth  = width;
  texture->baseHeight = height;
  texture->width      = potWidth;
  texture->height     = potHeight;
  texture->isReady    = true;

  if (processFunction(potWidth, potHeight, [&]() {
        _prepareWebGLTextureContinuation(texture, scene, noMipmap, isCompressed,
                                         samplingMode);
      })) {
    // Returning as texture needs extra async steps
    return;
  }

  _prepareWebGLTextureContinuation(texture, scene, noMipmap, isCompressed,
                                   samplingMode);
}

ArrayBufferView
Engine::_convertRGBtoRGBATextureData(const ArrayBufferView& rgbData, int width,
                                     int height, unsigned int textureType)
{
  // Create new RGBA data container.
  if (textureType == EngineConstants::TEXTURETYPE_FLOAT) {
    Float32Array rgbaData(static_cast<size_t>(width * height * 4));
    // Convert each pixel.
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        size_t index    = static_cast<size_t>((y * width + x) * 3);
        size_t newIndex = static_cast<size_t>((y * width + x) * 4);

        // Map Old Value to new value.
        rgbaData[newIndex + 0] = rgbData.float32Array[index + 0];
        rgbaData[newIndex + 1] = rgbData.float32Array[index + 1];
        rgbaData[newIndex + 2] = rgbData.float32Array[index + 2];

        // Add fully opaque alpha channel.
        rgbaData[newIndex + 3] = 1.f;
      }
    }
    return ArrayBufferView(rgbaData);
  }
  else {
    Uint32Array rgbaData(static_cast<size_t>(width * height * 4));
    // Convert each pixel.
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        size_t index    = static_cast<size_t>((y * width + x) * 3);
        size_t newIndex = static_cast<size_t>((y * width + x) * 4);

        // Map Old Value to new value.
        rgbaData[newIndex + 0] = rgbData.uint32Array[index + 0];
        rgbaData[newIndex + 1] = rgbData.uint32Array[index + 1];
        rgbaData[newIndex + 2] = rgbData.uint32Array[index + 2];

        // Add fully opaque alpha channel.
        rgbaData[newIndex + 3] = 1;
      }
    }
    return ArrayBufferView(rgbaData);
  }
}

void Engine::_releaseFramebufferObjects(InternalTexture* texture)
{
  if (texture->_framebuffer) {
    _gl->deleteFramebuffer(texture->_framebuffer.get());
    texture->_framebuffer = nullptr;
  }

  if (texture->_depthStencilBuffer) {
    _gl->deleteRenderbuffer(texture->_depthStencilBuffer.get());
    texture->_depthStencilBuffer = nullptr;
  }

  if (texture->_MSAAFramebuffer) {
    _gl->deleteFramebuffer(texture->_MSAAFramebuffer.get());
    texture->_MSAAFramebuffer.reset(nullptr);
  }

  if (texture->_MSAARenderBuffer) {
    _gl->deleteRenderbuffer(texture->_MSAARenderBuffer.get());
    texture->_MSAARenderBuffer.reset(nullptr);
  }
}

void Engine::_releaseTexture(InternalTexture* texture)
{
  _releaseFramebufferObjects(texture);

  _gl->deleteTexture(texture->_webGLTexture.get());

  // Unbind channels
  unbindAllTextures();

  _internalTexturesCache.erase(
    ::std::remove_if(_internalTexturesCache.begin(),
                     _internalTexturesCache.end(),
                     [&texture](const unique_ptr_t<InternalTexture>& _texture) {
                       return _texture.get() == texture;
                     }),
    _internalTexturesCache.end());

  // Integrated fixed lod samplers.
  if (texture->_lodTextureHigh) {
    texture->_lodTextureHigh->dispose();
  }
  if (texture->_lodTextureMid) {
    texture->_lodTextureMid->dispose();
  }
  if (texture->_lodTextureLow) {
    texture->_lodTextureLow->dispose();
  }
}

void Engine::setProgram(GL::IGLProgram* program)
{
  if (_currentProgram != program) {
    _gl->useProgram(program);
    _currentProgram = program;
  }
}

void Engine::bindSamplers(Effect* effect)
{
  setProgram(effect->getProgram());

  const auto& samplers = effect->getSamplers();
  for (size_t index = 0; index < samplers.size(); ++index) {
    auto uniform = effect->getUniform(samplers[index]);
    _gl->uniform1i(uniform, static_cast<int>(index));
  }
  _currentEffect = nullptr;
}

void Engine::activateTextureChannel(unsigned int textureChannel)
{
  if (_activeTextureChannel != textureChannel) {
    _gl->activeTexture(textureChannel);
    _activeTextureChannel = textureChannel;
  }
}

void Engine::_bindTextureDirectly(unsigned int target, InternalTexture* texture)
{
  if ((_boundTexturesCache.find(_activeTextureChannel)
       != _boundTexturesCache.end())
      && (_boundTexturesCache[_activeTextureChannel] != texture)) {
    _gl->bindTexture(target, texture ? texture->_webGLTexture.get() : nullptr);
    _boundTexturesCache[_activeTextureChannel] = texture;
  }
}

void Engine::_bindTexture(int channel, InternalTexture* texture)
{
  if (channel < 0) {
    return;
  }

  activateTextureChannel(GL::TEXTURE0 + static_cast<unsigned>(channel));
  _bindTextureDirectly(GL::TEXTURE_2D, texture);
}

void Engine::setTextureFromPostProcess(int channel, PostProcess* postProcess)
{
  const auto _ind = static_cast<size_t>(postProcess->_currentRenderTextureInd);
  _bindTexture(channel, postProcess ? postProcess->_textures[_ind] : nullptr);
}

void Engine::unbindAllTextures()
{
  for (unsigned int channel = 0,
                    ul = static_cast<unsigned>(_caps.maxTexturesImageUnits);
       channel < ul; ++channel) {
    activateTextureChannel(GL::TEXTURE0 + channel);
    _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
    if (webGLVersion() > 1.f) {
      _bindTextureDirectly(GL::TEXTURE_3D, nullptr);
    }
  }
}

void Engine::setTexture(int channel, GL::IGLUniformLocation* uniform,
                        BaseTexture* texture)
{
  if (channel < 0) {
    return;
  }

  if (_setTexture(static_cast<unsigned>(channel), texture)) {
    _gl->uniform1i(uniform, channel);
  }
}

bool Engine::_setTexture(unsigned int channel, BaseTexture* texture)
{
  // Not ready?
  if (!texture) {
    if ((_boundTexturesCache.find(channel) != _boundTexturesCache.end())
        && (_boundTexturesCache[channel] != nullptr)) {
      activateTextureChannel(GL::TEXTURE0 + channel);
      _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
      _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
      if (webGLVersion() > 1.f) {
        _bindTextureDirectly(GL::TEXTURE_3D, nullptr);
      }
    }
    return false;
  }

  // Video (not supported)
  bool alreadyActivated = false;
  if (texture->delayLoadState == EngineConstants::DELAYLOADSTATE_NOTLOADED) {
    // Delay loading
    texture->delayLoad();
    return false;
  }

  InternalTexture* internalTexture = nullptr;
  if (texture->isReady()) {
    internalTexture = texture->getInternalTexture();
  }
  else if (texture->isCube) {
    internalTexture = emptyCubeTexture();
  }
  else if (texture->is3D) {
    internalTexture = emptyTexture3D();
  }
  else {
    internalTexture = emptyTexture();
  }

  if (!alreadyActivated) {
    activateTextureChannel(GL::TEXTURE0 + channel);
  }

  if ((_boundTexturesCache.find(_activeTextureChannel)
       != _boundTexturesCache.end())
      && (_boundTexturesCache[_activeTextureChannel] == internalTexture)) {
    return false;
  }

  if (internalTexture && internalTexture->is3D) {
    _bindTextureDirectly(GL::TEXTURE_3D, internalTexture);

    if (internalTexture && internalTexture->_cachedWrapU != texture->wrapU) {
      internalTexture->_cachedWrapU = texture->wrapU;

      switch (texture->wrapU) {
        case TextureConstants::WRAP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_WRAP_S, GL::REPEAT);
          break;
        case TextureConstants::CLAMP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_WRAP_S,
                             GL::CLAMP_TO_EDGE);
          break;
        case TextureConstants::MIRROR_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_WRAP_S,
                             GL::MIRRORED_REPEAT);
          break;
      }
    }

    if (internalTexture && internalTexture->_cachedWrapV != texture->wrapV) {
      internalTexture->_cachedWrapV = texture->wrapV;
      switch (texture->wrapV) {
        case TextureConstants::WRAP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_WRAP_T, GL::REPEAT);
          break;
        case TextureConstants::CLAMP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_WRAP_T,
                             GL::CLAMP_TO_EDGE);
          break;
        case TextureConstants::MIRROR_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_WRAP_T,
                             GL::MIRRORED_REPEAT);
          break;
      }
    }

    if (internalTexture && internalTexture->_cachedWrapR != texture->wrapR) {
      internalTexture->_cachedWrapR = texture->wrapR;
      switch (texture->wrapV) {
        case TextureConstants::WRAP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_WRAP_R, GL::REPEAT);
          break;
        case TextureConstants::CLAMP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_WRAP_R,
                             GL::CLAMP_TO_EDGE);
          break;
        case TextureConstants::MIRROR_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_3D, GL::TEXTURE_WRAP_R,
                             GL::MIRRORED_REPEAT);
          break;
      }
    }

    _setAnisotropicLevel(GL::TEXTURE_3D, texture);
  }
  else if (internalTexture && internalTexture->isCube) {
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, internalTexture);

    if (internalTexture->_cachedCoordinatesMode != texture->coordinatesMode()) {
      internalTexture->_cachedCoordinatesMode = texture->coordinatesMode();
      // CUBIC_MODE and SKYBOX_MODE both require CLAMP_TO_EDGE.  All other modes
      // use REPEAT.
      auto textureWrapMode
        = (texture->coordinatesMode() != TextureConstants::CUBIC_MODE
           && texture->coordinatesMode() != TextureConstants::SKYBOX_MODE) ?
            GL::REPEAT :
            GL::CLAMP_TO_EDGE;
      _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_S,
                         textureWrapMode);
      _gl->texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_T,
                         textureWrapMode);
    }

    _setAnisotropicLevel(GL::TEXTURE_CUBE_MAP, texture);
  }
  else {
    _bindTextureDirectly(GL::TEXTURE_2D, internalTexture);

    if (internalTexture && internalTexture->_cachedWrapU != texture->wrapU) {
      internalTexture->_cachedWrapU = texture->wrapU;

      switch (texture->wrapU) {
        case TextureConstants::WRAP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S, GL::REPEAT);
          break;
        case TextureConstants::CLAMP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S,
                             GL::CLAMP_TO_EDGE);
          break;
        case TextureConstants::MIRROR_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S,
                             GL::MIRRORED_REPEAT);
          break;
        default:
          break;
      }
    }

    if (internalTexture && internalTexture->_cachedWrapV != texture->wrapV) {
      internalTexture->_cachedWrapV = texture->wrapV;
      switch (texture->wrapV) {
        case TextureConstants::WRAP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T, GL::REPEAT);
          break;
        case TextureConstants::CLAMP_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T,
                             GL::CLAMP_TO_EDGE);
          break;
        case TextureConstants::MIRROR_ADDRESSMODE:
          _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T,
                             GL::MIRRORED_REPEAT);
          break;
        default:
          break;
      }
    }

    _setAnisotropicLevel(GL::TEXTURE_2D, texture);
  }

  return true;
}

void Engine::setTextureArray(int channel, GL::IGLUniformLocation* uniform,
                             const vector_t<BaseTexture*>& textures)
{
  if (channel < 0 || !uniform) {
    return;
  }

  auto channeli = static_cast<unsigned>(channel);

  if (_textureUnits.empty() || _textureUnits.size() != textures.size()) {
    _textureUnits.clear();
    _textureUnits.resize(textures.size());
  }
  int _i = 0;
  for (unsigned int i = 0; i < textures.size(); ++i, ++_i) {
    _textureUnits[i] = channel + _i;
  }
  _gl->uniform1iv(uniform, _textureUnits);

  unsigned int index = 0;
  for (auto& texture : textures) {
    _setTexture(channeli + index, texture);
    ++index;
  }
}

void Engine::_setAnisotropicLevel(unsigned int key, BaseTexture* texture)
{
  auto internalTexture = texture->getInternalTexture();

  if (!internalTexture) {
    return;
  }

  auto anisotropicFilterExtension = _caps.textureAnisotropicFilterExtension;
  auto value                      = texture->anisotropicFilteringLevel;

  if (internalTexture->samplingMode
        != TextureConstants::LINEAR_LINEAR_MIPNEAREST
      && internalTexture->samplingMode
           != TextureConstants::LINEAR_LINEAR_MIPLINEAR
      && internalTexture->samplingMode != TextureConstants::LINEAR_LINEAR) {
    value = 1; // Forcing the anisotropic to 1 because else webgl will force
               // filters to linear
  }

  if (anisotropicFilterExtension
      && internalTexture->_cachedAnisotropicFilteringLevel != value) {
    _gl->texParameterf(
      key, AnisotropicFilterExtension::TEXTURE_MAX_ANISOTROPY_EXT,
      static_cast<float>(::std::min(value, _caps.maxAnisotropy)));
    internalTexture->_cachedAnisotropicFilteringLevel = value;
  }
}

Uint8Array Engine::readPixels(int x, int y, int width, int height)
{
  Uint8Array data(static_cast<unsigned int>(height * width * 4));
  _gl->readPixels(x, y, width, height, GL::RGBA, GL::UNSIGNED_BYTE, data);
  return data;
}

void Engine::unbindAllAttributes()
{
  if (_mustWipeVertexAttributes) {
    _mustWipeVertexAttributes = false;

    const unsigned int ul = static_cast<unsigned>(_caps.maxVertexAttribs);
    if (_vertexAttribArraysEnabled.size() < ul) {
      _vertexAttribArraysEnabled.resize(ul);
    }

    for (unsigned int i = 0; i < ul; ++i) {
      _gl->disableVertexAttribArray(i);
      _vertexAttribArraysEnabled[i]    = false;
      _currentBufferPointers[i].active = false;
    }
    _currentBufferPointers.clear();
    return;
  }

  for (unsigned int i = 0; i < _vertexAttribArraysEnabled.size(); i++) {
    if (i >= static_cast<unsigned>(_caps.maxVertexAttribs)
        || !_vertexAttribArraysEnabled[i]) {
      continue;
    }
    _gl->disableVertexAttribArray(i);
    _vertexAttribArraysEnabled[i]    = false;
    _currentBufferPointers[i].active = false;
  }
  _currentBufferPointers.clear();
}

void Engine::releaseEffects()
{
  for (auto& item : _compiledEffects) {
    _deleteProgram(item.second->getProgram());
  }

  _compiledEffects.clear();
}

// Dispose
void Engine::dispose(bool /*doNotRecurse*/)
{
  hideLoadingUI();
  stopRenderLoop();

  // Release postProcesses
  for (auto& postProcess : postProcesses) {
    postProcess->dispose();
  }

  // Empty texture
  if (_emptyTexture) {
    _releaseTexture(_emptyTexture);
    _emptyTexture = nullptr;
  }

  if (_emptyCubeTexture) {
    _releaseTexture(_emptyCubeTexture.get());
    _emptyCubeTexture.reset(nullptr);
  }

  // Rescale PP
  if (_rescalePostProcess) {
    _rescalePostProcess->dispose();
  }

  // Release scenes
  for (auto& scene : scenes) {
    scene->dispose();
  }
  scenes.clear();

  // Release audio engine
  // if (Engine::audioEngine) {
  //  Engine::audioEngine->dispose();
  // }

  // Release effects
  releaseEffects();

  // Unbind
  unbindAllAttributes();

  if (_dummyFramebuffer) {
    _gl->deleteFramebuffer(_dummyFramebuffer.get());
  }

  // WebVR
  // disableVR();

  // Remove from Instances
  Engine::Instances.erase(
    ::std::remove(Engine::Instances.begin(), Engine::Instances.end(), this),
    Engine::Instances.end());

  _workingCanvas  = nullptr;
  _workingContext = nullptr;
  _currentBufferPointers.clear();
  _renderingCanvas = nullptr;
  _currentProgram  = nullptr;

  onResizeObservable.clear();
  onCanvasBlurObservable.clear();
  onCanvasFocusObservable.clear();
  onCanvasPointerOutObservable.clear();
  onBeginFrameObservable.clear();
  onEndFrameObservable.clear();

  Effect::ResetCache();
}

// Loading screen
void Engine::displayLoadingUI()
{
  if (_loadingScreen) {
    _loadingScreen->displayLoadingUI();
  }
}

void Engine::hideLoadingUI()
{
  if (_loadingScreen) {
    _loadingScreen->hideLoadingUI();
  }
}

ILoadingScreen* Engine::loadingScreen()
{
  return _loadingScreen;
}

void Engine::setLoadingScreen(ILoadingScreen* loadingScreen_)
{
  _loadingScreen = loadingScreen_;
}

void Engine::setLoadingUIText(const string_t& text)
{
  _loadingScreen->loadingUIText = text;
}

void Engine::loadingUIBackgroundColor(const string_t& color)
{
  _loadingScreen->loadingUIBackgroundColor = color;
}

string_t Engine::getVertexShaderSource(GL::IGLProgram* program)
{
  auto shaders = _gl->getAttachedShaders(program);
  if (shaders.empty()) {
    BABYLON_LOGF_ERROR("Engine",
                       "Unable to get vertex shader source for program %d",
                       program->value);
    return "";
  }

  return _gl->getShaderSource(shaders[0]);
}

string_t Engine::getFragmentShaderSource(GL::IGLProgram* program)
{
  auto shaders = _gl->getAttachedShaders(program);
  if (shaders.size() < 2) {
    BABYLON_LOGF_ERROR("Engine",
                       "Unable to get fragment shader source for program %d",
                       program->value);
    return "";
  }

  return _gl->getShaderSource(shaders[1]);
}

unsigned int Engine::getError() const
{
  return _gl->getError();
}

// FPS
float Engine::getFps() const
{
  return _fps;
}

float Engine::getDeltaTime() const
{
  return _deltaTime;
}

void Engine::_measureFps()
{
  _performanceMonitor->sampleFrame();
  _fps       = _performanceMonitor->averageFPS();
  _deltaTime = _performanceMonitor->instantaneousFrameTime();
}

ArrayBufferView Engine::_readTexturePixels(InternalTexture* texture, int width,
                                           int height, int faceIndex)
{
  if (!_dummyFramebuffer) {
    auto dummy = _gl->createFramebuffer();

    if (!dummy) {
      BABYLON_LOG_ERROR("Engine", "Unable to create dummy framebuffer")
      return ArrayBufferView();
    }

    _dummyFramebuffer = ::std::move(dummy);
  }
  _gl->bindFramebuffer(GL::FRAMEBUFFER, _dummyFramebuffer.get());

  if (faceIndex > -1) {
    auto _faceIndex = static_cast<unsigned>(faceIndex);
    _gl->framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                              GL::TEXTURE_CUBE_MAP_POSITIVE_X + _faceIndex,
                              texture->_webGLTexture.get(), 0);
  }
  else {
    _gl->framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                              GL::TEXTURE_2D, texture->_webGLTexture.get(), 0);
  }

  auto readType = _getWebGLTextureType(texture->type);
  ArrayBufferView buffer;

  switch (readType) {
    case GL::UNSIGNED_BYTE: {
      buffer = ArrayBufferView(
        Uint8Array(static_cast<std::size_t>(4 * width * height)));
      readType = GL::UNSIGNED_BYTE;
      _gl->readPixels(0, 0, width, height, GL::RGBA, readType,
                      buffer.uint8Array);
      _gl->bindFramebuffer(GL::FRAMEBUFFER, _currentFramebuffer);

    } break;
    default: {
      buffer = ArrayBufferView(
        Float32Array(static_cast<std::size_t>(4 * width * height)));
      readType = GL::FLOAT;
      _gl->readPixels(0, 0, width, height, GL::RGBA, readType,
                      buffer.float32Array);
      _gl->bindFramebuffer(GL::FRAMEBUFFER, _currentFramebuffer);
    } break;
  }

  return buffer;
}

bool Engine::_canRenderToFloatFramebuffer()
{
  if (_webGLVersion > 1.f) {
    return _caps.colorBufferFloat;
  }
  return _canRenderToFramebuffer(EngineConstants::TEXTURETYPE_FLOAT);
}

bool Engine::_canRenderToHalfFloatFramebuffer()
{
  if (_webGLVersion > 1.f) {
    return _caps.colorBufferFloat;
  }

  return _canRenderToFramebuffer(EngineConstants::TEXTURETYPE_HALF_FLOAT);
}

// Thank you :
// http://stackoverflow.com/questions/28827511/webgl-ios-render-to-floating-point-texture
bool Engine::_canRenderToFramebuffer(unsigned int /*type*/)
{
#if 0
  // clear existing errors
  while (_gl->getError() != GL::NO_ERROR) {
  }

  bool successful = true;

  auto texture = _gl->createTexture();
  _gl->bindTexture(GL::TEXTURE_2D, texture.get());
  _gl->texImage2D(GL::TEXTURE_2D, 0, _getRGBABufferInternalSizedFormat(type), 1,
                  1, 0, GL::RGBA, _getWebGLTextureType(type), nullptr);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, GL::NEAREST);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, GL::NEAREST);

  auto fb = _gl->createFramebuffer();
  _gl->bindFramebuffer(GL::FRAMEBUFFER, fb.get());
  _gl->framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                            GL::TEXTURE_2D, texture.get(), 0);
  auto status = _gl->checkFramebufferStatus(GL::FRAMEBUFFER);

  successful = successful && (status == GL::FRAMEBUFFER_COMPLETE);
  successful = successful && (_gl->getError() == GL::NO_ERROR);

  // try render by clearing frame buffer's color buffer
  if (successful) {
    _gl->clear(GL::COLOR_BUFFER_BIT);
    successful = successful && (_gl->getError() == GL::NO_ERROR);
  }

  // try reading from frame to ensure render occurs (just creating the FBO is
  // not sufficient to determine if rendering is supported)
  if (successful) {
    // in practice it's sufficient to just read from the backbuffer rather than
    // handle potentially issues reading from the texture
    _gl->bindFramebuffer(GL::FRAMEBUFFER, nullptr);
    auto readFormat = GL::RGBA;
    auto readType   = GL::UNSIGNED_BYTE;
    Uint8Array buffer(4);
    _gl->readPixels(0, 0, 1, 1, readFormat, readType, buffer);
    successful = successful && (_gl->getError() == GL::NO_ERROR);
  }

  // clean up
  _gl->deleteTexture(texture.get());
  _gl->deleteFramebuffer(fb.get());
  _gl->bindFramebuffer(GL::FRAMEBUFFER, nullptr);

  // clear accumulated errors
  while (!successful && (_gl->getError() != GL::NO_ERROR)) {
  }

  return successful;
#endif
  return true;
}

GL::GLenum Engine::_getWebGLTextureType(unsigned int type) const
{
  if (type == EngineConstants::TEXTURETYPE_FLOAT) {
    return GL::FLOAT;
  }
  else if (type == EngineConstants::TEXTURETYPE_HALF_FLOAT) {
    // Add Half Float Constant.
    return _gl->HALF_FLOAT_OES;
  }

  return GL::UNSIGNED_BYTE;
}

GL::GLenum Engine::_getRGBABufferInternalSizedFormat(unsigned int type) const
{
  if (_webGLVersion == 1.f) {
    return GL::RGBA;
  }

  if (type == EngineConstants::TEXTURETYPE_FLOAT) {
    return _gl->RGBA32F;
  }
  else if (type == EngineConstants::TEXTURETYPE_HALF_FLOAT) {
    return _gl->RGBA16F;
  }

  return GL::RGBA;
}

Engine::GLQueryPtr Engine::createQuery()
{
  return _gl->createQuery();
}

Engine& Engine::deleteQuery(const GLQueryPtr& query)
{
  _gl->deleteQuery(query);

  return *this;
}

bool Engine::isQueryResultAvailable(const GLQueryPtr& query)
{
  return _gl->getQueryParameterb(query, GL::QUERY_RESULT_AVAILABLE);
}

unsigned int Engine::getQueryResult(const GLQueryPtr& query)
{
  return _gl->getQueryParameteri(query, GL::QUERY_RESULT);
}

Engine& Engine::beginOcclusionQuery(unsigned int algorithmType,
                                    const GLQueryPtr& query)
{
  const auto glAlgorithm = getGlAlgorithmType(algorithmType);
  _gl->beginQuery(glAlgorithm, query);

  return *this;
}

Engine& Engine::endOcclusionQuery(unsigned int algorithmType)
{
  const auto glAlgorithm = getGlAlgorithmType(algorithmType);
  _gl->endQuery(glAlgorithm);

  return *this;
}

Nullable<_TimeToken> Engine::startTimeQuery()
{
  return nullptr;
}

int Engine::endTimeQuery(Nullable<_TimeToken>& /*token*/)
{
  return -1;
}

Engine::GLTransformFeedbackPtr Engine::createTransformFeedback()
{
  return _gl->createTransformFeedback();
}

void Engine::deleteTransformFeedback(GL::IGLTransformFeedback* value)
{
  _gl->deleteTransformFeedback(value);
}

void Engine::bindTransformFeedback(GL::IGLTransformFeedback* value)
{
  _gl->bindTransformFeedback(GL::TRANSFORM_FEEDBACK, value);
}

void Engine::beginTransformFeedback(bool usePoints)
{
  _gl->beginTransformFeedback(usePoints ? GL::POINTS : GL::TRIANGLES);
}

void Engine::endTransformFeedback()
{
  _gl->endTransformFeedback();
}

void Engine::setTranformFeedbackVaryings(GL::IGLProgram* program,
                                         const vector_t<string_t>& value)
{
  _gl->transformFeedbackVaryings(program, value, GL::INTERLEAVED_ATTRIBS);
}

void Engine::bindTransformFeedbackBuffer(GL::IGLBuffer* value)
{
  _gl->bindBufferBase(GL::TRANSFORM_FEEDBACK_BUFFER, 0, value);
}

unsigned int Engine::getGlAlgorithmType(unsigned int algorithmType) const
{
  return algorithmType == AbstractMesh::OCCLUSION_ALGORITHM_TYPE_CONSERVATIVE ?
           GL::ANY_SAMPLES_PASSED_CONSERVATIVE :
           GL::ANY_SAMPLES_PASSED;
}

// Statics
bool Engine::isSupported()
{
  return true;
}

unique_ptr_t<GL::IGLShader> Engine::CompileShader(GL::IGLRenderingContext* gl,
                                                  const string_t& source,
                                                  const string_t& type,
                                                  const string_t& defines,
                                                  const string_t& shaderVersion)
{
  return CompileRawShader(
    gl, shaderVersion + ((!defines.empty()) ? defines + "\n" : "") + source,
    type);
}

unique_ptr_t<GL::IGLShader>
Engine::CompileRawShader(GL::IGLRenderingContext* gl, const string_t& source,
                         const string_t& type)
{
  auto shader = gl->createShader(type == "vertex" ? GL::VERTEX_SHADER :
                                                    GL::FRAGMENT_SHADER);
  gl->shaderSource(shader, source);
  gl->compileShader(shader);

  if (!gl->getShaderParameter(shader, GL::COMPILE_STATUS)) {
    auto log = gl->getShaderInfoLog(shader);
    if (!log.empty()) {
      BABYLON_LOG_ERROR("Engine", log);
    }
    return nullptr;
  }

  if (!shader) {
    BABYLON_LOG_ERROR("Engine",
                      "Something went wrong while compile the shader.");
    return nullptr;
  }

  return shader;
}

SamplingParameters Engine::GetSamplingParameters(unsigned int samplingMode,
                                                 bool generateMipMaps)
{
  GL::GLenum magFilter = GL::NEAREST;
  GL::GLenum minFilter = GL::NEAREST;

  switch (samplingMode) {
    case TextureConstants::BILINEAR_SAMPLINGMODE:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_NEAREST;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case TextureConstants::TRILINEAR_SAMPLINGMODE:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_LINEAR;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case TextureConstants::NEAREST_SAMPLINGMODE:
      magFilter = GL::NEAREST;
      if (generateMipMaps) {
        minFilter = GL::NEAREST_MIPMAP_LINEAR;
      }
      else {
        minFilter = GL::NEAREST;
      }
      break;
    case TextureConstants::NEAREST_NEAREST_MIPNEAREST:
      magFilter = GL::NEAREST;
      if (generateMipMaps) {
        minFilter = GL::NEAREST_MIPMAP_NEAREST;
      }
      else {
        minFilter = GL::NEAREST;
      }
      break;
    case TextureConstants::NEAREST_LINEAR_MIPNEAREST:
      magFilter = GL::NEAREST;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_NEAREST;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case TextureConstants::NEAREST_LINEAR_MIPLINEAR:
      magFilter = GL::NEAREST;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_LINEAR;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case TextureConstants::NEAREST_LINEAR:
      magFilter = GL::NEAREST;
      minFilter = GL::LINEAR;
      break;
    case TextureConstants::NEAREST_NEAREST:
      magFilter = GL::NEAREST;
      minFilter = GL::NEAREST;
      break;
    case TextureConstants::LINEAR_NEAREST_MIPNEAREST:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::NEAREST_MIPMAP_NEAREST;
      }
      else {
        minFilter = GL::NEAREST;
      }
      break;
    case TextureConstants::LINEAR_NEAREST_MIPLINEAR:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::NEAREST_MIPMAP_LINEAR;
      }
      else {
        minFilter = GL::NEAREST;
      }
      break;
    case TextureConstants::LINEAR_LINEAR:
      magFilter = GL::LINEAR;
      minFilter = GL::LINEAR;
      break;
    case TextureConstants::LINEAR_NEAREST:
      magFilter = GL::LINEAR;
      minFilter = GL::NEAREST;
      break;
  }

  return {static_cast<int>(minFilter), static_cast<int>(magFilter)};
}

} // end of namespace BABYLON
