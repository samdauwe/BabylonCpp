#include <babylon/postprocess/renderpipeline/pipelines/default_rendering_pipeline.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/core/variant.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
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
  const string_t& iName, bool hdr, Scene* scene,
  const unordered_map_t<string_t, Camera*>& cameras, bool automaticBuild)
    : PostProcessRenderPipeline(scene->getEngine(), iName)
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
    , chromaticAberrationEnabled{this,
                                 &DefaultRenderingPipeline::
                                   get_chromaticAberrationEnabled,
                                 &DefaultRenderingPipeline::
                                   set_chromaticAberrationEnabled}
    , grainEnabled{this, &DefaultRenderingPipeline::get_grainEnabled,
                   &DefaultRenderingPipeline::set_grainEnabled}
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
{
  _cameras         = cameras;
  _originalCameras = stl_util::extract_values(_cameras);

  // Initialize
  _scene           = scene;
  const auto& caps = scene->getEngine()->getCaps();
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

  // Attach
  scene->postProcessRenderPipelineManager()->addPipeline(this);

  auto engine = _scene->getEngine();
  // Create post processes before hand so they can be modified before enabled.
  // Block compilation flag is set to true to avoid compilation prior to use,
  // these will be updated on first use in build pipeline.
  sharpen = new SharpenPostProcess(
    "sharpen", ToVariant<float, PostProcessOptions>(1.f), nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, engine, false,
    _defaultPipelineTextureType, true);
  _sharpenEffect = new PostProcessRenderEffect(
    engine, SharpenPostProcessId,
    [this]() -> vector_t<PostProcess*> { return {sharpen}; }, true);

  depthOfField = new DepthOfFieldEffect(_scene, nullptr, _depthOfFieldBlurLevel,
                                        _defaultPipelineTextureType, true);

  bloom = new BloomEffect("", _scene, _bloomScale, _bloomWeight, bloomKernel,
                          _defaultPipelineTextureType, true);

  chromaticAberration = new ChromaticAberrationPostProcess(
    "ChromaticAberration", engine->getRenderWidth(), engine->getRenderHeight(),
    ToVariant<float, PostProcessOptions>(1.f), nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, engine, false,
    _defaultPipelineTextureType, true);
  _chromaticAberrationEffect = new PostProcessRenderEffect(
    engine, ChromaticAberrationPostProcessId,
    [this]() -> vector_t<PostProcess*> { return {chromaticAberration}; }, true);

  grain
    = new GrainPostProcess("Grain", ToVariant<float, PostProcessOptions>(1.f),
                           nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
                           engine, false, _defaultPipelineTextureType, true);
  _grainEffect = new PostProcessRenderEffect(
    engine, GrainPostProcessId,
    [this]() -> vector_t<PostProcess*> { return {grain}; }, true);

  _resizeObserver = engine->onResizeObservable.add(
    [this](Engine* engine, EventState& /*es*/) {
      _hardwareScaleLevel = engine->getHardwareScalingLevel();
      bloomKernel         = bloomKernel();
    });

  _imageProcessingConfigurationObserver
    = _scene->imageProcessingConfiguration()->onUpdateParameters.add(
      [this](ImageProcessingConfiguration* /*ipc*/, EventState& /*es*/) {
        bloom->_downscale->_exposure
          = _scene->imageProcessingConfiguration()->exposure();
      });

  _buildPipeline();
}

DefaultRenderingPipeline::~DefaultRenderingPipeline()
{
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
  bloom = new BloomEffect("", _scene, bloomScale, _bloomWeight, bloomKernel,
                          _defaultPipelineTextureType, false);
  bloom->threshold = oldBloom->threshold();
  for (auto& item : _cameras) {
    oldBloom->disposeEffects(item.second);
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

  depthOfField = new DepthOfFieldEffect(_scene, nullptr, _depthOfFieldBlurLevel,
                                        _defaultPipelineTextureType, false);
  depthOfField->focalLength   = oldDof->focalLength();
  depthOfField->focusDistance = oldDof->focusDistance();
  depthOfField->fStop         = oldDof->fStop();
  depthOfField->lensSize      = oldDof->lensSize();

  for (auto& camera : _cameras) {
    oldDof->disposeEffects(camera.second);
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
  PostProcess* postProcess, bool skipTextureSharing)
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

  auto engine = _scene->getEngine();

  _disposePostProcesses();
  if (!_cameras.empty()) {
    _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
      _name, stl_util::extract_values(_cameras));
    // get back cameras to be used to reattach pipeline
    _cameras.clear();
    for (auto& camera : _originalCameras) {
      _cameras[camera->name] = camera;
    }
  }
  _reset();
  _prevPostProcess     = nullptr;
  _prevPrevPostProcess = nullptr;
  _hasCleared          = false;

  if (depthOfFieldEnabled) {
    // TODO use indexing instead of "0"
    auto depthTexture
      = _scene->enableDepthRenderer(_cameras["0"])->getDepthMap();
    depthOfField->depthTexture = depthTexture;
    if (!depthOfField->_isReady()) {
      depthOfField->_updateEffects();
    }
    addEffect(depthOfField);
    _setAutoClearAndTextureSharing(depthOfField->_effects[0], true);
  }

  if (bloomEnabled) {
    if (!bloom->_isReady()) {
      bloom->_updateEffects();
    }
    addEffect(bloom);
    _setAutoClearAndTextureSharing(bloom->_effects[0], true);
  }

  if (_imageProcessingEnabled) {
    imageProcessing = new ImageProcessingPostProcess(
      "imageProcessing", 1.0, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
      engine, false, _defaultPipelineTextureType);
    if (_hdr) {
      addEffect(new PostProcessRenderEffect(
        engine, ImageProcessingPostProcessId,
        [this]() -> vector_t<PostProcess*> { return {imageProcessing}; },
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
    fxaa = new FxaaPostProcess("fxaa", 1.0, nullptr,
                               TextureConstants::BILINEAR_SAMPLINGMODE, engine,
                               false, _defaultPipelineTextureType);
    addEffect(new PostProcessRenderEffect(
      engine, FxaaPostProcessId,
      [this]() -> vector_t<PostProcess*> { return {fxaa}; }, true));
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
    auto camera = item.second;

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
  }
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

Json::object DefaultRenderingPipeline::serialize() const
{
  return Json::object();
}

unique_ptr_t<DefaultRenderingPipeline>
DefaultRenderingPipeline::Parse(const Json::value& /*source*/, Scene* /*scene*/,
                                const string_t& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
