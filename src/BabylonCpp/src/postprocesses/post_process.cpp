#include <babylon/postprocesses/post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/materials/textures/irender_target_options.h>

namespace BABYLON {

PostProcess::PostProcess(const std::string& iName, const std::string& fragmentUrl,
                         const std::vector<std::string>& parameters,
                         const std::vector<std::string>& samplers,
                         const std::variant<float, PostProcessOptions>& options,
                         const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
                         bool reusable, const std::string& defines, unsigned int textureType,
                         const std::string& vertexUrl,
                         const std::unordered_map<std::string, unsigned int>& indexParameters,
                         bool blockCompilation)
    : name{iName}
    , width{-1}
    , height{-1}
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
    , adaptScaleToCurrentViewport{false}
    , _currentRenderTextureInd{0}
    , _samples{1}
    , _camera{nullptr}
    , _scene{nullptr}
    , _engine{nullptr}
    , _renderRatio{1.f}
    , _reusable{false}
    , _parameters{parameters}
    , _scaleRatio{Vector2(1.f, 1.f)}
    , _shareOutputWithPostProcess{nullptr}
    , _texelSize{Vector2::Zero()}
    , _forcedOutputTexture{nullptr}
    , _blockCompilation{blockCompilation}
    , _defines{defines}
    , _onActivateObserver{nullptr}
    , _onSizeChangedObserver{nullptr}
    , _onApplyObserver{nullptr}
    , _onBeforeRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
{
  if (camera) {
    _camera = camera;
    _scene  = camera->getScene();
    _engine = _scene->getEngine();
  }
  else if (engine) {
    _engine = engine;
  }

  _options                 = options;
  renderTargetSamplingMode = samplingMode;
  _reusable                = reusable;
  _textureType             = textureType;

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

CameraPtr& PostProcess::getCamera()
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

std::string PostProcess::getClassName() const
{
  return "PostProcess";
}

Engine* PostProcess::getEngine()
{
  return _engine;
}

EffectPtr& PostProcess::getEffect()
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
  const std::function<void(Effect* effect, const std::string& errors)>& onError)
{
  std::unordered_map<std::string, std::string> baseName{{"vertex", _vertexUrl},
                                                        {"fragment", _fragmentUrl}};

  EffectCreationOptions options;
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

InternalTexturePtr PostProcess::activate(const CameraPtr& camera,
                                         const InternalTexturePtr& sourceTexture,
                                         bool forceDepthStencil)
{
  auto pCamera = camera ? camera : _camera;

  auto scene        = pCamera->getScene();
  auto engine       = scene->getEngine();
  const int maxSize = engine->getCaps().maxTextureSize;

  const int requiredWidth = static_cast<int>(
    static_cast<float>(sourceTexture ? sourceTexture->width : _engine->getRenderingCanvas()->width)
    * _renderRatio);
  const int requiredHeight
    = static_cast<int>(static_cast<float>(sourceTexture ? sourceTexture->height :
                                                          _engine->getRenderingCanvas()->height)
                       * _renderRatio);

  int desiredWidth = std::holds_alternative<PostProcessOptions>(_options) ?
                       std::get<PostProcessOptions>(_options).width :
                       requiredWidth;
  int desiredHeight = std::holds_alternative<PostProcessOptions>(_options) ?
                        std::get<PostProcessOptions>(_options).height :
                        requiredHeight;

  if (!_shareOutputWithPostProcess && !_forcedOutputTexture) {

    if (adaptScaleToCurrentViewport) {
      auto currentViewport = engine->currentViewport();

      if (currentViewport) {
        desiredWidth  = static_cast<int>(desiredWidth * currentViewport->width);
        desiredHeight = static_cast<int>(desiredHeight * currentViewport->height);
      }
    }

    if (renderTargetSamplingMode != Constants::TEXTURE_TRILINEAR_SAMPLINGMODE || alwaysForcePOT) {
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
      if (!_textures.empty()) {
        for (const auto& texture : _textures) {
          _engine->_releaseTexture(texture.get());
        }
        _textures.clear();
      }
      width  = desiredWidth;
      height = desiredHeight;

      auto textureSize = ISize(width, height);
      IRenderTargetOptions textureOptions;
      textureOptions.generateMipMaps = false;
      textureOptions.generateDepthBuffer
        = forceDepthStencil || (stl_util::index_of_raw_ptr(pCamera->_postProcesses, this) == 0);
      textureOptions.generateStencilBuffer
        = (forceDepthStencil || (stl_util::index_of_raw_ptr(pCamera->_postProcesses, this) == 0))
          && _engine->isStencilEnable();
      textureOptions.samplingMode = renderTargetSamplingMode;
      textureOptions.type         = _textureType;

      _textures.emplace_back(_engine->createRenderTargetTexture(textureSize, textureOptions));

      if (_reusable) {
        _textures.emplace_back(_engine->createRenderTargetTexture(textureSize, textureOptions));
      }

      _texelSize.copyFromFloats(1.f / width, 1.f / height);

      onSizeChangedObservable.notifyObservers(this);
    }

    for (auto& texture : _textures) {
      if (texture->samples != samples) {
        _engine->updateRenderTargetTextureSampleCount(texture, samples);
      }
    }
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
  }

  // Bind the input of this post process to be used as the output of the
  // previous post process.
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

  // Bind the output texture of the preivous post process as the input to this
  // post process.
  InternalTexturePtr source = nullptr;
  if (_shareOutputWithPostProcess) {
    source = _shareOutputWithPostProcess->inputTexture();
  }
  else if (_forcedOutputTexture) {
    source = _forcedOutputTexture;
  }
  else {
    if (!_textures.empty())
      source = inputTexture();
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
    return;
  }

  if (!_textures.empty()) {
    for (const auto& texture : _textures) {
      _engine->_releaseTexture(texture.get());
    }
  }

  _textures.clear();
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

} // end of namespace BABYLON
