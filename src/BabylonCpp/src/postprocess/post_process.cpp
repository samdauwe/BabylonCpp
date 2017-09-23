#include <babylon/postprocess/post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

PostProcess::PostProcess(
  const string_t& iName, const string_t& fragmentUrl,
  const vector_t<string_t>& parameters, const vector_t<string_t>& samplers,
  float renderRatio, Camera* camera, unsigned int samplingMode, Engine* engine,
  bool reusable, const string_t& defines, unsigned int textureType,
  const string_t& vertexUrl,
  const unordered_map_t<string_t, unsigned int>& indexParameters,
  bool blockCompilation)
    : PostProcess(iName, fragmentUrl, parameters, samplers, {-1, -1}, camera,
                  samplingMode, engine, reusable, defines, textureType,
                  vertexUrl, indexParameters, blockCompilation)
{
  _renderRatio = renderRatio;
}

PostProcess::PostProcess(
  const string_t& iName, const string_t& fragmentUrl,
  const vector_t<string_t>& parameters, const vector_t<string_t>& samplers,
  const PostProcessOptions& options, Camera* camera, unsigned int samplingMode,
  Engine* engine, bool reusable, const string_t& defines,
  unsigned int textureType, const string_t& vertexUrl,
  const unordered_map_t<string_t, unsigned int>& indexParameters,
  bool blockCompilation)
    : name{iName}
    , width{-1}
    , height{-1}
    , autoClear{true}
    , alphaMode{EngineConstants::ALPHA_DISABLE}
    , enablePixelPerfectMode{false}
    , scaleMode{EngineConstants::SCALEMODE_FLOOR}
    , alwaysForcePOT{false}
    , samples{1}
    , _currentRenderTextureInd{0}
    , _renderRatio{1.f}
    , _options{options}
    , _reusable{false}
    , _fragmentUrl{fragmentUrl}
    , _vertexUrl{vertexUrl}
    , _parameters{parameters}
    , _scaleRatio{Vector2(1.f, 1.f)}
    , _shareOutputWithPostProcess{nullptr}
    , _texelSize{Vector2::Zero()}
    , _forcedOutputTexture{nullptr}
{
  if (camera) {
    _camera = camera;
    _scene  = camera->getScene();
    _camera->attachPostProcess(this);
    _engine = _scene->getEngine();
  }
  else {
    _engine = engine;
  }

  renderTargetSamplingMode = samplingMode;
  _reusable                = reusable || false;
  _textureType             = textureType;

  _samplers.insert(_samplers.end(), samplers.begin(), samplers.end());
  _samplers.emplace_back("textureSampler");

  _parameters.insert(_parameters.end(), _parameters.begin(), _parameters.end());
  _parameters.emplace_back("scale");

  _indexParameters = indexParameters;

  if (!blockCompilation) {
    updateEffect(defines);
  }
}

PostProcess::~PostProcess()
{
}

void PostProcess::setOnActivate(
  const ::std::function<void(Camera* camera, EventState&)>& callback)
{
  if (_onActivateObserver) {
    onActivateObservable.remove(_onActivateObserver);
  }
  _onActivateObserver = onActivateObservable.add(callback);
}

void PostProcess::setOnSizeChanged(
  const ::std::function<void(PostProcess* postProcess, EventState&)>& callback)
{
  if (_onSizeChangedObserver) {
    onSizeChangedObservable.remove(_onSizeChangedObserver);
  }
  _onSizeChangedObserver = onSizeChangedObservable.add(callback);
}

void PostProcess::setOnApply(
  const ::std::function<void(Effect* effect, EventState&)>& callback)
{
  if (_onApplyObserver) {
    onApplyObservable.remove(_onApplyObserver);
  }
  _onApplyObserver = onApplyObservable.add(callback);
}

void PostProcess::setOnBeforeRender(
  const ::std::function<void(Effect* effect, EventState&)>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
}

void PostProcess::setOnAfterRender(
  const ::std::function<void(Effect* effect, EventState&)>& callback)
{
  if (_onAfterRenderObserver) {
    onAfterRenderObservable.remove(_onAfterRenderObserver);
  }
  _onAfterRenderObserver = onAfterRenderObservable.add(callback);
}

InternalTexture* PostProcess::outputTexture()
{
  return _textures[_currentRenderTextureInd];
}

void PostProcess::setOutputTexture(InternalTexture* value)
{
  _forcedOutputTexture = value;
}

Camera* PostProcess::getCamera()
{
  return _camera;
}

Vector2 PostProcess::texelSize()
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

Engine* PostProcess::getEngine()
{
  return _engine;
}

Effect* PostProcess::getEffect()
{
  return _effect;
}

PostProcess& PostProcess::shareOutputWith(PostProcess* postProcess)
{
  _disposeTextures();
  _shareOutputWithPostProcess = postProcess;

  return *this;
}

void PostProcess::updateEffect(
  const string_t& defines, const vector_t<string_t>& uniforms,
  const vector_t<string_t>& samplers,
  const unordered_map_t<string_t, unsigned int>& indexParameters,
  const ::std::function<void(Effect* effect)>& onCompiled,
  const ::std::function<void(Effect* effect, const string_t& errors)>& onError)
{
  unordered_map_t<string_t, string_t> baseName{{"vertex", _vertexUrl},
                                               {"fragment", _fragmentUrl}};

  EffectCreationOptions options;
  options.attributes    = {"position"};
  options.uniformsNames = !uniforms.empty() ? uniforms : _parameters;
  options.samplers      = !samplers.empty() ? samplers : _samplers;
  options.defines       = defines;
  options.onCompiled    = onCompiled;
  options.onError       = onError;
  options.indexParameters
    = !indexParameters.empty() ? indexParameters : _indexParameters;

  _effect = _engine->createEffect(baseName, options, _scene->getEngine());
}

bool PostProcess::isReusable() const
{
  return _reusable;
}

void PostProcess::markTextureDirty()
{
  width = -1;
}

void PostProcess::activate(Camera* camera, InternalTexture* sourceTexture,
                           bool forceDepthStencil)
{
  if (!_shareOutputWithPostProcess && !_forcedOutputTexture) {
    auto pCamera = camera ? camera : _camera;

    auto scene  = pCamera->getScene();
    auto engine = scene->getEngine();

    const int maxSize = engine->getCaps().maxTextureSize;

    const int requiredWidth = static_cast<int>(
      static_cast<float>(sourceTexture ? sourceTexture->width :
                                         _engine->getRenderingCanvas()->width)
      * _renderRatio);
    const int requiredHeight = static_cast<int>(
      static_cast<float>(sourceTexture ? sourceTexture->height :
                                         _engine->getRenderingCanvas()->height)
      * _renderRatio);

    int desiredWidth = _options.width == -1 ? requiredWidth : _options.width;
    int desiredHeight
      = _options.height == -1 ? requiredHeight : _options.height;

    if (renderTargetSamplingMode != TextureConstants::TRILINEAR_SAMPLINGMODE
        || alwaysForcePOT) {
      if (_options.width <= 0) {
        desiredWidth
          = engine->needPOTTextures() ?
              Tools::GetExponentOfTwo(desiredWidth, maxSize, scaleMode) :
              desiredWidth;
      }

      if (_options.height <= 0) {
        desiredHeight
          = engine->needPOTTextures() ?
              Tools::GetExponentOfTwo(desiredHeight, maxSize, scaleMode) :
              desiredHeight;
      }
    }

    if (width != desiredWidth || height != desiredHeight) {
      if (!_textures.empty()) {
        for (auto& texture : _textures) {
          _engine->_releaseTexture(texture);
        }
        _textures.clear();
      }
      width  = desiredWidth;
      height = desiredHeight;

      auto textureSize = ISize(width, height);
      IRenderTargetOptions textureOptions;
      textureOptions.generateMipMaps = false;
      textureOptions.generateDepthBuffer
        = forceDepthStencil
          || (stl_util::index_of(pCamera->_postProcesses, this) == 0);
      textureOptions.generateStencilBuffer
        = (forceDepthStencil
           || (stl_util::index_of(pCamera->_postProcesses, this) == 0))
          && _engine->isStencilEnable();
      textureOptions.samplingMode = renderTargetSamplingMode;
      textureOptions.type         = _textureType;

      _textures.emplace_back(
        _engine->createRenderTargetTexture(textureSize, textureOptions));

      if (_reusable) {
        _textures.emplace_back(
          _engine->createRenderTargetTexture(textureSize, textureOptions));
      }

      _texelSize.copyFromFloats(1.f / width, 1.f / height);

      onSizeChangedObservable.notifyObservers(this);
    }

    for (auto& texture : _textures) {
      if (texture->samples != samples) {
        _engine->updateRenderTargetTextureSampleCount(texture, samples);
      }
    }

    InternalTexture* target = nullptr;
    if (_shareOutputWithPostProcess) {
      target = _shareOutputWithPostProcess->outputTexture();
    }
    else if (_forcedOutputTexture) {
      target = _forcedOutputTexture;
      width  = _forcedOutputTexture->width;
      height = _forcedOutputTexture->height;
    }
    else {
      target = outputTexture();
    }

    if (enablePixelPerfectMode) {
      _scaleRatio.copyFromFloats(
        static_cast<float>(requiredWidth) / static_cast<float>(desiredWidth),
        static_cast<float>(requiredHeight) / static_cast<float>(desiredHeight));
      _engine->bindFramebuffer(target, 0, requiredWidth, requiredHeight, true);
    }
    else {
      _scaleRatio.copyFromFloats(1.f, 1.f);
      _engine->bindFramebuffer(target);
    }

    onActivateObservable.notifyObservers(camera);

    // Clear
    if (autoClear && alphaMode == EngineConstants::ALPHA_DISABLE) {
      _engine->clear(clearColor ? *clearColor : scene->clearColor, true, true,
                     true);
    }

    if (_reusable) {
      _currentRenderTextureInd = (_currentRenderTextureInd + 1) % 2;
    }
  }
}

bool PostProcess::isSupported() const
{
  return _effect->isSupported();
}

float PostProcess::aspectRatio() const
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

Effect* PostProcess::apply()
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
  _engine->setAlphaMode(static_cast<int>(alphaMode));
  if (alphaConstants) {
    const auto& _alphaConstants = *alphaConstants;
    getEngine()->setAlphaConstants(_alphaConstants.r, _alphaConstants.g,
                                   _alphaConstants.b, _alphaConstants.a);
  }

  // Texture
  InternalTexture* source = nullptr;
  if (_shareOutputWithPostProcess) {
    source = _shareOutputWithPostProcess->outputTexture();
  }
  else if (_forcedOutputTexture) {
    source = _forcedOutputTexture;
  }
  else {
    source = outputTexture();
  }
  _effect->_bindTexture("textureSampler", source);

  // Parameters
  _effect->setVector2("scale", _scaleRatio);
  onApplyObservable.notifyObservers(_effect);

  return _effect;
}

void PostProcess::_disposeTextures()
{
  if (_shareOutputWithPostProcess || _forcedOutputTexture) {
    return;
  }

  if (!_textures.empty()) {
    for (auto& texture : _textures) {
      _engine->_releaseTexture(texture);
    }
  }

  _textures.clear();
}

void PostProcess::dispose(Camera* camera)
{
  auto pCamera = camera ? camera : _camera;

  _disposeTextures();

  if (!pCamera) {
    return;
  }
  pCamera->detachPostProcess(this);

  const int index = stl_util::index_of(pCamera->_postProcesses, this);
  if (index == 0 && pCamera->_postProcesses.size() > 0) {
    _camera->_postProcesses[0]->markTextureDirty();
  }

  onActivateObservable.clear();
  onAfterRenderObservable.clear();
  onApplyObservable.clear();
  onBeforeRenderObservable.clear();
  onSizeChangedObservable.clear();
}

} // end of namespace BABYLON
