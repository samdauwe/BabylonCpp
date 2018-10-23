#include <babylon/postprocess/renderpipeline/pipelines/lens_rendering_pipeline.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/random.h>
#include <babylon/core/variant.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/icanvas_rendering_context2D.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/dynamic_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

LensRenderingPipeline::LensRenderingPipeline(
  const std::string& name, const LensRenderingPipelineParameters& parameters,
  Scene* scene, float ratio, const std::vector<CameraPtr>& cameras)
    : PostProcessRenderPipeline(scene->getEngine(), name)
    , _scene{scene}
    , _cameraList{cameras}
{
  // Fetch texture samplers
  _depthTexture
    = scene->enableDepthRenderer()->getDepthMap(); // Force depth renderer "on"
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
    [&]() -> std::vector<PostProcessPtr> {
      return {_chromaticAberrationPostProcess};
    },
    true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), HighlightsEnhancingEffect,
    [&]() -> std::vector<PostProcessPtr> { return {_highlightsPostProcess}; },
    true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), LensDepthOfFieldEffect,
    [&]() -> std::vector<PostProcessPtr> { return {_depthOfFieldPostProcess}; },
    true));

  if (stl_util::almost_equal(_highlightsGain, -1.f)) {
    _disableEffect(HighlightsEnhancingEffect, std::vector<CameraPtr>());
  }
}

LensRenderingPipeline::~LensRenderingPipeline()
{
}

void LensRenderingPipeline::addToScene(
  const LensRenderingPipelinePtr& lensRenderingPipeline)
{
  // Finish
  _scene->postProcessRenderPipelineManager()->addPipeline(
    lensRenderingPipeline);
  if (!_cameraList.empty()) {
    _scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(
      _name, _cameraList);
  }
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
}

void LensRenderingPipeline::disablePentagonBokeh()
{
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

void LensRenderingPipeline::dispose(bool disableDepthRender,
                                    bool /*disposeMaterialAndTextures */)
{
  // TODO FIXME
  // _scene->postProcessRenderPipelineManager->detachCamerasFromRenderPipeline(
  //  _name, _scene->cameras);

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
    {"chromatic_aberration", "screen_width", "screen_height", "direction",
     "radialIntensity", "centerPosition"}, // uniforms
    {},                                    // samplers
    ratio, nullptr, TextureConstants::TRILINEAR_SAMPLINGMODE,
    _scene->getEngine(), false);

  _chromaticAberrationPostProcess->setOnApply([&](Effect* effect, EventState&) {
    effect->setFloat("chromatic_aberration", _chromaticAberration);
    effect->setFloat("screen_width",
                     static_cast<float>(_scene->getEngine()->getRenderWidth()));
    effect->setFloat(
      "screen_height",
      static_cast<float>(_scene->getEngine()->getRenderHeight()));
    effect->setFloat("radialIntensity", 1.f);
    effect->setFloat2("direction", 17.f, 17.f);
    effect->setFloat2("centerPosition", 0.5f, 0.5f);
  });
}

void LensRenderingPipeline::_createHighlightsPostProcess(float ratio)
{
  _highlightsPostProcess = PostProcess::New(
    "LensHighlights", "lensHighlights",
    {"gain", "threshold", "screen_width", "screen_height"}, // uniforms
    {},                                                     // samplers
    ratio, nullptr, TextureConstants::TRILINEAR_SAMPLINGMODE,
    _scene->getEngine(), false, _dofPentagon ? "#define PENTAGON\n" : "");

  _highlightsPostProcess->setOnApply([&](Effect* effect, EventState&) {
    effect->setFloat("gain", _highlightsGain);
    effect->setFloat("threshold", _highlightsThreshold);
    effect->setTextureFromPostProcess("textureSampler",
                                      _chromaticAberrationPostProcess.get());
    effect->setFloat("screen_width",
                     static_cast<float>(_scene->getEngine()->getRenderWidth()));
    effect->setFloat(
      "screen_height",
      static_cast<float>(_scene->getEngine()->getRenderHeight()));
  });
}

void LensRenderingPipeline::_createDepthOfFieldPostProcess(float ratio)
{
  _depthOfFieldPostProcess = PostProcess::New(
    "LensDepthOfField", "depthOfField",
    {"grain_amount", "blur_noise", "screen_width", "screen_height",
     "distortion", "dof_enabled", "screen_distance", "aperture", "darken",
     "edge_blur", "highlights", "near", "far"},
    {"depthSampler", "grainSampler", "highlightsSampler"}, ratio, nullptr,
    TextureConstants::TRILINEAR_SAMPLINGMODE, _scene->getEngine(), false);

  _depthOfFieldPostProcess->setOnApply([&](Effect* effect, EventState&) {
    effect->setTexture("depthSampler", _depthTexture);
    effect->setTexture("grainSampler", _grainTexture);
    effect->setTextureFromPostProcess("textureSampler",
                                      _highlightsPostProcess.get());
    effect->setTextureFromPostProcess("highlightsSampler",
                                      _depthOfFieldPostProcess.get());

    effect->setFloat("grain_amount", _grainAmount);
    effect->setBool("blur_noise", _blurNoise);

    effect->setFloat("screen_width",
                     static_cast<float>(_scene->getEngine()->getRenderWidth()));
    effect->setFloat(
      "screen_height",
      static_cast<float>(_scene->getEngine()->getRenderHeight()));

    effect->setFloat("distortion", _distortion);

    effect->setBool("dof_enabled", !stl_util::almost_equal(_dofDistance, -1.f));
    effect->setFloat("screen_distance", 1.f / (0.1f - 1.f / _dofDistance));
    effect->setFloat("aperture", _dofAperture);
    effect->setFloat("darken", _dofDarken);

    effect->setFloat("edge_blur", _edgeBlur);

    effect->setBool("highlights",
                    !stl_util::almost_equal(_highlightsGain, -1.f));

    if (_scene->activeCamera) {
      effect->setFloat("near", _scene->activeCamera->minZ);
      effect->setFloat("far", _scene->activeCamera->maxZ);
    }
  });
}

void LensRenderingPipeline::_createGrainTexture()
{
  size_t size = 512;

  DynamicTextureOptions options;
  options.width  = static_cast<int>(size);
  options.height = static_cast<int>(size);

  _grainTexture
    = DynamicTexture::New("LensNoiseTexture", options, _scene, false,
                          TextureConstants::BILINEAR_SAMPLINGMODE);
  _grainTexture->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _grainTexture->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  ICanvasRenderingContext2D* context
    = std::static_pointer_cast<DynamicTexture>(_grainTexture)->getContext();

  auto rand
    = [](float min, float max) { return Math::random() * (max - min) + min; };

  std::string value;
  for (size_t x = 0; x < size; ++x) {
    for (size_t y = 0; y < size; ++y) {
      value = std::to_string(
        static_cast<int>(std::floor(rand(0.42f, 0.58f) * 255.f)));
      context->fillStyle = "rgb(" + value + ", " + value + ", " + value + ")";
      context->fillRect(static_cast<int>(x), static_cast<int>(y), 1, 1);
    }
  }
  std::static_pointer_cast<DynamicTexture>(_grainTexture)->update(false);
}

} // end of namespace BABYLON
