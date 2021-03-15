#include <babylon/engines/null_engine.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/materials/textures/render_target_creation_options.h>
#include <babylon/maths/isize.h>
#include <babylon/meshes/webgl/webgl_data_buffer.h>
#include <babylon/states/alpha_state.h>
#include <babylon/states/depth_culling_state.h>
#include <babylon/states/stencil_state.h>

namespace BABYLON {

bool NullEngine::isDeterministicLockStep() const
{
  return _options.deterministicLockstep;
}

unsigned int NullEngine::getLockstepMaxSteps() const
{
  return _options.lockstepMaxSteps;
}

float NullEngine::getHardwareScalingLevel() const
{
  return 1.f;
}

NullEngine::NullEngine(const NullEngineOptions& options) : Engine{nullptr}
{
  _options = options;

  Engine::Instances().emplace_back(this);

  // Init caps
  // We consider we are on a webgl1 capable device

  {
    _caps                                 = EngineCapabilities();
    _caps.maxTexturesImageUnits           = 16;
    _caps.maxVertexTextureImageUnits      = 16;
    _caps.maxCombinedTexturesImageUnits   = 32;
    _caps.maxTextureSize                  = 512;
    _caps.maxCubemapTextureSize           = 512;
    _caps.maxRenderTextureSize            = 512;
    _caps.maxVertexAttribs                = 16;
    _caps.maxVaryingVectors               = 16;
    _caps.maxFragmentUniformVectors       = 16;
    _caps.maxVertexUniformVectors         = 16;
    _caps.standardDerivatives             = false;
    _caps.astc                            = nullptr;
    _caps.pvrtc                           = nullptr;
    _caps.etc1                            = nullptr;
    _caps.etc2                            = nullptr;
    _caps.bptc                            = nullptr;
    _caps.maxAnisotropy                   = 0;
    _caps.uintIndices                     = false;
    _caps.fragmentDepthSupported          = false;
    _caps.highPrecisionShaderSupported    = true;
    _caps.colorBufferFloat                = false;
    _caps.textureFloat                    = false;
    _caps.textureFloatLinearFiltering     = false;
    _caps.textureFloatRender              = false;
    _caps.textureHalfFloat                = false;
    _caps.textureHalfFloatLinearFiltering = false;
    _caps.textureHalfFloatRender          = false;
    _caps.textureLOD                      = false;
    _caps.drawBuffersExtension            = false;
    _caps.depthTextureExtension           = false;
    _caps.vertexArrayObject               = false;
    _caps.instancedArrays                 = false;
    _caps.canUseTimestampForTimerQuery    = false;
    _caps.maxMSAASamples                  = 1;
    _caps.blendMinMax                     = false;
    _caps.canUseGLInstanceID              = false;
    _caps.canUseGLVertexID                = false;
  }

  {
    _features.forceBitmapOverHTMLImageElement           = false;
    _features.supportRenderAndCopyToLodForFloatTextures = false;
    _features.supportDepthStencilTexture                = false;
    _features.supportShadowSamplers                     = false;
    _features.uniformBufferHardCheckMatrix              = false;
    _features.allowTexturePrefiltering                  = false;
    _features.trackUbosInFrame                          = false;
    _features.supportCSM                                = false;
    _features.basisNeedsPOT                             = false;
    _features.support3DTextures                         = false;
    _features.needTypeSuffixInShaderConstants           = false;
    _features.supportMSAA                               = false;
    _features.supportSSAO2                              = false;
    _features.supportExtendedTextureFormats             = false;
    _features.supportSwitchCaseInShader                 = false;
    _features.supportSyncTextureRead                    = false;
    _features.needsInvertingBitmap                      = false;
    _features._collectUbosUpdatedInFrame                = false;
  }

  BABYLON_LOGF_INFO("Engine", "Babylon.js v%s - Null engine", Engine::Version().c_str())
}

NullEngine::~NullEngine() = default;

WebGLDataBufferPtr NullEngine::createVertexBuffer(const Float32Array& /*vertices*/)
{
  auto buffer        = std::make_shared<WebGLDataBuffer>(nullptr);
  buffer->references = 1;
  return buffer;
}

WebGLDataBufferPtr NullEngine::createIndexBuffer(const IndicesArray& /*indices*/,
                                                 bool /*updatable*/)
{
  auto buffer        = std::make_shared<WebGLDataBuffer>(nullptr);
  buffer->references = 1;
  return buffer;
}

void NullEngine::clear(const std::optional<Color4>& /*color*/, bool /*backBuffer*/, bool /*depth*/,
                       bool /*stencil*/)
{
}

int NullEngine::getRenderWidth(bool useScreen) const
{
  if (!useScreen && _currentRenderTarget) {
    return _currentRenderTarget->width;
  }

  return _options.renderWidth;
}

int NullEngine::getRenderHeight(bool useScreen) const
{
  if (!useScreen && _currentRenderTarget) {
    return _currentRenderTarget->height;
  }

  return _options.renderHeight;
}

void NullEngine::setViewport(const Viewport& viewport, const std::optional<int>& /*requiredWidth*/,
                             const std::optional<int>& /*requiredHeight*/
)
{
  _cachedViewport = viewport;
}

GL::IGLProgramPtr
NullEngine::createShaderProgram(const IPipelineContextPtr& /*pipelineContext*/,
                                const std::string& /*vertexCode*/,
                                const std::string& /*fragmentCode*/, const std::string& /*defines*/,
                                GL::IGLRenderingContext* /*context*/,
                                const std::vector<std::string>& /*transformFeedbackVaryings*/)
{
  auto program                      = std::make_shared<GL::IGLProgram>(0);
  program->__SPECTOR_rebuildProgram = nullptr;
  return program;
}

std::vector<WebGLUniformLocationPtr>
NullEngine::getUniforms(IPipelineContext* /*pipelineContext*/,
                        const std::vector<std::string>& /*uniformsNames*/)
{
  return {};
}

Int32Array NullEngine::getAttributes(IPipelineContext* /*pipelineContext*/,
                                     const std::vector<std::string>& /*attributesNames*/)
{
  return {};
}

void NullEngine::bindSamplers(Effect& /*effect*/)
{
  _currentEffect = nullptr;
}

void NullEngine::enableEffect(const EffectPtr& effect)
{
  _currentEffect = effect;

  if (effect->onBind) {
    effect->onBind(effect.get());
  }
  /* if (effect->_onBindObservable) */ {
    effect->_onBindObservable.notifyObservers(effect.get());
  }
}

void NullEngine::setState(bool /*culling*/, float /*zOffset*/, bool /*force*/, bool /*reverseSide*/)
{
}

bool NullEngine::setIntArray(const WebGLUniformLocationPtr& /*uniform*/,
                             const Int32Array& /*array*/)
{
  return true;
}

bool NullEngine::setIntArray2(const WebGLUniformLocationPtr& /*uniform*/,
                              const Int32Array& /*array*/)
{
  return true;
}

bool NullEngine::setIntArray3(const WebGLUniformLocationPtr& /*uniform*/,
                              const Int32Array& /*array*/)
{
  return true;
}

bool NullEngine::setIntArray4(const WebGLUniformLocationPtr& /*uniform*/,
                              const Int32Array& /*array*/)
{
  return true;
}

bool NullEngine::setFloatArray(const WebGLUniformLocationPtr& /*uniform*/,
                               const Float32Array& /*array*/)
{
  return true;
}

bool NullEngine::setFloatArray2(const WebGLUniformLocationPtr& /*uniform*/,
                                const Float32Array& /*array*/)
{
  return true;
}

bool NullEngine::setFloatArray3(const WebGLUniformLocationPtr& /*uniform*/,
                                const Float32Array& /*array*/)
{
  return true;
}

bool NullEngine::setFloatArray4(const WebGLUniformLocationPtr& /*uniform*/,
                                const Float32Array& /*array*/)
{
  return true;
}

bool NullEngine::setArray(const WebGLUniformLocationPtr& /*uniform*/, const Float32Array& /*array*/)
{
  return true;
}

bool NullEngine::setArray2(const WebGLUniformLocationPtr& /*uniform*/,
                           const Float32Array& /*array*/)
{
  return true;
}

bool NullEngine::setArray3(const WebGLUniformLocationPtr& /*uniform*/,
                           const Float32Array& /*array*/)
{
  return true;
}

bool NullEngine::setArray4(const WebGLUniformLocationPtr& /*uniform*/,
                           const Float32Array& /*array*/)
{
  return true;
}

bool NullEngine::setMatrices(const WebGLUniformLocationPtr& /*uniform*/,
                             const Float32Array& /*matrices*/)
{
  return true;
}

bool NullEngine::setMatrix3x3(const WebGLUniformLocationPtr& /*uniform*/,
                              const Float32Array& /*matrix*/)
{
  return true;
}

bool NullEngine::setMatrix2x2(const WebGLUniformLocationPtr& /*uniform*/,
                              const Float32Array& /*matrix*/)
{
  return true;
}

bool NullEngine::setFloat(const WebGLUniformLocationPtr& /*uniform*/, float /*value*/)
{
  return true;
}

bool NullEngine::setFloat2(const WebGLUniformLocationPtr& /*uniform*/, float /*x*/, float /*y*/)
{
  return true;
}

bool NullEngine::setFloat3(const WebGLUniformLocationPtr& /*uniform*/, float /*x*/, float /*y*/,
                           float /*z*/)
{
  return true;
}

bool NullEngine::setBool(const WebGLUniformLocationPtr& /*uniform*/, int /*value*/)
{
  return true;
}

bool NullEngine::setFloat4(const WebGLUniformLocationPtr& /*uniform*/, float /*x*/, float /*y*/,
                           float /*z*/, float /*w*/)
{
  return true;
}

void NullEngine::setAlphaMode(unsigned int mode, bool noDepthWriteChange)
{
  if (_alphaMode == mode) {
    return;
  }

  alphaState()->alphaBlend = (mode != Constants::ALPHA_DISABLE);

  if (!noDepthWriteChange) {
    setDepthWrite(mode == Constants::ALPHA_DISABLE);
  }
  _alphaMode = mode;
}

void NullEngine::bindBuffers(
  const std::unordered_map<std::string, VertexBufferPtr>& /*vertexBuffers*/,
  const WebGLDataBufferPtr& /*indexBuffer*/, const EffectPtr& /*effect*/,
  const std::unordered_map<std::string, VertexBufferPtr>& /*overrideVertexBuffers*/)
{
}

void NullEngine::wipeCaches(bool bruteForce)
{
  if (preventCacheWipeBetweenFrames) {
    return;
  }
  resetTextureCache();
  _currentEffect = nullptr;

  if (bruteForce) {
    _currentProgram = nullptr;

    stencilState()->reset();
    depthCullingState()->reset();
    alphaState()->reset();
  }

  _cachedVertexBuffers          = nullptr;
  _cachedIndexBuffer            = nullptr;
  _cachedEffectForVertexBuffers = nullptr;
}

void NullEngine::draw(bool /*useTriangles*/, int /*indexStart*/, int /*indexCount*/,
                      int /*instancesCount*/)
{
}

void NullEngine::drawElementsType(unsigned int /*fillMode*/, int /*indexStart*/,
                                  int /*verticesCount*/, int /*instancesCount*/)
{
}

void NullEngine::drawArraysType(unsigned int /*fillMode*/, int /*verticesStart*/,
                                int /*verticesCount*/, int /*instancesCount*/)
{
}

WebGLTexturePtr NullEngine::_createTexture()
{
  return std::make_shared<GL::IGLTexture>(0);
}

void NullEngine::_releaseTexture(const InternalTexturePtr& /*texture*/)
{
}

InternalTexturePtr NullEngine::createTexture(
  std::string url, bool noMipmap, bool invertY, Scene* /*scene*/, unsigned int samplingMode,
  const std::function<void(InternalTexture*, EventState&)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& /*onError*/,
  const std::optional<std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>>& /*buffer*/,
  const InternalTexturePtr& /*fallBack*/, const std::optional<unsigned int>& format,
  const std::string& /*forcedExtension*/, const std::string& /*mimeType*/,
  const LoaderOptionsPtr& /*loaderOptions*/)
{
  auto texture = InternalTexture::New(this, InternalTextureSource::Url);

  texture->url             = url;
  texture->generateMipMaps = !noMipmap;
  texture->samplingMode    = samplingMode;
  texture->invertY         = invertY;
  texture->baseWidth       = _options.textureSize;
  texture->baseHeight      = _options.textureSize;
  texture->width           = _options.textureSize;
  texture->height          = _options.textureSize;
  if (format.has_value()) {
    texture->format = *format;
  }

  texture->isReady = true;

  if (onLoad) {
    EventState es{0};
    onLoad(texture.get(), es);
  }

  _internalTexturesCache.emplace_back(texture);

  return texture;
}

InternalTexturePtr
NullEngine::createRenderTargetTexture(const std::variant<int, RenderTargetSize, float>& size,
                                      const IRenderTargetOptions& options)
{
  RenderTargetCreationOptions fullOptions;

  {
    fullOptions.generateMipMaps     = options.generateMipMaps;
    fullOptions.generateDepthBuffer = options.generateDepthBuffer.value_or(true);
    fullOptions.generateStencilBuffer
      = fullOptions.generateDepthBuffer.value() && options.generateStencilBuffer.value_or(false);
    fullOptions.type = options.type.value_or(Constants::TEXTURETYPE_UNSIGNED_INT);
    fullOptions.samplingMode
      = options.samplingMode.value_or(Constants::TEXTURE_TRILINEAR_SAMPLINGMODE);
  }

  auto texture = InternalTexture::New(this, InternalTextureSource::RenderTarget);

  int width = 0, height = 0;
  if (std::holds_alternative<RenderTargetSize>(size)) {
    auto textureSize = std::get<RenderTargetSize>(size);
    width            = textureSize.width;
    height           = textureSize.height;
  }
  else if (std::holds_alternative<float>(size)) {
    auto textureSize = std::get<float>(size);
    width            = static_cast<int>(textureSize);
    height           = static_cast<int>(textureSize);
  }
  else if (std::holds_alternative<int>(size)) {
    auto textureSize = std::get<int>(size);
    width            = textureSize;
    height           = textureSize;
  }

  texture->_depthStencilBuffer    = std::make_shared<GL::IGLRenderbuffer>(0);
  texture->_framebuffer           = std::make_shared<GL::IGLFramebuffer>(0);
  texture->baseWidth              = width;
  texture->baseHeight             = height;
  texture->width                  = width;
  texture->height                 = height;
  texture->isReady                = true;
  texture->samples                = 1;
  texture->generateMipMaps        = fullOptions.generateMipMaps.value_or(false);
  texture->samplingMode           = *fullOptions.samplingMode;
  texture->type                   = *fullOptions.type;
  texture->_generateDepthBuffer   = *fullOptions.generateDepthBuffer;
  texture->_generateStencilBuffer = fullOptions.generateStencilBuffer.value_or(false);

  _internalTexturesCache.emplace_back(texture);

  return texture;
}

void NullEngine::updateTextureSamplingMode(unsigned int samplingMode,
                                           const InternalTexturePtr& texture,
                                           bool /*generateMipMaps*/)
{
  texture->samplingMode = samplingMode;
}

void NullEngine::bindFramebuffer(const InternalTexturePtr& texture, unsigned int /*faceIndex*/,
                                 std::optional<int> requiredWidth,
                                 std::optional<int> requiredHeight,
                                 std::optional<bool> forceFullscreenViewport, int /*lodLevel*/,
                                 int /*layer*/)
{
  if (_currentRenderTarget) {
    unBindFramebuffer(_currentRenderTarget);
  }
  _currentRenderTarget = texture;
  _currentFramebuffer
    = texture->_MSAAFramebuffer ? texture->_MSAAFramebuffer : texture->_framebuffer;
  if (_cachedViewport && !forceFullscreenViewport) {
    setViewport(*_cachedViewport, requiredWidth, requiredHeight);
  }
}

void NullEngine::unBindFramebuffer(const InternalTexturePtr& texture,
                                   bool /*disableGenerateMipMaps*/,
                                   const std::function<void()>& onBeforeUnbind)
{
  _currentRenderTarget = nullptr;

  if (onBeforeUnbind) {
    if (texture->_MSAAFramebuffer) {
      _currentFramebuffer = texture->_framebuffer;
    }
    onBeforeUnbind();
  }
  _currentFramebuffer = nullptr;
}

WebGLDataBufferPtr NullEngine::createDynamicVertexBuffer(const Float32Array& /*vertices*/)
{
  auto buffer        = std::make_shared<WebGLDataBuffer>(nullptr);
  buffer->references = 1;
  buffer->capacity   = 1;
  return buffer;
}

void NullEngine::updateDynamicTexture(const InternalTexturePtr& /*texture*/, ICanvas* /*source*/,
                                      std::optional<bool> /*invertY*/, bool /*premulAlpha*/,
                                      std::optional<unsigned int> /*format*/,
                                      bool /*forceBindTexture*/)
{
}

bool NullEngine::areAllEffectsReady() const
{
  return true;
}

unsigned int NullEngine::getError() const
{
  return 0;
}

int NullEngine::_getUnpackAlignement()
{
  return 1;
}

void NullEngine::_unpackFlipY(bool /*value*/)
{
}

void NullEngine::updateDynamicIndexBuffer(const WebGLDataBufferPtr& /*indexBuffer*/,
                                          const IndicesArray& /*indices*/, int /*offset*/)
{
}

void NullEngine::updateDynamicVertexBuffer(const WebGLDataBufferPtr& /*vertexBuffer*/,
                                           const Float32Array& /*data*/, int /*byteOffset*/,
                                           int /*byteLength*/)
{
}

bool NullEngine::_bindTextureDirectly(unsigned int /*target*/, const InternalTexturePtr& texture,
                                      bool /*forTextureDataUpdate*/, bool /*force*/)
{
  if (stl_util::contains(_boundTexturesCache, _activeChannel)
      && _boundTexturesCache[_activeChannel] != texture) {
    _boundTexturesCache[_activeChannel] = texture;
    return true;
  }
  return false;
}

void NullEngine::_bindTexture(int channel, const InternalTexturePtr& texture,
                              const std::string& /*name*/)
{
  if (channel < 0) {
    return;
  }

  _bindTextureDirectly(0, texture);
}

void NullEngine::_deleteBuffer(GL::IGLBuffer* /*buffer*/)
{
}

void NullEngine::releaseEffects()
{
}

void NullEngine::displayLoadingUI()
{
}

void NullEngine::hideLoadingUI()
{
}

void NullEngine::_uploadCompressedDataToTextureDirectly(const InternalTexturePtr& /*texture*/,
                                                        unsigned int /*internalFormat*/,
                                                        int /*width*/, int /*height*/,
                                                        const Uint8Array& /*data*/,
                                                        unsigned int /*faceIndex*/, int /*lod*/)
{
}

void NullEngine::_uploadDataToTextureDirectly(const InternalTexturePtr& /*texture*/,
                                              const ArrayBufferView& /*imageData*/,
                                              unsigned int /*faceIndex*/, int /*lod*/,
                                              int /*babylonInternalFormat*/,
                                              bool /*useTextureWidthAndHeight*/)
{
}

void NullEngine::_uploadArrayBufferViewToTexture(const InternalTexturePtr& /*texture*/,
                                                 const Uint8Array& /*imageData*/,
                                                 unsigned int /*faceIndex*/, int /*lod*/)
{
}

void NullEngine::_uploadImageToTexture(const InternalTexturePtr& /*texture*/,
                                       const Image& /*image*/, unsigned int /*faceIndex*/,
                                       int /*lod*/)
{
}

} // end of namespace BABYLON
