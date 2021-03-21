#include <babylon/engines/engine.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine_store.h>
#include <babylon/engines/extensions/multiview_extension.h>
#include <babylon/engines/extensions/occlusion_query_extension.h>
#include <babylon/engines/extensions/transform_feedback_extension.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/webgl/webgl_pipeline_context.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/iloading_screen.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/webgl/webgl_data_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/particles/iparticle_system.h>
#include <babylon/particles/particle_system.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/post_process_manager.h>
#include <babylon/states/depth_culling_state.h>
#include <babylon/states/stencil_state.h>

namespace BABYLON {

AudioEnginePtr Engine::audioEngine = nullptr;

std::vector<Engine*>& Engine::Instances()
{
  return EngineStore::Instances;
}

Engine* Engine::LastCreatedEngine()
{
  return EngineStore::LastCreatedEngine();
}

Scene* Engine::LastCreatedScene()
{
  return EngineStore::LastCreatedScene();
}

void Engine::MarkAllMaterialsAsDirty(unsigned int flag,
                                     const std::function<bool(Material* mat)>& predicate)
{
  for (const auto& engine : Engine::Instances()) {
    for (const auto& scene : engine->scenes) {
      scene->markAllMaterialsAsDirty(flag, predicate);
    }
  }
}

ILoadingScreenPtr Engine::DefaultLoadingScreenFactory(ICanvas* /*canvas*/)
{
  return nullptr;
}

std::function<PostProcessPtr(Engine* engine)> Engine::_RescalePostProcessFactory = nullptr;

Engine::Engine(ICanvas* canvas, const EngineOptions& options)
    : ThinEngine{canvas, options}
    , performanceMonitor{this, &Engine::get_performanceMonitor}
    , loadingScreen{this, &Engine::get_loadingScreen, &Engine::set_loadingScreen}
    , loadingUIText{this, &Engine::set_loadingUIText}
    , loadingUIBackgroundColor{this, &Engine::set_loadingUIBackgroundColor}
    , _rescalePostProcess{nullptr}
    , _performanceMonitor{std::make_unique<PerformanceMonitor>()}
    , _multiviewExtension{std::make_unique<MultiviewExtension>(this)}
    , _occlusionQueryExtension{std::make_unique<OcclusionQueryExtension>(this)}
    , _transformFeedbackExtension{std::make_unique<TransformFeedbackExtension>(this)}
{
  Engine::Instances().emplace_back(this);

  if (!canvas) {
    return;
  }

  _sharedInit(canvas, !!options.doNotHandleTouchAction, options.audioEngine);

  _deterministicLockstep = options.deterministicLockstep;
  _lockstepMaxSteps      = options.lockstepMaxSteps;
  _timeStep              = options.timeStep.value_or(1.f / 60.f);
}

Engine::~Engine()
{
  stl_util::remove_vector_elements_equal(EngineStore::Instances, this);
}

void Engine::_sharedInit(ICanvas* canvas, bool doNotHandleTouchAction, bool audioEngine)
{
  ThinEngine::_sharedInit(canvas, doNotHandleTouchAction, audioEngine);

  _onCanvasFocus = [this]() -> void { onCanvasFocusObservable.notifyObservers(this); };

  _onCanvasBlur = [this]() -> void { onCanvasBlurObservable.notifyObservers(this); };

  _onBlur = [this]() -> void {
    if (disablePerformanceMonitorInBackground) {
      _performanceMonitor->disable();
    }
    _windowIsBackground = true;
  };

  _onFocus = [this]() -> void {
    if (disablePerformanceMonitorInBackground) {
      _performanceMonitor->enable();
    }
    _windowIsBackground = false;
  };

  _onCanvasPointerOut
    = [this](PointerEvent* ev) -> void { onCanvasPointerOutObservable.notifyObservers(ev); };

  if (!doNotHandleTouchAction) {
    _disableTouchAction();
  }
}

bool Engine::get__supportsHardwareTextureRescaling() const
{
  return Engine::_RescalePostProcessFactory != nullptr;
}

std::unique_ptr<PerformanceMonitor>& Engine::get_performanceMonitor()
{
  return _performanceMonitor;
}

ICanvas* Engine::getInputElement() const
{
  return _renderingCanvas;
}

float Engine::getAspectRatio(const Camera& camera, bool useScreen)
{
  const auto& viewport = camera.viewport;
  return static_cast<float>(getRenderWidth(useScreen) * viewport.width)
         / static_cast<float>(getRenderHeight(useScreen) * viewport.height);
}

float Engine::getScreenAspectRatio() const
{
  return static_cast<float>(getRenderWidth(true)) / static_cast<float>(getRenderHeight(true));
}

std::optional<ClientRect> Engine::getRenderingCanvasClientRect()
{
  if (!_renderingCanvas) {
    return std::nullopt;
  }

  return _renderingCanvas->getBoundingClientRect();
}

std::optional<ClientRect> Engine::getInputElementClientRect()
{
  if (!_renderingCanvas) {
    return std::nullopt;
  }

  return getInputElement()->getBoundingClientRect();
}

bool Engine::isDeterministicLockStep() const
{
  return _deterministicLockstep;
}

unsigned int Engine::getLockstepMaxSteps() const
{
  return _lockstepMaxSteps;
}

float Engine::getTimeStep() const
{
  return _timeStep * 1000.f;
}

void Engine::generateMipMapsForCubemap(const InternalTexturePtr& texture, bool unbind)
{
  if (texture->generateMipMaps) {
    auto& gl = *_gl;
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture, true);
    gl.generateMipmap(GL::TEXTURE_CUBE_MAP);
    if (unbind) {
      _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
    }
  }
}

void Engine::setState(bool culling, float zOffset, bool force, bool reverseSide)
{
  // Culling
  if (_depthCullingState->cull() != culling || force) {
    _depthCullingState->cull = culling;
  }

  // Cull face
  const auto cullFace = cullBackFaces ? GL::BACK : GL::FRONT;
  if ((_depthCullingState->cullFace().has_value()
       && *_depthCullingState->cullFace() != static_cast<int>(cullFace))
      || force) {
    _depthCullingState->cullFace = static_cast<int>(cullFace);
  }

  // Z offset
  setZOffset(zOffset);

  // Front face
  const auto frontFace = reverseSide ? GL::CW : GL::CCW;
  if (_depthCullingState->frontFace() != frontFace || force) {
    _depthCullingState->frontFace = frontFace;
  }
}

void Engine::setZOffset(float value)
{
  _depthCullingState->zOffset = value;
}

float Engine::getZOffset() const
{
  return _depthCullingState->zOffset();
}

bool Engine::getDepthBuffer() const
{
  return _depthCullingState->depthTest();
}

void Engine::setDepthBuffer(bool enable)
{
  _depthCullingState->depthTest = enable;
}

bool Engine::getDepthWrite() const
{
  return _depthCullingState->depthMask();
}

void Engine::setDepthWrite(bool enable)
{
  _depthCullingState->depthMask = enable;
}

bool Engine::getStencilBuffer() const
{
  return _stencilState->stencilTest();
}

void Engine::setStencilBuffer(bool enable)
{
  _stencilState->stencilTest = enable;
}

unsigned int Engine::getStencilMask() const
{
  return _stencilState->stencilMask();
}

void Engine::setStencilMask(unsigned int mask)
{
  _stencilState->stencilMask = mask;
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
  _stencilState->stencilFunc = stencilFunc;
}

void Engine::setStencilFunctionReference(int reference)
{
  _stencilState->stencilFuncRef = reference;
}

void Engine::setStencilFunctionMask(unsigned int mask)
{
  _stencilState->stencilFuncMask = mask;
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
  _stencilState->stencilOpStencilFail = operation;
}

void Engine::setStencilOperationDepthFail(unsigned int operation)
{
  _stencilState->stencilOpDepthFail = operation;
}

void Engine::setStencilOperationPass(unsigned int operation)
{
  _stencilState->stencilOpStencilDepthPass = operation;
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

int Engine::getDepthFunction() const
{
  return *_depthCullingState->depthFunc();
}

void Engine::setDepthFunction(int depthFunc)
{
  _depthCullingState->depthFunc = depthFunc;
}

void Engine::setDepthFunctionToGreater()
{
  _depthCullingState->depthFunc = GL::GREATER;
}

void Engine::setDepthFunctionToGreaterOrEqual()
{
  _depthCullingState->depthFunc = GL::GEQUAL;
}

void Engine::setDepthFunctionToLess()
{
  _depthCullingState->depthFunc = GL::LESS;
}

void Engine::setDepthFunctionToLessOrEqual()
{
  _depthCullingState->depthFunc = GL::LEQUAL;
}

void Engine::cacheStencilState()
{
  _cachedStencilBuffer             = getStencilBuffer();
  _cachedStencilFunction           = getStencilFunction();
  _cachedStencilMask               = getStencilMask();
  _cachedStencilOperationPass      = getStencilOperationPass();
  _cachedStencilOperationFail      = getStencilOperationFail();
  _cachedStencilOperationDepthFail = getStencilOperationDepthFail();
  _cachedStencilReference          = getStencilFunctionReference();
}

void Engine::restoreStencilState()
{
  setStencilFunction(_cachedStencilFunction);
  setStencilMask(_cachedStencilMask);
  setStencilBuffer(_cachedStencilBuffer);
  setStencilOperationPass(_cachedStencilOperationPass);
  setStencilOperationFail(_cachedStencilOperationFail);
  setStencilOperationDepthFail(_cachedStencilOperationDepthFail);
  setStencilFunctionReference(_cachedStencilReference);
}

Viewport Engine::setDirectViewport(int x, int y, int width, int height)
{
  auto iCurrentViewport = _cachedViewport;
  _cachedViewport       = std::nullopt;

  _viewport(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width),
            static_cast<float>(height));

  return *iCurrentViewport;
}

void Engine::scissorClear(int x, int y, int width, int height, const Color4& clearColor)
{
  enableScissor(x, y, width, height);
  clear(clearColor, true, true, true);
  disableScissor();
}

void Engine::enableScissor(int x, int y, int width, int height)
{
  // Change state
  _gl->enable(GL::SCISSOR_TEST);
  _gl->scissor(x, y, width, height);
}

void Engine::disableScissor()
{
  _gl->disable(GL::SCISSOR_TEST);
}

void Engine::_reportDrawCall()
{
  _drawCalls.addCount(1, false);
}

void Engine::initWebVR()
{
}

void Engine::_prepareVRComponent()
{
  // Do nothing as the engine side effect will overload it
}

void Engine::_connectVREvents(ICanvas* /*canvas*/)
{
  // Do nothing as the engine side effect will overload it
}

void Engine::_submitVRFrame()
{
  // Do nothing as the engine side effect will overload it
}

void Engine::disableVR()
{
  // Do nothing as the engine side effect will overload it
}

bool Engine::isVRPresenting() const
{
  return false;
}

void Engine::_requestVRFrame()
{
  // Do nothing as the engine side effect will overload it
}

std::string Engine::getVertexShaderSource(const WebGLProgramPtr& program)
{
  auto shaders = _gl->getAttachedShaders(program.get());

  if (shaders.empty()) {
    return "";
  }

  return _gl->getShaderSource(shaders[0]);
}

std::string Engine::getFragmentShaderSource(const WebGLProgramPtr& program)
{
  auto shaders = _gl->getAttachedShaders(program.get());

  if (shaders.size() < 2) {
    return "";
  }

  return _gl->getShaderSource(shaders[1]);
}

void Engine::setDepthStencilTexture(int channel, const WebGLUniformLocationPtr& uniform,
                                    const RenderTargetTexturePtr& texture, const std::string& name)
{
  if (channel < 0) {
    return;
  }

  if (uniform) {
    _boundUniforms[channel] = uniform;
  }

  if (!texture || !texture->depthStencilTexture()) {
    _setTexture(channel, nullptr, false, false, name);
  }
  else {
    _setTexture(channel, std::static_pointer_cast<BaseTexture>(texture), false, true, name);
  }
}

void Engine::setTextureFromPostProcess(int channel, const PostProcessPtr& postProcess,
                                       const std::string& name)
{
  InternalTexturePtr postProcessInput = nullptr;
  if (postProcess) {
    if (postProcess->_currentRenderTextureInd < postProcess->_textures.size()
        && postProcess->_textures[postProcess->_currentRenderTextureInd]) {
      postProcessInput = postProcess->_textures[postProcess->_currentRenderTextureInd];
    }
    else if (postProcess->_forcedOutputTexture) {
      postProcessInput = postProcess->_forcedOutputTexture;
    }
  }

  _bindTexture(channel, postProcessInput, name);
}

void Engine::setTextureFromPostProcessOutput(int channel, const PostProcessPtr& postProcess,
                                             const std::string& name)
{
  _bindTexture(channel, postProcess ? postProcess->_outputTexture : nullptr, name);
}

void Engine::_rebuildBuffers()
{
  // Index / Vertex
  for (const auto& scene : scenes) {
    scene->resetCachedMaterial();
    scene->_rebuildGeometries();
    scene->_rebuildTextures();
  }

  ThinEngine::_rebuildBuffers();
}

void Engine::_renderFrame()
{
  for (const auto& renderFunction : _activeRenderLoops) {
    renderFunction();
  }
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

      // Child canvases
      if (!_renderViews()) {
        // Main frame
        _renderFrame();
      }

      // Present
      endFrame();
    }
  }

  if (!_activeRenderLoops.empty()) {
    // Register new frame
  }
  else {
    _renderingQueueLaunched = false;
  }
}

bool Engine::_renderViews() const
{
  return false;
}

void Engine::switchFullscreen(bool requestPointerLock)
{
  if (isFullscreen) {
    exitFullscreen();
  }
  else {
    enterFullscreen(requestPointerLock);
  }
}

void Engine::enterFullscreen(bool requestPointerLock)
{
  if (!isFullscreen) {
    _pointerLockRequested = requestPointerLock;
    if (_renderingCanvas) {
      Engine::_RequestFullscreen(_renderingCanvas);
    }
  }
}

void Engine::exitFullscreen()
{
  if (isFullscreen) {
    Engine::_ExitFullscreen();
  }
}

void Engine::enterPointerlock()
{
  if (_renderingCanvas) {
    Engine::_RequestPointerlock(_renderingCanvas);
  }
}

void Engine::exitPointerlock()
{
  Engine::_ExitPointerlock();
}

void Engine::beginFrame()
{
  _measureFps();

  onBeginFrameObservable.notifyObservers(this);
  ThinEngine::beginFrame();
}

void Engine::endFrame()
{
  ThinEngine::endFrame();
  _submitVRFrame();

  onEndFrameObservable.notifyObservers(this);
}

void Engine::resize(bool forceSetSize)
{
  // We're not resizing the size of the canvas while in VR mode & presenting
  if (isVRPresenting()) {
    return;
  }

  ThinEngine::resize(forceSetSize);
}

bool Engine::setSize(int width, int height, bool forceSetSize)
{
  if (!_renderingCanvas) {
    return false;
  }

  if (!ThinEngine::setSize(width, height, forceSetSize)) {
    return false;
  }

  if (!scenes.empty()) {
    for (const auto& scene : scenes) {
      for (const auto& cam : scene->cameras) {
        cam->_currentRenderId = 0;
      }
    }

    if (onResizeObservable.hasObservers()) {
      onResizeObservable.notifyObservers(this);
    }
  }

  return true;
}

void Engine::_deletePipelineContext(const IPipelineContextPtr& pipelineContext)
{
  auto webGLPipelineContext = std::static_pointer_cast<WebGLPipelineContext>(pipelineContext);
  if (webGLPipelineContext && webGLPipelineContext->program) {
    if (webGLPipelineContext->transformFeedback) {
      deleteTransformFeedback(webGLPipelineContext->transformFeedback);
      webGLPipelineContext->transformFeedback = nullptr;
    }
  }
  ThinEngine::_deletePipelineContext(pipelineContext);
}

WebGLProgramPtr
Engine::createShaderProgram(const IPipelineContextPtr& pipelineContext,
                            const std::string& vertexCode, const std::string& fragmentCode,
                            const std::string& defines, WebGLRenderingContext* context,
                            const std::vector<std::string>& transformFeedbackVaryings)
{
  context = context ? context : _gl;

  onBeforeShaderCompilationObservable.notifyObservers(this);

  const auto program = ThinEngine::createShaderProgram(pipelineContext, vertexCode, fragmentCode,
                                                       defines, context, transformFeedbackVaryings);
  onAfterShaderCompilationObservable.notifyObservers(this);

  return program;
}

WebGLProgramPtr
Engine::_createShaderProgram(const WebGLPipelineContextPtr& pipelineContext,
                             const WebGLShaderPtr& vertexShader,
                             const WebGLShaderPtr& fragmentShader, WebGLRenderingContext* context,
                             const std::vector<std::string>& transformFeedbackVaryings)
{
  auto shaderProgram       = context->createProgram();
  pipelineContext->program = shaderProgram;

  if (!shaderProgram) {
    throw std::runtime_error("Unable to create program");
  }

  context->attachShader(shaderProgram.get(), vertexShader.get());
  context->attachShader(shaderProgram.get(), fragmentShader.get());

  if (webGLVersion() > 1.f && !transformFeedbackVaryings.empty()) {
    auto transformFeedback = createTransformFeedback();

    bindTransformFeedback(transformFeedback);
    setTranformFeedbackVaryings(shaderProgram, transformFeedbackVaryings);
    pipelineContext->transformFeedback = transformFeedback;
  }

  context->linkProgram(shaderProgram.get());

  if (webGLVersion() > 1.f && !transformFeedbackVaryings.empty()) {
    bindTransformFeedback(nullptr);
  }

  pipelineContext->context        = context;
  pipelineContext->vertexShader   = vertexShader;
  pipelineContext->fragmentShader = fragmentShader;

  if (!pipelineContext->isParallelCompiled) {
    _finalizePipelineContext(pipelineContext.get());
  }

  return shaderProgram;
}

void Engine::_releaseTexture(const InternalTexturePtr& texture)
{
  ThinEngine::_releaseTexture(texture);

  // Set output texture of post process to null if the texture has been released/disposed
  for (const auto& scene : scenes) {
    for (const auto& postProcess : scene->postProcesses) {
      if (postProcess->_outputTexture == texture) {
        postProcess->_outputTexture = nullptr;
      }
    }
    for (const auto& camera : scene->cameras) {
      for (const auto& postProcess : camera->_postProcesses) {
        if (postProcess) {
          if (postProcess->_outputTexture == texture) {
            postProcess->_outputTexture = nullptr;
          }
        }
      }
    }
  }
}

void Engine::_rescaleTexture(const InternalTexturePtr& source,
                             const InternalTexturePtr& destination, Scene* scene,
                             unsigned int internalFormat, const std::function<void()>& onComplete)
{
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, GL::LINEAR);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, GL::LINEAR);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
  _gl->texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

  IRenderTargetOptions options;
  options.generateMipMaps       = false;
  options.type                  = Constants::TEXTURETYPE_UNSIGNED_INT;
  options.samplingMode          = Constants::TEXTURE_BILINEAR_SAMPLINGMODE;
  options.generateDepthBuffer   = false;
  options.generateStencilBuffer = false;

  auto rtt
    = createRenderTargetTexture(RenderTargetSize{destination->width, destination->height}, options);

  if (!_rescalePostProcess && Engine::_RescalePostProcessFactory) {
    _rescalePostProcess = Engine::_RescalePostProcessFactory(this);
  }

  _rescalePostProcess->getEffect()->executeWhenCompiled([=](Effect* /*effect*/) {
    _rescalePostProcess->onApply
      = [=](Effect* effect, EventState&) { effect->_bindTexture("textureSampler", source); };

    auto hostingScene = scene;

    if (!hostingScene) {
      hostingScene = scenes.back();
    }
    hostingScene->postProcessManager->directRender({_rescalePostProcess}, rtt, true);

    _bindTextureDirectly(GL::TEXTURE_2D, destination, true);
    _gl->copyTexImage2D(GL::TEXTURE_2D, 0, internalFormat, 0, 0, destination->width,
                        destination->height, 0);

    unBindFramebuffer(rtt);
    _releaseTexture(rtt);

    if (onComplete) {
      onComplete();
    }
  });
}

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

void Engine::_uploadImageToTexture(const InternalTexturePtr& texture, const Image& image,
                                   unsigned int faceIndex, int lod)
{
  auto textureType    = _getWebGLTextureType(texture->type);
  auto format         = _getInternalFormat(texture->format);
  auto internalFormat = _getRGBABufferInternalSizedFormat(texture->type, format);

  auto bindTarget = texture->isCube ? GL::TEXTURE_CUBE_MAP : GL::TEXTURE_2D;

  _bindTextureDirectly(bindTarget, texture, true);
  _unpackFlipY(texture->invertY);

  GL::GLenum target = GL::TEXTURE_2D;
  if (texture->isCube) {
    target = static_cast<GL::GLenum>(GL::TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex);
  }

  _gl->texImage2D(target, lod, static_cast<int>(internalFormat), image.width, image.height, 0,
                  format, textureType, &image.data);
  _bindTextureDirectly(bindTarget, nullptr, true);
}

unsigned int Engine::updateRenderTargetTextureSampleCount(const InternalTexturePtr& texture,
                                                          unsigned int samples)
{
  if (webGLVersion() < 2.f || !texture) {
    return 1;
  }

  if (texture->samples == samples) {
    return samples;
  }

  auto& gl = *_gl;

  samples = std::min(samples, getCaps().maxMSAASamples);

  // Dispose previous render buffers
  if (texture->_depthStencilBuffer) {
    gl.deleteRenderbuffer(texture->_depthStencilBuffer.get());
    texture->_depthStencilBuffer = nullptr;
  }

  if (texture->_MSAAFramebuffer) {
    gl.deleteFramebuffer(texture->_MSAAFramebuffer.get());
    texture->_MSAAFramebuffer = nullptr;
  }

  if (texture->_MSAARenderBuffer) {
    gl.deleteRenderbuffer(texture->_MSAARenderBuffer.get());
    texture->_MSAARenderBuffer = nullptr;
  }

  if (samples > 1) {
    auto framebuffer = gl.createFramebuffer();

    if (!framebuffer) {
      throw std::runtime_error("Unable to create multi sampled framebuffer");
    }

    texture->_MSAAFramebuffer = std::move(framebuffer);
    _bindUnboundFramebuffer(texture->_MSAAFramebuffer);

    auto colorRenderbuffer = gl.createRenderbuffer();

    if (!colorRenderbuffer) {
      throw std::runtime_error("Unable to create multi sampled framebuffer");
    }

    gl.bindRenderbuffer(GL::RENDERBUFFER, colorRenderbuffer.get());
    gl.renderbufferStorageMultisample(GL::RENDERBUFFER, static_cast<int>(samples),
                                      _getRGBAMultiSampleBufferFormat(texture->type),
                                      texture->width, texture->height);

    gl.framebufferRenderbuffer(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0, GL::RENDERBUFFER,
                               colorRenderbuffer.get());

    texture->_MSAARenderBuffer = colorRenderbuffer;
  }
  else {
    _bindUnboundFramebuffer(texture->_framebuffer);
  }

  texture->samples             = samples;
  texture->_depthStencilBuffer = _setupFramebufferDepthAttachments(
    texture->_generateStencilBuffer, texture->_generateDepthBuffer, texture->width, texture->height,
    static_cast<int>(samples));

  _bindUnboundFramebuffer(nullptr);

  return samples;
}

void Engine::updateTextureComparisonFunction(const InternalTexturePtr& texture,
                                             int comparisonFunction)
{
  if (webGLVersion() == 1.f) {
    BABYLON_LOG_ERROR("Engine", "WebGL 1 does not support texture comparison.")
    return;
  }

  auto& gl = *_gl;

  if (texture->isCube) {
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, texture, true);

    if (comparisonFunction == 0) {
      gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_COMPARE_FUNC, Constants::LEQUAL);
      gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_COMPARE_MODE, GL::NONE);
    }
    else {
      gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_COMPARE_FUNC, comparisonFunction);
      gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_COMPARE_MODE, GL::COMPARE_REF_TO_TEXTURE);
    }

    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
  }
  else {
    _bindTextureDirectly(GL::TEXTURE_2D, texture, true);

    if (comparisonFunction == 0) {
      gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_COMPARE_FUNC, Constants::LEQUAL);
      gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_COMPARE_MODE, GL::NONE);
    }
    else {
      gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_COMPARE_FUNC, comparisonFunction);
      gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_COMPARE_MODE, GL::COMPARE_REF_TO_TEXTURE);
    }

    _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  }

  texture->_comparisonFunction = comparisonFunction;
}

EffectPtr Engine::createEffectForParticles(
  const std::string& fragmentName, const std::vector<std::string>& uniformsNames,
  std::vector<std::string> samplers, std::string defines, IEffectFallbacks* /*fallbacks*/,
  const std::function<void(Effect* effect)>& onCompiled,
  const std::function<void(Effect* effect, const std::string& errors)>& onError,
  const IParticleSystemPtr& particleSystem)
{
  std::vector<std::string> attributesNamesOrOptions;
  std::vector<std::string> effectCreationOption;
  std::vector<std::string> allSamplers;

  if (particleSystem) {
    particleSystem->fillUniformsAttributesAndSamplerNames(effectCreationOption,
                                                          attributesNamesOrOptions, allSamplers);
  }
  else {
    attributesNamesOrOptions = ParticleSystem::_GetAttributeNamesOrOptions();
    effectCreationOption     = ParticleSystem::_GetEffectCreationOptions();
  }

  if (!StringTools::contains(defines, " BILLBOARD")) {
    defines += "\n#define BILLBOARD\n";
  }

  if (!stl_util::contains(samplers, "diffuseSampler")) {
    samplers.emplace_back("diffuseSampler");
  }

  std::unordered_map<std::string, std::string> baseName{
    {"vertex", !particleSystem->vertexShaderName().empty() ? particleSystem->vertexShaderName() :
                                                             "particles"}, //
    {"fragmentElement", fragmentName}                                      //
  };

  IEffectCreationOptions options;
  options.attributes          = attributesNamesOrOptions;
  options.uniformBuffersNames = stl_util::concat(effectCreationOption, uniformsNames);
  options.samplers            = stl_util::concat(allSamplers, samplers);
  options.defines             = defines;
  options.onCompiled          = onCompiled;
  options.onError             = onError;

  return createEffect(baseName, options, this);
}

WebGLDataBufferPtr Engine::createInstancesBuffer(unsigned int capacity)
{
  auto buffer = _gl->createBuffer();

  if (!buffer) {
    throw std::runtime_error("Unable to create instance buffer");
  }

  auto result      = std::make_shared<WebGLDataBuffer>(buffer);
  result->capacity = capacity;

  bindArrayBuffer(result);
  _gl->bufferData(GL::ARRAY_BUFFER, capacity, GL::DYNAMIC_DRAW);

  return result;
}

void Engine::deleteInstancesBuffer(const WebGLDataBufferPtr& buffer)
{
  _gl->deleteBuffer(buffer->underlyingResource().get());
}

void Engine::dispose()
{
  hideLoadingUI();

  onNewSceneAddedObservable.clear();

  // Release postProcesses
  for (const auto& postProcess : postProcesses) {
    postProcess->dispose();
  }
  postProcesses.clear();

  // Rescale PP
  if (_rescalePostProcess) {
    _rescalePostProcess->dispose();
  }

  // Release scenes
  for (const auto& scene : scenes) {
    scene->dispose();
  }
  scenes.clear();

  // WebVR
  disableVR();

  ThinEngine::dispose();

  // Remove from Instances
  auto index = stl_util::index_of(EngineStore::Instances, this);

  if (index >= 0) {
    stl_util::splice(EngineStore::Instances, index, 1);
  }

  // Observables
  onResizeObservable.clear();
  onCanvasBlurObservable.clear();
  onCanvasFocusObservable.clear();
  onCanvasPointerOutObservable.clear();
  onBeginFrameObservable.clear();
  onEndFrameObservable.clear();
}

void Engine::_disableTouchAction()
{
}

void Engine::displayLoadingUI()
{
  const auto iLoadingScreen = loadingScreen();
  if (iLoadingScreen) {
    iLoadingScreen->displayLoadingUI();
  }
}

void Engine::hideLoadingUI()
{
  const auto iLoadingScreen = loadingScreen();
  if (iLoadingScreen) {
    iLoadingScreen->hideLoadingUI();
  }
}

ILoadingScreenPtr& Engine::get_loadingScreen()
{
  if (!_loadingScreen && _renderingCanvas) {
    _loadingScreen = Engine::DefaultLoadingScreenFactory(_renderingCanvas);
  }
  return _loadingScreen;
}

void Engine::set_loadingScreen(const ILoadingScreenPtr& iLoadingScreen)
{
  _loadingScreen = iLoadingScreen;
}

void Engine::set_loadingUIText(std::string text)
{
  loadingScreen()->loadingUIText = text;
}

void Engine::set_loadingUIBackgroundColor(std::string color)
{
  loadingScreen()->loadingUIBackgroundColor = color;
}

void Engine::_RequestPointerlock(ICanvas* /*element*/)
{
}

void Engine::_ExitPointerlock()
{
}

void Engine::_RequestFullscreen(ICanvas* /*element*/)
{
}

void Engine::_ExitFullscreen()
{
}

//--------------------------------------------------------------------------------------------------
//                              Multiview Extension
//--------------------------------------------------------------------------------------------------

InternalTexturePtr Engine::createMultiviewRenderTargetTexture(int width, int height)
{
  return _multiviewExtension->createMultiviewRenderTargetTexture(width, height);
}

void Engine::bindMultiviewFramebuffer(const InternalTexturePtr& multiviewTexture)
{
  _multiviewExtension->bindMultiviewFramebuffer(multiviewTexture);
}

//--------------------------------------------------------------------------------------------------
//                              Occlusion Query Extension
//--------------------------------------------------------------------------------------------------

WebGLQueryPtr Engine::createQuery()
{
  return _occlusionQueryExtension->createQuery();
}

Engine& Engine::deleteQuery(const WebGLQueryPtr& query)
{
  return _occlusionQueryExtension->deleteQuery(query);
}

bool Engine::isQueryResultAvailable(const WebGLQueryPtr& query)
{
  return _occlusionQueryExtension->isQueryResultAvailable(query);
}

unsigned int Engine::getQueryResult(const WebGLQueryPtr& query)
{
  return _occlusionQueryExtension->getQueryResult(query);
}

Engine& Engine::beginOcclusionQuery(unsigned int algorithmType, const WebGLQueryPtr& query)
{
  return _occlusionQueryExtension->beginOcclusionQuery(algorithmType, query);
}

Engine& Engine::endOcclusionQuery(unsigned int algorithmType)
{
  return _occlusionQueryExtension->endOcclusionQuery(algorithmType);
}

std::optional<_TimeToken> Engine::startTimeQuery()
{
  return _occlusionQueryExtension->startTimeQuery();
}

int Engine::endTimeQuery(std::optional<_TimeToken>& token)
{
  return _occlusionQueryExtension->endTimeQuery(token);
}

WebGLQueryPtr Engine::_createTimeQuery()
{
  return _occlusionQueryExtension->_createTimeQuery();
}

void Engine::_deleteTimeQuery(const WebGLQueryPtr& query)
{
  _occlusionQueryExtension->_deleteTimeQuery(query);
}

unsigned int Engine::_getGlAlgorithmType(unsigned int algorithmType) const
{
  return _occlusionQueryExtension->_getGlAlgorithmType(algorithmType);
}

unsigned int Engine::_getTimeQueryResult(const WebGLQueryPtr& query)
{
  return _occlusionQueryExtension->_getTimeQueryResult(query);
}

bool Engine::_getTimeQueryAvailability(const WebGLQueryPtr& query)
{
  return _occlusionQueryExtension->_getTimeQueryAvailability(query);
}

//--------------------------------------------------------------------------------------------------
//                              Transform Feedback Extension
//--------------------------------------------------------------------------------------------------

WebGLTransformFeedbackPtr Engine::createTransformFeedback()
{
  return _transformFeedbackExtension->createTransformFeedback();
}

void Engine::deleteTransformFeedback(const WebGLTransformFeedbackPtr& value)
{
  _transformFeedbackExtension->deleteTransformFeedback(value);
}

void Engine::bindTransformFeedback(const WebGLTransformFeedbackPtr& value)
{
  _transformFeedbackExtension->bindTransformFeedback(value);
}

void Engine::beginTransformFeedback(bool usePoints)
{
  _transformFeedbackExtension->beginTransformFeedback(usePoints);
}

void Engine::endTransformFeedback()
{
  _transformFeedbackExtension->endTransformFeedback();
}

void Engine::setTranformFeedbackVaryings(const WebGLProgramPtr& program,
                                         const std::vector<std::string>& value)
{
  _transformFeedbackExtension->setTranformFeedbackVaryings(program, value);
}

void Engine::bindTransformFeedbackBuffer(const WebGLDataBufferPtr& value)
{
  _transformFeedbackExtension->bindTransformFeedbackBuffer(value);
}

} // end of namespace BABYLON
