#include <babylon/postprocesses/renderpipeline/pipelines/ssao_rendering_pipeline.h>

#include <babylon/core/random.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas_rendering_context2D.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/dynamic_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/tmp_vectors.h>
#include <babylon/math/vector4.h>
#include <babylon/postprocesses/blur_post_process.h>
#include <babylon/postprocesses/pass_post_process.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

SSAORenderingPipeline::SSAORenderingPipeline(
  const std::string& iName, Scene* scene, float ratio,
  const std::vector<CameraPtr>& cameras)
    : SSAORenderingPipeline(iName, scene, {ratio, ratio}, cameras)
{
}

SSAORenderingPipeline::SSAORenderingPipeline(
  const std::string& iName, Scene* scene, const SSARatio& ratio,
  const std::vector<CameraPtr>& cameras)
    : PostProcessRenderPipeline(scene->getEngine(), iName)
    , totalStrength{1.f}
    , radius{0.0001f}
    , area{0.0075f}
    , fallOff{0.000001f}
    , base{0.5f}
    , _scene{scene}
    , _cameraList{cameras}
    , _firstUpdate{true}
{
  // Set up assets
  _createRandomTexture();
  // Force depth renderer "on"
  _depthTexture = scene->enableDepthRenderer()->getDepthMap();

  auto ssaoRatio    = ratio.ssaoRatio;
  auto combineRatio = ratio.combineRatio;

  _originalColorPostProcess = PassPostProcess::New(
    "SSAOOriginalSceneColor", combineRatio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false);
  _createSSAOPostProcess(ssaoRatio);
  _createBlurPostProcess(ssaoRatio);
  _createSSAOCombinePostProcess(combineRatio);

  // Set up pipeline
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAOOriginalSceneColorEffect,
    [&]() -> std::vector<PostProcessPtr> {
      return {_originalColorPostProcess};
    },
    true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAORenderEffect,
    [&]() -> std::vector<PostProcessPtr> { return {_ssaoPostProcess}; }, true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAOBlurHRenderEffect,
    [&]() -> std::vector<PostProcessPtr> { return {_blurHPostProcess}; },
    true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAOBlurVRenderEffect,
    [&]() -> std::vector<PostProcessPtr> { return {_blurVPostProcess}; },
    true));

  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAOCombineRenderEffect,
    [&]() -> std::vector<PostProcessPtr> { return {_ssaoCombinePostProcess}; },
    true));
}

SSAORenderingPipeline::~SSAORenderingPipeline() = default;

void SSAORenderingPipeline::addToScene(
  const SSAORenderingPipelinePtr& ssao2RenderingPipeline)
{
  // Finish
  _scene->postProcessRenderPipelineManager()->addPipeline(
    ssao2RenderingPipeline);
  if (!_cameraList.empty()) {
    _scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(
      _name, _cameraList);
  }
}

Scene* SSAORenderingPipeline::scene() const
{
  return _scene;
}

std::string SSAORenderingPipeline::getClassName() const
{
  return "SSAORenderingPipeline";
}

void SSAORenderingPipeline::dispose(bool disableDepthRender,
                                    bool /*disposeMaterialAndTextures*/)
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

  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
    _name, _scene->cameras);

  PostProcessRenderPipeline::dispose(disableDepthRender);
}

void SSAORenderingPipeline::_createBlurPostProcess(float ratio)
{
  auto size = 16.f;

  _blurHPostProcess = BlurPostProcess::New(
    "BlurH", Vector2(1.f, 0.f), size, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
    Constants::TEXTURETYPE_UNSIGNED_INT);
  _blurVPostProcess = BlurPostProcess::New(
    "BlurV", Vector2(0.f, 1.f), size, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
    Constants::TEXTURETYPE_UNSIGNED_INT);

  _blurHPostProcess->onActivateObservable.add([&, size](Camera*, EventState&) {
    auto dw = static_cast<float>(_blurHPostProcess->width)
              / static_cast<float>(_scene->getEngine()->getRenderWidth());
    _blurHPostProcess->kernel = size * dw;
  });

  _blurVPostProcess->onActivateObservable.add([&, size](Camera*, EventState&) {
    auto dw = static_cast<float>(_blurVPostProcess->height)
              / static_cast<float>(_scene->getEngine()->getRenderHeight());
    _blurVPostProcess->kernel = size * dw;
  });
}

void SSAORenderingPipeline::_rebuild()
{
  _firstUpdate = true;
  PostProcessRenderPipeline::_rebuild();
}

void SSAORenderingPipeline::_createSSAOPostProcess(float ratio)
{
  const unsigned int numSamples = 16;
  const Float32Array sampleSphere{{
    0.5381f,  0.1856f,  -0.4319f, //
    0.1379f,  0.2486f,  0.4430f,  //
    0.3371f,  0.5679f,  -0.0057f, //
    -0.6999f, -0.0451f, -0.0019f, //
    0.0689f,  -0.1598f, -0.8547f, //
    0.0560f,  0.0069f,  -0.1843f, //
    -0.0146f, 0.1402f,  0.0762f,  //
    0.0100f,  -0.1924f, -0.0344f, //
    -0.3577f, -0.5301f, -0.4358f, //
    -0.3169f, 0.1063f,  0.0158f,  //
    0.0103f,  -0.5869f, 0.0046f,  //
    -0.0897f, -0.4940f, 0.3287f,  //
    0.7119f,  -0.0154f, -0.0918f, //
    -0.0533f, 0.0596f,  -0.5411f, //
    0.0352f,  -0.0631f, 0.5460f,  //
    -0.4776f, 0.2847f,  -0.0271f  //
  }};
  float samplesFactor = 1.f / static_cast<float>(numSamples);

  _ssaoPostProcess = PostProcess::New(
    "ssao", "ssao",
    {"sampleSphere", "samplesFactor", "randTextureTiles", "totalStrength",
     "radius", "area", "fallOff", "base", "range", "viewport"},
    {"randomSampler"}, ratio, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
    _scene->getEngine(), false,
    "#define SAMPLES " + std::to_string(numSamples) + "\n#define SSAO");

  _ssaoPostProcess->onApply = [&](Effect* effect, EventState&) {
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
  };
}

void SSAORenderingPipeline::_createSSAOCombinePostProcess(float ratio)
{
  _ssaoCombinePostProcess = PostProcess::New(
    "ssaoCombine", "ssaoCombine", {}, {"originalColor", "viewport"}, ratio,
    nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(),
    false);

  _ssaoCombinePostProcess->onApply = [&](Effect* effect, EventState&) {
    effect->setVector4("viewport", TmpVectors::Vector4Array[0].copyFromFloats(
                                     0.f, 0.f, 1.f, 1.f));
    effect->setTextureFromPostProcess("originalColor",
                                      _originalColorPostProcess.get());
  };
}

void SSAORenderingPipeline::_createRandomTexture()
{
  size_t size = 512;

  DynamicTextureOptions options;
  options.width  = static_cast<int>(size);
  options.height = static_cast<int>(size);

  _randomTexture
    = DynamicTexture::New("SSAORandomTexture", options, _scene, false,
                          TextureConstants::TRILINEAR_SAMPLINGMODE);
  _randomTexture->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _randomTexture->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  auto context
    = std::static_pointer_cast<DynamicTexture>(_randomTexture)->getContext();

  const auto rand
    = [](float min, float max) { return Math::random() * (max - min) + min; };

  auto randVector = Vector3::Zero();

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
