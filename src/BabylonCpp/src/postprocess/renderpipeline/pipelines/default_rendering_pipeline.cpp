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
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/chromatic_aberration_post_process.h>
#include <babylon/postprocess/depth_of_field_effect.h>
#include <babylon/postprocess/fxaa_post_process.h>
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
    , pass{nullptr}
    , highlights{nullptr}
    , blurX{nullptr}
    , blurY{nullptr}
    , copyBack{nullptr}
    , fxaa{nullptr}
    , imageProcessing{nullptr}
    , finalMerge{nullptr}
    , bloomKernel{64.f}
    , sharpenEnabled{this, &DefaultRenderingPipeline::get_sharpenEnabled,
                     &DefaultRenderingPipeline::set_sharpenEnabled}
    , bloomWeight{this, &DefaultRenderingPipeline::get_bloomWeight,
                  &DefaultRenderingPipeline::set_bloomWeight}
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
    , msaaEnabled{this, &DefaultRenderingPipeline::get_msaaEnabled,
                  &DefaultRenderingPipeline::set_msaaEnabled}
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
    , _sharpenEnabled{false}
    , _bloomEnabled{false}
    , _depthOfFieldEnabled{false}
    , _depthOfFieldBlurLevel{DepthOfFieldEffectBlurLevel::Low}
    , _fxaaEnabled{false}
    , _msaaEnabled{false}
    , _imageProcessingEnabled{true}
    , _bloomScale{0.6f}
    , _chromaticAberrationEnabled{false}
    , _buildAllowed{automaticBuild}
    , _bloomWeight{0.15f}
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

  chromaticAberration = new ChromaticAberrationPostProcess(
    "ChromaticAberration", engine->getRenderWidth(), engine->getRenderHeight(),
    ToVariant<float, PostProcessOptions>(1.f), nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, engine, false,
    _defaultPipelineTextureType, true);
  _chromaticAberrationEffect = new PostProcessRenderEffect(
    engine, ChromaticAberrationPostProcessId,
    [this]() -> vector_t<PostProcess*> { return {chromaticAberration}; }, true);

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

void DefaultRenderingPipeline::set_bloomWeight(float value)
{
  if (stl_util::almost_equal(_bloomWeight, value)) {
    return;
  }
  _bloomWeight = value;

  if (_hdr && copyBack) {
    copyBack->alphaConstants = Color4(value, value, value, value);
  }
}

float DefaultRenderingPipeline::get_bloomWeight() const
{
  return _bloomWeight;
}

void DefaultRenderingPipeline::set_bloomScale(float value)
{
  if (stl_util::almost_equal(_bloomScale, value)) {
    return;
  }
  _bloomScale = value;

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
                                        _defaultPipelineTextureType);
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

void DefaultRenderingPipeline::set_msaaEnabled(bool enabled)
{
  if (_msaaEnabled == enabled) {
    return;
  }
  _msaaEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::get_msaaEnabled() const
{
  return _msaaEnabled;
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
  if (_prevPostProcess && _prevPostProcess->autoClear) {
    postProcess->autoClear = false;
  }
  else {
    postProcess->autoClear = true;
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

  if (sharpenEnabled) {
    if (!sharpen->isReady()) {
      sharpen->updateEffect();
    }
    addEffect(_sharpenEffect);
    _setAutoClearAndTextureSharing(sharpen);
  }

  if (depthOfFieldEnabled) {
    // TODO use indexing instead of "0"
    auto depthTexture
      = _scene->enableDepthRenderer(_cameras["0"])->getDepthMap();
    depthOfField->depthTexture = depthTexture;
    if (!depthOfField->_isReady()) {
      depthOfField->_updateEffects();
    }
    addEffect(depthOfField);
    // _setAutoClearAndTextureSharing(depthOfField->_depthOfFieldMerge);
  }

  if (bloomEnabled()) {
    pass = new PassPostProcess("sceneRenderTarget", 1.0, nullptr,
                               TextureConstants::BILINEAR_SAMPLINGMODE, engine,
                               false, _defaultPipelineTextureType);
    _setAutoClearAndTextureSharing(pass, true);
    addEffect(new PostProcessRenderEffect(
      engine, PassPostProcessId,
      [&]() -> vector_t<PostProcess*> { return {pass}; }, true));

    if (!_hdr) { // Need to enhance highlights if not using float rendering
      highlights
        = new HighlightsPostProcess("highlights", bloomScale(), nullptr,
                                    TextureConstants::BILINEAR_SAMPLINGMODE,
                                    engine, false, _defaultPipelineTextureType);
      addEffect(new PostProcessRenderEffect(
        engine, HighLightsPostProcessId,
        [&]() -> vector_t<PostProcess*> { return {highlights}; }, true));
      highlights->autoClear      = false;
      highlights->alwaysForcePOT = true;
    }

    blurX
      = new BlurPostProcess("horizontal blur", Vector2(1.f, 0.f), 10.f,
                            ToVariant<float, PostProcessOptions>(bloomScale()),
                            nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
                            engine, false, _defaultPipelineTextureType);
    addEffect(new PostProcessRenderEffect(
      engine, BlurXPostProcessId,
      [&]() -> vector_t<PostProcess*> { return {blurX}; }, true));
    blurX->alwaysForcePOT = true;
    blurX->autoClear      = false;
    blurX->onActivateObservable.add(
      [&](Camera* /*camera*/, EventState& /*es*/) {
        const auto dw = static_cast<float>(blurX->width)
                        / static_cast<float>(engine->getRenderWidth(true));
        blurX->kernel = bloomKernel * dw;
      });

    blurY
      = new BlurPostProcess("vertical blur", Vector2(0.f, 1.f), 10.0,
                            ToVariant<float, PostProcessOptions>(bloomScale()),
                            nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
                            engine, false, _defaultPipelineTextureType);
    addEffect(new PostProcessRenderEffect(
      engine, BlurYPostProcessId,
      [&]() -> vector_t<PostProcess*> { return {blurY}; }, true));
    blurY->alwaysForcePOT = true;
    blurY->autoClear      = false;
    blurY->onActivateObservable.add(
      [&](Camera* /*camera*/, EventState& /*es*/) {
        const auto dh = static_cast<float>(blurY->height)
                        / static_cast<float>(engine->getRenderHeight(true));
        blurY->kernel = bloomKernel * dh;
      });

    copyBack = new PassPostProcess("bloomBlendBlit", bloomScale(), nullptr,
                                   TextureConstants::BILINEAR_SAMPLINGMODE,
                                   engine, false, _defaultPipelineTextureType);
    addEffect(new PostProcessRenderEffect(
      engine, CopyBackPostProcessId,
      [&]() -> vector_t<PostProcess*> { return {copyBack}; }, true));
    copyBack->alwaysForcePOT = true;
    if (_hdr) {
      copyBack->alphaMode      = EngineConstants::ALPHA_INTERPOLATE;
      const auto w             = bloomWeight();
      copyBack->alphaConstants = Color4(w, w, w, w);
    }
    else {
      copyBack->alphaMode = EngineConstants::ALPHA_SCREENMODE;
    }
    copyBack->autoClear = false;
  }

  if (_imageProcessingEnabled) {
    imageProcessing = new ImageProcessingPostProcess(
      "imageProcessing", 1.0, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
      engine, false, _defaultPipelineTextureType);
    if (_hdr) {
      addEffect(new PostProcessRenderEffect(
        engine, ImageProcessingPostProcessId,
        [&]() -> vector_t<PostProcess*> { return {imageProcessing}; }, true));
    }
    else {
      _scene->imageProcessingConfiguration()->setApplyByPostProcess(false);
    }
  }

  if (_hdr && imageProcessing) {
    finalMerge = imageProcessing;
  }
  else {
    finalMerge = new PassPostProcess(
      "finalMerge", 1.0, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
      engine, false, _defaultPipelineTextureType);
    addEffect(new PostProcessRenderEffect(
      engine, FinalMergePostProcessId,
      [&]() -> vector_t<PostProcess*> { return {finalMerge}; }, true));

    _setAutoClearAndTextureSharing(finalMerge, true);

    finalMerge->autoClear = !bloomEnabled() && (!_hdr || !imageProcessing);
  }

  if (bloomEnabled()) {
    if (_hdr) { // Share render targets to save memory
      copyBack->shareOutputWith(blurX);
      if (imageProcessing) {
        imageProcessing->shareOutputWith(pass);
        imageProcessing->autoClear = false;
      }
      else {
        finalMerge->shareOutputWith(pass);
      }
    }
    else {
      finalMerge->shareOutputWith(pass);
    }
  }

  if (fxaaEnabled) {
    fxaa = new FxaaPostProcess("fxaa", 1.f, nullptr,
                               TextureConstants::BILINEAR_SAMPLINGMODE, engine,
                               false, _defaultPipelineTextureType);
    addEffect(new PostProcessRenderEffect(
      engine, FxaaPostProcessId,
      [this]() -> vector_t<PostProcess*> { return {fxaa}; }, true));
    _setAutoClearAndTextureSharing(fxaa);
  }

  if (chromaticAberrationEnabled) {
    if (!chromaticAberration->isReady()) {
      chromaticAberration->updateEffect();
    }
    addEffect(_chromaticAberrationEffect);
    _setAutoClearAndTextureSharing(chromaticAberration);
  }

  if (!_cameras.empty()) {
    _scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(
      _name, stl_util::extract_values(_cameras));
  }

  if (msaaEnabled) {
    if (!_enableMSAAOnFirstPostProcess()) {
      BABYLON_LOG_WARN("DefaultRenderingPipeline",
                       "MSAA failed to enable, MSAA is only supported in "
                       "browsers that support webGL >= 2.0");
    }
  }
}

void DefaultRenderingPipeline::_disposePostProcesses(bool disposeNonRecreated)
{
  for (auto& item : _cameras) {
    auto camera = item.second;

    if (pass) {
      pass->dispose(camera);
    }

    if (highlights) {
      highlights->dispose(camera);
    }

    if (blurX) {
      blurX->dispose(camera);
    }

    if (blurY) {
      blurY->dispose(camera);
    }

    if (copyBack) {
      copyBack->dispose(camera);
    }

    if (imageProcessing) {
      imageProcessing->dispose(camera);
    }

    if (fxaa) {
      fxaa->dispose(camera);
    }

    if (finalMerge) {
      finalMerge->dispose(camera);
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

      if (chromaticAberration) {
        chromaticAberration->dispose(camera);
      }
    }
  }

  pass            = nullptr;
  highlights      = nullptr;
  blurX           = nullptr;
  blurY           = nullptr;
  copyBack        = nullptr;
  imageProcessing = nullptr;
  fxaa            = nullptr;
  finalMerge      = nullptr;
  depthOfField    = nullptr;

  if (disposeNonRecreated) {
    sharpen             = nullptr;
    depthOfField        = nullptr;
    chromaticAberration = nullptr;
  }
}

void DefaultRenderingPipeline::dispose(bool /*doNotRecurse*/,
                                       bool /*disposeMaterialAndTextures*/)
{
  _disposePostProcesses(true);

  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
    _name, getCameras());

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
