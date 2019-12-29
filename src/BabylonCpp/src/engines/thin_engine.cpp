#include <babylon/engines/thin_engine.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine_store.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/webgl/webgl_pipeline_context.h>
#include <babylon/interfaces/igl_rendering_context.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/iinternal_texture_loader.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/states/alpha_state.h>
#include <babylon/states/depth_culling_state.h>
#include <babylon/states/stencil_state.h>

namespace BABYLON {

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
  EffectCreationOptions& options, ThinEngine* engine,
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
    auto name = vertex + "+" + fragment + "@" + options.defines;
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

  auto shaderVersion  = (_webGLVersion > 1.f) ? "#version 300 es\n#define WEBGL2 \n" : "";
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

  return pipelineContext;
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
    _finalizePipelineContext(pipelineContext);
  }

  return shaderProgram;
}

void ThinEngine::_finalizePipelineContext(const WebGLPipelineContextPtr& pipelineContext)
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

bool ThinEngine::_isRenderingStateCompiled(const IPipelineContextPtr& pipelineContext)
{
  auto webGLPipelineContext = std::static_pointer_cast<WebGLPipelineContext>(pipelineContext);
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

void ThinEngine::setInt(GL::IGLUniformLocation* uniform, int value)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1i(uniform, value);
}

void ThinEngine::setIntArray(GL::IGLUniformLocation* uniform, const Int32Array& array)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1iv(uniform, array);
}

void ThinEngine::setIntArray2(GL::IGLUniformLocation* uniform, const Int32Array& array)
{
  if (!uniform || array.size() % 2 != 0) {
    return;
  }

  _gl->uniform2iv(uniform, array);
}

void ThinEngine::setIntArray3(GL::IGLUniformLocation* uniform, const Int32Array& array)
{
  if (!uniform || array.size() % 3 != 0) {
    return;
  }

  _gl->uniform3iv(uniform, array);
}

void ThinEngine::setIntArray4(GL::IGLUniformLocation* uniform, const Int32Array& array)
{
  if (!uniform || array.size() % 4 != 0) {
    return;
  }

  _gl->uniform4iv(uniform, array);
}

void ThinEngine::setArray(GL::IGLUniformLocation* uniform, const Float32Array& array)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1fv(uniform, array);
}

void ThinEngine::setArray2(GL::IGLUniformLocation* uniform, const Float32Array& array)
{
  if (!uniform || array.size() % 2 != 0) {
    return;
  }

  _gl->uniform2fv(uniform, array);
}

void ThinEngine::setArray3(GL::IGLUniformLocation* uniform, const Float32Array& array)
{
  if (!uniform || array.size() % 3 != 0) {
    return;
  }

  _gl->uniform3fv(uniform, array);
}

void ThinEngine::setArray4(GL::IGLUniformLocation* uniform, const Float32Array& array)
{
  if (!uniform || array.size() % 4 != 0) {
    return;
  }

  _gl->uniform4fv(uniform, array);
}

void ThinEngine::setMatrices(GL::IGLUniformLocation* uniform, const Float32Array& matrices)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix4fv(uniform, false, matrices);
}

void ThinEngine::setMatrix3x3(GL::IGLUniformLocation* uniform, const Float32Array& matrix)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix3fv(uniform, false, matrix);
}

void ThinEngine::setMatrix2x2(GL::IGLUniformLocation* uniform, const Float32Array& matrix)
{
  if (!uniform) {
    return;
  }

  _gl->uniformMatrix2fv(uniform, false, matrix);
}

void ThinEngine::setFloat(GL::IGLUniformLocation* uniform, float value)
{
  if (!uniform) {
    return;
  }

  _gl->uniform1f(uniform, value);
}

void ThinEngine::setFloat2(GL::IGLUniformLocation* uniform, float x, float y)
{
  if (!uniform) {
    return;
  }

  _gl->uniform2f(uniform, x, y);
}

void ThinEngine::setFloat3(GL::IGLUniformLocation* uniform, float x, float y, float z)
{
  if (!uniform) {
    return;
  }

  _gl->uniform3f(uniform, x, y, z);
}

void ThinEngine::setFloat4(GL::IGLUniformLocation* uniform, float x, float y, float z, float w)
{
  if (!uniform) {
    return;
  }

  _gl->uniform4f(uniform, x, y, z, w);
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
    _depthCullingState.depthFunc = GL::LEQUAL;

    _alphaState.reset();
    _alphaMode     = Constants::ALPHA_ADD;
    _alphaEquation = Constants::ALPHA_DISABLE;

    _colorWrite        = true;
    _colorWriteChanged = true;

    _unpackFlipYCached = nullptr;

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
    case Constants::TEXTURE_BILINEAR_SAMPLINGMODE:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_NEAREST;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case Constants::TEXTURE_TRILINEAR_SAMPLINGMODE:
      magFilter = GL::LINEAR;
      if (generateMipMaps) {
        minFilter = GL::LINEAR_MIPMAP_LINEAR;
      }
      else {
        minFilter = GL::LINEAR;
      }
      break;
    case Constants::TEXTURE_NEAREST_SAMPLINGMODE:
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
    case Constants::TEXTURE_NEAREST_NEAREST:
      magFilter = GL::NEAREST;
      minFilter = GL::NEAREST;
      break;
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
    case Constants::TEXTURE_LINEAR_LINEAR:
      magFilter = GL::LINEAR;
      minFilter = GL::LINEAR;
      break;
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
  const std::string& forcedExtension, const std::vector<IInternalTextureLoaderPtr>& excludeLoaders,
  const std::string& mimeType)
{
  // assign a new string, so that the original is still available in case of fallback
  auto url      = urlArg;
  auto fromData = url.substr(0, 5) == "data:";
  auto fromBlob = url.substr(0, 5) == "blob:";
  auto isBase64 = fromData && String::contains(url, ";base64,");

  auto texture = fallback ? fallback : InternalTexture::New(this, InternalTexture::DATASOURCE_URL);

  // establish the file extension, if possible
  auto lastDot = String::lastIndexOf(url, ".");
  auto extension
    = !forcedExtension.empty() ?
        forcedExtension :
        (lastDot > -1 ? String::toLowerCase(url.substr(static_cast<size_t>(lastDot))) : "");
  auto filteredFormat = excludedCompressedTextureFormats(url, _textureFormatInUse);

  IInternalTextureLoaderPtr loader = nullptr;
  for (const auto& availableLoader : ThinEngine::_TextureLoaders) {
    if (!stl_util::contains(excludeLoaders, availableLoader)
        && availableLoader->canLoad(extension, filteredFormat, fallback, isBase64,
                                    buffer ? true : false)) {
      loader = availableLoader;
      break;
    }
  }

  if (loader) {
    url = loader->transformUrl(url, filteredFormat);
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
      auto fallbackUrl = loader->getFallbackTextureUrl(url, _textureFormatInUse);
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
    const auto callback = [=](const std::variant<std::string, ArrayBuffer>& data,
                              const std::string & /*responseURL*/) -> void {
      loader->loadData(std::get<ArrayBuffer>(data), texture,
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
          onError("Unable to load: only ArrayBuffer supported here", "");
        }
      }
    }
  }
  else {
    auto onload = [=](const Image& img) {
      if (fromBlob && !_doNotHandleContextLost) {
        // We need to store the image if we need to rebuild the texture in case of a webgl context
        // lost
        texture->_buffer = img;
      }

      _prepareWebGLTexture(
        texture, scene, img.width, img.height, texture->invertY, noMipmap, false,
        [&](int potWidth, int potHeight, const std::function<void()>& continuationCallback) {
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
              || Engine::_RescalePostProcessFactory == nullptr) {
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
            auto source = InternalTexture::New(this, InternalTexture::DATASOURCE_TEMP);
            _bindTextureDirectly(GL::TEXTURE_2D, source);
            _gl->texImage2D(GL::TEXTURE_2D, 0, static_cast<int>(internalFormat), img.width,
                            img.height, 0, GL::RGBA, GL::UNSIGNED_BYTE, &img.data);

            _rescaleTexture(source, texture, scene, internalFormat, [&]() {
              _releaseTexture(source.get());
              _bindTextureDirectly(GL::TEXTURE_2D, texture);

              continuationCallback();
            });
          }

          return true;
        },
        samplingMode);
    };

    if (!fromData || isBase64) {
      if (url.empty()) {
        // onload(buffer);
      }
      else {
        ThinEngine::_FileToolsLoadImage(url, onload, onInternalError, invertY);
      }
    }
    else if (buffer.has_value()
             && (std::holds_alternative<std::string>(*buffer)
                 || std::holds_alternative<ArrayBuffer>(*buffer)
                 || std::holds_alternative<Image>(*buffer))) {
      ThinEngine::_FileToolsLoadImage(*buffer, invertY, onload, onInternalError);
    }
  }

  return texture;
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

unsigned int ThinEngine::_getTextureTarget(InternalTexture* texture) const
{
  if (texture->isCube) {
    return GL::TEXTURE_CUBE_MAP;
  }
  else if (texture->is3D) {
    return GL::TEXTURE_3D;
  }
  else if (texture->is2DArray) {
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

void ThinEngine::updateTextureWrappingMode(InternalTexture* texture, std::optional<int> wrapU,
                                           std::optional<int> wrapV, std::optional<int> wrapR)
{
  const auto target = _getTextureTarget(texture);

  if (wrapU) {
    _setTextureParameterInteger(target, GL::TEXTURE_WRAP_S, _getTextureWrapMode(*wrapU), texture);
    texture->_cachedWrapU = *wrapU;
  }
  if (wrapV) {
    _setTextureParameterInteger(target, GL::TEXTURE_WRAP_T, _getTextureWrapMode(*wrapV), texture);
    texture->_cachedWrapV = *wrapV;
  }
  if (wrapR) {
    _setTextureParameterInteger(target, GL::TEXTURE_WRAP_R, _getTextureWrapMode(*wrapR), texture);
    texture->_cachedWrapR = *wrapR;
  }

  _bindTextureDirectly(target, nullptr);
}

void ThinEngine::_setupDepthStencilTexture(InternalTexture* internalTexture,
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
        static_cast<int>(_getRGBABufferInternalSizedFormat(texture->type, format)) :
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

  gl.bindRenderbuffer(GL::RENDERBUFFER, depthStencilBuffer);

  if (samples > 1) {
    gl.renderbufferStorageMultisample(GL::RENDERBUFFER, samples, msInternalFormat, width, height);
  }
  else {
    gl.renderbufferStorage(GL::RENDERBUFFER, internalFormat, width, height);
  }

  gl.framebufferRenderbuffer(GL::FRAMEBUFFER, attachment, GL::RENDERBUFFER,
                             depthStencilBuffer.get());
  return depthStencilBuffer;
}

void ThinEngine::_releaseFramebufferObjects(InternalTexture* texture)
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
    texture->_MSAARenderBuffer.reset(nullptr);
  }
}

void ThinEngine::_releaseTexture(InternalTexture* texture)
{
  _releaseFramebufferObjects(texture);

  _deleteTexture(texture->_webGLTexture);

  // Unbind channels
  unbindAllTextures();

  stl_util::remove_vector_elements_equal_sharedptr(_internalTexturesCache, texture);

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
  _gl->uniform1i(uniform, destination);
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

  _activeChannel = channel;
  if (internalTexture && internalTexture->isMultiview) {
    if (needToBind) {
      _bindTextureDirectly(GL::TEXTURE_2D_ARRAY, internalTexture, isPartOfTextureArray);
    }
  }
  else if (internalTexture && (internalTexture->is3D || internalTexture->is2DArray)) {
    auto is3D   = internalTexture->is3D;
    auto target = is3D ? GL::TEXTURE_3D : GL::TEXTURE_2D_ARRAY;

    if (needToBind) {
      _bindTextureDirectly(target, internalTexture, isPartOfTextureArray);
    }

    if (internalTexture && internalTexture->_cachedWrapU != static_cast<int>(texture->wrapU)) {
      internalTexture->_cachedWrapU = static_cast<int>(texture->wrapU);
      _setTextureParameterInteger(target, GL::TEXTURE_WRAP_S,
                                  static_cast<int>(_getTextureWrapMode(texture->wrapU)),
                                  internalTexture);
    }

    if (internalTexture && internalTexture->_cachedWrapV != static_cast<int>(texture->wrapV)) {
      internalTexture->_cachedWrapV = static_cast<int>(texture->wrapV);
      _setTextureParameterInteger(target, GL::TEXTURE_WRAP_T,
                                  static_cast<int>(_getTextureWrapMode(texture->wrapV)),
                                  internalTexture);
    }

    if (is3D && internalTexture
        && internalTexture->_cachedWrapR != static_cast<int>(texture->wrapR)) {
      internalTexture->_cachedWrapR = static_cast<int>(texture->wrapR);
      _setTextureParameterInteger(target, GL::TEXTURE_WRAP_R,
                                  static_cast<int>(_getTextureWrapMode(texture->wrapR)),
                                  internalTexture);
    }

    _setAnisotropicLevel(target, texture);
  }
  else if (internalTexture && internalTexture->isCube) {
    if (needToBind) {
      _bindTextureDirectly(GL::TEXTURE_CUBE_MAP, internalTexture, isPartOfTextureArray);
    }

    if (internalTexture->_cachedCoordinatesMode != static_cast<int>(texture->coordinatesMode())) {
      internalTexture->_cachedCoordinatesMode = static_cast<int>(texture->coordinatesMode());
      // CUBIC_MODE and SKYBOX_MODE both require CLAMP_TO_EDGE.  All other modes use REPEAT.
      auto textureWrapMode = (texture->coordinatesMode() != Constants::TEXTURE_CUBIC_MODE
                              && texture->coordinatesMode() != Constants::TEXTURE_SKYBOX_MODE) ?
                               GL::REPEAT :
                               GL::CLAMP_TO_EDGE;
      _setTextureParameterInteger(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_S,
                                  static_cast<int>(textureWrapMode), internalTexture);
      _setTextureParameterInteger(GL::TEXTURE_CUBE_MAP, GL::TEXTURE_WRAP_T,
                                  static_cast<int>(textureWrapMode));
    }

    _setAnisotropicLevel(GL::TEXTURE_CUBE_MAP, texture);
  }
  else {
    if (needToBind) {
      _bindTextureDirectly(GL::TEXTURE_2D, internalTexture, isPartOfTextureArray);
    }

    if (internalTexture && internalTexture->_cachedWrapU != static_cast<int>(texture->wrapU)) {
      internalTexture->_cachedWrapU = static_cast<int>(texture->wrapU);
      _setTextureParameterInteger(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S,
                                  static_cast<int>(_getTextureWrapMode(texture->wrapU)),
                                  internalTexture);
    }

    if (internalTexture && internalTexture->_cachedWrapV != static_cast<int>(texture->wrapV)) {
      internalTexture->_cachedWrapV = static_cast<int>(texture->wrapV);
      _setTextureParameterInteger(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T,
                                  static_cast<int>(_getTextureWrapMode(texture->wrapV)),
                                  internalTexture);
    }

    _setAnisotropicLevel(GL::TEXTURE_2D, texture);
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

void ThinEngine::_setAnisotropicLevel(unsigned int target, const BaseTexturePtr& texture)
{
  auto internalTexture = texture->getInternalTexture();

  if (!internalTexture) {
    return;
  }

  auto anisotropicFilterExtension = _caps.textureAnisotropicFilterExtension;
  auto value                      = texture->anisotropicFilteringLevel;

  if (internalTexture->samplingMode != Constants::TEXTURE_LINEAR_LINEAR_MIPNEAREST
      && internalTexture->samplingMode != Constants::TEXTURE_LINEAR_LINEAR_MIPLINEAR
      && internalTexture->samplingMode != Constants::TEXTURE_LINEAR_LINEAR) {
    value = 1; // Forcing the anisotropic to 1 because else webgl will force filters to linear
  }

  if (anisotropicFilterExtension
      && internalTexture->_cachedAnisotropicFilteringLevel != static_cast<int>(value)) {
    _setTextureParameterFloat(target, AnisotropicFilterExtension::TEXTURE_MAX_ANISOTROPY_EXT,
                              static_cast<float>(std::min(value, _caps.maxAnisotropy)),
                              internalTexture);
    internalTexture->_cachedAnisotropicFilteringLevel = static_cast<int>(value);
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

    for (int i = 0; i < _caps.maxVertexAttribs; i++) {
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
  if (onBeforeTextureInitObservable) {
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
  _boundRenderFunction   = nullptr;

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
  while (gl.getError() != GL::NO_ERROR) {
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
  successful = successful && (gl.getError() == GL::NO_ERROR);

  // try render by clearing frame buffer's color buffer
  if (successful) {
    gl.clear(GL::COLOR_BUFFER_BIT);
    successful = successful && (gl.getError() == GL::NO_ERROR);
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
    successful = successful && (gl.getError() == GL::NO_ERROR);
  }

  // clean up
  gl.deleteTexture(texture.get());
  gl.deleteFramebuffer(fb.get());
  gl.bindFramebuffer(GL::FRAMEBUFFER, nullptr);

  // clear accumulated errors
  while (!successful && (gl.getError() != GL::NO_ERROR)) {
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
  const auto _format = format.value_or(std::numeric_limits<unsigned>::max());

  if (_webGLVersion == 1.f) {
    if (format.has_value()) {
      switch (_format) {
        case Constants::TEXTUREFORMAT_ALPHA:
          return GL::ALPHA;
        case Constants::TEXTUREFORMAT_LUMINANCE:
          return GL::LUMINANCE;
        case Constants::TEXTUREFORMAT_LUMINANCE_ALPHA:
          return GL::LUMINANCE_ALPHA;
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

} // end of namespace BABYLON
