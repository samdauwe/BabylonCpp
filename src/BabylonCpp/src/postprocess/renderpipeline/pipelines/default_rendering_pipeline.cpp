#include <babylon/postprocess/renderpipeline/pipelines/default_rendering_pipeline.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json.h>
#include <babylon/core/variant.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/depth_of_field_effect.h>
#include <babylon/postprocess/fxaa_post_process.h>
#include <babylon/postprocess/highlights_post_process.h>
#include <babylon/postprocess/image_processing_post_process.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>

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
    , _bloomEnabled{false}
    , _depthOfFieldEnabled{false}
    , _depthOfFieldBlurLevel{DepthOfFieldEffectBlurLevel::Low}
    , _fxaaEnabled{false}
    , _imageProcessingEnabled{true}
    , _bloomScale{0.6f}
    , _buildAllowed{automaticBuild}
    , _bloomWeight{0.15f}
{
  _cameras = cameras;

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

  _buildPipeline();
}

DefaultRenderingPipeline::~DefaultRenderingPipeline()
{
}

void DefaultRenderingPipeline::setBloomWeight(float value)
{
  if (stl_util::almost_equal(_bloomWeight, value)) {
    return;
  }
  _bloomWeight = value;

  if (_hdr && copyBack) {
    copyBack->alphaConstants = Color4(value, value, value, value);
  }
}

float DefaultRenderingPipeline::bloomWeight() const
{
  return _bloomWeight;
}

void DefaultRenderingPipeline::setBloomScale(float value)
{
  if (stl_util::almost_equal(_bloomScale, value)) {
    return;
  }
  _bloomScale = value;

  _buildPipeline();
}

float DefaultRenderingPipeline::bloomScale() const
{
  return _bloomScale;
}

void DefaultRenderingPipeline::setBloomEnabled(bool enabled)
{
  if (_bloomEnabled == enabled) {
    return;
  }
  _bloomEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::bloomEnabled() const
{
  return _bloomEnabled;
}

bool DefaultRenderingPipeline::depthOfFieldEnabled() const
{
  return _depthOfFieldEnabled;
}

void DefaultRenderingPipeline::setDepthOfFieldEnabled(bool enabled)
{
  if (_depthOfFieldEnabled == enabled) {
    return;
  }
  _depthOfFieldEnabled = enabled;

  _buildPipeline();
}

DepthOfFieldEffectBlurLevel
DefaultRenderingPipeline::depthOfFieldBlurLevel() const
{
  return _depthOfFieldBlurLevel;
}

void DefaultRenderingPipeline::setDepthOfFieldBlurLevel(
  DepthOfFieldEffectBlurLevel value)
{
  if (_depthOfFieldBlurLevel == value) {
    return;
  }
  _depthOfFieldBlurLevel = value;

  _buildPipeline();
}

void DefaultRenderingPipeline::setFxaaEnabled(bool enabled)
{
  if (_fxaaEnabled == enabled) {
    return;
  }
  _fxaaEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::fxaaEnabled() const
{
  return _fxaaEnabled;
}

void DefaultRenderingPipeline::setImageProcessingEnabled(bool enabled)
{
  if (_imageProcessingEnabled == enabled) {
    return;
  }
  _imageProcessingEnabled = enabled;

  _buildPipeline();
}

bool DefaultRenderingPipeline::imageProcessingEnabled() const
{
  return _imageProcessingEnabled;
}

void DefaultRenderingPipeline::prepare()
{
  auto previousState = _buildAllowed;
  _buildAllowed      = true;
  _buildPipeline();
  _buildAllowed = previousState;
}

void DefaultRenderingPipeline::_buildPipeline()
{
  if (!_buildAllowed) {
    return;
  }

  auto engine = _scene->getEngine();

  _disposePostProcesses();
  _reset();

  if (depthOfFieldEnabled()) {
    depthOfField = new DepthOfFieldEffect(_scene, _depthOfFieldBlurLevel,
                                          _defaultPipelineTextureType);
    addEffect(depthOfField);
  }

  if (bloomEnabled()) {
    pass = new PassPostProcess("sceneRenderTarget", 1.0, nullptr,
                               TextureConstants::BILINEAR_SAMPLINGMODE, engine,
                               false, _defaultPipelineTextureType);
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

  if (fxaaEnabled()) {
    fxaa = new FxaaPostProcess("fxaa", 1.0, nullptr,
                               TextureConstants::BILINEAR_SAMPLINGMODE, engine,
                               false, _defaultPipelineTextureType);
    addEffect(new PostProcessRenderEffect(
      engine, FxaaPostProcessId,
      [&]() -> vector_t<PostProcess*> { return {fxaa}; }, true));

    fxaa->autoClear = !bloomEnabled() && (!_hdr || !imageProcessing);
  }
  else if (_hdr && imageProcessing) {
    finalMerge = imageProcessing;
  }
  else {
    finalMerge = new PassPostProcess(
      "finalMerge", 1.0, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
      engine, false, _defaultPipelineTextureType);
    addEffect(new PostProcessRenderEffect(
      engine, FinalMergePostProcessId,
      [&]() -> vector_t<PostProcess*> { return {finalMerge}; }, true));

    finalMerge->autoClear = !bloomEnabled() && (!_hdr || !imageProcessing);
  }

  if (bloomEnabled()) {
    if (_hdr) { // Share render targets to save memory
      copyBack->shareOutputWith(blurX);
      if (imageProcessing) {
        imageProcessing->shareOutputWith(pass);
        imageProcessing->autoClear = false;
      }
      else if (fxaa) {
        fxaa->shareOutputWith(pass);
      }
      else {
        finalMerge->shareOutputWith(pass);
      }
    }
    else {
      if (fxaa) {
        fxaa->shareOutputWith(pass);
      }
      else {
        finalMerge->shareOutputWith(pass);
      }
    }
  }

  if (!_cameras.empty()) {
    _scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(
      _name, stl_util::extract_values(_cameras));
  }
}

void DefaultRenderingPipeline::_disposePostProcesses()
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

    if (depthOfField) {
      depthOfField->disposeEffects(camera);
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
}

void DefaultRenderingPipeline::dispose(bool /*doNotRecurse*/,
                                       bool /*disposeMaterialAndTextures*/)
{
  _disposePostProcesses();

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
