#include <babylon/engines/thin_engine.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/babylon_version.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine_store.h>
#include <babylon/engines/extensions/alpha_extension.h>
#include <babylon/engines/extensions/cube_texture_extension.h>
#include <babylon/engines/extensions/dynamic_texture_extension.h>
#include <babylon/engines/extensions/multi_render_extension.h>
#include <babylon/engines/extensions/render_target_cube_extension.h>
#include <babylon/engines/extensions/render_target_extension.h>
#include <babylon/engines/extensions/uniform_buffer_extension.h>
#include <babylon/engines/instancing_attribute_info.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/webgl/webgl2_shader_processor.h>
#include <babylon/engines/webgl/webgl_pipeline_context.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/icanvas_rendering_context2D.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/iinternal_texture_loader.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/loaders/dds_texture_loader.h>
#include <babylon/materials/textures/loaders/env_texture_loader.h>
#include <babylon/materials/textures/loaders/ktx_texture_loader.h>
#include <babylon/materials/textures/loaders/tga_texture_loader.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/viewport.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/webgl/webgl_data_buffer.h>
#include <babylon/misc/dds.h>
#include <babylon/misc/file_tools.h>
#include <babylon/misc/string_tools.h>
#include <babylon/states/alpha_state.h>
#include <babylon/states/depth_culling_state.h>
#include <babylon/states/stencil_state.h>

namespace BABYLON {

// Register the loaders
std::vector<IInternalTextureLoaderPtr> ThinEngine::_TextureLoaders = {
  std::make_shared<_DDSTextureLoader>(), // DDS Texture Loader
  std::make_shared<_ENVTextureLoader>(), // ENV Texture Loader
  std::make_shared<_KTXTextureLoader>(), // KTX Texture Loader
  std::make_shared<_TGATextureLoader>()  // TGA Texture Loader
};

std::string ThinEngine::Version()
{
  return BABYLONCPP_VERSION;
}

std::string ThinEngine::description() const
{
  std::ostringstream description;
  description << "WebGL" << webGLVersion();

  if (_caps.parallelShaderCompile) {
    description << " - Parallel shader compilation";
  }

  return description.str();
}

float ThinEngine::CollisionsEpsilon = 0.001f;

std::string ThinEngine::ShadersRepository()
{
  return Effect::ShadersRepository;
}

void ThinEngine::setShadersRepository(const std::string& value)
{
  Effect::ShadersRepository = value;
}

std::string
ThinEngine::excludedCompressedTextureFormats(const std::string& url,
                                             const std::string& iTextureFormatInUse) const
{
  const auto skipCompression = [this, &url]() -> bool {
    for (const auto& entry : _excludedCompressedTextures) {
      const auto strRegExPattern = "\\b" + entry + "\\b";
      std::regex regex(strRegExPattern, std::regex::optimize);
      std::smatch match;

      if (!url.empty()
          && (url == entry || (std::regex_search(url, match, regex) && !match.empty()))) {
        return true;
      }
    }
    return false;
  };

  return skipCompression() ? "" : iTextureFormatInUse;
}

ThinEngine::ThinEngine(ICanvas* canvas, const EngineOptions& options)
    : supportsUniformBuffers{this, &ThinEngine::get_supportsUniformBuffers}
    , _shouldUseHighPrecisionShader{this, &ThinEngine::get__shouldUseHighPrecisionShader}
    , needPOTTextures{this, &ThinEngine::get_needPOTTextures}
    , doNotHandleContextLost{this, &ThinEngine::get_doNotHandleContextLost,
                             &ThinEngine::set_doNotHandleContextLost}
    , _alphaState{std::make_unique<AlphaState>()}
    , texturesSupported{this, &ThinEngine::get_texturesSupported}
    , textureFormatInUse{this, &ThinEngine::get_textureFormatInUse}
    , currentViewport{this, &ThinEngine::get_currentViewport}
    , emptyTexture{this, &ThinEngine::get_emptyTexture}
    , emptyTexture3D{this, &ThinEngine::get_emptyTexture3D}
    , emptyTexture2DArray{this, &ThinEngine::get_emptyTexture2DArray}
    , emptyCubeTexture{this, &ThinEngine::get_emptyCubeTexture}
    , webGLVersion{this, &ThinEngine::get_webGLVersion}
    , isStencilEnable{this, &ThinEngine::get_isStencilEnable}
    , depthCullingState{this, &ThinEngine::get_depthCullingState}
    , alphaState{this, &ThinEngine::get_alphaState}
    , stencilState{this, &ThinEngine::get_stencilState}
    , _depthCullingState{std::make_unique<DepthCullingState>()}
    , _stencilState{std::make_unique<StencilState>()}
    , _supportsHardwareTextureRescaling{this, &ThinEngine::get__supportsHardwareTextureRescaling}
    , _alphaExtension{std::make_unique<AlphaExtension>(this)}
    , _cubeTextureExtension{std::make_unique<CubeTextureExtension>(this)}
    , _dynamicTextureExtension{std::make_unique<DynamicTextureExtension>(this)}
    , _multiRenderExtension{std::make_unique<MultiRenderExtension>(this)}
    , _renderTargetExtension{std::make_unique<RenderTargetExtension>(this)}
    , _renderTargetCubeExtension{std::make_unique<RenderTargetCubeExtension>(this)}
    , _uniformBufferExtension{std::make_unique<UniformBufferExtension>(this)}
{
  if (!canvas) {
    return;
  }

  _renderingCanvas = canvas;

  // GL
  if (!options.disableWebGL2Support) {
    _gl = canvas->getContext3d(options);
    if (_gl) {
      _webGLVersion = 2.f;
    }
  }

  if (!_gl) {
    if (!canvas) {
      BABYLON_LOG_ERROR("ThinEngine", "The provided canvas is null or undefined")
      return;
    }
    _gl = canvas->getContext3d(options);
  }

  if (!_gl) {
    BABYLON_LOG_ERROR("ThinEngine", "GL not supported")
    return;
  }

  // Viewport
  _hardwareScalingLevel = options.adaptToDeviceRatio ? 1.f : 1.f;
  resize();

  _isStencilEnable = options.stencil;
  _initGLContext();

  // Prepare buffer pointers
  for (unsigned int i = 0, ul = static_cast<unsigned>(_caps.maxVertexAttribs); i < ul; ++i) {
    _currentBufferPointers[i] = BufferPointer();
  }

  // Shader processor
  if (webGLVersion() > 1.f) {
    _shaderProcessor = std::make_shared<WebGL2ShaderProcessor>();
  }

  // Detect if we are running on a faulty buggy OS.
  _badOS = false;

  // Detect if we are running on a faulty buggy desktop OS.
  _badDesktopOS = false;

  _creationOptions = options;
  BABYLON_LOGF_INFO("ThinEngine", "Babylon.js v%s - %s", ThinEngine::Version().c_str(),
                    description().c_str())
}

ThinEngine::~ThinEngine()
{
}

bool ThinEngine::get_supportsUniformBuffers() const
{
  return webGLVersion() > 1.f && !disableUniformBuffers;
}

bool ThinEngine::get__shouldUseHighPrecisionShader() const
{
  return _caps.highPrecisionShaderSupported && _highPrecisionShadersAllowed;
}

bool ThinEngine::get_needPOTTextures() const
{
  return _webGLVersion < 2.f || forcePOTTextures;
}

bool ThinEngine::get_doNotHandleContextLost() const
{
  return _doNotHandleContextLost;
}

void ThinEngine::set_doNotHandleContextLost(bool value)
{
  _doNotHandleContextLost = value;
}

bool ThinEngine::get__supportsHardwareTextureRescaling() const
{
  return false;
}

std::vector<std::string>& ThinEngine::get_texturesSupported()
{
  return _texturesSupported;
}

std::string ThinEngine::get_textureFormatInUse() const
{
  return _textureFormatInUse;
}

std::optional<Viewport>& ThinEngine::get_currentViewport()
{
  return _cachedViewport;
}

InternalTexturePtr& ThinEngine::get_emptyTexture()
{
  if (!_emptyTexture) {
    _emptyTexture = createRawTexture(Uint8Array(4), 1, 1, Constants::TEXTUREFORMAT_RGBA, false,
                                     false, Constants::TEXTURE_NEAREST_SAMPLINGMODE);
  }

  return _emptyTexture;
}

InternalTexturePtr& ThinEngine::get_emptyTexture3D()
{
  if (!_emptyTexture3D) {
    _emptyTexture3D = createRawTexture3D(Uint8Array(4), 1, 1, 1, Constants::TEXTUREFORMAT_RGBA,
                                         false, false, Constants::TEXTURE_NEAREST_SAMPLINGMODE);
  }

  return _emptyTexture3D;
}

InternalTexturePtr& ThinEngine::get_emptyTexture2DArray()
{
  if (!_emptyTexture2DArray) {
    _emptyTexture2DArray
      = createRawTexture2DArray(Uint8Array(4), 1, 1, 1, Constants::TEXTUREFORMAT_RGBA, false, false,
                                Constants::TEXTURE_NEAREST_SAMPLINGMODE);
  }

  return _emptyTexture2DArray;
}

InternalTexturePtr& ThinEngine::get_emptyCubeTexture()
{
  if (!_emptyCubeTexture) {
    Uint8Array faceData(4);
    std::vector<ArrayBufferView> cubeData{faceData, faceData, faceData,
                                          faceData, faceData, faceData};
    _emptyCubeTexture = createRawCubeTexture(cubeData, 1, Constants::TEXTUREFORMAT_RGBA,
                                             Constants::TEXTURETYPE_UNSIGNED_INT, false, false,
                                             Constants::TEXTURE_NEAREST_SAMPLINGMODE);
  }

  return _emptyCubeTexture;
}

void ThinEngine::_rebuildInternalTextures()
{
  const auto currentState
    = _internalTexturesCache; // Do a copy because the rebuild will add proxies

  for (const auto& internalTexture : currentState) {
    internalTexture->_rebuild();
  }
}

void ThinEngine::_rebuildEffects()
{
  for (const auto& item : _compiledEffects) {
    const auto& effect = item.second;

    effect->_prepareEffect();
  }

  Effect::ResetCache();
}

bool ThinEngine::areAllEffectsReady() const
{
  for (const auto& compiledEffectItem : _compiledEffects) {
    const auto& effect = compiledEffectItem.second;

    if (!effect->isReady()) {
      return false;
    }
  }

  return true;
}

void ThinEngine::_rebuildBuffers()
{
  // Uniforms
  for (const auto& uniformBuffer : _uniformBuffers) {
    uniformBuffer->_rebuild();
  }
}

void ThinEngine::_initGLContext()
{
  // Caps
  _caps.maxTexturesImageUnits         = _gl->getParameteri(GL::MAX_TEXTURE_IMAGE_UNITS);
  _caps.maxCombinedTexturesImageUnits = _gl->getParameteri(GL::MAX_COMBINED_TEXTURE_IMAGE_UNITS);
  _caps.maxVertexTextureImageUnits    = _gl->getParameteri(GL::MAX_VERTEX_TEXTURE_IMAGE_UNITS);
  _caps.maxTextureSize                = _gl->getParameteri(GL::MAX_TEXTURE_SIZE);
  _caps.maxSamples                = _webGLVersion > 1.f ? _gl->getParameteri(GL::MAX_SAMPLES) : 1;
  _caps.maxCubemapTextureSize     = _gl->getParameteri(GL::MAX_CUBE_MAP_TEXTURE_SIZE);
  _caps.maxRenderTextureSize      = _gl->getParameteri(GL::MAX_RENDERBUFFER_SIZE);
  _caps.maxVertexAttribs          = _gl->getParameteri(GL::MAX_VERTEX_ATTRIBS);
  _caps.maxVaryingVectors         = _gl->getParameteri(GL::MAX_VARYING_VECTORS);
  _caps.maxFragmentUniformVectors = _gl->getParameteri(GL::MAX_FRAGMENT_UNIFORM_VECTORS);
  _caps.maxVertexUniformVectors   = _gl->getParameteri(GL::MAX_VERTEX_UNIFORM_VECTORS);
  _caps.parallelShaderCompile
    = _gl->getExtension("KHR_parallel_shader_compile") ? std::nullopt : std::nullopt;
  _caps.standardDerivatives
    = _webGLVersion > 1.f || (_gl->getExtension("OES_standard_derivatives") != nullptr);
  _caps.maxAnisotropy = 1;
  _caps.astc          = _gl->getExtension("WEBGL_compressed_texture_astc") ?
                 _gl->getExtension("WEBGL_compressed_texture_astc") :
                 _gl->getExtension("WEBKIT_WEBGL_compressed_texture_astc");
  _caps.s3tc = (_gl->getExtension("WEBGL_compressed_texture_s3tc")
                || _gl->getExtension("WEBKIT_WEBGL_compressed_texture_s3tc")) ?
                 std::nullopt :
                 std::nullopt;
  _caps.pvrtc = _gl->getExtension("WEBGL_compressed_texture_pvrtc") ?
                  _gl->getExtension("WEBGL_compressed_texture_pvrtc") :
                  _gl->getExtension("WEBKIT_WEBGL_compressed_texture_pvrtc");
  _caps.etc1 = _gl->getExtension("WEBGL_compressed_texture_etc1") ?
                 _gl->getExtension("WEBGL_compressed_texture_etc1") :
                 _gl->getExtension("WEBKIT_WEBGL_compressed_texture_etc1");
  _caps.etc2
    = _gl->getExtension("WEBGL_compressed_texture_etc") ?
        _gl->getExtension("WEBGL_compressed_texture_etc") :
        _gl->getExtension("WEBKIT_WEBGL_compressed_texture_etc") ?
        _gl->getExtension("WEBKIT_WEBGL_compressed_texture_etc") :
        _gl->getExtension("WEBGL_compressed_texture_es3_0"); // also a requirement of OpenGL ES 3
  _caps.textureAnisotropicFilterExtension
    = (_gl->getExtension("EXT_texture_filter_anisotropic")
       || _gl->getExtension("WEBKIT_EXT_texture_filter_anisotropic")
       || _gl->getExtension("MOZ_EXT_texture_filter_anisotropic")) ?
        std::nullopt :
        std::nullopt;
  _caps.uintIndices = _webGLVersion > 1.f || _gl->getExtension("OES_element_index_uint") != nullptr;
  _caps.fragmentDepthSupported
    = _webGLVersion > 1 || _gl->getExtension("EXT_frag_depth") != nullptr;
  _caps.highPrecisionShaderSupported = false;
  _caps.timerQuery                   = _gl->getExtension("EXT_disjoint_timer_query_webgl2")
                         || _gl->getExtension("EXT_disjoint_timer_query") ?
                       nullptr :
                       nullptr;
  _caps.canUseTimestampForTimerQuery = false;
  _caps.drawBuffersExtension         = false;
  _caps.maxMSAASamples               = 1;
  _caps.colorBufferFloat = _webGLVersion > 1 && _gl->getExtension("EXT_color_buffer_float");
  _caps.textureFloat = (_webGLVersion > 1 || _gl->getExtension("OES_texture_float")) ? true : false;
  _caps.textureHalfFloat
    = (_webGLVersion > 1 || _gl->getExtension("OES_texture_half_float")) ? true : false;
  _caps.textureHalfFloatRender          = false;
  _caps.textureFloatLinearFiltering     = false;
  _caps.textureFloatRender              = false;
  _caps.textureHalfFloatLinearFiltering = false;
  _caps.vertexArrayObject               = false;
  _caps.instancedArrays                 = false;
  _caps.textureLOD
    = (_webGLVersion > 1.f || _gl->getExtension("EXT_shader_texture_lod")) ? true : false;
  _caps.blendMinMax           = false;
  _caps.multiview             = _gl->getExtension("OVR_multiview2");
  _caps.oculusMultiview       = _gl->getExtension("OCULUS_multiview");
  _caps.depthTextureExtension = false;

  // Those parameters cannot always be reliably queried
  // (GlGetError returns INVALID_ENUM under windows 10 (VM with parallels desktop opengl driver)
  // In case of failure, we set reasonable values
  {
    _caps.maxVaryingVectors = _gl->getParameteri(GL::MAX_VARYING_VECTORS);
    if (_caps.maxVaryingVectors == 0) {
      BABYLON_LOGF_WARN(
        "ThinEngine",
        "_gl->getParameteri(GL::MAX_VARYING_VECTORS) failed => using %i as a default...", 16)
      _caps.maxVaryingVectors = 16;
    }

    _caps.maxFragmentUniformVectors = _gl->getParameteri(GL::MAX_FRAGMENT_UNIFORM_VECTORS);
    if (_caps.maxFragmentUniformVectors == 0) {
      BABYLON_LOGF_WARN(
        "ThinEngine",
        "_gl->getParameteri(GL::MAX_FRAGMENT_UNIFORM_VECTORS) failed => using %i as a default...",
        256)
      _caps.maxFragmentUniformVectors = 256;
    }

    _caps.maxVertexUniformVectors = _gl->getParameteri(GL::MAX_VERTEX_UNIFORM_VECTORS);
    if (_caps.maxVertexUniformVectors == 0) {
      BABYLON_LOGF_WARN(
        "ThinEngine",
        "_gl->getParameteri(GL::MAX_VERTEX_UNIFORM_VECTORS) failed => using %i as a default...",
        256)
      _caps.maxVertexUniformVectors = 256;
    }
  }

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

  // Extensions
  auto extensionList = StringTools::split(_gl->getString(GL::EXTENSIONS), ' ');
  std::set<std::string> extensions;
  for (const auto& extension : extensionList) {
    extensions.insert(extension);
  }

  _caps.maxAnisotropy
    = static_cast<unsigned>(_gl->getParameteri(GL::MAX_TEXTURE_MAX_ANISOTROPY_EXT));
  _caps.uintIndices                  = (_webGLVersion > 1.f);
  _caps.fragmentDepthSupported       = (_webGLVersion > 1.f);
  _caps.highPrecisionShaderSupported = false;

  // Checks if some of the format renders first to allow the use of webgl inspector.
  _caps.colorBufferFloat = (_webGLVersion > 1.f);

  _caps.textureFloat
    = (_webGLVersion > 1.f) || stl_util::contains(extensions, "GL_ARB_texture_float");
  _caps.textureFloatLinearFiltering = _caps.textureFloat;
  _caps.textureFloatRender          = _caps.textureFloat && _canRenderToFloatFramebuffer();

  _caps.textureHalfFloat                = (_webGLVersion > 1.f);
  _caps.textureHalfFloatLinearFiltering = (_webGLVersion > 1.f);
  if (_webGLVersion > 1) {
    _gl->HALF_FLOAT_OES = 0x140B;
  }
  _caps.textureHalfFloatRender = _caps.textureHalfFloat && _canRenderToHalfFloatFramebuffer();

  _caps.textureLOD = (_webGLVersion > 1.f);

  _caps.multiview = nullptr;

  // Draw buffers
  if (_webGLVersion > 1.f) {
    _caps.drawBuffersExtension = true;
  }

  // Shader compiler threads
  _caps.parallelShaderCompile = std::nullopt;

  // Depth Texture
  if (_webGLVersion > 1.f) {
    _caps.depthTextureExtension = true;
  }

  // Vertex array object
  if (disableVertexArrayObjects) {
    _caps.vertexArrayObject = false;
  }
  else if (_webGLVersion > 1.f) {
    _caps.vertexArrayObject = true;
  }

  // Instances count
  if (_webGLVersion > 1.f) {
    _caps.instancedArrays = true;
  }

  if (_webGLVersion > 1.f) {
    _caps.blendMinMax = true;
  }

  auto highp = _gl->getShaderPrecisionFormat(GL::FRAGMENT_SHADER, GL::HIGH_FLOAT);
  if (highp) {
    _caps.highPrecisionShaderSupported = highp ? highp->precision != 0 : false;
  }

  // Depth buffer
  _depthCullingState->depthTest = true;
  _depthCullingState->depthFunc = GL::LEQUAL;
  _depthCullingState->depthMask = true;

  // Texture maps
  _maxSimultaneousTextures = static_cast<unsigned>(_caps.maxCombinedTexturesImageUnits);
  for (unsigned int slot = 0; slot < _maxSimultaneousTextures; ++slot) {
    _nextFreeTextureSlots.emplace_back(slot);
  }

  // Create the core context VAO
#if !defined(__EMSCRIPTEN__)
  if (!_coreContextVAO) {
    _coreContextVAO = _gl->createVertexArray();
  }
#endif
}

float ThinEngine::get_webGLVersion() const
{
  return _webGLVersion;
}

std::string ThinEngine::getClassName() const
{
  return "ThinEngine";
}

bool ThinEngine::get_isStencilEnable() const
{
  return _isStencilEnable;
}

void ThinEngine::_prepareWorkingCanvas()
{
  if (_workingCanvas) {
    return;
  }

  _workingCanvas = _renderingCanvas;
  auto context   = _workingCanvas->getContext2d();

  if (context) {
    _workingContext = context;
  }
}

void ThinEngine::resetTextureCache()
{
  for (auto& boundTextureItem : _boundTexturesCache) {
    boundTextureItem.second = nullptr;
  }

  _currentTextureChannel = -1;
}

GL::GLInfo ThinEngine::getGlInfo()
{
  return {
    _glVendor,   // vendor
    _glRenderer, // renderer
    _glVersion   // version
  };
}

void ThinEngine::setHardwareScalingLevel(float level)
{
  _hardwareScalingLevel = level;
  resize();
}

float ThinEngine::getHardwareScalingLevel() const
{
  return _hardwareScalingLevel;
}

std::vector<InternalTexturePtr>& ThinEngine::getLoadedTexturesCache()
{
  return _internalTexturesCache;
}

EngineCapabilities& ThinEngine::getCaps()
{
  return _caps;
}

void ThinEngine::stopRenderLoop()
{
  _activeRenderLoops.clear();
}

void ThinEngine::stopRenderLoop(const SA::delegate<void()>& renderFunction)
{
  _activeRenderLoops.erase(
    std::remove(_activeRenderLoops.begin(), _activeRenderLoops.end(), renderFunction),
    _activeRenderLoops.end());
}

void ThinEngine::_renderLoop()
{
  if (!_contextWasLost) {
    auto shouldRender = true;
    if (!renderEvenInBackground && _windowIsBackground) {
      shouldRender = false;
    }

    if (shouldRender) {
      // Start new frame
      beginFrame();

      for (const auto& renderFunction : _activeRenderLoops) {
        renderFunction();
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

ICanvas* ThinEngine::getRenderingCanvas()
{
  return _renderingCanvas;
}

int ThinEngine::getRenderWidth(bool useScreen) const
{
  if (!useScreen && _currentRenderTarget) {
    return _currentRenderTarget->width;
  }

  return _gl->drawingBufferWidth;
}

int ThinEngine::getRenderHeight(bool useScreen) const
{
  if (!useScreen && _currentRenderTarget) {
    return _currentRenderTarget->height;
  }

  return _gl->drawingBufferHeight;
}

void ThinEngine::runRenderLoop(const std::function<void()>& renderFunction)
{
  if (std::find(_activeRenderLoops.begin(), _activeRenderLoops.end(), renderFunction)
      != _activeRenderLoops.end()) {
    return;
  }

  _activeRenderLoops.emplace_back(renderFunction);

  if (!_renderingQueueLaunched) {
    _renderingQueueLaunched = true;
  }
}

void ThinEngine::renderFunction(const std::function<void()>& renderFunction)
{
  if (!_contextWasLost) {
    auto shouldRender = true;
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
}

void ThinEngine::clear(const std::optional<Color4>& color, bool backBuffer, bool depth,
                       bool stencil)
{
  applyStates();

  auto mode = 0u;
  if (backBuffer && color) {
    _gl->clearColor(color->r, color->g, color->b, color->a);
    mode |= GL::COLOR_BUFFER_BIT;
  }
  if (depth) {
    if (useReverseDepthBuffer) {
      _depthCullingState->depthFunc = GL::GREATER;
      _gl->clearDepth(0.f);
    }
    else {
      _gl->clearDepth(1.f);
    }
    mode |= GL::DEPTH_BUFFER_BIT;
  }
  if (stencil) {
    _gl->clearStencil(0);
    mode |= GL::STENCIL_BUFFER_BIT;
  }
  _gl->clear(mode);
}

void ThinEngine::_viewport(float x, float y, float width, float height)
{
  if (!stl_util::almost_equal(x, _viewportCached.x) || !stl_util::almost_equal(y, _viewportCached.y)
      || !stl_util::almost_equal(width, _viewportCached.z)
      || !stl_util::almost_equal(height, _viewportCached.w)) {
    _viewportCached.x = x;
    _viewportCached.y = y;
    _viewportCached.z = width;
    _viewportCached.w = height;

    _gl->viewport(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width),
                  static_cast<int>(height));
  }
}

void ThinEngine::setViewport(const Viewport& viewport, const std::optional<int>& requiredWidth,
                             const std::optional<int>& requiredHeight)
{

  auto width  = requiredWidth.value_or(getRenderWidth());
  auto height = requiredHeight.value_or(getRenderHeight());
  auto x      = viewport.x;
  auto y      = viewport.y;

  _cachedViewport = viewport;

  _viewport(x * width, y * height, width * viewport.width, height * viewport.height);
}

void ThinEngine::beginFrame()
{
}

void ThinEngine::endFrame()
{
  // Force a flush in case we are using a bad OS.
  if (_badOS) {
    flushFramebuffer();
  }
}

void ThinEngine::resize()
{
  const auto width  = _renderingCanvas ? _renderingCanvas->clientWidth : 0;
  const auto height = _renderingCanvas ? _renderingCanvas->clientHeight : 0;

  setSize(static_cast<int>(width / _hardwareScalingLevel),
          static_cast<int>(height / _hardwareScalingLevel));
}

void ThinEngine::setSize(int width, int height)
{
  if (!_renderingCanvas) {
    return;
  }

  if (_renderingCanvas->width == width && _renderingCanvas->height == height) {
    return;
  }

  _renderingCanvas->width  = width;
  _renderingCanvas->height = height;
}

void ThinEngine::bindFramebuffer(const InternalTexturePtr& texture, unsigned int faceIndex,
                                 std::optional<int> requiredWidth,
                                 std::optional<int> requiredHeight,
                                 std::optional<bool> forceFullscreenViewport, int lodLevel,
                                 int layer)
{
  if (_currentRenderTarget) {
    unBindFramebuffer(_currentRenderTarget);
  }
  _currentRenderTarget = texture;
  _bindUnboundFramebuffer(texture->_MSAAFramebuffer ? texture->_MSAAFramebuffer :
                                                      texture->_framebuffer);

  auto& gl = *_gl;
  if (texture->is2DArray) {
    gl.framebufferTextureLayer(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0, texture->_webGLTexture.get(),
                               lodLevel, layer);
  }
  else if (texture->isCube) {
    gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0,
                            GL::TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex,
                            texture->_webGLTexture.get(), lodLevel);
  }

  const auto& depthStencilTexture = texture->_depthStencilTexture;
  if (depthStencilTexture) {
    const auto attachment = (depthStencilTexture->_generateStencilBuffer) ?
                              GL::DEPTH_STENCIL_ATTACHMENT :
                              GL::DEPTH_ATTACHMENT;
    if (texture->is2DArray) {
      gl.framebufferTextureLayer(GL::FRAMEBUFFER, attachment,
                                 depthStencilTexture->_webGLTexture.get(), lodLevel, layer);
    }
    else if (texture->isCube) {
      gl.framebufferTexture2D(GL::FRAMEBUFFER, attachment,
                              GL::TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex,
                              depthStencilTexture->_webGLTexture.get(), lodLevel);
    }
    else {
      gl.framebufferTexture2D(GL::FRAMEBUFFER, attachment, GL::TEXTURE_2D,
                              depthStencilTexture->_webGLTexture.get(), lodLevel);
    }
  }

  if (_cachedViewport && !forceFullscreenViewport && requiredWidth.has_value()
      && requiredHeight.has_value()) {
    setViewport(*_cachedViewport, *requiredWidth, *requiredHeight);
  }
  else {
    if (!requiredWidth) {
      requiredWidth = texture->width;
      if (lodLevel) {
        requiredWidth = static_cast<int>(*requiredWidth / std::pow(2, lodLevel));
      }
    }
    if (!requiredHeight) {
      requiredHeight = texture->height;
      if (lodLevel) {
        requiredHeight = static_cast<int>(*requiredHeight / std::pow(2, lodLevel));
      }
    }

    _viewport(0.f, 0.f, static_cast<float>(*requiredWidth), static_cast<float>(*requiredHeight));
  }

  wipeCaches();
}

WebGLFramebufferPtr ThinEngine::_getRealFrameBuffer(const WebGLFramebufferPtr& framebuffer)
{
  WebGLFramebufferPtr realFrameBuffer = nullptr;
  if (framebuffer) {
    realFrameBuffer      = framebuffer;
    auto previousId      = _gl->getParameteri(GL::FRAMEBUFFER_BINDING);
    _previousFrameBuffer = std::make_unique<GL::IGLFramebuffer>(previousId);
  }
  else {
    if (_previousFrameBuffer) {
      realFrameBuffer = std::move(_previousFrameBuffer);
      _previousFrameBuffer.reset();
    }
  }
  return realFrameBuffer;
}

void ThinEngine::_bindUnboundFramebuffer(const WebGLFramebufferPtr& framebuffer)
{
  if (_currentFramebuffer != framebuffer) {
    auto realFrameBuffer = _getRealFrameBuffer(framebuffer);
    _gl->bindFramebuffer(GL::FRAMEBUFFER, realFrameBuffer.get());
    _currentFramebuffer = framebuffer;
  }
}

void ThinEngine::unBindFramebuffer(const InternalTexturePtr& texture, bool disableGenerateMipMaps,
                                   const std::function<void()>& onBeforeUnbind)
{
  // If MSAA, we need to bitblt back to main texture
  auto& gl = *_gl;

  if (texture->_MSAAFramebuffer) {
    gl.bindFramebuffer(GL::READ_FRAMEBUFFER, texture->_MSAAFramebuffer.get());
    gl.bindFramebuffer(GL::DRAW_FRAMEBUFFER, texture->_framebuffer.get());
    gl.blitFramebuffer(0, 0, texture->width, texture->height, 0, 0, texture->width, texture->height,
                       GL::COLOR_BUFFER_BIT, GL::NEAREST);
  }

  if (texture->generateMipMaps && !disableGenerateMipMaps && !texture->isCube) {
    _bindTextureDirectly(GL::TEXTURE_2D, texture, true);
    gl.generateMipmap(GL::TEXTURE_2D);
    _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
  }

  if (onBeforeUnbind) {
    if (texture->_MSAAFramebuffer) {
      // Bind the correct framebuffer
      _bindUnboundFramebuffer(texture->_framebuffer);
    }
    onBeforeUnbind();
  }

  _currentRenderTarget = nullptr;

  _bindUnboundFramebuffer(nullptr);
}

void ThinEngine::flushFramebuffer()
{
  _gl->flush();
}

void ThinEngine::restoreDefaultFramebuffer()
{
  if (_currentRenderTarget) {
    unBindFramebuffer(_currentRenderTarget);
  }
  else {
    _bindUnboundFramebuffer(nullptr);
  }

  if (_cachedViewport) {
    setViewport(*_cachedViewport);
  }

  wipeCaches();
}

void ThinEngine::_resetVertexBufferBinding()
{
  bindArrayBuffer(nullptr);
  _cachedVertexBuffersMap.clear();
  _cachedVertexBuffers = nullptr;
}

WebGLDataBufferPtr ThinEngine::createVertexBuffer(const Float32Array& data)
{
  return _createVertexBuffer(data, GL::STATIC_DRAW);
}

WebGLDataBufferPtr ThinEngine::_createVertexBuffer(const Float32Array& data, unsigned int usage)
{
  auto vbo = _gl->createBuffer();

  if (!vbo) {
    throw std::runtime_error("Unable to create vertex buffer");
  }

  auto dataBuffer = std::make_shared<WebGLDataBuffer>(vbo);
  bindArrayBuffer(dataBuffer);

  _gl->bufferData(GL::ARRAY_BUFFER, data, usage);

  _resetVertexBufferBinding();

  dataBuffer->references = 1;
  return dataBuffer;
}

WebGLDataBufferPtr ThinEngine::createDynamicVertexBuffer(const Float32Array& data)
{
  return _createVertexBuffer(data, GL::DYNAMIC_DRAW);
}

void ThinEngine::_resetIndexBufferBinding()
{
  bindIndexBuffer(nullptr);
  _cachedIndexBuffer = nullptr;
}

WebGLDataBufferPtr ThinEngine::createIndexBuffer(const IndicesArray& indices, bool updatable)
{
  auto vbo        = _gl->createBuffer();
  auto dataBuffer = std::make_shared<WebGLDataBuffer>(vbo);

  if (!vbo) {
    throw std::runtime_error("Unable to create index buffer");
  }

  bindIndexBuffer(dataBuffer);

  Uint16Array uint16ArrayResult;
  Uint32Array uint32ArrayResult;
  _normalizeIndexData(indices, uint16ArrayResult, uint32ArrayResult);
  if (!uint16ArrayResult.empty()) {
    _gl->bufferData(GL::ELEMENT_ARRAY_BUFFER, uint16ArrayResult,
                    updatable ? GL::DYNAMIC_DRAW : GL::STATIC_DRAW);
  }
  else {
    _gl->bufferData(GL::ELEMENT_ARRAY_BUFFER, uint32ArrayResult,
                    updatable ? GL::DYNAMIC_DRAW : GL::STATIC_DRAW);
  }
  _resetIndexBufferBinding();
  dataBuffer->references = 1;
  dataBuffer->is32Bits   = !uint32ArrayResult.empty();
  return dataBuffer;
}

void ThinEngine::_normalizeIndexData(const IndicesArray& indices, Uint16Array& uint16ArrayResult,
                                     Uint32Array& uint32ArrayResult)
{
  // Check 32 bit support
  if (_caps.uintIndices) {
    // number[] or Int32Array, check if 32 bit is necessary
    using namespace std::placeholders;
    auto it = std::find_if(indices.begin(), indices.end(),
                           std::bind(std::greater<uint32_t>(), _1, 65535));
    if (it != indices.end()) {
      // 32 bit is necessary
      uint32ArrayResult.assign(indices.begin(), indices.end());
    }
    else {
      // 16 bit is sufficient
      for (auto v : indices) {
        uint16ArrayResult.push_back(static_cast<uint16_t>(v));
      }
    }
  }
}

void ThinEngine::bindArrayBuffer(const WebGLDataBufferPtr& buffer)
{
  if (!_vaoRecordInProgress) {
    _unbindVertexArrayObject();
  }
  bindBuffer(buffer, GL::ARRAY_BUFFER);
}

void ThinEngine::bindIndexBuffer(const WebGLDataBufferPtr& buffer)
{
  if (!_vaoRecordInProgress) {
    _unbindVertexArrayObject();
  }
  bindBuffer(buffer, GL::ELEMENT_ARRAY_BUFFER);
}

void ThinEngine::bindBuffer(const WebGLDataBufferPtr& buffer, int target)
{
  if (_vaoRecordInProgress || (_currentBoundBuffer.find(target) == _currentBoundBuffer.end())
      || (_currentBoundBuffer[target] != buffer)) {
    _gl->bindBuffer(static_cast<unsigned int>(target),
                    buffer ? buffer->underlyingResource().get() : nullptr);
    _currentBoundBuffer[target] = buffer;
  }
}

void ThinEngine::updateArrayBuffer(const Float32Array& data)
{
  _gl->bufferSubData(GL::ARRAY_BUFFER, 0, data);
}

void ThinEngine::_vertexAttribPointer(const WebGLDataBufferPtr& buffer, unsigned int indx, int size,
                                      unsigned int type, bool normalized, int stride, int offset)
{
  auto& pointer = _currentBufferPointers[indx];

  auto changed = false;
  if (!pointer.active) {
    changed            = true;
    pointer.active     = true;
    pointer.index      = indx;
    pointer.size       = size;
    pointer.type       = type;
    pointer.normalized = normalized;
    pointer.stride     = stride;
    pointer.offset     = offset;
    pointer.buffer     = buffer;
  }
  else {
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

void ThinEngine::_bindIndexBufferWithCache(const WebGLDataBufferPtr& indexBuffer)
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

void ThinEngine::_bindVertexBuffersAttributes(
  const std::unordered_map<std::string, VertexBufferPtr>& vertexBuffers, const EffectPtr& effect)
{
  auto attributes = effect->getAttributesNames();

  if (!_vaoRecordInProgress) {
    _unbindVertexArrayObject();
  }

  unbindAllAttributes();

  auto _order = 0u;
  for (unsigned int index = 0; index < attributes.size(); ++index) {
    auto order = effect->getAttributeLocation(index);

    if (order >= 0) {
      _order = static_cast<unsigned int>(order);

      if (!stl_util::contains(vertexBuffers, attributes[index])) {
        continue;
      }

      const auto& vertexBuffer = vertexBuffers.at(attributes[index]);
      if (!vertexBuffer) {
        continue;
      }

      _gl->enableVertexAttribArray(_order);
      if (!_vaoRecordInProgress) {
        _vertexAttribArraysEnabled[_order] = true;
      }

      auto buffer = vertexBuffer->getBuffer();
      if (buffer) {
        _vertexAttribPointer(buffer, _order, static_cast<int>(vertexBuffer->getSize()),
                             vertexBuffer->type, vertexBuffer->normalized,
                             static_cast<int>(vertexBuffer->byteStride),
                             static_cast<int>(vertexBuffer->byteOffset));

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

WebGLVertexArrayObjectPtr ThinEngine::recordVertexArrayObject(
  const std::unordered_map<std::string, VertexBufferPtr>& vertexBuffers,
  const WebGLDataBufferPtr& indexBuffer, const EffectPtr& effect)
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

void ThinEngine::bindVertexArrayObject(const WebGLVertexArrayObjectPtr& vertexArrayObject,
                                       const WebGLDataBufferPtr& indexBuffer)
{
  if (_cachedVertexArrayObject != vertexArrayObject) {
    _cachedVertexArrayObject = vertexArrayObject;

    _gl->bindVertexArray(vertexArrayObject.get());
    _cachedVertexBuffersMap.clear();
    _cachedVertexBuffers = nullptr;
    _cachedIndexBuffer   = nullptr;

    _uintIndicesCurrentlySet  = indexBuffer != nullptr && indexBuffer->is32Bits;
    _mustWipeVertexAttributes = true;
  }
}

void ThinEngine::bindBuffersDirectly(const WebGLDataBufferPtr& vertexBuffer,
                                     const WebGLDataBufferPtr& indexBuffer,
                                     const Float32Array& vertexDeclaration, int vertexStrideSize,
                                     const EffectPtr& effect)
{
  if (_cachedVertexBuffers != vertexBuffer || _cachedEffectForVertexBuffers != effect) {
    _cachedVertexBuffers          = vertexBuffer;
    _cachedEffectForVertexBuffers = effect;

    auto attributesCount = effect->getAttributesCount();

    _unbindVertexArrayObject();
    unbindAllAttributes();

    auto offset = 0;
    for (unsigned int index = 0; index < attributesCount; ++index) {

      if (index < vertexDeclaration.size()) {

        auto vertexDeclarationi = static_cast<int>(vertexDeclaration[index]);
        auto order              = effect->getAttributeLocation(index);

        if (order >= 0) {
          auto _order = static_cast<unsigned int>(order);
          _gl->enableVertexAttribArray(_order);
          _vertexAttribArraysEnabled[_order] = true;
          _vertexAttribPointer(vertexBuffer, _order, vertexDeclarationi, GL::FLOAT, false,
                               vertexStrideSize, offset);
        }

        offset += vertexDeclarationi * 4;
      }
    }
  }

  _bindIndexBufferWithCache(indexBuffer);
}

void ThinEngine::_unbindVertexArrayObject()
{
  if (!_cachedVertexArrayObject) {
    return;
  }

  _cachedVertexArrayObject = nullptr;
  _gl->bindVertexArray(nullptr);

  // Make sure that the core context VAO is bound
#if !defined(__EMSCRIPTEN__)
  _gl->bindVertexArray(_coreContextVAO.get());
#endif
}

void ThinEngine::bindBuffers(const std::unordered_map<std::string, VertexBufferPtr>& vertexBuffers,
                             const WebGLDataBufferPtr& indexBuffer, const EffectPtr& effect)
{
  if (_cachedVertexBuffersMap != vertexBuffers || _cachedEffectForVertexBuffers != effect) {
    _cachedVertexBuffersMap       = vertexBuffers;
    _cachedEffectForVertexBuffers = effect;

    _bindVertexBuffersAttributes(vertexBuffers, effect);
  }

  _bindIndexBufferWithCache(indexBuffer);
}

void ThinEngine::unbindInstanceAttributes()
{
  WebGLDataBufferPtr boundBuffer = nullptr;
  for (size_t i = 0, ul = _currentInstanceLocations.size(); i < ul; ++i) {
    const auto& instancesBuffer = _currentInstanceBuffers[i];
    if (boundBuffer != instancesBuffer && instancesBuffer->references) {
      boundBuffer = instancesBuffer;
      bindArrayBuffer(instancesBuffer);
    }
    const auto offsetLocation = static_cast<unsigned int>(_currentInstanceLocations[i]);
    _gl->vertexAttribDivisor(offsetLocation, 0);
  }
  _currentInstanceBuffers.clear();
  _currentInstanceLocations.clear();
}

void ThinEngine::releaseVertexArrayObject(const WebGLVertexArrayObjectPtr& vao)
{
  _gl->deleteVertexArray(vao.get());
}

bool ThinEngine::_releaseBuffer(const WebGLDataBufferPtr& buffer)
{
  buffer->references--;

  if (buffer->references == 0) {
    _deleteBuffer(buffer);
    return true;
  }

  return false;
}

void ThinEngine::_deleteBuffer(const WebGLDataBufferPtr& buffer)
{
  _gl->deleteBuffer(buffer->underlyingResource().get());
}

void ThinEngine::updateAndBindInstancesBuffer(
  const WebGLDataBufferPtr& instancesBuffer, const Float32Array& data,
  std::variant<Uint32Array, std::vector<InstancingAttributeInfo>>& offsetLocations)
{
  bindArrayBuffer(instancesBuffer);
  if (!data.empty()) {
    _gl->bufferSubData(GL::ARRAY_BUFFER, 0, data);
  }

  if (std::holds_alternative<std::vector<InstancingAttributeInfo>>(offsetLocations)) {
    bindInstancesBuffer(instancesBuffer,
                        std::get<std::vector<InstancingAttributeInfo>>(offsetLocations), true);
  }
  else if (std::holds_alternative<Uint32Array>(offsetLocations)) {
    const auto& _offsetLocations = std::get<Uint32Array>(offsetLocations);
    for (unsigned int index = 0; index < 4; index++) {
      const auto offsetLocation = _offsetLocations[index];

      if (!stl_util::contains(_vertexAttribArraysEnabled, offsetLocation)
          || !_vertexAttribArraysEnabled[offsetLocation]) {
        _gl->enableVertexAttribArray(offsetLocation);
        _vertexAttribArraysEnabled[offsetLocation] = true;
      }

      _vertexAttribPointer(instancesBuffer, offsetLocation, 4, GL::FLOAT, false, 64,
                           static_cast<int>(index) * 16);
      _gl->vertexAttribDivisor(offsetLocation, 1);
      _currentInstanceLocations.emplace_back(offsetLocation);
      _currentInstanceBuffers.emplace_back(instancesBuffer);
    }
  }
}

void ThinEngine::bindInstancesBuffer(const WebGLDataBufferPtr& instancesBuffer,
                                     std::vector<InstancingAttributeInfo>& attributesInfo,
                                     bool computeStride)
{
  bindArrayBuffer(instancesBuffer);

  auto stride = 0;
  if (computeStride) {
    for (const auto& ai : attributesInfo) {
      stride += ai.attributeSize * 4;
    }
  }

  for (auto& ai : attributesInfo) {
    if (!ai.index.has_value()) {
      ai.index = _currentEffect->getAttributeLocationByName(ai.attributeName);
    }

    if (!stl_util::contains(_vertexAttribArraysEnabled, *ai.index)
        || !_vertexAttribArraysEnabled[*ai.index]) {
      _gl->enableVertexAttribArray(*ai.index);
      _vertexAttribArraysEnabled[*ai.index] = true;
    }

    _vertexAttribPointer(instancesBuffer, *ai.index, ai.attributeSize,
                         ai.attributeType.value_or(GL::FLOAT), ai.normalized.value_or(false),
                         stride, ai.offset);
    _gl->vertexAttribDivisor(*ai.index, ai.divisor.value_or(1));
    _currentInstanceLocations.emplace_back(*ai.index);
    _currentInstanceBuffers.emplace_back(instancesBuffer);
  }
}

void ThinEngine::disableInstanceAttributeByName(const std::string& name)
{
  if (!_currentEffect) {
    return;
  }

  const auto attributeLocation = _currentEffect->getAttributeLocationByName(name);
  if (attributeLocation >= 0) {
    disableInstanceAttribute(static_cast<unsigned int>(attributeLocation));
  }
}

void ThinEngine::disableInstanceAttribute(unsigned int attributeLocation)
{
  auto shouldClean        = false;
  auto index              = 0;
  auto _attributeLocation = static_cast<int>(attributeLocation);
  while ((index = stl_util::index_of(_currentInstanceLocations, _attributeLocation)) != -1) {
    stl_util::splice(_currentInstanceLocations, index, 1);
    stl_util::splice(_currentInstanceBuffers, index, 1);

    shouldClean = true;
    index       = stl_util::index_of(_currentInstanceLocations, _attributeLocation);
  }

  if (shouldClean) {
    _gl->vertexAttribDivisor(attributeLocation, 0);
    disableAttributeByIndex(attributeLocation);
  }
}

void ThinEngine::disableAttributeByIndex(unsigned int attributeLocation)
{
  _gl->disableVertexAttribArray(attributeLocation);
  _vertexAttribArraysEnabled[attributeLocation]    = false;
  _currentBufferPointers[attributeLocation].active = false;
}

void ThinEngine::draw(bool useTriangles, int indexStart, int indexCount, int instancesCount)
{
  drawElementsType(useTriangles ? Constants::MATERIAL_TriangleFillMode :
                                  Constants::MATERIAL_WireFrameFillMode,
                   indexStart, indexCount, instancesCount);
}

void ThinEngine::drawPointClouds(int verticesStart, int verticesCount, int instancesCount)
{
  drawArraysType(Constants::MATERIAL_PointFillMode, verticesStart, verticesCount, instancesCount);
}

void ThinEngine::drawUnIndexed(bool useTriangles, int verticesStart, int verticesCount,
                               int instancesCount)
{
  drawArraysType(useTriangles ? Constants::MATERIAL_TriangleFillMode :
                                Constants::MATERIAL_WireFrameFillMode,
                 verticesStart, verticesCount, instancesCount);
}

void ThinEngine::drawElementsType(unsigned int fillMode, int indexStart, int indexCount,
                                  int instancesCount)
{
  // Apply states
  applyStates();

  _reportDrawCall();

  // Render
  const auto drawMode = _drawMode(fillMode);
  auto indexFormat    = _uintIndicesCurrentlySet ? GL::UNSIGNED_INT : GL::UNSIGNED_SHORT;
  auto mult           = _uintIndicesCurrentlySet ? 4 : 2;
  if (instancesCount) {
    _gl->drawElementsInstanced(drawMode, indexCount, indexFormat, indexStart * mult,
                               instancesCount);
  }
  else {
    _gl->drawElements(drawMode, indexCount, indexFormat, indexStart * mult);
  }
}

void ThinEngine::drawArraysType(unsigned int fillMode, int verticesStart, int verticesCount,
                                int instancesCount)
{
  // Apply states
  applyStates();

  _reportDrawCall();

  // Render
  const auto drawMode = _drawMode(fillMode);
  if (instancesCount) {
    _gl->drawArraysInstanced(drawMode, verticesStart, verticesCount, instancesCount);
  }
  else {
    _gl->drawArrays(drawMode, verticesStart, verticesCount);
  }
}

unsigned int ThinEngine::_drawMode(unsigned int fillMode) const
{
  switch (fillMode) {
    // Triangle views
    case Constants::MATERIAL_TriangleFillMode:
      return GL::TRIANGLES;
    case Constants::MATERIAL_PointFillMode:
      return GL::POINTS;
    case Constants::MATERIAL_WireFrameFillMode:
      return GL::LINES;
    // Draw modes
    case Constants::MATERIAL_PointListDrawMode:
      return GL::POINTS;
    case Constants::MATERIAL_LineListDrawMode:
      return GL::LINES;
    case Constants::MATERIAL_LineLoopDrawMode:
      return GL::LINE_LOOP;
    case Constants::MATERIAL_LineStripDrawMode:
      return GL::LINE_STRIP;
    case Constants::MATERIAL_TriangleStripDrawMode:
      return GL::TRIANGLE_STRIP;
    case Constants::MATERIAL_TriangleFanDrawMode:
      return GL::TRIANGLE_FAN;
    default:
      return GL::TRIANGLES;
  }
}

void ThinEngine::_reportDrawCall()
{
  // Will be implemented by children
}

void ThinEngine::_releaseEffect(Effect* effect)
{
  if (stl_util::contains(_compiledEffects, effect->_key)) {
    _compiledEffects.erase(effect->_key);

    _deletePipelineContext(
      std::static_pointer_cast<WebGLPipelineContext>(effect->getPipelineContext()));
  }
}

void ThinEngine::_deletePipelineContext(const IPipelineContextPtr& pipelineContext)
{
  auto webGLPipelineContext = std::static_pointer_cast<WebGLPipelineContext>(pipelineContext);
  if (webGLPipelineContext && webGLPipelineContext->program) {
    webGLPipelineContext->program->__SPECTOR_rebuildProgram = nullptr;

    _gl->deleteProgram(webGLPipelineContext->program.get());
  }
}

EffectPtr ThinEngine::createEffect(
  const std::variant<std::string, std::unordered_map<std::string, std::string>>& baseName,
  IEffectCreationOptions& options, ThinEngine* engine,
  const std::function<void(const EffectPtr& effect)>& onCompiled)
{
  std::string name;
  if (std::holds_alternative<std::string>(baseName)) {
    auto _baseName = std::get<std::string>(baseName);
    name           = _baseName + "+" + _baseName + "@" + options.defines;
  }
  else if (std::holds_alternative<std::unordered_map<std::string, std::string>>(baseName)) {
    auto _baseName = std::get<std::unordered_map<std::string, std::string>>(baseName);
    auto vertex    = stl_util::contains(_baseName, "vertexElement") ?
                    _baseName["vertexElement"] :
                    stl_util::contains(_baseName, "vertex") ? _baseName["vertex"] : "vertex";
    auto fragment
      = stl_util::contains(_baseName, "fragmentElement") ?
          _baseName["fragmentElement"] :
          stl_util::contains(_baseName, "fragment") ? _baseName["fragment"] : "fragment";
    name = vertex + "+" + fragment + "@" + options.defines;
  }

  if (stl_util::contains(_compiledEffects, name)) {
    auto compiledEffect = _compiledEffects[name];
    if (onCompiled && compiledEffect->isReady()) {
      onCompiled(compiledEffect);
    }
    return compiledEffect;
  }
  auto effect            = Effect::New(baseName, options, engine);
  effect->_key           = name;
  _compiledEffects[name] = effect;

  return effect;
}

std::string ThinEngine::_ConcatenateShader(const std::string& source, const std::string& defines,
                                           const std::string& shaderVersion)
{
  return shaderVersion + ((!defines.empty()) ? defines + "\n" : "") + source;
}

WebGLShaderPtr ThinEngine::_compileShader(const std::string& source, const std::string& type,
                                          const std::string& defines,
                                          const std::string& shaderVersion)
{
  return _compileRawShader(ThinEngine::_ConcatenateShader(source, defines, shaderVersion), type);
}

WebGLShaderPtr ThinEngine::_compileRawShader(const std::string& source, const std::string& type)
{
  auto& gl    = *_gl;
  auto shader = gl.createShader(type == "vertex" ? GL::VERTEX_SHADER : GL::FRAGMENT_SHADER);

  if (!shader) {
    throw std::runtime_error("Something went wrong while compile the shader.");
  }

  gl.shaderSource(shader.get(), source);
  gl.compileShader(shader.get());

  return shader;
}

WebGLProgramPtr
ThinEngine::createRawShaderProgram(const IPipelineContextPtr& pipelineContext,
                                   const std::string& vertexCode, const std::string& fragmentCode,
                                   WebGLRenderingContext* context,
                                   const std::vector<std::string>& transformFeedbackVaryings)
{
  context = context ? context : _gl;

  auto vertexShader   = _compileRawShader(vertexCode, "vertex");
  auto fragmentShader = _compileRawShader(fragmentCode, "fragment");

  return _createShaderProgram(std::static_pointer_cast<WebGLPipelineContext>(pipelineContext),
                              vertexShader, fragmentShader, context, transformFeedbackVaryings);
}

WebGLProgramPtr
ThinEngine::createShaderProgram(const IPipelineContextPtr& pipelineContext,
                                const std::string& vertexCode, const std::string& fragmentCode,
                                const std::string& defines, WebGLRenderingContext* context,
                                const std::vector<std::string>& transformFeedbackVaryings)
{
  context = context ? context : _gl;
#if defined(__EMSCRIPTEN__)
  auto shaderVersion = (_webGLVersion > 1.f) ? "#version 300 es\n#define WEBGL2 \n" : "";
#elif defined(GLES3)
  auto shaderVersion = (_webGLVersion > 1.f) ? "#version 300 es\n#define WEBGL2 \n" : "";
#else
  auto shaderVersion = (_webGLVersion > 1.f) ? "#version 330\n#define WEBGL2 \n" : "";
#endif
  auto vertexShader   = _compileShader(vertexCode, "vertex", defines, shaderVersion);
  auto fragmentShader = _compileShader(fragmentCode, "fragment", defines, shaderVersion);

  return _createShaderProgram(std::static_pointer_cast<WebGLPipelineContext>(pipelineContext),
                              vertexShader, fragmentShader, context, transformFeedbackVaryings);
}

IPipelineContextPtr ThinEngine::createPipelineContext()
{
  auto pipelineContext    = std::make_shared<WebGLPipelineContext>();
  pipelineContext->engine = this;

  if (_caps.parallelShaderCompile) {
    pipelineContext->isParallelCompiled = true;
  }

  return std::static_pointer_cast<IPipelineContext>(pipelineContext);
}

WebGLProgramPtr ThinEngine::_createShaderProgram(
  const WebGLPipelineContextPtr& pipelineContext, const WebGLShaderPtr& vertexShader,
  const WebGLShaderPtr& fragmentShader, WebGLRenderingContext* context,
  const std::vector<std::string>& /*transformFeedbackVaryings*/)
{
  auto shaderProgram       = context->createProgram();
  pipelineContext->program = shaderProgram;

  if (!shaderProgram) {
    throw std::runtime_error("Unable to create program");
  }

  context->attachShader(shaderProgram.get(), vertexShader.get());
  context->attachShader(shaderProgram.get(), fragmentShader.get());

  context->linkProgram(shaderProgram.get());

  pipelineContext->context        = context;
  pipelineContext->vertexShader   = vertexShader;
  pipelineContext->fragmentShader = fragmentShader;

  if (!pipelineContext->isParallelCompiled) {
    _finalizePipelineContext(pipelineContext.get());
  }

  return shaderProgram;
}

void ThinEngine::_finalizePipelineContext(WebGLPipelineContext* pipelineContext)
{
  const auto& context        = pipelineContext->context;
  const auto& vertexShader   = pipelineContext->vertexShader;
  const auto& fragmentShader = pipelineContext->fragmentShader;
  const auto& program        = pipelineContext->program;

  auto linked = context->getProgramParameter(program.get(), GL::LINK_STATUS);
  if (!linked) { // Get more info
    // Vertex
    if (!_gl->getShaderParameter(vertexShader.get(), GL::COMPILE_STATUS)) {
      auto log = _gl->getShaderInfoLog(vertexShader.get());
      if (!log.empty()) {
        pipelineContext->vertexCompilationError = log;
        throw std::runtime_error("VERTEX SHADER " + log);
      }
    }

    // Fragment
    if (!_gl->getShaderParameter(fragmentShader.get(), GL::COMPILE_STATUS)) {
      auto log = _gl->getShaderInfoLog(fragmentShader.get());
      if (!log.empty()) {
        pipelineContext->fragmentCompilationError = log;
        throw std::runtime_error("FRAGMENT SHADER " + log);
      }
    }

    auto error = context->getProgramInfoLog(program.get());
    if (!error.empty()) {
      pipelineContext->programLinkError = error;
      throw std::runtime_error(error);
    }
  }

  if (validateShaderPrograms) {
    context->validateProgram(program.get());
    auto validated = context->getProgramParameter(program.get(), GL::VALIDATE_STATUS);

    if (!validated) {
      auto error = context->getProgramInfoLog(program.get());
      if (!error.empty()) {
        pipelineContext->programValidationError = error;
        throw std::runtime_error(error);
      }
    }
  }

  context->deleteShader(vertexShader.get());
  context->deleteShader(fragmentShader.get());

  pipelineContext->vertexShader   = nullptr;
  pipelineContext->fragmentShader = nullptr;

  if (pipelineContext->onCompiled) {
    pipelineContext->onCompiled();
    pipelineContext->onCompiled = nullptr;
  }
}

void ThinEngine::_preparePipelineContext(const IPipelineContextPtr& pipelineContext,
                                         const std::string& vertexSourceCode,
                                         const std::string& fragmentSourceCode, bool createAsRaw,
                                         bool /*rebuildRebind*/, const std::string& defines,
                                         const std::vector<std::string>& transformFeedbackVaryings)
{
  auto webGLRenderingState = std::static_pointer_cast<WebGLPipelineContext>(pipelineContext);

  if (createAsRaw) {
    webGLRenderingState->program
      = createRawShaderProgram(webGLRenderingState, vertexSourceCode, fragmentSourceCode, nullptr,
                               transformFeedbackVaryings);
  }
  else {
    webGLRenderingState->program
      = createShaderProgram(webGLRenderingState, vertexSourceCode, fragmentSourceCode, defines,
                            nullptr, transformFeedbackVaryings);
  }
  webGLRenderingState->program->__SPECTOR_rebuildProgram = nullptr; // rebuildRebind;
}

bool ThinEngine::_isRenderingStateCompiled(IPipelineContext* pipelineContext)
{
  auto webGLPipelineContext = static_cast<WebGLPipelineContext*>(pipelineContext);
  if (_gl->getProgramParameter(webGLPipelineContext->program.get(),
                               _caps.parallelShaderCompile->COMPLETION_STATUS_KHR)) {
    _finalizePipelineContext(webGLPipelineContext);
    return true;
  }

  return false;
}

void ThinEngine::_executeWhenRenderingStateIsCompiled(const IPipelineContextPtr& pipelineContext,
                                                      const std::function<void()>& action)
{
  auto webGLPipelineContext = std::static_pointer_cast<WebGLPipelineContext>(pipelineContext);

  if (!webGLPipelineContext->isParallelCompiled) {
    action();
    return;
  }

  auto oldHandler = webGLPipelineContext->onCompiled;

  if (oldHandler) {
    webGLPipelineContext->onCompiled = [=]() -> void {
      oldHandler();
      action();
    };
  }
  else {
    webGLPipelineContext->onCompiled = action;
  }
}

std::unordered_map<std::string, WebGLUniformLocationPtr>
ThinEngine::getUniforms(const IPipelineContextPtr& pipelineContext,
                        const std::vector<std::string>& uniformsNames)
{
  std::unordered_map<std::string, WebGLUniformLocationPtr> results;
  auto webGLPipelineContext = std::static_pointer_cast<WebGLPipelineContext>(pipelineContext);

  for (const auto& uniformsName : uniformsNames) {
    results[uniformsName]
      = _gl->getUniformLocation(webGLPipelineContext->program.get(), uniformsName);
  }

  return results;
}

Int32Array ThinEngine::getAttributes(const IPipelineContextPtr& pipelineContext,
                                     const std::vector<std::string>& attributesNames)
{
  Int32Array results;
  auto webGLPipelineContext = std::static_pointer_cast<WebGLPipelineContext>(pipelineContext);

  for (const auto& attributesName : attributesNames) {
    try {
      results.emplace_back(
        _gl->getAttribLocation(webGLPipelineContext->program.get(), attributesName));
    }
    catch (...) {
      results.emplace_back(-1);
    }
  }

  return results;
}

void ThinEngine::enableEffect(const EffectPtr& effect)
{
  if (!effect || effect == _currentEffect) {
    return;
  }

  // Use program
  bindSamplers(*effect);

  _currentEffect = effect;

  if (effect->onBind) {
    effect->onBind(effect.get());
  }
  effect->onBindObservable().notifyObservers(effect.get());
}

void ThinEngine::setInt(const WebGLUniformLocationPtr& uniform, int value)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1i(uniform.get(), value);
}

void ThinEngine::setIntArray(const WebGLUniformLocationPtr& uniform, const Int32Array& array)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1iv(uniform.get(), array);
}

void ThinEngine::setIntArray2(const WebGLUniformLocationPtr& uniform, const Int32Array& array)
{
  if (!uniform || array.size() % 2 != 0) {
    return;
  }

  _gl->uniform2iv(uniform.get(), array);
}

void ThinEngine::setIntArray3(const WebGLUniformLocationPtr& uniform, const Int32Array& array)
{
  if (!uniform || array.size() % 3 != 0) {
    return;
  }

  _gl->uniform3iv(uniform.get(), array);
}

void ThinEngine::setIntArray4(const WebGLUniformLocationPtr& uniform, const Int32Array& array)
{
  if (!uniform || array.size() % 4 != 0) {
    return;
  }

  _gl->uniform4iv(uniform.get(), array);
}

void ThinEngine::setArray(const WebGLUniformLocationPtr& uniform, const Float32Array& array)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1fv(uniform.get(), array);
}

void ThinEngine::setArray2(const WebGLUniformLocationPtr& uniform, const Float32Array& array)
{
  if (!uniform || array.size() % 2 != 0) {
    return;
  }

  _gl->uniform2fv(uniform.get(), array);
}

void ThinEngine::setArray3(const WebGLUniformLocationPtr& uniform, const Float32Array& array)
{
  if (!uniform || array.size() % 3 != 0) {
    return;
  }

  _gl->uniform3fv(uniform.get(), array);
}

void ThinEngine::setArray4(const WebGLUniformLocationPtr& uniform, const Float32Array& array)
{
  if (!uniform || array.size() % 4 != 0) {
    return;
  }

  _gl->uniform4fv(uniform.get(), array);
}

void ThinEngine::setMatrices(const WebGLUniformLocationPtr& uniform, const Float32Array& matrices)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix4fv(uniform.get(), false, matrices);
}

void ThinEngine::setMatrix3x3(const WebGLUniformLocationPtr& uniform, const Float32Array& matrix)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix3fv(uniform.get(), false, matrix);
}

void ThinEngine::setMatrix2x2(const WebGLUniformLocationPtr& uniform, const Float32Array& matrix)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix2fv(uniform.get(), false, matrix);
}

void ThinEngine::setFloat(const WebGLUniformLocationPtr& uniform, float value)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1f(uniform.get(), value);
}

void ThinEngine::setFloat2(const WebGLUniformLocationPtr& uniform, float x, float y)
{
  if (!uniform) {
    return;
  }

  _gl->uniform2f(uniform.get(), x, y);
}

void ThinEngine::setFloat3(const WebGLUniformLocationPtr& uniform, float x, float y, float z)
{
  if (!uniform) {
    return;
  }

  _gl->uniform3f(uniform.get(), x, y, z);
}

void ThinEngine::setFloat4(const WebGLUniformLocationPtr& uniform, float x, float y, float z,
                           float w)
{
  if (!uniform) {
    return;
  }

  _gl->uniform4f(uniform.get(), x, y, z, w);
}

void ThinEngine::applyStates()
{
  _depthCullingState->apply(*_gl);
  _stencilState->apply(*_gl);
  _alphaState->apply(*_gl);

  if (_colorWriteChanged) {
    _colorWriteChanged = false;
    const auto enable  = _colorWrite;
    _gl->colorMask(enable, enable, enable, enable);
  }
}

void ThinEngine::setColorWrite(bool enable)
{
  if (enable != _colorWrite) {
    _colorWriteChanged = true;
    _colorWrite        = enable;
  }
}

bool ThinEngine::getColorWrite() const
{
  return _colorWrite;
}

std::unique_ptr<DepthCullingState>& ThinEngine::get_depthCullingState()
{
  return _depthCullingState;
}

std::unique_ptr<AlphaState>& ThinEngine::get_alphaState()
{
  return _alphaState;
}

std::unique_ptr<StencilState>& ThinEngine::get_stencilState()
{
  return _stencilState;
}

void ThinEngine::clearInternalTexturesCache()
{
  _internalTexturesCache = {};
}

void ThinEngine::wipeCaches(bool bruteForce)
{
  if (preventCacheWipeBetweenFrames && !bruteForce) {
    return;
  }
  _currentEffect    = nullptr;
  _viewportCached.x = 0;
  _viewportCached.y = 0;
  _viewportCached.z = 0;
  _viewportCached.w = 0;

  // Done before in case we clean the attributes
  _unbindVertexArrayObject();

  if (bruteForce) {
    _currentProgram = nullptr;
    resetTextureCache();

    _stencilState.reset();

    _depthCullingState.reset();
    _depthCullingState->depthFunc = GL::LEQUAL;

    _alphaState.reset();
    _alphaMode     = Constants::ALPHA_ADD;
    _alphaEquation = Constants::ALPHA_DISABLE;

    _colorWrite        = true;
    _colorWriteChanged = true;

    _unpackFlipYCached = std::nullopt;

    _gl->pixelStorei(GL::UNPACK_COLORSPACE_CONVERSION_WEBGL, GL::NONE);
    _gl->pixelStorei(GL::UNPACK_PREMULTIPLY_ALPHA_WEBGL, 0);

    _mustWipeVertexAttributes = true;
    unbindAllAttributes();
  }

  _resetVertexBufferBinding();
  _cachedIndexBuffer            = nullptr;
  _cachedEffectForVertexBuffers = nullptr;
  bindIndexBuffer(nullptr);
}

SamplingParameters ThinEngine::_getSamplingParameters(unsigned int samplingMode,
                                                      bool generateMipMaps)
{
  GL::GLenum magFilter = GL::NEAREST;
  GL::GLenum minFilter = GL::NEAREST;

  switch (samplingMode) {
    case Constants::TEXTURE_LINEAR_LINEAR_MIPNEAREST:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_NEAREST;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case Constants::TEXTURE_LINEAR_LINEAR_MIPLINEAR:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_LINEAR;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case Constants::TEXTURE_NEAREST_NEAREST_MIPLINEAR:
      magFilter = GL::NEAREST;
      if (generateMipMaps) {
        minFilter = GL::NEAREST_MIPMAP_LINEAR;
      }
      else {
        minFilter = GL::NEAREST;
      }
      break;
    case Constants::TEXTURE_NEAREST_NEAREST_MIPNEAREST:
      magFilter = GL::NEAREST;
      if (generateMipMaps) {
        minFilter = GL::NEAREST_MIPMAP_NEAREST;
      }
      else {
        minFilter = GL::NEAREST;
      }
      break;
    case Constants::TEXTURE_NEAREST_LINEAR_MIPNEAREST:
      magFilter = GL::NEAREST;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_NEAREST;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case Constants::TEXTURE_NEAREST_LINEAR_MIPLINEAR:
      magFilter = GL::NEAREST;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_LINEAR;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case Constants::TEXTURE_NEAREST_LINEAR:
      magFilter = GL::NEAREST;
      minFilter = GL::LINEAR;
      break;
    /*case Constants::TEXTURE_NEAREST_NEAREST:
      magFilter = GL::NEAREST;
      minFilter = GL::NEAREST;
      break;*/
    case Constants::TEXTURE_LINEAR_NEAREST_MIPNEAREST:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::NEAREST_MIPMAP_NEAREST;
      }
      else {
        minFilter = GL::NEAREST;
      }
      break;
    case Constants::TEXTURE_LINEAR_NEAREST_MIPLINEAR:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::NEAREST_MIPMAP_LINEAR;
      }
      else {
        minFilter = GL::NEAREST;
      }
      break;
    /*case Constants::TEXTURE_LINEAR_LINEAR:
      magFilter = GL::LINEAR;
      minFilter = GL::LINEAR;
      break;*/
    case Constants::TEXTURE_LINEAR_NEAREST:
      magFilter = GL::LINEAR;
      minFilter = GL::NEAREST;
      break;
  }

  return {
    static_cast<int>(minFilter), //
    static_cast<int>(magFilter)  //
  };
}

WebGLTexturePtr ThinEngine::_createTexture()
{
  auto texture = _gl->createTexture();

  if (!texture) {
    throw std::runtime_error("Unable to create texture");
  }

  return texture;
}

InternalTexturePtr ThinEngine::createTexture(
  const std::string& urlArg, bool noMipmap, bool invertY, Scene* scene, unsigned int samplingMode,
  const std::function<void(InternalTexture*, EventState&)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  const std::optional<std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>>& buffer,
  const InternalTexturePtr& fallback, const std::optional<unsigned int>& format,
  const std::string& forcedExtension, const std::string& mimeType)
{
  // assign a new string, so that the original is still available in case of fallback
  auto url      = urlArg;
  auto fromData = url.substr(0, 5) == "data:";
  auto fromBlob = url.substr(0, 5) == "blob:";
  auto isBase64 = fromData && StringTools::contains(url, ";base64,");

  auto texture = fallback ? fallback : InternalTexture::New(this, InternalTextureSource::Url);

  // establish the file extension, if possible
  auto lastDot = StringTools::lastIndexOf(url, ".");
  auto extension
    = !forcedExtension.empty() ?
        forcedExtension :
        (lastDot > -1 ? StringTools::toLowerCase(url.substr(static_cast<size_t>(lastDot))) : "");
  auto filteredFormat = excludedCompressedTextureFormats(url, _textureFormatInUse);

  IInternalTextureLoaderPtr loader = nullptr;
  for (const auto& availableLoader : ThinEngine::_TextureLoaders) {
    if (availableLoader->canLoad(extension)) {
      loader = availableLoader;
      break;
    }
  }

  if (scene) {
    scene->_addPendingData(texture);
  }
  texture->url             = url;
  texture->generateMipMaps = !noMipmap;
  texture->samplingMode    = samplingMode;
  texture->invertY         = invertY;

  if (!_doNotHandleContextLost && buffer.has_value()) {
    // Keep a link to the buffer only if we plan to handle context lost
    texture->_buffer = *buffer;
  }

  Observer<InternalTexture>::Ptr onLoadObserver = nullptr;
  if (onLoad && !fallback) {
    onLoadObserver = texture->onLoadedObservable.add(onLoad);
  }

  if (!fallback) {
    _internalTexturesCache.emplace_back(texture);
  }

  const auto onInternalError = [=](const std::string& message, const std::string& exception) {
    if (scene) {
      scene->_removePendingData(texture);
    }

    auto customFallback = false;
    if (loader) {
      std::string fallbackUrl = "";
      if (!fallbackUrl.empty()) {
        // Add Back
        customFallback = true;
        createTexture(urlArg, noMipmap, texture->invertY, scene, samplingMode, nullptr, onError,
                      buffer, texture);
        return;
      }
    }

    if (!customFallback) {
      if (onLoadObserver) {
        texture->onLoadedObservable.remove(onLoadObserver);
      }
      if (EngineStore::UseFallbackTexture) {
        createTexture(EngineStore::FallbackTexture, noMipmap, texture->invertY, scene, samplingMode,
                      nullptr, onError, buffer, texture);
        return;
      }
    }

    if (onError) {
      onError(message.empty() ? "Unknown error" : message, exception);
    }
  };

  // processing for non-image formats
  if (loader) {
    const auto callback = [=](const std::variant<std::string, ArrayBufferView>& data,
                              const std::string & /*responseURL*/) -> void {
      loader->loadData(std::get<ArrayBufferView>(data), texture,
                       [=](int width, int height, bool loadMipmap, bool isCompressed,
                           const std::function<void()>& done, bool loadFailed) -> void {
                         if (loadFailed) {
                           onInternalError("TextureLoader failed to load data", "");
                         }
                         else {
                           _prepareWebGLTexture(
                             texture, scene, width, height, texture->invertY, !loadMipmap,
                             isCompressed,
                             [&](int /*width*/, int /*height*/,
                                 const std::function<void()> & /*continuationCallback*/) -> bool {
                               done();
                               return false;
                             },
                             samplingMode);
                         }
                       });
    };

    if (!buffer.has_value()) {
      _loadFile(url, callback, nullptr, true, onInternalError);
    }
    else {
      // callback(buffer as ArrayBuffer);
      if (std::holds_alternative<ArrayBuffer>(*buffer)) {
        callback(std::get<ArrayBuffer>(*buffer), "");
      }
      else {
        if (onError) {
          onError("Unable to load: only ArrayBuffer or ArrayBufferView is supported", "");
        }
      }
    }
  }
  else {
    auto onload = [this, fromBlob, texture, scene, noMipmap, format, extension,
                   samplingMode](const Image& img) {
      if (fromBlob && !_doNotHandleContextLost) {
        // We need to store the image if we need to rebuild the texture in case of a webgl context
        // lost
        texture->_buffer = img;
      }

      _prepareWebGLTexture(
        texture, scene, img.width, img.height, texture->invertY, noMipmap, false,
        [this, scene, img, format, extension,
         texture](int potWidth, int potHeight, const std::function<void()>& continuationCallback) {
          auto isPot = (img.width == potWidth && img.height == potHeight);
          auto internalFormat
            = (format ? _getInternalFormat(*format) : ((extension == ".jpg") ? GL::RGB : GL::RGBA));

          if (isPot) {
            _gl->texImage2D(GL::TEXTURE_2D, 0, static_cast<int>(internalFormat), img.width,
                            img.height, 0, GL::RGBA, GL::UNSIGNED_BYTE, &img.data);
            return false;
          }

          auto maxTextureSize = _caps.maxTextureSize;

          if (img.width > maxTextureSize || img.height > maxTextureSize
              || !_supportsHardwareTextureRescaling) {
            _prepareWorkingCanvas();
            if (!_workingCanvas || !_workingContext) {
              return false;
            }

            _workingCanvas->width  = potWidth;
            _workingCanvas->height = potHeight;

            _workingContext->drawImage(img, 0, 0, img.width, img.height, 0, 0, potWidth, potHeight);
#if 0
            _gl->texImage2D(GL::TEXTURE_2D, 0, static_cast<int>(internalFormat),
                            internalFormat, GL::UNSIGNED_BYTE, _workingCanvas);
#endif
            texture->width  = potWidth;
            texture->height = potHeight;

            return false;
          }
          else {
            // Using shaders when possible to rescale because canvas.drawImage is lossy
            auto source = InternalTexture::New(this, InternalTextureSource::Temp);
            _bindTextureDirectly(GL::TEXTURE_2D, source);
            _gl->texImage2D(GL::TEXTURE_2D, 0, static_cast<int>(internalFormat), img.width,
                            img.height, 0, GL::RGBA, GL::UNSIGNED_BYTE, &img.data);

            _rescaleTexture(source, texture, scene, internalFormat,
                            [this, texture, source, continuationCallback]() {
                              _releaseTexture(source);
                              _bindTextureDirectly(GL::TEXTURE_2D, texture);

                              continuationCallback();
                            });
          }

          return true;
        },
        samplingMode);
    };

    if (!fromData || isBase64) {
      if (url.empty() && buffer.has_value() && std::holds_alternative<Image>(*buffer)) {
        onload(std::get<Image>(*buffer));
      }
      else {
        ThinEngine::_FileToolsLoadImageFromUrl(url, onload, onInternalError, invertY, mimeType);
      }
    }
    else if (buffer.has_value()
             && (std::holds_alternative<std::string>(*buffer)
                 || std::holds_alternative<ArrayBuffer>(*buffer)
                 || std::holds_alternative<Image>(*buffer))) {
      ThinEngine::_FileToolsLoadImage(*buffer, invertY, onload, onInternalError, mimeType);
    }
  }

  return texture;
}

InternalTexturePtr ThinEngine::createPrefilteredCubeTexture(
  const std::string& rootUrl, Scene* scene, float lodScale, float lodOffset,
  const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  unsigned int format, const std::string& forcedExtension, bool createPolynomials)
{
  const auto callback = [=](const std::optional<CubeTextureData>& loadData) {
    if (!loadData.has_value()) {
      if (onLoad) {
        onLoad(std::nullopt);
      }
      return;
    }

    auto texture = loadData->texture;
    if (!createPolynomials) {
      texture->_sphericalPolynomial = std::make_shared<SphericalPolynomial>();
    }
    else if (loadData->info.sphericalPolynomial) {
      texture->_sphericalPolynomial = loadData->info.sphericalPolynomial;
    }
    texture->_source = InternalTextureSource::CubePrefiltered;

    if (_caps.textureLOD) {
      // Do not add extra process if texture lod is supported.
      if (onLoad) {
        onLoad(loadData);
      }
      return;
    }

    unsigned int mipSlices = 3;

    auto& gl   = *_gl;
    auto width = loadData->width;
    if (!width) {
      return;
    }

    std::vector<BaseTexturePtr> textures;
    for (unsigned int i = 0; i < mipSlices; ++i) {
      // compute LOD from even spacing in smoothness (matching shader calculation)
      float smoothness = static_cast<float>(i) / static_cast<float>(mipSlices - 1);
      float roughness  = 1.f - smoothness;

      float minLODIndex = lodOffset;                                  // roughness = 0
      float maxLODIndex = Scalar::Log2(width) * lodScale + lodOffset; // roughness = 1

      float lodIndex = minLODIndex + (maxLODIndex - minLODIndex) * roughness;
      int mipmapIndex
        = static_cast<int>(std::round(std::min(std::max(lodIndex, 0.f), maxLODIndex)));

      auto glTextureFromLod    = InternalTexture::New(this, InternalTextureSource::Temp);
      glTextureFromLod->type   = texture->type;
      glTextureFromLod->format = texture->format;
      glTextureFromLod->width  = static_cast<int>(
        std::pow(2.f, std::max(static_cast<float>(Scalar::Log2(width) - mipmapIndex), 0.f)));
      glTextureFromLod->height = glTextureFromLod->width;
      glTextureFromLod->isCube = true;
      _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, glTextureFromLod, true);

      gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MAG_FILTER, GL::LINEAR);
      gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_MIN_FILTER, GL::LINEAR);
      gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
      gl.texParameteri(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

      if (loadData->isDDS && !loadData->data.empty()) {
        auto info  = loadData->info;
        auto& data = loadData->data[0];
        _unpackFlipY(info.isCompressed);

        DDSTools::UploadDDSLevels(this, glTextureFromLod, data, info, true, 6u, mipmapIndex);
      }
      else {
        BABYLON_LOG_WARN("ThinEngine", "DDS is the only prefiltered cube map supported so far.")
      }

      _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);

      // Wrap in a base texture for easy binding.
      auto lodTexture      = BaseTexture::New(scene);
      lodTexture->isCube   = true;
      lodTexture->_texture = glTextureFromLod;

      glTextureFromLod->isReady = true;
      textures.emplace_back(lodTexture);
    }

    texture->_lodTextureHigh = textures[2];
    texture->_lodTextureMid  = textures[1];
    texture->_lodTextureLow  = textures[0];

    if (onLoad) {
      onLoad(loadData);
    }
  };

  return createCubeTexture(rootUrl, scene, {}, false, callback, onError, format, forcedExtension,
                           createPolynomials, lodScale, lodOffset);
}

void ThinEngine::_FileToolsLoadImageFromUrl(
  std::string url, const std::function<void(const Image& img)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  bool flipVertically, const std::string& /*mimeType*/)
{
  FileTools::LoadImageFromUrl(url, onLoad, onError, flipVertically);
}

void ThinEngine::_FileToolsLoadImage(
  const std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>& input, bool invertY,
  const std::function<void(const Image& img)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  const std::string& /*mimeType*/)
{
  FileTools::LoadImageFromBuffer(input, invertY, onLoad, onError);
}

void ThinEngine::_rescaleTexture(const InternalTexturePtr& /*source*/,
                                 const InternalTexturePtr& /*destination*/, Scene* /*scene*/,
                                 unsigned int /*internalFormat*/,
                                 const std::function<void()>& /*onComplete*/)
{
}

InternalTexturePtr ThinEngine::createRawTexture(const Uint8Array& /*data*/, int /*width*/,
                                                int /*height*/, unsigned int /*format*/,
                                                bool /*generateMipMaps*/, bool /*invertY*/,
                                                unsigned int /*samplingMode*/,
                                                const std::string& /*compression*/,
                                                unsigned int /*type*/)
{
  return nullptr;
}

InternalTexturePtr ThinEngine::createRawCubeTexture(const std::vector<ArrayBufferView>& /*data*/,
                                                    int /*size*/, unsigned int /*format*/,
                                                    unsigned int /*type*/, bool /*generateMipMaps*/,
                                                    bool /*invertY*/, unsigned int /*samplingMode*/,
                                                    const std::string& /*compression*/)
{
  return nullptr;
}

InternalTexturePtr ThinEngine::createRawTexture3D(const ArrayBufferView& /*data*/, int /*width*/,
                                                  int /*height*/, int /*depth*/,
                                                  unsigned int /*format*/, bool /*generateMipMaps*/,
                                                  bool /*invertY*/, unsigned int /*samplingMode*/,
                                                  const std::string& /*compression*/,
                                                  unsigned int /*textureType*/)
{
  return nullptr;
}

InternalTexturePtr ThinEngine::createRawTexture2DArray(
  const ArrayBufferView& /*data*/, int /*width*/, int /*height*/, int /*depth*/,
  unsigned int /*format*/, bool /*generateMipMaps*/, bool /*invertY*/,
  unsigned int /*samplingMode*/, const std::string& /*compression*/, unsigned int /*textureType*/)
{
  return nullptr;
}

void ThinEngine::_unpackFlipY(bool value)
{
  if (!_unpackFlipYCached.has_value() || *_unpackFlipYCached != value) {
    _gl->pixelStorei(GL::UNPACK_FLIP_Y_WEBGL, value ? 1 : 0);

    if (enableUnpackFlipYCached) {
      _unpackFlipYCached = value;
    }
  }
}

int ThinEngine::_getUnpackAlignement()
{
  return _gl->getParameteri(GL::UNPACK_ALIGNMENT);
}

unsigned int ThinEngine::_getTextureTarget(const InternalTexturePtr& texture) const
{
  if (texture->isCube) {
    return GL::TEXTURE_CUBE_MAP;
  }
  else if (texture->is3D) {
    return GL::TEXTURE_3D;
  }
  else if (texture->is2DArray || texture->isMultiview) {
    return GL::TEXTURE_2D_ARRAY;
  }
  return GL::TEXTURE_2D;
}

void ThinEngine::updateTextureSamplingMode(unsigned int samplingMode,
                                           const InternalTexturePtr& texture, bool generateMipMaps)
{
  const auto target = _getTextureTarget(texture);
  auto filters = _getSamplingParameters(samplingMode, texture->generateMipMaps || generateMipMaps);

  _setTextureParameterInteger(target, GL::TEXTURE_MAG_FILTER, filters.mag, texture);
  _setTextureParameterInteger(target, GL::TEXTURE_MIN_FILTER, filters.min);

  if (generateMipMaps) {
    texture->generateMipMaps = true;
    _gl->generateMipmap(target);
  }

  _bindTextureDirectly(target, nullptr);

  texture->samplingMode = samplingMode;
}

void ThinEngine::updateTextureWrappingMode(const InternalTexturePtr& texture,
                                           std::optional<unsigned int> wrapU,
                                           std::optional<unsigned int> wrapV,
                                           std::optional<unsigned int> wrapR)
{
  const auto target = _getTextureTarget(texture);

  if (wrapU) {
    _setTextureParameterInteger(target, GL::TEXTURE_WRAP_S,
                                static_cast<int>(_getTextureWrapMode(*wrapU)), texture);
    texture->_cachedWrapU = static_cast<int>(*wrapU);
  }
  if (wrapV) {
    _setTextureParameterInteger(target, GL::TEXTURE_WRAP_T,
                                static_cast<int>(_getTextureWrapMode(*wrapV)), texture);
    texture->_cachedWrapV = static_cast<int>(*wrapV);
  }
  if ((texture->is2DArray || texture->is3D) && wrapR) {
    _setTextureParameterInteger(target, GL::TEXTURE_WRAP_R,
                                static_cast<int>(_getTextureWrapMode(*wrapR)), texture);
    texture->_cachedWrapR = static_cast<int>(*wrapR);
  }

  _bindTextureDirectly(target, nullptr);
}

void ThinEngine::_setupDepthStencilTexture(const InternalTexturePtr& internalTexture,
                                           const std::variant<int, ISize>& size,
                                           bool generateStencil, bool bilinearFiltering,
                                           int comparisonFunction)
{
  auto width
    = std::holds_alternative<int>(size) ? std::get<int>(size) : std::get<ISize>(size).width;
  auto height
    = std::holds_alternative<int>(size) ? std::get<int>(size) : std::get<ISize>(size).height;
  internalTexture->baseWidth              = width;
  internalTexture->baseHeight             = height;
  internalTexture->width                  = width;
  internalTexture->height                 = height;
  internalTexture->isReady                = true;
  internalTexture->samples                = 1;
  internalTexture->generateMipMaps        = false;
  internalTexture->_generateDepthBuffer   = true;
  internalTexture->_generateStencilBuffer = generateStencil;
  internalTexture->samplingMode = bilinearFiltering ? Constants::TEXTURE_BILINEAR_SAMPLINGMODE :
                                                      Constants::TEXTURE_NEAREST_SAMPLINGMODE;
  internalTexture->type                = Constants::TEXTURETYPE_UNSIGNED_INT;
  internalTexture->_comparisonFunction = comparisonFunction;

  auto& gl                = *_gl;
  auto target             = internalTexture->isCube ? GL::TEXTURE_CUBE_MAP : GL::TEXTURE_2D;
  auto samplingParameters = _getSamplingParameters(internalTexture->samplingMode, false);
  gl.texParameteri(target, GL::TEXTURE_MAG_FILTER, samplingParameters.mag);
  gl.texParameteri(target, GL::TEXTURE_MIN_FILTER, samplingParameters.min);
  gl.texParameteri(target, GL::TEXTURE_WRAP_S, GL::CLAMP_TO_EDGE);
  gl.texParameteri(target, GL::TEXTURE_WRAP_T, GL::CLAMP_TO_EDGE);

  if (comparisonFunction == 0) {
    gl.texParameteri(target, GL::TEXTURE_COMPARE_FUNC, Constants::LEQUAL);
    gl.texParameteri(target, GL::TEXTURE_COMPARE_MODE, GL::NONE);
  }
  else {
    gl.texParameteri(target, GL::TEXTURE_COMPARE_FUNC, comparisonFunction);
    gl.texParameteri(target, GL::TEXTURE_COMPARE_MODE, GL::COMPARE_REF_TO_TEXTURE);
  }
}

void ThinEngine::_uploadCompressedDataToTextureDirectly(const InternalTexturePtr& texture,
                                                        unsigned int internalFormat, int width,
                                                        int height, const Uint8Array& data,
                                                        unsigned int faceIndex, int lod)
{
  auto& gl = *_gl;

  GL::GLenum target = GL::TEXTURE_2D;
  if (texture->isCube) {
    target = GL::TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex;
  }

  gl.compressedTexImage2D(target, lod, internalFormat, width, height, 0, data);
}

void ThinEngine::_uploadDataToTextureDirectly(const InternalTexturePtr& texture,
                                              const ArrayBufferView& imageData,
                                              unsigned int faceIndex, int lod,
                                              int babylonInternalFormat,
                                              bool useTextureWidthAndHeight)
{
  auto& gl = *_gl;

  auto textureType = _getWebGLTextureType(texture->type);
  auto format      = _getInternalFormat(texture->format);
  auto internalFormat
    = babylonInternalFormat == -1 ?
        static_cast<int>(_getRGBABufferInternalSizedFormat(texture->type, texture->format)) :
        static_cast<int>(_getInternalFormat(static_cast<unsigned int>(babylonInternalFormat)));

  _unpackFlipY(texture->invertY);

  GL::GLenum target = GL::TEXTURE_2D;
  if (texture->isCube) {
    target = GL::TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex;
  }

  const auto lodMaxWidth
    = static_cast<int>(std::round(static_cast<float>(std::log(texture->width)) * Math::LOG2E));
  const auto lodMaxHeight
    = static_cast<int>(std::round(static_cast<float>(std::log(texture->height)) * Math::LOG2E));
  const auto width
    = useTextureWidthAndHeight ? texture->width : std::pow(2, std::max(lodMaxWidth - lod, 0));
  const auto height
    = useTextureWidthAndHeight ? texture->height : std::pow(2, std::max(lodMaxHeight - lod, 0));

  gl.texImage2D(target, lod, internalFormat, static_cast<int>(width), static_cast<int>(height), 0,
                format, textureType, &imageData.uint8Array());
}

void ThinEngine::updateTextureData(const InternalTexturePtr& texture,
                                   const ArrayBufferView& imageData, int xOffset, int yOffset,
                                   int width, int height, unsigned int faceIndex, int lod)
{
  auto& gl = *_gl;

  const auto textureType = _getWebGLTextureType(texture->type);
  const auto format      = _getInternalFormat(texture->format);

  _unpackFlipY(texture->invertY);

  unsigned int target = GL::TEXTURE_2D;
  if (texture->isCube) {
    target = GL::TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex;
  }

  gl.texSubImage2D(target, lod, xOffset, yOffset, width, height, format, textureType,
                   imageData.uint8Array());
}

void ThinEngine::_uploadArrayBufferViewToTexture(const InternalTexturePtr& texture,
                                                 const Uint8Array& imageData,
                                                 unsigned int faceIndex, int lod)
{
  auto bindTarget = texture->isCube ? GL::TEXTURE_CUBE_MAP : GL::TEXTURE_2D;

  _bindTextureDirectly(bindTarget, texture, true);

  _uploadDataToTextureDirectly(texture, imageData, faceIndex, lod);

  _bindTextureDirectly(bindTarget, nullptr, true);
}

void ThinEngine::_prepareWebGLTextureContinuation(const InternalTexturePtr& texture, Scene* scene,
                                                  bool noMipmap, bool isCompressed,
                                                  unsigned int samplingMode)
{
  if (!_gl) {
    return;
  }
  auto& gl = *_gl;

  auto filters = _getSamplingParameters(samplingMode, !noMipmap);

  gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, filters.mag);
  gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, filters.min);

  if (!noMipmap && !isCompressed) {
    gl.generateMipmap(GL::TEXTURE_2D);
  }

  _bindTextureDirectly(GL::TEXTURE_2D, nullptr);

  // resetTextureCache();
  if (scene) {
    scene->_removePendingData(texture);
  }

  texture->onLoadedObservable.notifyObservers(texture.get());
  texture->onLoadedObservable.clear();
}

void ThinEngine::_prepareWebGLTexture(
  const InternalTexturePtr& texture, Scene* scene, int width, int height,
  std::optional<bool> invertY, bool noMipmap, bool isCompressed,
  const std::function<bool(int width, int height,
                           const std::function<void()>& continuationCallback)>& processFunction,
  unsigned int samplingMode)
{
  auto maxTextureSize = getCaps().maxTextureSize;
  auto potWidth
    = std::min(maxTextureSize,
               needPOTTextures() ? ThinEngine::GetExponentOfTwo(width, maxTextureSize) : width);
  auto potHeight
    = std::min(maxTextureSize,
               needPOTTextures() ? ThinEngine::GetExponentOfTwo(height, maxTextureSize) : height);

  if (!_gl) {
    return;
  }

  if (!texture->_webGLTexture) {
    // resetTextureCache();
    if (scene) {
      scene->_removePendingData(texture);
    }

    return;
  }

  _bindTextureDirectly(GL::TEXTURE_2D, texture, true);
  _unpackFlipY(!invertY.has_value() ? true : (*invertY ? true : false));

  texture->baseWidth  = width;
  texture->baseHeight = height;
  texture->width      = potWidth;
  texture->height     = potHeight;
  texture->isReady    = true;

  if (processFunction(potWidth, potHeight, [=]() {
        _prepareWebGLTextureContinuation(texture, scene, noMipmap, isCompressed, samplingMode);
      })) {
    // Returning as texture needs extra async steps
    return;
  }

  _prepareWebGLTextureContinuation(texture, scene, noMipmap, isCompressed, samplingMode);
}

WebGLRenderbufferPtr ThinEngine::_setupFramebufferDepthAttachments(bool generateStencilBuffer,
                                                                   bool generateDepthBuffer,
                                                                   int width, int height,
                                                                   int samples)
{
  // Create the depth/stencil buffer
  if (generateStencilBuffer && generateDepthBuffer) {
    return _getDepthStencilBuffer(width, height, samples, GL::DEPTH_STENCIL, GL::DEPTH24_STENCIL8,
                                  GL::DEPTH_STENCIL_ATTACHMENT);
  }
  if (generateDepthBuffer) {
    auto depthFormat = GL::DEPTH_COMPONENT16;
    if (_webGLVersion > 1.f) {
      depthFormat = GL::DEPTH_COMPONENT32F;
    }

    return _getDepthStencilBuffer(width, height, samples, depthFormat, depthFormat,
                                  GL::DEPTH_ATTACHMENT);
  }
  if (generateStencilBuffer) {
    return _getDepthStencilBuffer(width, height, samples, GL::STENCIL_INDEX8, GL::STENCIL_INDEX8,
                                  GL::STENCIL_ATTACHMENT);
  }

  return nullptr;
}

WebGLRenderbufferPtr ThinEngine::_getDepthStencilBuffer(int width, int height, int samples,
                                                        unsigned int internalFormat,
                                                        unsigned int msInternalFormat,
                                                        unsigned int attachment)
{
  auto& gl                = *_gl;
  auto depthStencilBuffer = gl.createRenderbuffer();

  gl.bindRenderbuffer(GL::RENDERBUFFER, depthStencilBuffer.get());

  if (samples > 1) {
    gl.renderbufferStorageMultisample(GL::RENDERBUFFER, samples, msInternalFormat, width, height);
  }
  else {
    gl.renderbufferStorage(GL::RENDERBUFFER, internalFormat, width, height);
  }

  gl.framebufferRenderbuffer(GL::FRAMEBUFFER, attachment, GL::RENDERBUFFER,
                             depthStencilBuffer.get());

  gl.bindRenderbuffer(GL::RENDERBUFFER, nullptr);

  return depthStencilBuffer;
}

void ThinEngine::_releaseFramebufferObjects(const InternalTexturePtr& texture)
{
  auto& gl = *_gl;

  if (texture->_framebuffer) {
    gl.deleteFramebuffer(texture->_framebuffer.get());
    texture->_framebuffer = nullptr;
  }

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
}

void ThinEngine::_releaseTexture(const InternalTexturePtr& texture)
{
  _releaseFramebufferObjects(texture);

  _deleteTexture(texture->_webGLTexture);

  // Unbind channels
  unbindAllTextures();

  stl_util::remove_vector_elements_equal_sharedptr(_internalTexturesCache, texture.get());

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

  // Integrated irradiance map.
  if (texture->_irradianceTexture) {
    texture->_irradianceTexture->dispose();
  }
}

void ThinEngine::_deleteTexture(const WebGLTexturePtr& texture)
{
  _gl->deleteTexture(texture.get());
}

void ThinEngine::_setProgram(const WebGLProgramPtr& program)
{
  if (_currentProgram != program) {
    _gl->useProgram(program.get());
    _currentProgram = program;
  }
}

void ThinEngine::bindSamplers(Effect& effect)
{
  auto webGLPipelineContext
    = std::static_pointer_cast<WebGLPipelineContext>(effect.getPipelineContext());
  if (webGLPipelineContext) {
    _setProgram(webGLPipelineContext->program);
  }
  const auto& samplers = effect.getSamplers();
  for (size_t index = 0; index < samplers.size(); ++index) {
    auto uniform = effect.getUniform(samplers[index]);

    if (uniform) {
      _boundUniforms[static_cast<int>(index)] = uniform;
    }
  }
  _currentEffect = nullptr;
}

void ThinEngine::_activateCurrentTexture()
{
  if (_currentTextureChannel != _activeChannel) {
    _gl->activeTexture((*_gl)["TEXTURE0"] + static_cast<unsigned int>(_activeChannel));
    _currentTextureChannel = _activeChannel;
  }
}

bool ThinEngine::_bindTextureDirectly(unsigned int target, const InternalTexturePtr& texture,
                                      bool forTextureDataUpdate, bool force)
{
  auto wasPreviouslyBound    = false;
  auto isTextureForRendering = texture && texture->_associatedChannel > -1;
  if (forTextureDataUpdate && isTextureForRendering) {
    _activeChannel = texture->_associatedChannel;
  }

  InternalTexturePtr currentTextureBound = nullptr;
  if (stl_util::contains(_boundTexturesCache, _activeChannel)) {
    currentTextureBound = _boundTexturesCache[_activeChannel];
  }

  if (currentTextureBound != texture || force) {
    _activateCurrentTexture();

    if (texture && texture->isMultiview) {
      _gl->bindTexture(target, texture ? texture->_colorTextureArray.get() : nullptr);
    }
    else {
      _gl->bindTexture(target, texture ? texture->_webGLTexture.get() : nullptr);
    }

    _boundTexturesCache[_activeChannel] = texture;

    if (texture) {
      texture->_associatedChannel = _activeChannel;
    }
  }
  else if (forTextureDataUpdate) {
    wasPreviouslyBound = true;
    _activateCurrentTexture();
  }

  if (isTextureForRendering && !forTextureDataUpdate) {
    _bindSamplerUniformToChannel(texture->_associatedChannel, _activeChannel);
  }

  return wasPreviouslyBound;
}

void ThinEngine::_bindTexture(int channel, const InternalTexturePtr& texture)
{
  if (channel < 0) {
    return;
  }

  if (texture) {
    texture->_associatedChannel = channel;
  }

  _activeChannel = channel;
  _bindTextureDirectly(GL::TEXTURE_2D, texture);
}

void ThinEngine::unbindAllTextures()
{
  for (unsigned int channel = 0; channel < _maxSimultaneousTextures; ++channel) {
    _activeChannel = static_cast<int>(channel);
    _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
    _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
    if (webGLVersion() > 1.f) {
      _bindTextureDirectly(GL::TEXTURE_3D, nullptr);
      _bindTextureDirectly(GL::TEXTURE_2D_ARRAY, nullptr);
    }
  }
}

void ThinEngine::setTexture(int channel, const WebGLUniformLocationPtr& uniform,
                            const BaseTexturePtr& texture)
{
  if (channel < 0) {
    return;
  }

  if (uniform) {
    _boundUniforms[channel] = uniform;
  }

  _setTexture(channel, texture);
}

void ThinEngine::_bindSamplerUniformToChannel(int sourceSlot, int destination)
{
  if (!stl_util::contains(_boundUniforms, sourceSlot)) {
    return;
  }

  auto& uniform = _boundUniforms[sourceSlot];
  if (!uniform || uniform->_currentState == destination) {
    return;
  }
  _gl->uniform1i(uniform.get(), destination);
  uniform->_currentState = destination;
}

unsigned int ThinEngine::_getTextureWrapMode(unsigned int mode) const
{
  switch (mode) {
    case Constants::TEXTURE_WRAP_ADDRESSMODE:
      return GL::REPEAT;
    case Constants::TEXTURE_CLAMP_ADDRESSMODE:
      return GL::CLAMP_TO_EDGE;
    case Constants::TEXTURE_MIRROR_ADDRESSMODE:
      return GL::MIRRORED_REPEAT;
  }
  return GL::REPEAT;
}

bool ThinEngine::_setTexture(int channel, const BaseTexturePtr& texture, bool isPartOfTextureArray,
                             bool depthStencilTexture)
{
  // Not ready?
  if (!texture) {
    if (stl_util::contains(_boundTexturesCache, channel)
        && (_boundTexturesCache[channel] != nullptr)) {
      _activeChannel = channel;
      _bindTextureDirectly(GL::TEXTURE_2D, nullptr);
      _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, nullptr);
      if (webGLVersion() > 1.f) {
        _bindTextureDirectly(GL::TEXTURE_3D, nullptr);
        _bindTextureDirectly(GL::TEXTURE_2D_ARRAY, nullptr);
      }
    }
    return false;
  }

  // Video (not supported)
  if (texture->delayLoadState == Constants::DELAYLOADSTATE_NOTLOADED) {
    // Delay loading
    texture->delayLoad();
    return false;
  }

  InternalTexturePtr internalTexture = nullptr;
  if (depthStencilTexture) {
    internalTexture = std::static_pointer_cast<RenderTargetTexture>(texture)->depthStencilTexture;
  }
  else if (texture->isReady()) {
    internalTexture = texture->getInternalTexture();
  }
  else if (texture->isCube()) {
    internalTexture = emptyCubeTexture();
  }
  else if (texture->is3D()) {
    internalTexture = emptyTexture3D();
  }
  else if (texture->is2DArray()) {
    internalTexture = emptyTexture2DArray();
  }
  else {
    internalTexture = emptyTexture();
  }

  if (!isPartOfTextureArray && internalTexture) {
    internalTexture->_associatedChannel = channel;
  }

  auto needToBind = true;
  if ((_boundTexturesCache.find(channel) != _boundTexturesCache.end())
      && (_boundTexturesCache[channel] == internalTexture)) {
    if (!isPartOfTextureArray) {
      _bindSamplerUniformToChannel(internalTexture->_associatedChannel, channel);
    }
    needToBind = false;
  }

  _activeChannel    = channel;
  const auto target = _getTextureTarget(internalTexture);
  if (needToBind) {
    _bindTextureDirectly(target, internalTexture, isPartOfTextureArray);
  }

  if (internalTexture && internalTexture->isMultiview) {
    // CUBIC_MODE and SKYBOX_MODE both require CLAMP_TO_EDGE.  All other modes use REPEAT.
    if (internalTexture->isCube
        && internalTexture->_cachedCoordinatesMode
             != static_cast<int>(texture->coordinatesMode())) {
      internalTexture->_cachedCoordinatesMode = static_cast<int>(texture->coordinatesMode());

      const auto textureWrapMode
        = (texture->coordinatesMode() != Constants::TEXTURE_CUBIC_MODE
           && texture->coordinatesMode() != Constants::TEXTURE_SKYBOX_MODE) ?
            Constants::TEXTURE_WRAP_ADDRESSMODE :
            Constants::TEXTURE_CLAMP_ADDRESSMODE;
      texture->wrapU = textureWrapMode;
      texture->wrapV = textureWrapMode;
    }

    if (internalTexture->_cachedWrapU != static_cast<int>(texture->wrapU)) {
      internalTexture->_cachedWrapU = static_cast<int>(texture->wrapU);
      _setTextureParameterInteger(target, GL::TEXTURE_WRAP_S,
                                  static_cast<int>(_getTextureWrapMode(texture->wrapU)),
                                  internalTexture);
    }

    if (internalTexture->_cachedWrapV != static_cast<int>(texture->wrapV)) {
      internalTexture->_cachedWrapV = static_cast<int>(texture->wrapV);
      _setTextureParameterInteger(target, GL::TEXTURE_WRAP_T,
                                  static_cast<int>(_getTextureWrapMode(texture->wrapV)),
                                  internalTexture);
    }

    if (internalTexture->is3D
        && internalTexture->_cachedWrapR != static_cast<int>(texture->wrapR)) {
      internalTexture->_cachedWrapR = static_cast<int>(texture->wrapR);
      _setTextureParameterInteger(target, GL::TEXTURE_WRAP_R,
                                  static_cast<int>(_getTextureWrapMode(texture->wrapR)),
                                  internalTexture);
    }

    _setAnisotropicLevel(target, internalTexture, texture->anisotropicFilteringLevel);
  }

  return true;
}

void ThinEngine::setTextureArray(int channel, const WebGLUniformLocationPtr& uniform,
                                 const std::vector<BaseTexturePtr>& textures)
{
  if (channel < 0 || !uniform) {
    return;
  }

  if (_textureUnits.empty() || _textureUnits.size() != textures.size()) {
    _textureUnits.clear();
    _textureUnits.resize(textures.size());
  }
  auto _channel = static_cast<size_t>(channel);
  for (size_t i = 0; i < textures.size(); ++i) {
    auto texture = textures[i]->getInternalTexture();

    if (texture) {
      _textureUnits[i]            = static_cast<int>(_channel + i);
      texture->_associatedChannel = static_cast<int>(_channel + i);
    }
    else {
      _textureUnits[i] = -1;
    }
  }
  _gl->uniform1iv(uniform.get(), _textureUnits);

  size_t index = 0;
  for (const auto& texture : textures) {
    _setTexture(_textureUnits[index], texture, true);
    ++index;
  }
}

void ThinEngine::_setAnisotropicLevel(unsigned int target,
                                      const InternalTexturePtr& internalTexture,
                                      unsigned int anisotropicFilteringLevel)
{
  auto anisotropicFilterExtension = _caps.textureAnisotropicFilterExtension;

  if (internalTexture->samplingMode != Constants::TEXTURE_LINEAR_LINEAR_MIPNEAREST
      && internalTexture->samplingMode != Constants::TEXTURE_LINEAR_LINEAR_MIPLINEAR
      && internalTexture->samplingMode != Constants::TEXTURE_LINEAR_LINEAR) {
    anisotropicFilteringLevel
      = 1; // Forcing the anisotropic to 1 because else webgl will force filters to linear
  }

  if (anisotropicFilterExtension
      && internalTexture->_cachedAnisotropicFilteringLevel
           != static_cast<int>(anisotropicFilteringLevel)) {
    _setTextureParameterFloat(
      target, AnisotropicFilterExtension::TEXTURE_MAX_ANISOTROPY_EXT,
      static_cast<float>(std::min(anisotropicFilteringLevel, _caps.maxAnisotropy)),
      internalTexture);
    internalTexture->_cachedAnisotropicFilteringLevel = static_cast<int>(anisotropicFilteringLevel);
  }
}

void ThinEngine::_setTextureParameterFloat(unsigned int target, unsigned int parameter, float value,
                                           const InternalTexturePtr& texture)
{
  _bindTextureDirectly(target, texture, true, true);
  _gl->texParameterf(target, parameter, value);
}

void ThinEngine::_setTextureParameterInteger(unsigned int target, unsigned int parameter, int value,
                                             const InternalTexturePtr& texture)
{
  if (texture) {
    _bindTextureDirectly(target, texture, true, true);
  }
  _gl->texParameteri(target, parameter, value);
}

void ThinEngine::unbindAllAttributes()
{
  if (_mustWipeVertexAttributes) {
    _mustWipeVertexAttributes = false;

    for (unsigned int i = 0, ul = static_cast<unsigned int>(_caps.maxVertexAttribs); i < ul; ++i) {
      disableAttributeByIndex(i);
    }
    return;
  }

  for (unsigned int i = 0; i < _vertexAttribArraysEnabled.size(); ++i) {
    if (static_cast<int>(i) >= _caps.maxVertexAttribs || !_vertexAttribArraysEnabled[i]) {
      continue;
    }

    disableAttributeByIndex(i);
  }
}

void ThinEngine::releaseEffects()
{
  for (const auto& compiledEffectItem : _compiledEffects) {
    auto webGLPipelineContext = std::static_pointer_cast<WebGLPipelineContext>(
      compiledEffectItem.second->getPipelineContext());
    _deletePipelineContext(webGLPipelineContext);
  }

  _compiledEffects = {};
}

void ThinEngine::dispose()
{
  stopRenderLoop();

  // Clear observables
  /* if (onBeforeTextureInitObservable) */ {
    onBeforeTextureInitObservable.clear();
  }

  // Empty texture
  if (_emptyTexture) {
    _releaseTexture(_emptyTexture);
    _emptyTexture = nullptr;
  }
  if (_emptyCubeTexture) {
    _releaseTexture(_emptyCubeTexture);
    _emptyCubeTexture = nullptr;
  }

  // Release effects
  releaseEffects();

  // Unbind
  unbindAllAttributes();
  _boundUniforms = {};

  _workingCanvas         = nullptr;
  _workingContext        = nullptr;
  _currentBufferPointers = {};
  _renderingCanvas       = nullptr;
  _currentProgram        = nullptr;

  Effect::ResetCache();
}

unsigned int ThinEngine::getError() const
{
  return _gl->getError();
}

bool ThinEngine::_canRenderToFloatFramebuffer()
{
  if (_webGLVersion > 1.f) {
    return _caps.colorBufferFloat;
  }
  return _canRenderToFramebuffer(Constants::TEXTURETYPE_FLOAT);
}

bool ThinEngine::_canRenderToHalfFloatFramebuffer()
{
  if (_webGLVersion > 1.f) {
    return _caps.colorBufferFloat;
  }

  return _canRenderToFramebuffer(Constants::TEXTURETYPE_HALF_FLOAT);
}

bool ThinEngine::_canRenderToFramebuffer(unsigned int type)
{
  auto& gl = *_gl;

  // clear existing errors
  const GL::GLenum GL_NO_ERROR = 0x0000;
  while (gl.getError() != GL_NO_ERROR) {
  }

  auto successful = true;

  auto texture = gl.createTexture();
  gl.bindTexture(GL::TEXTURE_2D, texture.get());
  gl.texImage2D(GL::TEXTURE_2D, 0, static_cast<int>(_getRGBABufferInternalSizedFormat(type)), 1, 1,
                0, GL::RGBA, _getWebGLTextureType(type), nullptr);
  gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER, GL::NEAREST);
  gl.texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER, GL::NEAREST);

  auto fb = gl.createFramebuffer();
  gl.bindFramebuffer(GL::FRAMEBUFFER, fb.get());
  gl.framebufferTexture2D(GL::FRAMEBUFFER, GL::COLOR_ATTACHMENT0, GL::TEXTURE_2D, texture.get(), 0);
  auto status = gl.checkFramebufferStatus(GL::FRAMEBUFFER);

  successful = successful && (status == GL::FRAMEBUFFER_COMPLETE);
  successful = successful && (gl.getError() == GL_NO_ERROR);

  // try render by clearing frame buffer's color buffer
  if (successful) {
    gl.clear(GL::COLOR_BUFFER_BIT);
    successful = successful && (gl.getError() == GL_NO_ERROR);
  }

  // try reading from frame to ensure render occurs (just creating the FBO is not sufficient to
  // determine if rendering is supported)
  if (successful) {
    // in practice it's sufficient to just read from the backbuffer rather than handle potentially
    // issues reading from the texture
    gl.bindFramebuffer(GL::FRAMEBUFFER, nullptr);
    auto readFormat = GL::RGBA;
    auto readType   = GL::UNSIGNED_BYTE;
    Uint8Array buffer(4);
    gl.readPixels(0, 0, 1, 1, readFormat, readType, buffer);
    successful = successful && (gl.getError() == GL_NO_ERROR);
  }

  // clean up
  gl.deleteTexture(texture.get());
  gl.deleteFramebuffer(fb.get());
  gl.bindFramebuffer(GL::FRAMEBUFFER, nullptr);

  // clear accumulated errors
  while (!successful && (gl.getError() != GL_NO_ERROR)) {
  }

  return successful;
}

unsigned int ThinEngine::_getWebGLTextureType(unsigned int type) const
{
  if (_webGLVersion == 1.f) {
    switch (type) {
      case Constants::TEXTURETYPE_FLOAT:
        return GL::FLOAT;
      case Constants::TEXTURETYPE_HALF_FLOAT:
        return GL::HALF_FLOAT_OES;
      case Constants::TEXTURETYPE_UNSIGNED_BYTE:
        return GL::UNSIGNED_BYTE;
      case Constants::TEXTURETYPE_UNSIGNED_SHORT_4_4_4_4:
        return GL::UNSIGNED_SHORT_4_4_4_4;
      case Constants::TEXTURETYPE_UNSIGNED_SHORT_5_5_5_1:
        return GL::UNSIGNED_SHORT_5_5_5_1;
      case Constants::TEXTURETYPE_UNSIGNED_SHORT_5_6_5:
        return GL::UNSIGNED_SHORT_5_6_5;
    }
    return GL::UNSIGNED_BYTE;
  }

  switch (type) {
    case Constants::TEXTURETYPE_BYTE:
      return GL::BYTE;
    case Constants::TEXTURETYPE_UNSIGNED_BYTE:
      return GL::UNSIGNED_BYTE;
    case Constants::TEXTURETYPE_SHORT:
      return GL::SHORT;
    case Constants::TEXTURETYPE_UNSIGNED_SHORT:
      return GL::UNSIGNED_SHORT;
    case Constants::TEXTURETYPE_INT:
      return GL::INT;
    case Constants::TEXTURETYPE_UNSIGNED_INTEGER: // Refers to UNSIGNED_INT
      return GL::UNSIGNED_INT;
    case Constants::TEXTURETYPE_FLOAT:
      return GL::FLOAT;
    case Constants::TEXTURETYPE_HALF_FLOAT:
      return GL::HALF_FLOAT;
    case Constants::TEXTURETYPE_UNSIGNED_SHORT_4_4_4_4:
      return GL::UNSIGNED_SHORT_4_4_4_4;
    case Constants::TEXTURETYPE_UNSIGNED_SHORT_5_5_5_1:
      return GL::UNSIGNED_SHORT_5_5_5_1;
    case Constants::TEXTURETYPE_UNSIGNED_SHORT_5_6_5:
      return GL::UNSIGNED_SHORT_5_6_5;
    case Constants::TEXTURETYPE_UNSIGNED_INT_2_10_10_10_REV:
      return GL::UNSIGNED_INT_2_10_10_10_REV;
    case Constants::TEXTURETYPE_UNSIGNED_INT_24_8:
      return GL::UNSIGNED_INT_24_8;
    case Constants::TEXTURETYPE_UNSIGNED_INT_10F_11F_11F_REV:
      return GL::UNSIGNED_INT_10F_11F_11F_REV;
    case Constants::TEXTURETYPE_UNSIGNED_INT_5_9_9_9_REV:
      return GL::UNSIGNED_INT_5_9_9_9_REV;
    case Constants::TEXTURETYPE_FLOAT_32_UNSIGNED_INT_24_8_REV:
      return GL::FLOAT_32_UNSIGNED_INT_24_8_REV;
  }

  return GL::UNSIGNED_BYTE;
}

unsigned int ThinEngine::_getInternalFormat(unsigned int format) const
{
  auto internalFormat = GL::RGBA;

  switch (format) {
    case Constants::TEXTUREFORMAT_ALPHA:
      internalFormat = GL::ALPHA;
      break;
    case Constants::TEXTUREFORMAT_LUMINANCE:
      internalFormat = GL::LUMINANCE;
      break;
    case Constants::TEXTUREFORMAT_LUMINANCE_ALPHA:
      internalFormat = GL::LUMINANCE_ALPHA;
      break;
    case Constants::TEXTUREFORMAT_RED:
      internalFormat = GL::RED;
      break;
    case Constants::TEXTUREFORMAT_RG:
      internalFormat = GL::RG;
      break;
    case Constants::TEXTUREFORMAT_RGB:
      internalFormat = GL::RGB;
      break;
    case Constants::TEXTUREFORMAT_RGBA:
      internalFormat = GL::RGBA;
      break;
  }

  if (_webGLVersion > 1.f) {
    switch (format) {
      case Constants::TEXTUREFORMAT_RED_INTEGER:
        internalFormat = GL::RED_INTEGER;
        break;
      case Constants::TEXTUREFORMAT_RG_INTEGER:
        internalFormat = GL::RG_INTEGER;
        break;
      case Constants::TEXTUREFORMAT_RGB_INTEGER:
        internalFormat = GL::RGB_INTEGER;
        break;
      case Constants::TEXTUREFORMAT_RGBA_INTEGER:
        internalFormat = GL::RGBA_INTEGER;
        break;
    }
  }

  return internalFormat;
}

unsigned int
ThinEngine::_getRGBABufferInternalSizedFormat(unsigned int type,
                                              const std::optional<unsigned int>& format) const
{
  const auto _format = format.value_or(std::numeric_limits<unsigned int>::max());

  if (_webGLVersion == 1.f) {
    if (format.has_value()) {
      switch (_format) {
        case Constants::TEXTUREFORMAT_ALPHA:
          return GL::ALPHA;
        case Constants::TEXTUREFORMAT_LUMINANCE:
          return GL::LUMINANCE;
        case Constants::TEXTUREFORMAT_LUMINANCE_ALPHA:
          return GL::LUMINANCE_ALPHA;
        case Constants::TEXTUREFORMAT_RGB:
          return GL::RGB;
      }
    }
    return GL::RGBA;
  }

  switch (type) {
    case Constants::TEXTURETYPE_BYTE:
      switch (_format) {
        case Constants::TEXTUREFORMAT_RED:
          return GL::R8_SNORM;
        case Constants::TEXTUREFORMAT_RG:
          return GL::RG8_SNORM;
        case Constants::TEXTUREFORMAT_RGB:
          return GL::RGB8_SNORM;
        case Constants::TEXTUREFORMAT_RED_INTEGER:
          return GL::R8I;
        case Constants::TEXTUREFORMAT_RG_INTEGER:
          return GL::RG8I;
        case Constants::TEXTUREFORMAT_RGB_INTEGER:
          return GL::RGB8I;
        case Constants::TEXTUREFORMAT_RGBA_INTEGER:
          return GL::RGBA8I;
        default:
          return GL::RGBA8_SNORM;
      }
    case Constants::TEXTURETYPE_UNSIGNED_BYTE:
      switch (_format) {
        case Constants::TEXTUREFORMAT_RED:
          return GL::R8;
        case Constants::TEXTUREFORMAT_RG:
          return GL::RG8;
        case Constants::TEXTUREFORMAT_RGB:
          return GL::RGB8; // By default. Other possibilities are RGB565, SRGB8.
        case Constants::TEXTUREFORMAT_RGBA:
          return GL::RGBA8; // By default. Other possibilities are RGB5_A1, RGBA4, SRGB8_ALPHA8.
        case Constants::TEXTUREFORMAT_RED_INTEGER:
          return GL::R8UI;
        case Constants::TEXTUREFORMAT_RG_INTEGER:
          return GL::RG8UI;
        case Constants::TEXTUREFORMAT_RGB_INTEGER:
          return GL::RGB8UI;
        case Constants::TEXTUREFORMAT_RGBA_INTEGER:
          return GL::RGBA8UI;
        case Constants::TEXTUREFORMAT_ALPHA:
          return GL::ALPHA;
        case Constants::TEXTUREFORMAT_LUMINANCE:
          return GL::LUMINANCE;
        case Constants::TEXTUREFORMAT_LUMINANCE_ALPHA:
          return GL::LUMINANCE_ALPHA;
        default:
          return GL::RGBA8;
      }
    case Constants::TEXTURETYPE_SHORT:
      switch (_format) {
        case Constants::TEXTUREFORMAT_RED_INTEGER:
          return GL::R16I;
        case Constants::TEXTUREFORMAT_RG_INTEGER:
          return GL::RG16I;
        case Constants::TEXTUREFORMAT_RGB_INTEGER:
          return GL::RGB16I;
        case Constants::TEXTUREFORMAT_RGBA_INTEGER:
          return GL::RGBA16I;
        default:
          return GL::RGBA16I;
      }
    case Constants::TEXTURETYPE_UNSIGNED_SHORT:
      switch (_format) {
        case Constants::TEXTUREFORMAT_RED_INTEGER:
          return GL::R16UI;
        case Constants::TEXTUREFORMAT_RG_INTEGER:
          return GL::RG16UI;
        case Constants::TEXTUREFORMAT_RGB_INTEGER:
          return GL::RGB16UI;
        case Constants::TEXTUREFORMAT_RGBA_INTEGER:
          return GL::RGBA16UI;
        default:
          return GL::RGBA16UI;
      }
    case Constants::TEXTURETYPE_INT:
      switch (_format) {
        case Constants::TEXTUREFORMAT_RED_INTEGER:
          return GL::R32I;
        case Constants::TEXTUREFORMAT_RG_INTEGER:
          return GL::RG32I;
        case Constants::TEXTUREFORMAT_RGB_INTEGER:
          return GL::RGB32I;
        case Constants::TEXTUREFORMAT_RGBA_INTEGER:
          return GL::RGBA32I;
        default:
          return GL::RGBA32I;
      }
    case Constants::TEXTURETYPE_UNSIGNED_INTEGER: // Refers to UNSIGNED_INT
      switch (_format) {
        case Constants::TEXTUREFORMAT_RED_INTEGER:
          return GL::R32UI;
        case Constants::TEXTUREFORMAT_RG_INTEGER:
          return GL::RG32UI;
        case Constants::TEXTUREFORMAT_RGB_INTEGER:
          return GL::RGB32UI;
        case Constants::TEXTUREFORMAT_RGBA_INTEGER:
          return GL::RGBA32UI;
        default:
          return GL::RGBA32UI;
      }
    case Constants::TEXTURETYPE_FLOAT:
      switch (_format) {
        case Constants::TEXTUREFORMAT_RED:
          return GL::R32F; // By default. Other possibility is R16F.
        case Constants::TEXTUREFORMAT_RG:
          return GL::RG32F; // By default. Other possibility is RG16F.
        case Constants::TEXTUREFORMAT_RGB:
          return GL::RGB32F; // By default. Other possibilities are RGB16F, R11F_G11F_B10F, RGB9_E5.
        case Constants::TEXTUREFORMAT_RGBA:
          return GL::RGBA32F; // By default. Other possibility is RGBA16F.
        default:
          return GL::RGBA32F;
      }
    case Constants::TEXTURETYPE_HALF_FLOAT:
      switch (_format) {
        case Constants::TEXTUREFORMAT_RED:
          return GL::R16F;
        case Constants::TEXTUREFORMAT_RG:
          return GL::RG16F;
        case Constants::TEXTUREFORMAT_RGB:
          return GL::RGB16F; // By default. Other possibilities are R11F_G11F_B10F, RGB9_E5.
        case Constants::TEXTUREFORMAT_RGBA:
          return GL::RGBA16F;
        default:
          return GL::RGBA16F;
      }
    case Constants::TEXTURETYPE_UNSIGNED_SHORT_5_6_5:
      return GL::RGB565;
    case Constants::TEXTURETYPE_UNSIGNED_INT_10F_11F_11F_REV:
      return GL::R11F_G11F_B10F;
    case Constants::TEXTURETYPE_UNSIGNED_INT_5_9_9_9_REV:
      return GL::RGB9_E5;
    case Constants::TEXTURETYPE_UNSIGNED_SHORT_4_4_4_4:
      return GL::RGBA4;
    case Constants::TEXTURETYPE_UNSIGNED_SHORT_5_5_5_1:
      return GL::RGB5_A1;
    case Constants::TEXTURETYPE_UNSIGNED_INT_2_10_10_10_REV:
      switch (_format) {
        case Constants::TEXTUREFORMAT_RGBA:
          return GL::RGB10_A2; // By default. Other possibility is RGB5_A1.
        case Constants::TEXTUREFORMAT_RGBA_INTEGER:
          return GL::RGB10_A2UI;
        default:
          return GL::RGB10_A2;
      }
  }

  return GL::RGBA8;
}

unsigned int ThinEngine::_getRGBAMultiSampleBufferFormat(unsigned int type) const
{
  if (type == Constants::TEXTURETYPE_FLOAT) {
    return GL::RGBA32F;
  }
  else if (type == Constants::TEXTURETYPE_HALF_FLOAT) {
    return GL::RGBA16F;
  }

  return GL::RGBA8;
}

IFileRequest ThinEngine::_loadFile(
  const std::string& url,
  const std::function<void(const std::variant<std::string, ArrayBufferView>& data,
                           const std::string& responseURL)>& onSuccess,
  const std::function<void(const ProgressEvent& event)>& onProgress, bool useArrayBuffer,
  const std::function<void(const std::string& message, const std::string& exception)>& onError)
{
  FileTools::LoadFile(url, onSuccess, onProgress, useArrayBuffer, onError);
  return IFileRequest();
}

Uint8Array ThinEngine::readPixels(int x, int y, int width, int height, bool hasAlpha)
{
  const auto numChannels = hasAlpha ? 4 : 3;
  const auto format      = hasAlpha ? GL::RGBA : GL::RGB;
  Uint8Array data(static_cast<size_t>(height * width * numChannels));
  _gl->readPixels(x, y, width, height, format, GL::UNSIGNED_BYTE, data);
  return data;
}

bool ThinEngine::isSupported()
{
  return true;
}

int ThinEngine::CeilingPOT(int x)
{
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;
  return x;
}

int ThinEngine::FloorPOT(int x)
{
  x = x | (x >> 1);
  x = x | (x >> 2);
  x = x | (x >> 4);
  x = x | (x >> 8);
  x = x | (x >> 16);
  return x - (x >> 1);
}

int ThinEngine::NearestPOT(int x)
{
  auto c = ThinEngine::CeilingPOT(x);
  auto f = ThinEngine::FloorPOT(x);
  return (c - x) > (x - f) ? f : c;
}

int ThinEngine::GetExponentOfTwo(int value, int max, unsigned int mode)
{
  auto pot = 0;

  switch (mode) {
    case Constants::SCALEMODE_FLOOR:
      pot = ThinEngine::FloorPOT(value);
      break;
    case Constants::SCALEMODE_NEAREST:
      pot = ThinEngine::NearestPOT(value);
      break;
    case Constants::SCALEMODE_CEILING:
      pot = ThinEngine::CeilingPOT(value);
      break;
    default:
      pot = ThinEngine::CeilingPOT(value);
  }

  return std::min(pot, max);
}

//--------------------------------------------------------------------------------------------------
//                              Alpha Extension
//--------------------------------------------------------------------------------------------------

void ThinEngine::setAlphaConstants(float r, float g, float b, float a)
{
  _alphaExtension->setAlphaConstants(r, g, b, a);
}

void ThinEngine::setAlphaMode(unsigned int mode, bool noDepthWriteChange)
{
  _alphaExtension->setAlphaMode(mode, noDepthWriteChange);
}

unsigned int ThinEngine::getAlphaMode() const
{
  return _alphaExtension->getAlphaMode();
}

void ThinEngine::setAlphaEquation(unsigned int equation)
{
  _alphaExtension->setAlphaEquation(equation);
}

unsigned int ThinEngine::getAlphaEquation() const
{
  return _alphaExtension->getAlphaEquation();
}

//--------------------------------------------------------------------------------------------------
//                              Cube Texture Extension
//--------------------------------------------------------------------------------------------------

InternalTexturePtr
ThinEngine::_createDepthStencilCubeTexture(int size, const DepthTextureCreationOptions& options)
{
  return _cubeTextureExtension->_createDepthStencilCubeTexture(size, options);
}

InternalTexturePtr ThinEngine::createCubeTexture(
  std::string rootUrl, Scene* scene, const std::vector<std::string>& files, bool noMipmap,
  const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  unsigned int format, const std::string& forcedExtension, bool createPolynomials, float lodScale,
  float lodOffset, const InternalTexturePtr& fallback)
{
  return _cubeTextureExtension->createCubeTexture(rootUrl, scene, files, noMipmap, onLoad, onError,
                                                  format, forcedExtension, createPolynomials,
                                                  lodScale, lodOffset, fallback);
}

void ThinEngine::_cascadeLoadImgs(
  const std::string& rootUrl, Scene* scene,
  const std::function<void(const std::vector<Image>& images)>& onfinish,
  const std::vector<std::string>& files,
  const std::function<void(const std::string& message, const std::string& exception)>& onError)
{
  _cubeTextureExtension->_cascadeLoadImgs(rootUrl, scene, onfinish, files, onError);
}

void ThinEngine::_setCubeMapTextureParams(bool loadMipmap)
{
  _cubeTextureExtension->_setCubeMapTextureParams(loadMipmap);
}

//--------------------------------------------------------------------------------------------------
//                              Dynamic Texture Extension
//--------------------------------------------------------------------------------------------------

InternalTexturePtr ThinEngine::createDynamicTexture(int width, int height, bool generateMipMaps,
                                                    unsigned int samplingMode)
{
  return _dynamicTextureExtension->createDynamicTexture(width, height, generateMipMaps,
                                                        samplingMode);
}

void ThinEngine::updateDynamicTexture(const InternalTexturePtr& texture, ICanvas* canvas,
                                      bool invertY, bool premulAlpha,
                                      std::optional<unsigned int> format, bool forceBindTexture)
{
  _dynamicTextureExtension->updateDynamicTexture(texture, canvas, invertY, premulAlpha, format,
                                                 forceBindTexture);
}

//--------------------------------------------------------------------------------------------------
//                              Multi Render Extension
//--------------------------------------------------------------------------------------------------

void ThinEngine::unBindMultiColorAttachmentFramebuffer(
  const std::vector<InternalTexturePtr>& textures, bool disableGenerateMipMaps,
  const std::function<void()>& onBeforeUnbind)
{
  _multiRenderExtension->unBindMultiColorAttachmentFramebuffer(textures, disableGenerateMipMaps,
                                                               onBeforeUnbind);
}

std::vector<InternalTexturePtr>
ThinEngine::createMultipleRenderTarget(ISize size, const IMultiRenderTargetOptions& options)
{
  return _multiRenderExtension->createMultipleRenderTarget(size, options);
}

unsigned int ThinEngine::updateMultipleRenderTargetTextureSampleCount(
  const std::vector<InternalTexturePtr>& textures, unsigned int samples)
{
  return _multiRenderExtension->updateMultipleRenderTargetTextureSampleCount(textures, samples);
}

//--------------------------------------------------------------------------------------------------
//                              Render Target Extension
//--------------------------------------------------------------------------------------------------

InternalTexturePtr ThinEngine::createRenderTargetTexture(const std::variant<ISize, float>& size,
                                                         const IRenderTargetOptions& options)
{
  return _renderTargetExtension->createRenderTargetTexture(size, options);
}

InternalTexturePtr ThinEngine::createDepthStencilTexture(const std::variant<int, ISize>& size,
                                                         const DepthTextureCreationOptions& options)
{
  return _renderTargetExtension->createDepthStencilTexture(size, options);
}

InternalTexturePtr
ThinEngine::_createDepthStencilTexture(const std::variant<int, ISize>& size,
                                       const DepthTextureCreationOptions& options)
{
  return _renderTargetExtension->_createDepthStencilTexture(size, options);
}

//--------------------------------------------------------------------------------------------------
//                              Render Target Cube Extension
//--------------------------------------------------------------------------------------------------

InternalTexturePtr ThinEngine::createRenderTargetCubeTexture(const ISize& size,
                                                             const IRenderTargetOptions& options)
{
  return _renderTargetCubeExtension->createRenderTargetCubeTexture(size, options);
}

//--------------------------------------------------------------------------------------------------
//                              Uniform Buffer Extension
//--------------------------------------------------------------------------------------------------

WebGLDataBufferPtr ThinEngine::createUniformBuffer(const Float32Array& elements)
{
  return _uniformBufferExtension->createUniformBuffer(elements);
}

WebGLDataBufferPtr ThinEngine::createDynamicUniformBuffer(const Float32Array& elements)
{
  return _uniformBufferExtension->createDynamicUniformBuffer(elements);
}

void ThinEngine::updateUniformBuffer(const WebGLDataBufferPtr& uniformBuffer,
                                     const Float32Array& elements, int offset, int count)
{
  _uniformBufferExtension->updateUniformBuffer(uniformBuffer, elements, offset, count);
}

void ThinEngine::bindUniformBuffer(const WebGLDataBufferPtr& buffer)
{
  _uniformBufferExtension->bindUniformBuffer(buffer);
}

void ThinEngine::bindUniformBufferBase(const WebGLDataBufferPtr& buffer, unsigned int location)
{
  _uniformBufferExtension->bindUniformBufferBase(buffer, location);
}

void ThinEngine::bindUniformBlock(const IPipelineContextPtr& pipelineContext,
                                  const std::string& blockName, unsigned int index)
{
  _uniformBufferExtension->bindUniformBlock(pipelineContext, blockName, index);
}

} // end of namespace BABYLON
