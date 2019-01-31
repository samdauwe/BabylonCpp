#include <babylon/postprocess/renderpipeline/pipelines/default_rendering_pipeline.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/layer/glow_layer.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/postprocess/bloom_effect.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/chromatic_aberration_post_process.h>
#include <babylon/postprocess/depth_of_field_effect.h>
#include <babylon/postprocess/extract_highlights_post_process.h>
#include <babylon/postprocess/fxaa_post_process.h>
#include <babylon/postprocess/grain_post_process.h>
#include <babylon/postprocess/highlights_post_process.h>
#include <babylon/postprocess/image_processing_post_process.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/postprocess/sharpen_post_process.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

DefaultRenderingPipeline::DefaultRenderingPipeline(
  const std::string& iName, bool hdr, Scene* scene,
  const std::unordered_map<std::string, CameraPtr>& cameras,
  bool automaticBuild)
    : PostProcessRenderPipeline(scene ? scene->getEngine() :
                                        Engine::LastCreatedScene()->getEngine(),
                                iName)
    , fxaa{nullptr}
    , imageProcessing{nullptr}
    , bloomKernel{this, &DefaultRenderingPipeline::get_bloomKernel,
                  &DefaultRenderingPipeline::set_bloomKernel}
    , sharpenEnabled{this, &DefaultRenderingPipeline::get_sharpenEnabled,
                     &DefaultRenderingPipeline::set_sharpenEnabled}
    , bloomWeight{this, &DefaultRenderingPipeline::get_bloomWeight,
                  &DefaultRenderingPipeline::set_bloomWeight}
    , bloomThreshold{this, &DefaultRenderingPipeline::get_bloomThreshold,
                     &DefaultRenderingPipeline::set_bloomThreshold}
    , bloomScale{this, &DefaultRenderingPipeline::get_bloomScale,
                 &DefaultRenderingPipeline::set_bloomScale}
    , bloomEnabled{this, &DefaultRenderingPipeline::get_bloomEnabled,
                   &DefaultRenderingPipeline::set_bloomEnabled}
    , depthOfFieldEnabled{this,
                          &DefaultRenderingPipeline::get_depthOfFieldEnabled,
                          &DefaultRenderingPipeline::set_depthOfFieldEnabled}
    , depthOfFieldBlurLevel{this,
                            &DefaultRenderingPipeline::
                              get_depthOfFieldBlurLevel,
                            &DefaultRenderingPipeline::
                              set_depthOfFieldBlurLevel}
    , fxaaEnabled{this, &DefaultRenderingPipeline::get_fxaaEnabled,
                  &DefaultRenderingPipeline::set_fxaaEnabled}
    , samples{this, &DefaultRenderingPipeline::get_samples,
              &DefaultRenderingPipeline::set_samples}
    , imageProcessingEnabled{this,
                             &DefaultRenderingPipeline::
                               get_imageProcessingEnabled,
                             &DefaultRenderingPipeline::
                               set_imageProcessingEnabled}
    , glowLayerEnabled{this, &DefaultRenderingPipeline::get_glowLayerEnabled,
                       &DefaultRenderingPipeline::set_glowLayerEnabled}
    , chromaticAberrationEnabled{this,
                                 &DefaultRenderingPipeline::
                                   get_chromaticAberrationEnabled,
                                 &DefaultRenderingPipeline::
                                   set_chromaticAberrationEnabled}
    , grainEnabled{this, &DefaultRenderingPipeline::get_grainEnabled,
                   &DefaultRenderingPipeline::set_grainEnabled}
    , _glowLayer{nullptr}
    , _imageProcessingConfigurationObserver{nullptr}
    , _sharpenEnabled{false}
    , _bloomEnabled{false}
    , _depthOfFieldEnabled{false}
    , _depthOfFieldBlurLevel{DepthOfFieldEffectBlurLevel::Low}
    , _fxaaEnabled{false}
    , _imageProcessingEnabled{true}
    , _bloomScale{0.5f}
    , _chromaticAberrationEnabled{false}
    , _grainEnabled{false}
    , _buildAllowed{automaticBuild}
    , _resizeObserver{nullptr}
    , _hardwareScaleLevel{1.f}
    , _bloomKernel{64.f}
    , _bloomWeight{0.15f}
    , _bloomThreshold{0.9f}
    , _samples{1}
    , _hasCleared{false}
    , _prevPostProcess{nullptr}
    , _prevPrevPostProcess{nullptr}
    , _depthOfFieldSceneObserver{nullptr}
{
  _cameras             = cameras;
  _camerasToBeAttached = stl_util::extract_values(_cameras);

  // Initialize
  _scene           = scene ? scene : Engine::LastCreatedScene();
  const auto& caps = _scene->getEngine()->getCaps();
  _hdr             = hdr;

  // Misc
  if (hdr) {
    if (caps.textureHalfFloatRender) {
      _defaultPipelineTextureType = EngineConstants::TEXTURETYPE_HALF_FLOAT;
    }
    else if (caps.textureFloatRender) {
      _defaultPipelineTextureType = EngineConstants::TEXTURETYPE_FLOAT;
    }
  }
  else {
    _defaultPipelineTextureType = EngineConstants::TEXTURETYPE_UNSIGNED_INT;
  }

  auto iEngine = _scene->getEngine();
  // Create post processes before hand so they can be modified before enabled.
  // Block compilation flag is set to true to avoid compilation prior to use,
  // these will be updated on first use in build pipeline.
  sharpen = SharpenPostProcess::New(
    "sharpen", 1.f, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, iEngine,
    false, _defaultPipelineTextureType, true);
  _sharpenEffect = PostProcessRenderEffect::New(
    iEngine, SharpenPostProcessId,
    [this]() -> std::vector<PostProcessPtr> { return {sharpen}; }, true);

  depthOfField = DepthOfFieldEffect::New(
    _scene, nullptr, _depthOfFieldBlurLevel, _defaultPipelineTextureType, true);

  bloom = BloomEffect::New("", _scene, _bloomScale, _bloomWeight, bloomKernel,
                           _defaultPipelineTextureType, true);

  chromaticAberration = ChromaticAberrationPostProcess::New(
    "ChromaticAberration", iEngine->getRenderWidth(),
    iEngine->getRenderHeight(), 1.f, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, iEngine, false,
    _defaultPipelineTextureType, true);
  _chromaticAberrationEffect = PostProcessRenderEffect::New(
    iEngine, ChromaticAberrationPostProcessId,
    [this]() -> std::vector<PostProcessPtr> { return {chromaticAberration}; },
    true);

  grain = GrainPostProcess::New(
    "Grain", 1.f, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, iEngine,
    false, _defaultPipelineTextureType, true);
  _grainEffect = PostProcessRenderEffect::New(
    iEngine, GrainPostProcessId,
    [this]() -> std::vector<PostProcessPtr> { return {grain}; }, true);

  _resizeObserver = iEngine->onResizeObservable.add(
    [this](Engine* iEngine, EventState& /*es*/) {
      _hardwareScaleLevel = iEngine->getHardwareScalingLevel() * 1.f;
      bloomKernel         = bloomKernel();
    });

  _imageProcessingConfigurationObserver
    = _scene->imageProcessingConfiguration()->onUpdateParameters.add(
      [this](ImageProcessingConfiguration* /*ipc*/, EventState& /*es*/) {
        bloom->_downscale->_exposure
          = _scene->imageProcessingConfiguration()->exposure();
      });
}

DefaultRenderingPipeline::~DefaultRenderingPipeline()
{
}

void DefaultRenderingPipeline::addToScene(
  const DefaultRenderingPipelinePtr& renderingPipeline)
{
  // Attach
  _scene->postProcessRenderPipelineManager()->addPipeline(renderingPipeline);

  // Build the pipeline
  renderingPipeline->_buildPipeline();
}

void DefaultRenderingPipeline::set_sharpenEnabled(bool enabled)
{
  if (_sharpenEnabled == enabled) {
    return;
  }
  _sharpenEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::get_sharpenEnabled() const
{
  return _sharpenEnabled;
}

void DefaultRenderingPipeline::set_bloomKernel(float value)
{
  _bloomKernel  = value;
  bloom->kernel = value / _hardwareScaleLevel;
}

float DefaultRenderingPipeline::get_bloomKernel() const
{
  return _bloomKernel;
}

void DefaultRenderingPipeline::set_bloomWeight(float value)
{
  if (stl_util::almost_equal(_bloomWeight, value)) {
    return;
  }

  bloom->weight = value;

  _bloomWeight = value;
}

float DefaultRenderingPipeline::get_bloomWeight() const
{
  return _bloomWeight;
}

void DefaultRenderingPipeline::set_bloomThreshold(float value)
{
  if (stl_util::almost_equal(_bloomThreshold, value)) {
    return;
  }
  bloom->threshold = value;
  _bloomThreshold  = value;
}

float DefaultRenderingPipeline::get_bloomThreshold() const
{
  return _bloomThreshold;
}

void DefaultRenderingPipeline::set_bloomScale(float value)
{
  if (stl_util::almost_equal(_bloomScale, value)) {
    return;
  }
  _bloomScale = value;

  // recreate bloom and dispose old as this setting is not dynamic
  _rebuildBloom();

  _buildPipeline();
}

float DefaultRenderingPipeline::get_bloomScale() const
{
  return _bloomScale;
}

void DefaultRenderingPipeline::set_bloomEnabled(bool enabled)
{
  if (_bloomEnabled == enabled) {
    return;
  }
  _bloomEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::get_bloomEnabled() const
{
  return _bloomEnabled;
}

void DefaultRenderingPipeline::_rebuildBloom()
{
  // recreate bloom and dispose old as this setting is not dynamic
  auto oldBloom = bloom;
  bloom = BloomEffect::New("", _scene, bloomScale, _bloomWeight, bloomKernel,
                           _defaultPipelineTextureType, false);
  bloom->threshold = oldBloom->threshold();
  for (auto& item : _cameras) {
    oldBloom->disposeEffects(item.second.get());
  }
}

bool DefaultRenderingPipeline::get_depthOfFieldEnabled() const
{
  return _depthOfFieldEnabled;
}

void DefaultRenderingPipeline::set_depthOfFieldEnabled(bool enabled)
{
  if (_depthOfFieldEnabled == enabled) {
    return;
  }
  _depthOfFieldEnabled = enabled;

  _buildPipeline();
}

DepthOfFieldEffectBlurLevel&
DefaultRenderingPipeline::get_depthOfFieldBlurLevel()
{
  return _depthOfFieldBlurLevel;
}

void DefaultRenderingPipeline::set_depthOfFieldBlurLevel(
  const DepthOfFieldEffectBlurLevel& value)
{
  if (_depthOfFieldBlurLevel == value) {
    return;
  }
  _depthOfFieldBlurLevel = value;

  // recreate dof and dispose old as this setting is not dynamic
  auto oldDof = depthOfField;

  depthOfField
    = DepthOfFieldEffect::New(_scene, nullptr, _depthOfFieldBlurLevel,
                              _defaultPipelineTextureType, false);
  depthOfField->focalLength   = oldDof->focalLength();
  depthOfField->focusDistance = oldDof->focusDistance();
  depthOfField->fStop         = oldDof->fStop();
  depthOfField->lensSize      = oldDof->lensSize();

  for (auto& camera : _cameras) {
    oldDof->disposeEffects(camera.second.get());
  }

  _buildPipeline();
}

void DefaultRenderingPipeline::set_fxaaEnabled(bool enabled)
{
  if (_fxaaEnabled == enabled) {
    return;
  }
  _fxaaEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::get_fxaaEnabled() const
{
  return _fxaaEnabled;
}

void DefaultRenderingPipeline::set_samples(unsigned int sampleCount)
{
  if (_samples == sampleCount) {
    return;
  }
  _samples = sampleCount;

  _buildPipeline();
}

unsigned int DefaultRenderingPipeline::get_samples() const
{
  return _samples;
}

void DefaultRenderingPipeline::set_imageProcessingEnabled(bool enabled)
{
  if (_imageProcessingEnabled == enabled) {
    return;
  }
  _imageProcessingEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::get_imageProcessingEnabled() const
{
  return _imageProcessingEnabled;
}

void DefaultRenderingPipeline::set_glowLayerEnabled(bool enabled)
{
  if (enabled && !_glowLayer) {
    _glowLayer = GlowLayer::New("", _scene);
  }
  else if (!enabled && _glowLayer) {
    _glowLayer->dispose();
    _glowLayer = nullptr;
  }
}

bool DefaultRenderingPipeline::get_glowLayerEnabled() const
{
  return _glowLayer == nullptr;
}

void DefaultRenderingPipeline::set_chromaticAberrationEnabled(bool enabled)
{
  if (_chromaticAberrationEnabled == enabled) {
    return;
  }
  _chromaticAberrationEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::get_chromaticAberrationEnabled() const
{
  return _chromaticAberrationEnabled;
}

void DefaultRenderingPipeline::set_grainEnabled(bool enabled)
{
  if (_grainEnabled == enabled) {
    return;
  }
  _grainEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::get_grainEnabled() const
{
  return _grainEnabled;
}

void DefaultRenderingPipeline::prepare()
{
  auto previousState = _buildAllowed;
  _buildAllowed      = true;
  _buildPipeline();
  _buildAllowed = previousState;
}

void DefaultRenderingPipeline::_setAutoClearAndTextureSharing(
  const PostProcessPtr& postProcess, bool skipTextureSharing)
{
  if (_hasCleared) {
    postProcess->autoClear = false;
  }
  else {
    postProcess->autoClear = true;
    _scene->autoClear      = false;
    _hasCleared            = true;
  }

  if (!skipTextureSharing) {
    if (_prevPrevPostProcess) {
      postProcess->shareOutputWith(_prevPrevPostProcess);
    }
    else {
      postProcess->useOwnOutput();
    }

    if (_prevPostProcess) {
      _prevPrevPostProcess = _prevPostProcess;
    }
    _prevPostProcess = postProcess;
  }
}

void DefaultRenderingPipeline::_buildPipeline()
{
  if (!_buildAllowed) {
    return;
  }

  _scene->autoClear = true;

  auto iEngine = _scene->getEngine();

  _disposePostProcesses();
  if (!_cameras.empty()) {
    _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
      _name, stl_util::extract_values(_cameras));
    // get back cameras to be used to reattach pipeline
    _cameras.clear();
    for (auto& camera : _camerasToBeAttached) {
      _cameras[camera->name] = camera;
    }
  }
  _reset();
  _prevPostProcess     = nullptr;
  _prevPrevPostProcess = nullptr;
  _hasCleared          = false;

  if (depthOfFieldEnabled) {
    // Multi camera suport
    if (_cameras.size() > 1) {
      for (const auto& camera : _cameras) {
        auto depthRenderer = _scene->enableDepthRenderer(camera.second);
        depthRenderer->useOnlyInActiveCamera = true;
      }
#if 0
      _depthOfFieldSceneObserver
        = _scene->onAfterRenderTargetsRenderObservable.add(
          [this](Scene* scene, EventState& /*es*/) {
            auto cameras = stl_util::extract_values(_cameras);
            if (stl_util::contains(_cameras, scene->activeCamera)) {
              depthOfField->depthTexture
                = scene->enableDepthRenderer(scene->activeCamera)
                    ->getDepthMap();
            }
          });
#endif
    }
    else {
      _scene->onAfterRenderTargetsRenderObservable.remove(
        _depthOfFieldSceneObserver);
      auto depthRenderer         = _scene->enableDepthRenderer(_cameras["0"]);
      depthOfField->depthTexture = depthRenderer->getDepthMap();
    }

    if (!depthOfField->_isReady()) {
      depthOfField->_updateEffects();
    }
    addEffect(depthOfField);
    _setAutoClearAndTextureSharing(depthOfField->_effects[0], true);
  }
  else {
    _scene->onAfterRenderTargetsRenderObservable.remove(
      _depthOfFieldSceneObserver);
  }

  if (bloomEnabled) {
    if (!bloom->_isReady()) {
      bloom->_updateEffects();
    }
    addEffect(bloom);
    _setAutoClearAndTextureSharing(bloom->_effects[0], true);
  }

  if (_imageProcessingEnabled) {
    imageProcessing = ImageProcessingPostProcess::New(
      "imageProcessing", 1.f, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
      iEngine, false, _defaultPipelineTextureType);
    if (_hdr) {
      addEffect(PostProcessRenderEffect::New(
        iEngine, ImageProcessingPostProcessId,
        [this]() -> std::vector<PostProcessPtr> { return {imageProcessing}; },
        true));
      _setAutoClearAndTextureSharing(imageProcessing);
    }
    else {
      _scene->imageProcessingConfiguration()->applyByPostProcess = false;
    }
  }

  if (sharpenEnabled) {
    if (!sharpen->isReady()) {
      sharpen->updateEffect();
    }
    addEffect(_sharpenEffect);
    _setAutoClearAndTextureSharing(sharpen);
  }

  if (grainEnabled) {
    if (!grain->isReady()) {
      grain->updateEffect();
    }
    addEffect(_grainEffect);
    _setAutoClearAndTextureSharing(grain);
  }

  if (chromaticAberrationEnabled) {
    if (!chromaticAberration->isReady()) {
      chromaticAberration->updateEffect();
    }
    addEffect(_chromaticAberrationEffect);
    _setAutoClearAndTextureSharing(chromaticAberration);
  }

  if (fxaaEnabled) {
    fxaa = FxaaPostProcess::New("fxaa", 1.f, nullptr,
                                TextureConstants::BILINEAR_SAMPLINGMODE,
                                iEngine, false, _defaultPipelineTextureType);
    addEffect(PostProcessRenderEffect::New(
      iEngine, FxaaPostProcessId,
      [this]() -> std::vector<PostProcessPtr> { return {fxaa}; }, true));
    _setAutoClearAndTextureSharing(fxaa, true);
  }

  if (!_cameras.empty()) {
    _scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(
      _name, stl_util::extract_values(_cameras));
  }

  if (!_enableMSAAOnFirstPostProcess(samples) && samples > 1) {
    BABYLON_LOG_WARN("DefaultRenderingPipeline",
                     "MSAA failed to enable, MSAA is only supported in "
                     "browsers that support webGL >= 2.0");
  }
}

void DefaultRenderingPipeline::_disposePostProcesses(bool disposeNonRecreated)
{
  for (auto& item : _cameras) {
    auto camera = item.second.get();

    if (imageProcessing) {
      imageProcessing->dispose(camera);
    }

    if (fxaa) {
      fxaa->dispose(camera);
    }

    // These are created in the constructor and should not be disposed on every
    // pipeline change
    if (disposeNonRecreated) {
      if (sharpen) {
        sharpen->dispose(camera);
      }

      if (depthOfField) {
        _scene->onAfterRenderTargetsRenderObservable.remove(
          _depthOfFieldSceneObserver);
        depthOfField->disposeEffects(camera);
      }

      if (bloom) {
        bloom->disposeEffects(camera);
      }

      if (chromaticAberration) {
        chromaticAberration->dispose(camera);
      }

      if (grain) {
        grain->dispose(camera);
      }

      if (_glowLayer) {
        _glowLayer->dispose();
      }
    }
  }

  imageProcessing = nullptr;
  fxaa            = nullptr;

  if (disposeNonRecreated) {
    sharpen                    = nullptr;
    _sharpenEffect             = nullptr;
    depthOfField               = nullptr;
    bloom                      = nullptr;
    chromaticAberration        = nullptr;
    _chromaticAberrationEffect = nullptr;
    grain                      = nullptr;
    _grainEffect               = nullptr;
    _glowLayer                 = nullptr;
  }
}

void DefaultRenderingPipeline::addCamera(Camera* camera)
{
  _camerasToBeAttached.emplace_back(camera);
  _buildPipeline();
}

void DefaultRenderingPipeline::removeCamera(Camera* camera)
{
  _camerasToBeAttached.erase(std::remove_if(_camerasToBeAttached.begin(),
                                            _camerasToBeAttached.end(),
                                            [camera](const CameraPtr& _camera) {
                                              return _camera.get() == camera;
                                            }),
                             _camerasToBeAttached.end());
  _buildPipeline();
}

void DefaultRenderingPipeline::dispose(bool /*doNotRecurse*/,
                                       bool /*disposeMaterialAndTextures*/)
{
  _disposePostProcesses(true);
  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
    _name, stl_util::extract_values(_cameras));
  _scene->autoClear = true;
  if (_resizeObserver) {
    _scene->getEngine()->onResizeObservable.remove(_resizeObserver);
    _resizeObserver = nullptr;
  }
  _scene->imageProcessingConfiguration()->onUpdateParameters.remove(
    _imageProcessingConfigurationObserver);
  PostProcessRenderPipeline::dispose();
}

json DefaultRenderingPipeline::serialize() const
{
  return nullptr;
}

std::unique_ptr<DefaultRenderingPipeline>
DefaultRenderingPipeline::Parse(const json& /*source*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
