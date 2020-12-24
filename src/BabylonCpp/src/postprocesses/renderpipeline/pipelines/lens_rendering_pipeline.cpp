#include <babylon/postprocesses/renderpipeline/pipelines/lens_rendering_pipeline.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/icanvas_rendering_context2D.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/dynamic_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

LensRenderingPipeline::LensRenderingPipeline(const std::string& iName,
                                             const LensRenderingPipelineParameters& parameters,
                                             Scene* scene, float ratio,
                                             const std::vector<CameraPtr>& cameras)
    : PostProcessRenderPipeline(scene->getEngine(), iName)
    , scene{this, &LensRenderingPipeline::get_scene}
    , edgeBlur{this, &LensRenderingPipeline::get_edgeBlur, &LensRenderingPipeline::set_edgeBlur}
    , grainAmount{this, &LensRenderingPipeline::get_grainAmount,
                  &LensRenderingPipeline::set_grainAmount}
    , chromaticAberration{this, &LensRenderingPipeline::get_chromaticAberration,
                          &LensRenderingPipeline::set_chromaticAberration}
    , dofAperture{this, &LensRenderingPipeline::get_dofAperture,
                  &LensRenderingPipeline::set_dofAperture}
    , edgeDistortion{this, &LensRenderingPipeline::get_edgeDistortion,
                     &LensRenderingPipeline::set_edgeDistortion}
    , dofDistortion{this, &LensRenderingPipeline::get_dofDistortion,
                    &LensRenderingPipeline::set_dofDistortion}
    , darkenOutOfFocus{this, &LensRenderingPipeline::get_darkenOutOfFocus,
                       &LensRenderingPipeline::set_darkenOutOfFocus}
    , blurNoise{this, &LensRenderingPipeline::get_blurNoise, &LensRenderingPipeline::set_blurNoise}
    , pentagonBokeh{this, &LensRenderingPipeline::get_pentagonBokeh,
                    &LensRenderingPipeline::set_pentagonBokeh}
    , highlightsGain{this, &LensRenderingPipeline::get_highlightsGain,
                     &LensRenderingPipeline::set_highlightsGain}
    , highlightsThreshold{this, &LensRenderingPipeline::get_highlightsThreshold,
                          &LensRenderingPipeline::set_highlightsThreshold}
    , _cameraList{cameras}
    , _pentagonBokehIsEnabled{false}
{
  _scene = scene;

  // Fetch texture samplers
  _depthTexture = scene->enableDepthRenderer()->getDepthMap(); // Force depth renderer "on"
  if (parameters.grain_texture) {
    _grainTexture = parameters.grain_texture;
  }
  else {
    _createGrainTexture();
  }

  // save parameters
  _edgeBlur            = parameters.edge_blur;
  _grainAmount         = parameters.grain_amount;
  _chromaticAberration = parameters.chromatic_aberration;
  _distortion          = parameters.distortion;
  _highlightsGain      = parameters.dof_gain;
  _highlightsThreshold = parameters.dof_threshold;
  _dofDistance         = parameters.dof_focus_distance;
  _dofAperture         = parameters.dof_aperture;
  _dofDarken           = parameters.dof_darken;
  _dofPentagon         = parameters.dof_pentagon;
  _blurNoise           = parameters.blur_noise;

  // Create effects
  _createChromaticAberrationPostProcess(ratio);
  _createHighlightsPostProcess(ratio);
  _createDepthOfFieldPostProcess(ratio / 4.f);

  // Set up pipeline
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), LensChromaticAberrationEffect,
    [=]() -> std::vector<PostProcessPtr> { return {_chromaticAberrationPostProcess}; }, true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), HighlightsEnhancingEffect,
    [=]() -> std::vector<PostProcessPtr> { return {_highlightsPostProcess}; }, true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), LensDepthOfFieldEffect,
    [=]() -> std::vector<PostProcessPtr> { return {_depthOfFieldPostProcess}; }, true));

  if (stl_util::almost_equal(_highlightsGain, -1.f)) {
    _disableEffect(HighlightsEnhancingEffect, std::vector<CameraPtr>());
  }
}

LensRenderingPipeline::~LensRenderingPipeline() = default;

void LensRenderingPipeline::addToScene(const LensRenderingPipelinePtr& lensRenderingPipeline)
{
  // Finish
  _scene->postProcessRenderPipelineManager()->addPipeline(lensRenderingPipeline);
  if (!_cameraList.empty()) {
    _scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(_name, _cameraList);
  }
}

std::string LensRenderingPipeline::getClassName() const
{
  return "LensRenderingPipeline";
}

Scene*& LensRenderingPipeline::get_scene()
{
  return _scene;
}

float LensRenderingPipeline::get_edgeBlur() const
{
  return _edgeBlur;
}

void LensRenderingPipeline::set_edgeBlur(float value)
{
  setEdgeBlur(value);
}

float LensRenderingPipeline::get_grainAmount() const
{
  return _grainAmount;
}

void LensRenderingPipeline::set_grainAmount(float value)
{
  setGrainAmount(value);
}

float LensRenderingPipeline::get_chromaticAberration() const
{
  return _chromaticAberration;
}

void LensRenderingPipeline::set_chromaticAberration(float value)
{
  setChromaticAberration(value);
}

float LensRenderingPipeline::get_dofAperture() const
{
  return _dofAperture;
}

void LensRenderingPipeline::set_dofAperture(float value)
{
  setAperture(value);
}

float LensRenderingPipeline::get_edgeDistortion() const
{
  return _distortion;
}

void LensRenderingPipeline::set_edgeDistortion(float value)
{
  setEdgeDistortion(value);
}

float LensRenderingPipeline::get_dofDistortion() const
{
  return _dofDistance;
}

void LensRenderingPipeline::set_dofDistortion(float value)
{
  setFocusDistance(value);
}

float LensRenderingPipeline::get_darkenOutOfFocus() const
{
  return _dofDarken;
}

void LensRenderingPipeline::set_darkenOutOfFocus(float value)
{
  setDarkenOutOfFocus(value);
}

bool LensRenderingPipeline::get_blurNoise() const
{
  return _blurNoise;
}

void LensRenderingPipeline::set_blurNoise(bool value)
{
  _blurNoise = value;
}

bool LensRenderingPipeline::get_pentagonBokeh() const
{
  return _pentagonBokehIsEnabled;
}

void LensRenderingPipeline::set_pentagonBokeh(bool value)
{
  if (value) {
    enablePentagonBokeh();
  }
  else {
    disablePentagonBokeh();
  }
}

float LensRenderingPipeline::get_highlightsGain() const
{
  return _highlightsGain;
}

void LensRenderingPipeline::set_highlightsGain(float value)
{
  setHighlightsGain(value);
}

float LensRenderingPipeline::get_highlightsThreshold() const
{
  return _highlightsThreshold;
}

void LensRenderingPipeline::set_highlightsThreshold(float value)
{
  setHighlightsThreshold(value);
}

void LensRenderingPipeline::setEdgeBlur(float amount)
{
  _edgeBlur = amount;
}

void LensRenderingPipeline::disableEdgeBlur()
{
  _edgeBlur = 0.f;
}

void LensRenderingPipeline::setGrainAmount(float amount)
{
  _grainAmount = amount;
}

void LensRenderingPipeline::disableGrain()
{
  _grainAmount = 0.f;
}

void LensRenderingPipeline::setChromaticAberration(float amount)
{
  _chromaticAberration = amount;
}

void LensRenderingPipeline::disableChromaticAberration()
{
  _chromaticAberration = 0.f;
}

void LensRenderingPipeline::setEdgeDistortion(float amount)
{
  _distortion = amount;
}

void LensRenderingPipeline::disableEdgeDistortion()
{
  _distortion = 0;
}

void LensRenderingPipeline::setFocusDistance(float amount)
{
  _dofDistance = amount;
}

void LensRenderingPipeline::disableDepthOfField()
{
  _dofDistance = -1.f;
}

void LensRenderingPipeline::setAperture(float amount)
{
  _dofAperture = amount;
}

void LensRenderingPipeline::setDarkenOutOfFocus(float amount)
{
  _dofDarken = amount;
}

void LensRenderingPipeline::enablePentagonBokeh()
{
  _highlightsPostProcess->updateEffect("#define PENTAGON\n");
  _pentagonBokehIsEnabled = true;
}

void LensRenderingPipeline::disablePentagonBokeh()
{
  _pentagonBokehIsEnabled = false;
  _highlightsPostProcess->updateEffect();
}

void LensRenderingPipeline::enableNoiseBlur()
{
  _blurNoise = true;
}

void LensRenderingPipeline::disableNoiseBlur()
{
  _blurNoise = false;
}

void LensRenderingPipeline::setHighlightsGain(float amount)
{
  _highlightsGain = amount;
}

void LensRenderingPipeline::setHighlightsThreshold(float amount)
{
  if (stl_util::almost_equal(_highlightsGain, -1.f)) {
    _highlightsGain = 1.f;
  }
  _highlightsThreshold = amount;
}

void LensRenderingPipeline::disableHighlights()
{
  _highlightsGain = -1.f;
}

void LensRenderingPipeline::dispose(bool disableDepthRender, bool /*disposeMaterialAndTextures */)
{
  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(_name,
                                                                              _scene->cameras);

  _chromaticAberrationPostProcess = nullptr;
  _highlightsPostProcess          = nullptr;
  _depthOfFieldPostProcess        = nullptr;

  _grainTexture->dispose();

  if (disableDepthRender) {
    _scene->disableDepthRenderer();
  }
}

void LensRenderingPipeline::_createChromaticAberrationPostProcess(float ratio)
{
  _chromaticAberrationPostProcess = PostProcess::New(
    "LensChromaticAberration", "chromaticAberration",
    {"chromatic_aberration", "screen_width", "screen_height", "direction", "radialIntensity",
     "centerPosition"}, // uniforms
    {},                 // samplers
    ratio, nullptr, TextureConstants::TRILINEAR_SAMPLINGMODE, _scene->getEngine(), false);

  _chromaticAberrationPostProcess->onApply = [&](Effect* effect, EventState&) {
    effect->setFloat("chromatic_aberration", _chromaticAberration);
    effect->setFloat("screen_width", static_cast<float>(_scene->getEngine()->getRenderWidth()));
    effect->setFloat("screen_height", static_cast<float>(_scene->getEngine()->getRenderHeight()));
    effect->setFloat("radialIntensity", 1.f);
    effect->setFloat2("direction", 17.f, 17.f);
    effect->setFloat2("centerPosition", 0.5f, 0.5f);
  };
}

void LensRenderingPipeline::_createHighlightsPostProcess(float ratio)
{
  _highlightsPostProcess
    = PostProcess::New("LensHighlights", "lensHighlights",
                       {"gain", "threshold", "screen_width", "screen_height"}, // uniforms
                       {},                                                     // samplers
                       ratio, nullptr, TextureConstants::TRILINEAR_SAMPLINGMODE,
                       _scene->getEngine(), false, _dofPentagon ? "#define PENTAGON\n" : "");

  _highlightsPostProcess->onApply = [&](Effect* effect, EventState&) {
    effect->setFloat("gain", _highlightsGain);
    effect->setFloat("threshold", _highlightsThreshold);
    effect->setTextureFromPostProcess("textureSampler", _chromaticAberrationPostProcess);
    effect->setFloat("screen_width", static_cast<float>(_scene->getEngine()->getRenderWidth()));
    effect->setFloat("screen_height", static_cast<float>(_scene->getEngine()->getRenderHeight()));
  };
}

void LensRenderingPipeline::_createDepthOfFieldPostProcess(float ratio)
{
  _depthOfFieldPostProcess = PostProcess::New(
    "LensDepthOfField", "depthOfField",
    {"grain_amount", "blur_noise", "screen_width", "screen_height", "distortion", "dof_enabled",
     "screen_distance", "aperture", "darken", "edge_blur", "highlights", "near", "far"},
    {"depthSampler", "grainSampler", "highlightsSampler"}, ratio, nullptr,
    TextureConstants::TRILINEAR_SAMPLINGMODE, _scene->getEngine(), false);

  _depthOfFieldPostProcess->onApply = [&](Effect* effect, EventState&) {
    effect->setTexture("depthSampler", _depthTexture);
    effect->setTexture("grainSampler", _grainTexture);
    effect->setTextureFromPostProcess("textureSampler", _highlightsPostProcess);
    effect->setTextureFromPostProcess("highlightsSampler", _depthOfFieldPostProcess);

    effect->setFloat("grain_amount", _grainAmount);
    effect->setBool("blur_noise", _blurNoise);

    effect->setFloat("screen_width", static_cast<float>(_scene->getEngine()->getRenderWidth()));
    effect->setFloat("screen_height", static_cast<float>(_scene->getEngine()->getRenderHeight()));

    effect->setFloat("distortion", _distortion);

    effect->setBool("dof_enabled", !stl_util::almost_equal(_dofDistance, -1.f));
    effect->setFloat("screen_distance", 1.f / (0.1f - 1.f / _dofDistance));
    effect->setFloat("aperture", _dofAperture);
    effect->setFloat("darken", _dofDarken);

    effect->setFloat("edge_blur", _edgeBlur);

    effect->setBool("highlights", !stl_util::almost_equal(_highlightsGain, -1.f));

    if (_scene->activeCamera()) {
      effect->setFloat("near", _scene->activeCamera()->minZ);
      effect->setFloat("far", _scene->activeCamera()->maxZ);
    }
  };
}

void LensRenderingPipeline::_createGrainTexture()
{
  size_t size = 512;

  DynamicTextureOptions options;
  options.width  = static_cast<int>(size);
  options.height = static_cast<int>(size);

  _grainTexture        = DynamicTexture::New("LensNoiseTexture", options, _scene, false,
                                      TextureConstants::BILINEAR_SAMPLINGMODE);
  _grainTexture->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _grainTexture->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  ICanvasRenderingContext2D* context
    = std::static_pointer_cast<DynamicTexture>(_grainTexture)->getContext();

  const auto rand
    = [](float min, float max) -> float { return Math::random() * (max - min) + min; };

  std::string value;
  for (size_t x = 0; x < size; ++x) {
    for (size_t y = 0; y < size; ++y) {
      value              = std::to_string(static_cast<int>(std::floor(rand(0.42f, 0.58f) * 255.f)));
      context->fillStyle = "rgb(" + value + ", " + value + ", " + value + ")";
      context->fillRect(static_cast<int>(x), static_cast<int>(y), 1, 1);
    }
  }
  std::static_pointer_cast<DynamicTexture>(_grainTexture)->update(false);
}

} // end of namespace BABYLON
