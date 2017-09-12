#include <babylon/postprocess/renderpipeline/pipelines/ssao2_rendering_pipeline.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/core/random.h>
#include <babylon/engine/engine.h>
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

SSAO2RenderingPipeline::SSAO2RenderingPipeline(
  const std::string& name, Scene* scene, float ratio,
  const std::vector<Camera*>& cameras)
    : SSAO2RenderingPipeline(name, scene, {ratio, ratio}, cameras)
{
}

SSAO2RenderingPipeline::SSAO2RenderingPipeline(
  const std::string& name, Scene* scene, const SSAO2Ratio& ratio,
  const std::vector<Camera*>& cameras)
    : PostProcessRenderPipeline(scene->getEngine(), name)
    , totalStrength{1.f}
    , maxZ{100.f}
    , minZAspect{0.2f}
    , radius{2.f}
    , area{0.0075f}
    , fallOff{0.000001f}
    , base{0.1f}
    , _samples{8}
    , _expensiveBlur{true}
    , _scene{scene}
    , _depthTexture{nullptr}
    , _normalTexture{nullptr}
    , _randomTexture{nullptr}
    , _originalColorPostProcess{nullptr}
    , _ssaoPostProcess{nullptr}
    , _blurHPostProcess{nullptr}
    , _blurVPostProcess{nullptr}
    , _ssaoCombinePostProcess{nullptr}
    , _firstUpdate{true}
{
  if (!isSupported()) {
    BABYLON_LOG_ERROR("SSAO2RenderingPipeline",
                      "SSAO 2 needs WebGL 2 support.");
    return;
  }

  _ratio.ssaoRatio = ratio.ssaoRatio;
  _ratio.blurRatio = ratio.blurRatio;

  // Set up assets
  _createRandomTexture();
  // TODO FIXME
  _depthTexture  = scene->enableDepthRenderer()->getDepthMap();
  _normalTexture = scene->enableDepthRenderer()->getDepthMap();

  _originalColorPostProcess = new PassPostProcess(
    "SSAOOriginalSceneColor", 1.f, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false);
  _createSSAOPostProcess(1.0);
  _createBlurPostProcess(_ratio.ssaoRatio, _ratio.blurRatio);
  _createSSAOCombinePostProcess(_ratio.blurRatio);

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

SSAO2RenderingPipeline::~SSAO2RenderingPipeline()
{
}

void SSAO2RenderingPipeline::setSamples(unsigned int n)
{
  _ssaoPostProcess->updateEffect("#define SAMPLES " + ::std::to_string(n)
                                 + "\n#define SSAO");
  _samples      = n;
  _sampleSphere = _generateHemisphere();

  _firstUpdate = true;
}

unsigned int SSAO2RenderingPipeline::samples() const
{
  return _samples;
}

void SSAO2RenderingPipeline::setExpensiveBlur(bool b)
{
  _blurHPostProcess->updateEffect(
    "#define BILATERAL_BLUR\n#define BILATERAL_BLUR_H\n#define SAMPLES "
    "16\n#define EXPENSIVE "
      + std::string(b ? "1" : "0") + "\n",
    {}, {"textureSampler", "depthSampler"});
  _blurVPostProcess->updateEffect(
    "#define BILATERAL_BLUR\n#define SAMPLES 16\n#define EXPENSIVE "
      + std::string(b ? "1" : "0") + "\n",
    {}, {"textureSampler", "depthSampler"});
  _expensiveBlur = b;
  _firstUpdate   = true;
}

bool SSAO2RenderingPipeline::expensiveBlur()
{
  return _expensiveBlur;
}

bool SSAO2RenderingPipeline::IsSupported()
{
  auto engine = Engine::LastCreatedEngine();
  return engine->webGLVersion() > 1.f;
}

void SSAO2RenderingPipeline::dispose(bool disableGeometryBufferRenderer)
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

  if (disableGeometryBufferRenderer) {
    _scene->disableGeometryBufferRenderer();
  }

  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
    _name, stl_util::to_raw_ptr_vector(_scene->cameras));
}

void SSAO2RenderingPipeline::_createBlurPostProcess(float ssaoRatio,
                                                    float blurRatio)
{
  Float32Array samplerOffsets;
  samplerOffsets.reserve(16);

  for (int i = -8; i < 8; ++i) {
    samplerOffsets.emplace_back(static_cast<float>(i) * 2.f + 0.5f);
  }

  _blurHPostProcess = new PostProcess(
    "BlurH", "ssao", {"outSize", "samplerOffsets"}, {"depthSampler"}, ssaoRatio,
    nullptr, TextureConstants::TRILINEAR_SAMPLINGMODE, _scene->getEngine(),
    false,
    "#define BILATERAL_BLUR\n#define BILATERAL_BLUR_H\n#define SAMPLES 16");
  _blurHPostProcess->setOnApply([&](Effect* effect) {
    effect->setFloat("outSize", _ssaoCombinePostProcess->width);
    effect->setFloat("near", _scene->activeCamera->minZ);
    effect->setFloat("far", _scene->activeCamera->maxZ);
    effect->setFloat("radius", radius);
    effect->setTexture("depthSampler", _depthTexture);

    if (_firstUpdate) {
      effect->setArray("samplerOffsets", samplerOffsets);
    }
  });

  _blurVPostProcess = new PostProcess(
    "BlurV", "ssao", {"outSize", "samplerOffsets"}, {"depthSampler"}, blurRatio,
    nullptr, TextureConstants::TRILINEAR_SAMPLINGMODE, _scene->getEngine(),
    false, "#define BILATERAL_BLUR\n#define SAMPLES 16");
  _blurVPostProcess->setOnApply([&](Effect* effect) {
    effect->setFloat("outSize", _ssaoCombinePostProcess->height);
    effect->setFloat("near", _scene->activeCamera->minZ);
    effect->setFloat("far", _scene->activeCamera->maxZ);
    effect->setFloat("radius", radius);
    effect->setTexture("depthSampler", _depthTexture);

    if (_firstUpdate) {
      effect->setArray("samplerOffsets", samplerOffsets);
      _firstUpdate = false;
    }
  });
}

Float32Array SSAO2RenderingPipeline::_generateHemisphere()
{
  auto numSamples = samples();
  Float32Array result;
  Vector3 vector;
  float scale = 0.f;

  const auto rand
    = [](float min, float max) { return Math::random() * (max - min) + min; };

  const auto lerp = [](float start, float end, float percent) {
    return (start + percent * (end - start));
  };

  for (unsigned int i = 0; i < numSamples; ++i) {
    vector = Vector3(rand(-1.f, 1.f), rand(-1.f, 1.f), rand(0.3f, 1.f));
    vector.normalize();
    scale = static_cast<float>(i) / static_cast<float>(numSamples);
    scale = lerp(0.1f, 1.f, scale * scale);
    vector.scaleInPlace(scale);

    stl_util::concat(result, {vector.x, vector.y, vector.z});
  }

  return result;
}

void SSAO2RenderingPipeline::_createSSAOPostProcess(float ratio)
{
  const unsigned int numSamples = samples();

  Float32Array sampleSphere = _generateHemisphere();

  float samplesFactor = 1.f / static_cast<float>(numSamples);

  _ssaoPostProcess = new PostProcess(
    "ssao2", "ssao2",
    {"sampleSphere", "samplesFactor", "randTextureTiles", "totalStrength",
     "radius", "base", "range", "projection", "near", "far", "texelSize",
     "xViewport", "yViewport", "maxZ", "minZAspect"},
    {"randomSampler", "normalSampler"}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
    "#define SAMPLES " + ::std::to_string(numSamples) + "\n#define SSAO");

  _ssaoPostProcess->setOnApply([&](Effect* effect) {
    if (_firstUpdate) {
      effect->setArray3("sampleSphere", sampleSphere);
      effect->setFloat("randTextureTiles", 4.f);
    }

    effect->setFloat("samplesFactor", samplesFactor);
    effect->setFloat("totalStrength", totalStrength);
    effect->setFloat2("texelSize",
                      1.f / static_cast<float>(_ssaoPostProcess->width),
                      1.f / static_cast<float>(_ssaoPostProcess->height));
    effect->setFloat("radius", radius);
    effect->setFloat("maxZ", maxZ);
    effect->setFloat("minZAspect", minZAspect);
    effect->setFloat("base", base);
    effect->setFloat("near", _scene->activeCamera->minZ);
    effect->setFloat("far", _scene->activeCamera->maxZ);
    effect->setFloat("xViewport", std::tan(_scene->activeCamera->fov / 2.f)
                                    * _scene->getEngine()->getAspectRatio(
                                        _scene->activeCamera, true));
    effect->setFloat("yViewport", std::tan(_scene->activeCamera->fov / 2.f));
    effect->setMatrix("projection", _scene->getProjectionMatrix());

    effect->setTexture("textureSampler", _depthTexture);
    effect->setTexture("normalSampler", _normalTexture);
    effect->setTexture("randomSampler", _randomTexture);
  });
}

void SSAO2RenderingPipeline::_createSSAOCombinePostProcess(float ratio)
{
  _ssaoCombinePostProcess = new PostProcess(
    "ssaoCombine", "ssaoCombine", {}, {"originalColor"}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false);

  _ssaoCombinePostProcess->setOnApply([&](Effect* effect) {
    effect->setTextureFromPostProcess("originalColor",
                                      _originalColorPostProcess);
  });
}

void SSAO2RenderingPipeline::_createRandomTexture()
{
  size_t size = 512;

  DynamicTextureOptions options;
  options.width  = static_cast<int>(size);
  options.height = static_cast<int>(size);

  _randomTexture
    = new DynamicTexture("SSAORandomTexture", options, _scene, false,
                         TextureConstants::TRILINEAR_SAMPLINGMODE);
  _randomTexture->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _randomTexture->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  auto context = dynamic_cast<DynamicTexture*>(_randomTexture)->getContext();

  const auto rand
    = [](float min, float max) { return Math::random() * (max - min) + min; };

  auto randVector = Vector3::Zero();

  for (size_t x = 0; x < size; ++x) {
    for (size_t y = 0; y < size; ++y) {
      randVector.x = rand(0.f, 1.f);
      randVector.y = rand(0.f, 1.f);
      randVector.z = 0.f;
      randVector.normalize();
      randVector.scaleInPlace(255);

      randVector.x = std::floor(randVector.x);
      randVector.y = std::floor(randVector.y);

      context->fillStyle = "rgb(" + ::std::to_string(randVector.x) + ", "
                           + ::std::to_string(randVector.y) + ", "
                           + ::std::to_string(randVector.z) + ")";
      context->fillRect(static_cast<int>(x), static_cast<int>(y), 1, 1);
    }
  }

  _randomTexture->update(false);
}

} // end of namespace BABYLON
