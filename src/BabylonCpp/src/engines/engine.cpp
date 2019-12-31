#include <babylon/engines/engine.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine_store.h>
#include <babylon/engines/extensions/multiview_extension.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/webgl/webgl_pipeline_context.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/iloading_screen.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/webgl/webgl_data_buffer.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/post_process_manager.h>
#include <babylon/states/depth_culling_state.h>
#include <babylon/states/stencil_state.h>

namespace BABYLON {

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
  auto currentViewport = _cachedViewport;
  _cachedViewport      = std::nullopt;

  _viewport(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width),
            static_cast<float>(height));

  return *currentViewport;
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
                                    const RenderTargetTexturePtr& texture)
{
  if (channel < 0) {
    return;
  }

  if (uniform) {
    _boundUniforms[channel] = uniform;
  }

  if (!texture || !texture->depthStencilTexture) {
    _setTexture(channel, nullptr);
  }
  else {
    _setTexture(channel, texture, false, true);
  }
}

void Engine::setTextureFromPostProcess(int channel, const PostProcessPtr& postProcess)
{
  const auto _ind = static_cast<size_t>(postProcess->_currentRenderTextureInd);
  _bindTexture(channel, postProcess ? postProcess->_textures[_ind] : nullptr);
}

void Engine::setTextureFromPostProcessOutput(int channel, const PostProcessPtr& postProcess)
{
  _bindTexture(channel, postProcess ? postProcess->_outputTexture : nullptr);
}

ArrayBufferView Engine::_convertRGBtoRGBATextureData(const ArrayBufferView& rgbData, int width,
                                                     int height, unsigned int textureType)
{
  // Create new RGBA data container.
  if (textureType == Constants::TEXTURETYPE_FLOAT) {
    Float32Array rgbaData(static_cast<size_t>(width * height * 4));
    const auto rgbDataFloat32Array = rgbData.float32Array();
    // Convert each pixel.
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        auto index    = static_cast<size_t>((y * width + x) * 3);
        auto newIndex = static_cast<size_t>((y * width + x) * 4);

        // Map Old Value to new value.
        rgbaData[newIndex + 0] = rgbDataFloat32Array[index + 0];
        rgbaData[newIndex + 1] = rgbDataFloat32Array[index + 1];
        rgbaData[newIndex + 2] = rgbDataFloat32Array[index + 2];

        // Add fully opaque alpha channel.
        rgbaData[newIndex + 3] = 1.f;
      }
    }
    return ArrayBufferView(rgbaData);
  }
  else {
    Uint32Array rgbaData(static_cast<size_t>(width * height * 4));
    const auto rgbDataUint32Array = rgbData.uint32Array();
    // Convert each pixel.
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        auto index    = static_cast<size_t>((y * width + x) * 3);
        auto newIndex = static_cast<size_t>((y * width + x) * 4);

        // Map Old Value to new value.
        rgbaData[newIndex + 0] = rgbDataUint32Array[index + 0];
        rgbaData[newIndex + 1] = rgbDataUint32Array[index + 1];
        rgbaData[newIndex + 2] = rgbDataUint32Array[index + 2];

        // Add fully opaque alpha channel.
        rgbaData[newIndex + 3] = 1;
      }
    }
    return ArrayBufferView(rgbaData);
  }
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

void Engine::resize()
{
  // We're not resizing the size of the canvas while in VR mode & presenting
  if (isVRPresenting()) {
    return;
  }

  ThinEngine::resize();
}

std::string& Engine::setTextureFormatToUse(const std::vector<std::string>& formatsAvailable)
{
  for (const auto& textureSupported1 : _texturesSupported) {
    for (const auto& formatAvailable : formatsAvailable) {
      if (textureSupported1 == String::toLowerCase(formatAvailable)) {
        _textureFormatInUse = textureSupported1;
        return _textureFormatInUse;
      }
    }
  }
  // actively set format to nothing, to allow this to be called more than once and possibly fail the
  // 2nd time
  _textureFormatInUse.clear();
  return _textureFormatInUse;
}

void Engine::setCompressedTextureExclusions(const std::vector<std::string>& skippedFiles)
{
  _excludedCompressedTextures = skippedFiles;
}

void Engine::setSize(int width, int height)
{
  if (!_renderingCanvas) {
    return;
  }

  ThinEngine::setSize(width, height);

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
}

void Engine::updateDynamicVertexBuffer(const WebGLDataBufferPtr& vertexBuffer,
                                       const Float32Array& data, int byteOffset, int byteLength)
{
  bindArrayBuffer(vertexBuffer);

  if (byteOffset == -1) {
    byteOffset = 0;
  }

  const auto dataLength = static_cast<int>(data.size() * sizeof(float));

  if (byteLength == -1 || (byteLength >= dataLength && byteOffset == 0)) {
    _gl->bufferSubData(GL::ARRAY_BUFFER, byteOffset, data);
  }
  else {
    auto byteArray = stl_util::to_array<uint8_t>(data, static_cast<size_t>(byteOffset),
                                                 static_cast<size_t>(byteLength));
    _gl->bufferSubData(GL::ARRAY_BUFFER, 0, byteArray);
  }

  _resetVertexBufferBinding();
}

void Engine::_deletePipelineContext(const IPipelineContextPtr& pipelineContext)
{
  auto webGLPipelineContext = std::static_pointer_cast<WebGLPipelineContext>(pipelineContext);
  if (webGLPipelineContext && webGLPipelineContext->program) {
    if (webGLPipelineContext->transformFeedback) {
      deleteTransformFeedback(webGLPipelineContext->transformFeedback.get());
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
  context = context ? context _gl;

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
                             const std::vector<std::string>& transformFeedbackVaryings = {})
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

    bindTransformFeedback(transformFeedback.get());
    setTranformFeedbackVaryings(shaderProgram.get(), transformFeedbackVaryings);
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

  auto rtt = createRenderTargetTexture(ISize(destination->width, destination->height), options);

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
    _releaseTexture(rtt.get());

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

void Engine::setFrameBufferDepthStencilTexture(const RenderTargetTexturePtr& renderTarget)
{
  // Create the framebuffer
  auto internalTexture = renderTarget->getInternalTexture();
  if (!internalTexture || !internalTexture->_framebuffer || !renderTarget->depthStencilTexture) {
    return;
  }

  auto& gl                  = *_gl;
  auto& depthStencilTexture = renderTarget->depthStencilTexture;

  _bindUnboundFramebuffer(internalTexture->_framebuffer);
  if (depthStencilTexture->isCube) {
    if (depthStencilTexture->_generateStencilBuffer) {
      gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::DEPTH_STENCIL_ATTACHMENT,
                              GL::TEXTURE_CUBE_MAP_POSITIVE_X,
                              depthStencilTexture->_webGLTexture.get(), 0);
    }
    else {
      gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::DEPTH_ATTACHMENT,
                              GL::TEXTURE_CUBE_MAP_POSITIVE_X,
                              depthStencilTexture->_webGLTexture.get(), 0);
    }
  }
  else {
    if (depthStencilTexture->_generateStencilBuffer) {
      gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::DEPTH_STENCIL_ATTACHMENT, GL::TEXTURE_2D,
                              depthStencilTexture->_webGLTexture.get(), 0);
    }
    else {
      gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::DEPTH_ATTACHMENT, GL::TEXTURE_2D,
                              depthStencilTexture->_webGLTexture.get(), 0);
    }
  }
  _bindUnboundFramebuffer(nullptr);
}

void Engine::updateDynamicIndexBuffer(const WebGLDataBufferPtr& indexBuffer,
                                      const IndicesArray& indices, int /*offset*/)
{
  // Force cache update
  _currentBoundBuffer[GL::ELEMENT_ARRAY_BUFFER] = nullptr;
  bindIndexBuffer(indexBuffer);
  auto arrayBuffer = indices;

  _gl->bufferData(GL::ELEMENT_ARRAY_BUFFER, arrayBuffer, GL::DYNAMIC_DRAW);

  _resetIndexBufferBinding();
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

    gl.bindRenderbuffer(GL::RENDERBUFFER, colorRenderbuffer);
    gl.renderbufferStorageMultisample(GL::RENDERBUFFER, static_cast<int>(samples),
                                      _getRGBAMultiSampleBufferFormat(texture->type),
                                      texture->width, texture->height);

    gl.framebufferRenderbuffer(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0, GL::RENDERBUFFER,
                               colorRenderbuffer.get());

    texture->_MSAARenderBuffer = std::move(colorRenderbuffer);
  }
  else {
    _bindUnboundFramebuffer(texture->_framebuffer);
  }

  texture->samples             = samples;
  texture->_depthStencilBuffer = _setupFramebufferDepthAttachments(
    texture->_generateStencilBuffer, texture->_generateDepthBuffer, texture->width, texture->height,
    static_cast<int>(samples));

  _gl->bindRenderbuffer(GL::RENDERBUFFER, nullptr);
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

ArrayBufferView Engine::_readTexturePixels(const InternalTexturePtr& texture, int width, int height,
                                           int faceIndex, int level,
                                           std::optional<ArrayBufferView> buffer)
{
  auto& gl = *_gl;
  if (!_dummyFramebuffer) {
    auto dummy = gl.createFramebuffer();

    if (!dummy) {
      throw std::runtime_error("Unable to create dummy framebuffer");
    }

    _dummyFramebuffer = dummy;
  }
  gl.bindFramebuffer(GL::FRAMEBUFFER, _dummyFramebuffer.get());

  if (faceIndex > -1) {
    auto _faceIndex = static_cast<unsigned>(faceIndex);
    gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                            GL::TEXTURE_CUBE_MAP_POSITIVE_X + _faceIndex,
                            texture->_webGLTexture.get(), level);
  }
  else {
    gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0, GL::TEXTURE_2D,
                            texture->_webGLTexture.get(), level);
  }

  auto readType = _getWebGLTextureType(texture->type);

  switch (readType) {
    case GL::UNSIGNED_BYTE: {
      if (!buffer.has_value()) {
        buffer = ArrayBufferView(Uint8Array(static_cast<std::size_t>(4 * width * height)));
      }
      readType = GL::UNSIGNED_BYTE;
      gl.readPixels(0, 0, width, height, GL::RGBA, readType, buffer->uint8Array());
      gl.bindFramebuffer(GL::FRAMEBUFFER, _currentFramebuffer.get());
    } break;
    default: {
      if (!buffer.has_value()) {
        buffer = ArrayBufferView(Float32Array(static_cast<std::size_t>(4 * width * height)));
      }
      readType                = GL::FLOAT;
      auto bufferFloat32Array = buffer->float32Array();
      gl.readPixels(0, 0, width, height, GL::RGBA, readType, bufferFloat32Array);
      gl.bindFramebuffer(GL::FRAMEBUFFER, _currentFramebuffer.get());
      buffer = ArrayBufferView(bufferFloat32Array);
    } break;
  }

  return *buffer;
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

  if (_dummyFramebuffer) {
    _gl->deleteFramebuffer(_dummyFramebuffer.get());
  }

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
  const auto _loadingScreen = loadingScreen();
  if (_loadingScreen) {
    _loadingScreen->displayLoadingUI();
  }
}

void Engine::hideLoadingUI()
{
  const auto _loadingScreen = loadingScreen();
  if (_loadingScreen) {
    _loadingScreen->hideLoadingUI();
  }
}

ILoadingScreenPtr& Engine::get_loadingScreen()
{
  if (!_loadingScreen && _renderingCanvas) {
    _loadingScreen = Engine::DefaultLoadingScreenFactory(_renderingCanvas);
  }
  return _loadingScreen;
}

void Engine::set_loadingScreen(const ILoadingScreenPtr& loadingScreen)
{
  _loadingScreen = loadingScreen;
}

void Engine::set_loadingUIText(const std::string& text)
{
  loadingScreen()->loadingUIText = text;
}

void Engine::set_loadingUIBackgroundColor(const std::string& color)
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

} // end of namespace BABYLON
