#include <babylon/postprocess/ssao_rendering_pipeline.h>

#include <babylon/core/random.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas_rendering_context2D.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/dynamic_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

SSAORenderingPipeline::SSAORenderingPipeline(
  const std::string& name, Scene* scene, float ratio,
  const std::vector<Camera*>& cameras)
    : SSAORenderingPipeline(name, scene, {ratio, ratio}, cameras)
{
}

SSAORenderingPipeline::SSAORenderingPipeline(
  const std::string& name, Scene* scene, const SSARatio& ratio,
  const std::vector<Camera*>& cameras)
    : PostProcessRenderPipeline(scene->getEngine(), name)
    , totalStrength{1.f}
    , radius{0.0001f}
    , area{0.0075f}
    , fallOff{0.000001f}
    , base{0.5f}
    , _scene{scene}
    , _firstUpdate{true}
{
  // Set up assets
  _createRandomTexture();
  // Force depth renderer "on"
  _depthTexture = scene->enableDepthRenderer()->getDepthMap();

  _ratio.ssaoRatio    = ratio.ssaoRatio;
  _ratio.combineRatio = ratio.combineRatio;

  _originalColorPostProcess = new PassPostProcess(
    "SSAOOriginalSceneColor", _ratio.combineRatio, nullptr,
    Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false);
  _createSSAOPostProcess(ratio.ssaoRatio);
  _createBlurPostProcess(ratio.ssaoRatio);
  _createSSAOCombinePostProcess(ratio.combineRatio);

  // Set up pipeline
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), SSAOOriginalSceneColorEffect,
    [&]() { return _originalColorPostProcess; }, true));
  addEffect(new PostProcessRenderEffect(scene->getEngine(), SSAORenderEffect,
                                        [&]() { return _ssaoPostProcess; },
                                        true));
  addEffect(
    new PostProcessRenderEffect(scene->getEngine(), SSAOBlurHRenderEffect,
                                [&]() { return _blurHPostProcess; }, true));
  addEffect(
    new PostProcessRenderEffect(scene->getEngine(), SSAOBlurVRenderEffect,
                                [&]() { return _blurVPostProcess; }, true));

  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), SSAOCombineRenderEffect,
    [&]() { return _ssaoCombinePostProcess; }, true));

  // Finish
  scene->postProcessRenderPipelineManager()->addPipeline(this);
  if (!cameras.empty()) {
    scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(
      name, cameras);
  }
}

SSAORenderingPipeline::~SSAORenderingPipeline()
{
}

void SSAORenderingPipeline::dispose(bool disableDepthRender)
{
  for (auto& camera : _scene->cameras) {
    auto _camera = camera.get();
    _originalColorPostProcess->dispose(_camera);
    _ssaoPostProcess->dispose(_camera);
    _blurHPostProcess->dispose(_camera);
    _blurVPostProcess->dispose(_camera);
    _ssaoCombinePostProcess->dispose(_camera);
  }

  _randomTexture->dispose();

  if (disableDepthRender) {
    _scene->disableDepthRenderer();
  }

  // TODO FIXME
  //_scene->postProcessRenderPipelineManager->detachCamerasFromRenderPipeline(
  //  _name, _scene->cameras);
}

void SSAORenderingPipeline::_createBlurPostProcess(float ratio)
{
  Float32Array samplerOffsets;
  samplerOffsets.reserve(16);

  for (int i = -8; i < 8; i++) {
    samplerOffsets.emplace_back(static_cast<float>(i) * 2.f);
  }

  _blurHPostProcess = new PostProcess(
    "BlurH", "ssao", {"outSize", "samplerOffsets"}, {"depthSampler"}, ratio,
    nullptr, Texture::TRILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
    "#define BILATERAL_BLUR\n#define BILATERAL_BLUR_H\n#define SAMPLES 16");
  _blurHPostProcess->setOnApply([&](Effect* effect) {
    effect->setFloat("outSize",
                     static_cast<float>(_ssaoCombinePostProcess->width));
    effect->setTexture("depthSampler", _depthTexture);

    if (_firstUpdate) {
      effect->setArray("samplerOffsets", samplerOffsets);
    }
  });

  _blurVPostProcess = new PostProcess(
    "BlurV", "ssao", {"outSize", "samplerOffsets"}, {"depthSampler"}, ratio,
    nullptr, Texture::TRILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
    "#define BILATERAL_BLUR\n#define SAMPLES 16");
  _blurVPostProcess->setOnApply([&](Effect* effect) {
    effect->setFloat("outSize",
                     static_cast<float>(_ssaoCombinePostProcess->height));
    effect->setTexture("depthSampler", _depthTexture);

    if (_firstUpdate) {
      effect->setArray("samplerOffsets", samplerOffsets);
      _firstUpdate = false;
    }
  });
}

void SSAORenderingPipeline::_createSSAOPostProcess(float ratio)
{
  const unsigned int numSamples = 16;
  Float32Array sampleSphere{
    {0.5381f,  0.1856f,  -0.4319f, 0.1379f,  0.2486f,  0.4430f,  0.3371f,
     0.5679f,  -0.0057f, -0.6999f, -0.0451f, -0.0019f, 0.0689f,  -0.1598f,
     -0.8547f, 0.0560f,  0.0069f,  -0.1843f, -0.0146f, 0.1402f,  0.0762f,
     0.0100f,  -0.1924f, -0.0344f, -0.3577f, -0.5301f, -0.4358f, -0.3169f,
     0.1063f,  0.0158f,  0.0103f,  -0.5869f, 0.0046f,  -0.0897f, -0.4940f,
     0.3287f,  0.7119f,  -0.0154f, -0.0918f, -0.0533f, 0.0596f,  -0.5411f,
     0.0352f,  -0.0631f, 0.5460f,  -0.4776f, 0.2847f,  -0.0271f}};
  float samplesFactor = 1.f / static_cast<float>(numSamples);

  _ssaoPostProcess = new PostProcess(
    "ssao", "ssao",
    {"sampleSphere", "samplesFactor", "randTextureTiles", "totalStrength",
     "radius", "area", "fallOff", "base", "range", "viewport"},
    {"randomSampler"}, ratio, nullptr, Texture::BILINEAR_SAMPLINGMODE,
    _scene->getEngine(), false,
    "#define SAMPLES " + std::to_string(numSamples) + "\n#define SSAO");

  _ssaoPostProcess->setOnApply([&](Effect* effect) {
    if (_firstUpdate) {
      effect->setArray3("sampleSphere", sampleSphere);
      effect->setFloat("samplesFactor", samplesFactor);
      effect->setFloat("randTextureTiles", 4.0);
    }

    effect->setFloat("totalStrength", totalStrength);
    effect->setFloat("radius", radius);
    effect->setFloat("area", area);
    effect->setFloat("fallOff", fallOff);
    effect->setFloat("base", base);

    effect->setTexture("textureSampler", _depthTexture);
    effect->setTexture("randomSampler", _randomTexture);
  });
}

void SSAORenderingPipeline::_createSSAOCombinePostProcess(float ratio)
{
  _ssaoCombinePostProcess = new PostProcess(
    "ssaoCombine", "ssaoCombine", {}, {"originalColor"}, ratio, nullptr,
    Texture::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false);

  _ssaoCombinePostProcess->setOnApply([&](Effect* effect) {
    effect->setTextureFromPostProcess("originalColor",
                                      _originalColorPostProcess);
  });
}

void SSAORenderingPipeline::_createRandomTexture()
{
  size_t size = 512;

  DynamicTextureOptions options;
  options.width  = static_cast<int>(size);
  options.height = static_cast<int>(size);

  _randomTexture = new DynamicTexture("SSAORandomTexture", options, _scene,
                                      false, Texture::TRILINEAR_SAMPLINGMODE);
  _randomTexture->wrapU = Texture::WRAP_ADDRESSMODE;
  _randomTexture->wrapV = Texture::WRAP_ADDRESSMODE;

  auto context = dynamic_cast<DynamicTexture*>(_randomTexture)->getContext();

  auto rand
    = [](float min, float max) { return Math::random() * (max - min) + min; };

  Vector3 randVector = Vector3::Zero();

  for (size_t x = 0; x < size; ++x) {
    for (size_t y = 0; y < size; ++y) {
      randVector.x = std::floor(rand(-1.f, 1.f) * 255.f);
      randVector.y = std::floor(rand(-1.f, 1.f) * 255.f);
      randVector.z = std::floor(rand(-1.f, 1.f) * 255.f);

      context->fillStyle = "rgb(" + std::to_string(randVector.x) + ", "
                           + std::to_string(randVector.y) + ", "
                           + std::to_string(randVector.z) + ")";
      context->fillRect(static_cast<int>(x), static_cast<int>(y), 1, 1);
    }
  }

  _randomTexture->update(false);
}

} // end of namespace BABYLON
