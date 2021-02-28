#include <babylon/postprocesses/post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/misc/string_tools.h>
#include <babylon/rendering/pre_pass_effect_configuration.h>
#include <babylon/rendering/pre_pass_renderer.h>

namespace BABYLON {

PostProcess::PostProcess(const std::string& iName, const std::string& fragmentUrl,
                         const std::vector<std::string>& parameters,
                         const std::vector<std::string>& samplers,
                         const std::variant<float, PostProcessOptions>& options,
                         const CameraPtr& camera, const std::optional<unsigned int>& samplingMode,
                         Engine* engine, bool reusable, const std::string& defines,
                         unsigned int textureType, const std::string& vertexUrl,
                         const std::unordered_map<std::string, unsigned int>& indexParameters,
                         bool blockCompilation, unsigned int textureFormat)
    : width{-1}
    , height{-1}
    , nodeMaterialSource{nullptr}
    , _outputTexture{nullptr}
    , autoClear{true}
    , alphaMode{Constants::ALPHA_DISABLE}
    , enablePixelPerfectMode{false}
    , forceFullscreenViewport{true}
    , scaleMode{Constants::SCALEMODE_FLOOR}
    , alwaysForcePOT{false}
    , samples{this, &PostProcess::get_samples, &PostProcess::set_samples}
    , onActivate{this, &PostProcess::set_onActivate}
    , onSizeChanged{this, &PostProcess::set_onSizeChanged}
    , onApply{this, &PostProcess::set_onApply}
    , onBeforeRender{this, &PostProcess::set_onBeforeRender}
    , onAfterRender{this, &PostProcess::set_onAfterRender}
    , inputTexture{this, &PostProcess::get_inputTexture, &PostProcess::set_inputTexture}
    , texelSize{this, &PostProcess::get_texelSize}
    , isSupported{this, &PostProcess::get_isSupported}
    , aspectRatio{this, &PostProcess::get_aspectRatio}
    , adaptScaleToCurrentViewport{false}
    , _forcedOutputTexture{nullptr}
    , _currentRenderTextureInd{0}
    , _prePassEffectConfiguration{nullptr}
    , _scene{nullptr}
    , _samples{1}
    , _camera{nullptr}
    , _engine{nullptr}
    , _renderRatio{1.f}
    , _reusable{false}
    , _renderId{0}
    , _parameters{parameters}
    , _scaleRatio{Vector2(1.f, 1.f)}
    , _shareOutputWithPostProcess{nullptr}
    , _texelSize{Vector2::Zero()}
    , _blockCompilation{blockCompilation}
    , _defines{defines}
    , _onActivateObserver{nullptr}
    , _onSizeChangedObserver{nullptr}
    , _onApplyObserver{nullptr}
    , _onBeforeRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
{
  name = iName;
  if (camera) {
    _camera = camera;
    _scene  = camera->getScene();
    _engine = _scene->getEngine();
  }
  else if (engine) {
    _engine = engine;
  }

  _options                 = options;
  renderTargetSamplingMode = samplingMode.value_or(Constants::TEXTURE_NEAREST_SAMPLINGMODE);
  _reusable                = reusable;
  _textureType             = textureType;
  _textureFormat           = textureFormat;

  _samplers.insert(_samplers.end(), samplers.begin(), samplers.end());
  _samplers.emplace_back("textureSampler");

  _fragmentUrl = fragmentUrl;
  _vertexUrl   = !vertexUrl.empty() ? vertexUrl : "postprocess";
  _parameters.insert(_parameters.end(), _parameters.begin(), _parameters.end());

  _parameters.emplace_back("scale");

  _indexParameters = indexParameters;
}

PostProcess::~PostProcess() = default;

void PostProcess::add(const PostProcessPtr& newPostProcess)
{
  if (_camera) {
    _camera->attachPostProcess(newPostProcess);

    _engine->postProcesses.emplace_back(newPostProcess);
    uniqueId = _scene->getUniqueId();
  }
  else if (_engine) {
    _engine->postProcesses.emplace_back(newPostProcess);
  }

  if (!newPostProcess->_blockCompilation) {
    updateEffect(newPostProcess->_defines);
  }
}

std::string PostProcess::getEffectName() const
{
  return _fragmentUrl;
}

unsigned int PostProcess::get_samples() const
{
  return _samples;
}

void PostProcess::set_samples(unsigned int n)
{
  _samples = std::min(n, _engine->getCaps().maxMSAASamples);

  for (const auto& texture : _textures) {
    if (texture->samples != _samples) {
      _engine->updateRenderTargetTextureSampleCount(texture, _samples);
    }
  }
}

void PostProcess::set_onActivate(const std::function<void(Camera* camera, EventState&)>& callback)
{
  if (_onActivateObserver) {
    onActivateObservable.remove(_onActivateObserver);
  }
  if (callback) {
    _onActivateObserver = onActivateObservable.add(callback);
  }
}

void PostProcess::set_onSizeChanged(
  const std::function<void(PostProcess* postProcess, EventState&)>& callback)
{
  if (_onSizeChangedObserver) {
    onSizeChangedObservable.remove(_onSizeChangedObserver);
  }
  _onSizeChangedObserver = onSizeChangedObservable.add(callback);
}

void PostProcess::set_onApply(const std::function<void(Effect* effect, EventState&)>& callback)
{
  if (_onApplyObserver) {
    onApplyObservable.remove(_onApplyObserver);
  }
  _onApplyObserver = onApplyObservable.add(callback);
}

void PostProcess::set_onBeforeRender(
  const std::function<void(Effect* effect, EventState&)>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
}

void PostProcess::set_onAfterRender(
  const std::function<void(Effect* effect, EventState&)>& callback)
{
  if (_onAfterRenderObserver) {
    onAfterRenderObservable.remove(_onAfterRenderObserver);
  }
  _onAfterRenderObserver = onAfterRenderObservable.add(callback);
}

InternalTexturePtr& PostProcess::get_inputTexture()
{
  return _textures[_currentRenderTextureInd];
}

void PostProcess::set_inputTexture(const InternalTexturePtr& value)
{
  _forcedOutputTexture = value;
}

void PostProcess::restoreDefaultInputTexture()
{
  if (_forcedOutputTexture) {
    _forcedOutputTexture = nullptr;
    markTextureDirty();
  }
}

CameraPtr& PostProcess::getCamera()
{
  return _camera;
}

Vector2& PostProcess::get_texelSize()
{
  if (_shareOutputWithPostProcess) {
    return _shareOutputWithPostProcess->texelSize();
  }

  if (_forcedOutputTexture) {
    _texelSize.copyFromFloats(1.f / _forcedOutputTexture->width,
                              1.f / _forcedOutputTexture->height);
  }

  return _texelSize;
}

std::string PostProcess::getClassName() const
{
  return "PostProcess";
}

std::string PostProcess::toString() const
{
  return "";
}

Engine* PostProcess::getEngine()
{
  return _engine;
}

const EffectPtr& PostProcess::getEffect() const
{
  return _effect;
}

PostProcess& PostProcess::shareOutputWith(const PostProcessPtr& postProcess)
{
  _disposeTextures();
  _shareOutputWithPostProcess = postProcess;

  return *this;
}

void PostProcess::useOwnOutput()
{
  if (_textures.empty()) {
    _textures.resize(2);
  }

  _shareOutputWithPostProcess = nullptr;
}

void PostProcess::updateEffect(
  const std::string& defines, const std::vector<std::string>& uniforms,
  const std::vector<std::string>& samplers,
  const std::unordered_map<std::string, unsigned int>& indexParameters,
  const std::function<void(Effect* effect)>& onCompiled,
  const std::function<void(Effect* effect, const std::string& errors)>& onError,
  const std::string& vertexUrl, const std::string& fragmentUrl)
{
  _postProcessDefines = defines;

  std::unordered_map<std::string, std::string> baseName{
    {"vertex", !vertexUrl.empty() ? vertexUrl : _vertexUrl},
    {"fragment", !fragmentUrl.empty() ? fragmentUrl : _fragmentUrl}};

  IEffectCreationOptions options;
  options.attributes      = {"position"};
  options.uniformsNames   = !uniforms.empty() ? uniforms : _parameters;
  options.samplers        = !samplers.empty() ? samplers : _samplers;
  options.defines         = !defines.empty() ? defines : "";
  options.onCompiled      = onCompiled;
  options.onError         = onError;
  options.indexParameters = !indexParameters.empty() ? indexParameters : _indexParameters;

  _effect = _engine->createEffect(baseName, options, _scene ? _scene->getEngine() : _engine);
}

bool PostProcess::isReusable() const
{
  return _reusable;
}

void PostProcess::markTextureDirty()
{
  width = -1;
}

InternalTexturePtr
PostProcess::_createRenderTargetTexture(const RenderTargetSize& textureSize,
                                        const IRenderTargetOptions& textureOptions,
                                        unsigned int channel)
{
  for (const auto& textureCacheItem : _textureCache) {
    if (textureCacheItem.texture->width == textureSize.width
        && textureCacheItem.texture->height == textureSize.height
        && textureCacheItem.postProcessChannel == channel) {
      return textureCacheItem.texture;
    }
  }

  const auto tex = _engine->createRenderTargetTexture(textureSize, textureOptions);
  _textureCache.emplace_back(TextureCache{
    tex,     // texture
    channel, // postProcessChannel
    -1,      // lastUsedRenderId
  });

  return tex;
}

void PostProcess::_flushTextureCache()
{
  const auto currentRenderId = _renderId;

  for (auto it = _textureCache.begin(); it != _textureCache.end();) {
    const auto& textureCacheItem = *it;
    if (currentRenderId - textureCacheItem.lastUsedRenderId > 100) {
      auto currentlyUsed = false;
      for (auto j = 0ull; j < _textures.size(); ++j) {
        if (_textures[j] == textureCacheItem.texture) {
          currentlyUsed = true;
          break;
        }
      }

      if (!currentlyUsed) {
        _engine->_releaseTexture(textureCacheItem.texture);
        it = _textureCache.erase(it);
      }
      else {
        ++it;
      }
    }
  }
}

void PostProcess::_resize(int iWidth, int iHeight, Camera* camera, bool needMipMaps,
                          bool forceDepthStencil)
{
  if (!_textures.empty()) {
    _textures.clear();
  }

  width  = iWidth;
  height = iHeight;

  auto textureSize = RenderTargetSize{width, height};
  IRenderTargetOptions textureOptions;
  textureOptions.generateMipMaps = needMipMaps;
  textureOptions.generateDepthBuffer
    = forceDepthStencil || (stl_util::index_of_raw_ptr(camera->_postProcesses, this) == 0);
  textureOptions.generateStencilBuffer
    = (forceDepthStencil || (stl_util::index_of_raw_ptr(camera->_postProcesses, this) == 0))
      && _engine->isStencilEnable();
  textureOptions.samplingMode = renderTargetSamplingMode;
  textureOptions.type         = _textureType;
  textureOptions.format       = _textureFormat;

  _textures.emplace_back(_engine->createRenderTargetTexture(textureSize, textureOptions /*, 0*/));

  if (_reusable) {
    _textures.emplace_back(_engine->createRenderTargetTexture(textureSize, textureOptions /*, 1*/));
  }

  _texelSize.copyFromFloats(1.f / width, 1.f / height);

  onSizeChangedObservable.notifyObservers(this);
}

InternalTexturePtr PostProcess::activate(const CameraPtr& camera,
                                         const InternalTexturePtr& sourceTexture,
                                         bool forceDepthStencil)
{
  auto pCamera = camera ? camera : _camera;

  auto scene         = pCamera->getScene();
  auto engine        = scene->getEngine();
  const auto maxSize = engine->getCaps().maxTextureSize;

  const auto requiredWidth = static_cast<int>(
    static_cast<float>(sourceTexture ? sourceTexture->width : _engine->getRenderingCanvas()->width)
    * _renderRatio);
  const auto requiredHeight
    = static_cast<int>(static_cast<float>(sourceTexture ? sourceTexture->height :
                                                          _engine->getRenderingCanvas()->height)
                       * _renderRatio);

  auto desiredWidth  = std::holds_alternative<PostProcessOptions>(_options) ?
                         std::get<PostProcessOptions>(_options).width :
                         requiredWidth;
  auto desiredHeight = std::holds_alternative<PostProcessOptions>(_options) ?
                         std::get<PostProcessOptions>(_options).height :
                         requiredHeight;

  const auto needMipMaps = renderTargetSamplingMode != Constants::TEXTURE_NEAREST_LINEAR
                           && renderTargetSamplingMode != Constants::TEXTURE_NEAREST_NEAREST
                           && renderTargetSamplingMode != Constants::TEXTURE_LINEAR_LINEAR;

  if (!_shareOutputWithPostProcess && !_forcedOutputTexture) {

    if (adaptScaleToCurrentViewport) {
      auto currentViewport = engine->currentViewport();

      if (currentViewport) {
        desiredWidth  = static_cast<int>(desiredWidth * currentViewport->width);
        desiredHeight = static_cast<int>(desiredHeight * currentViewport->height);
      }
    }

    if (needMipMaps || alwaysForcePOT) {
      if (!std::holds_alternative<PostProcessOptions>(_options)) {
        desiredWidth = engine->needPOTTextures() ?
                         Engine::GetExponentOfTwo(desiredWidth, maxSize, scaleMode) :
                         desiredWidth;
      }

      if (!std::holds_alternative<PostProcessOptions>(_options)) {
        desiredHeight = engine->needPOTTextures() ?
                          Engine::GetExponentOfTwo(desiredHeight, maxSize, scaleMode) :
                          desiredHeight;
      }
    }

    if (width != desiredWidth || height != desiredHeight) {
      _resize(desiredWidth, desiredHeight, pCamera.get(), needMipMaps, forceDepthStencil);
    }

    for (const auto& texture : _textures) {
      if (texture->samples != samples) {
        _engine->updateRenderTargetTextureSampleCount(texture, samples);
      }
    }

    _flushTextureCache();
    _renderId++;
  }

  InternalTexturePtr target = nullptr;
  if (_shareOutputWithPostProcess) {
    target = _shareOutputWithPostProcess->inputTexture();
  }
  else if (_forcedOutputTexture) {
    target = _forcedOutputTexture;

    width  = _forcedOutputTexture->width;
    height = _forcedOutputTexture->height;
  }
  else {
    target = inputTexture();

    std::optional<TextureCache> cache = std::nullopt;
    for (const auto& textureCacheItem : _textureCache) {
      if (textureCacheItem.texture == target) {
        cache = textureCacheItem;
        break;
      }
    }

    if (cache) {
      cache->lastUsedRenderId = _renderId;
    }
  }

  // Bind the input of this post process to be used as the output of the previous post process.
  if (enablePixelPerfectMode) {
    _scaleRatio.copyFromFloats(static_cast<float>(requiredWidth) / static_cast<float>(desiredWidth),
                               static_cast<float>(requiredHeight)
                                 / static_cast<float>(desiredHeight));
    _engine->bindFramebuffer(target, 0u, requiredWidth, requiredHeight, forceFullscreenViewport);
  }
  else {
    _scaleRatio.copyFromFloats(1.f, 1.f);
    _engine->bindFramebuffer(target, 0u, std::nullopt, std::nullopt, forceFullscreenViewport);
  }

  _engine->_debugInsertMarker(StringTools::printf("post process %s input", name.c_str()));

  onActivateObservable.notifyObservers(camera.get());

  // Clear
  if (autoClear && alphaMode == Constants::ALPHA_DISABLE) {
    _engine->clear(clearColor ? *clearColor : scene->clearColor, scene->_allowPostProcessClearColor,
                   true, true);
  }

  if (_reusable) {
    _currentRenderTextureInd = (_currentRenderTextureInd + 1) % 2;
  }
  return target;
}

bool PostProcess::get_isSupported() const
{
  return _effect->isSupported();
}

float PostProcess::get_aspectRatio() const
{
  if (_shareOutputWithPostProcess) {
    return _shareOutputWithPostProcess->aspectRatio();
  }

  if (_forcedOutputTexture) {
    auto size = static_cast<float>(_forcedOutputTexture->width)
                / static_cast<float>(_forcedOutputTexture->height);
    return size;
  }

  return static_cast<float>(width) / static_cast<float>(height);
}

bool PostProcess::isReady() const
{
  return _effect && _effect->isReady();
}

EffectPtr PostProcess::apply()
{
  // Check
  if (!_effect || !_effect->isReady()) {
    return nullptr;
  }

  // States
  _engine->enableEffect(_effect);
  _engine->setState(false);
  _engine->setDepthBuffer(false);
  _engine->setDepthWrite(false);

  // Alpha
  _engine->setAlphaMode(alphaMode);
  if (alphaConstants) {
    const auto& _alphaConstants = *alphaConstants;
    getEngine()->setAlphaConstants(_alphaConstants.r, _alphaConstants.g, _alphaConstants.b,
                                   _alphaConstants.a);
  }

  // Bind the output texture of the previous post process as the input to this post process.
  InternalTexturePtr source = nullptr;
  if (_shareOutputWithPostProcess) {
    source = _shareOutputWithPostProcess->inputTexture();
  }
  else if (_forcedOutputTexture) {
    source = _forcedOutputTexture;
  }
  else {
    if (!_textures.empty()) {
      source = inputTexture();
    }
  }
  _effect->_bindTexture("textureSampler", source);

  // Parameters
  _effect->setVector2("scale", _scaleRatio);
  onApplyObservable.notifyObservers(_effect.get());

  return _effect;
}

void PostProcess::_disposeTextures()
{
  if (_shareOutputWithPostProcess || _forcedOutputTexture) {
    _disposeTextureCache();
    return;
  }

  _disposeTextureCache();
  _textures.clear();
}

void PostProcess::_disposeTextureCache()
{
  for (auto i = _textureCache.size(); i-- > 0;) {
    _engine->_releaseTexture(_textureCache[i].texture);
  }

  _textureCache.clear();
}

bool PostProcess::setPrePassRenderer(const PrePassRendererPtr& prePassRenderer)
{
  if (_prePassEffectConfiguration) {
    _prePassEffectConfiguration
      = prePassRenderer->addEffectConfiguration(_prePassEffectConfiguration);
    _prePassEffectConfiguration->enabled = true;
    return true;
  }

  return false;
}

void PostProcess::dispose(Camera* camera)
{
  auto pCamera = camera ? camera : _camera.get();

  _disposeTextures();

  if (_scene) {
    stl_util::remove_vector_elements_equal_sharedptr(_scene->postProcesses, this);
  }
  else {
    stl_util::remove_vector_elements_equal_sharedptr(_engine->postProcesses, this);
  }

  if (!pCamera) {
    return;
  }
  pCamera->detachPostProcess(this);

  const auto index = stl_util::index_of_raw_ptr(pCamera->_postProcesses, this);
  if (index == 0 && !pCamera->_postProcesses.empty()) {
    auto firstPostProcess = _camera->_getFirstPostProcess();
    if (firstPostProcess) {
      firstPostProcess->markTextureDirty();
    }
  }

  onActivateObservable.clear();
  onAfterRenderObservable.clear();
  onApplyObservable.clear();
  onBeforeRenderObservable.clear();
  onSizeChangedObservable.clear();
}

json PostProcess::serialize() const
{
  return nullptr;
}

PostProcessPtr PostProcess::clone() const
{
  return nullptr;
}

PostProcessPtr PostProcess::Parse(const json& /*parsedPostProcess*/, Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
