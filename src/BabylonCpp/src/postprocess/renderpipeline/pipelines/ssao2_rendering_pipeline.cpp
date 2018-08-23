#include <babylon/postprocess/renderpipeline/pipelines/ssao2_rendering_pipeline.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/core/random.h>
#include <babylon/core/variant.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas_rendering_context2D.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/dynamic_texture.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/scalar.h>
#include <babylon/math/tmp.h>
#include <babylon/math/vector4.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/rendering/depth_renderer.h>
#include <babylon/rendering/geometry_buffer_renderer.h>

namespace BABYLON {

SSAO2RenderingPipeline::SSAO2RenderingPipeline(
  const string_t& name, Scene* scene, float ratio,
  const vector_t<CameraPtr>& cameras)
    : SSAO2RenderingPipeline(name, scene, {ratio, ratio}, cameras)
{
}

SSAO2RenderingPipeline::SSAO2RenderingPipeline(
  const string_t& name, Scene* scene, const SSAO2Ratio& iRatio,
  const vector_t<CameraPtr>& cameras)
    : PostProcessRenderPipeline(scene->getEngine(), name)
    , totalStrength{1.f}
    , maxZ{100.f}
    , minZAspect{0.2f}
    , samples{this, &SSAO2RenderingPipeline::get_samples,
              &SSAO2RenderingPipeline::set_samples}
    , expensiveBlur{this, &SSAO2RenderingPipeline::get_expensiveBlur,
                    &SSAO2RenderingPipeline::set_expensiveBlur}
    , radius{2.f}
    , area{0.0075f}
    , fallOff{0.000001f}
    , base{0.1f}
    , _samples{8}
    , _ratio{iRatio}
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

  auto ssaoRatio = _ratio.ssaoRatio;
  auto blurRatio = _ratio.blurRatio;

  // Set up assets
  auto geometryBufferRenderer = scene->enableGeometryBufferRenderer();
  _createRandomTexture();
  _depthTexture  = geometryBufferRenderer->getGBuffer()->textures()[0];
  _normalTexture = geometryBufferRenderer->getGBuffer()->textures()[1];

  _originalColorPostProcess = new PassPostProcess(
    "SSAOOriginalSceneColor", 1.f, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false);
  _createSSAOPostProcess(1.0);
  _createBlurPostProcess(ssaoRatio, blurRatio);
  _createSSAOCombinePostProcess(blurRatio);

  // Set up pipeline
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), SSAOOriginalSceneColorEffect,
    [&]() -> vector_t<PostProcess*> { return {_originalColorPostProcess}; },
    true));
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), SSAORenderEffect,
    [&]() -> vector_t<PostProcess*> { return {_ssaoPostProcess}; }, true));
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), SSAOBlurHRenderEffect,
    [&]() -> vector_t<PostProcess*> { return {_blurHPostProcess}; }, true));
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), SSAOBlurVRenderEffect,
    [&]() -> vector_t<PostProcess*> { return {_blurVPostProcess}; }, true));

  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), SSAOCombineRenderEffect,
    [&]() -> vector_t<PostProcess*> { return {_ssaoCombinePostProcess}; },
    true));

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

void SSAO2RenderingPipeline::set_samples(unsigned int n)
{
  _ssaoPostProcess->updateEffect("#define SAMPLES " + ::std::to_string(n)
                                 + "\n#define SSAO");
  _samples      = n;
  _sampleSphere = _generateHemisphere();

  _firstUpdate = true;
}

unsigned int SSAO2RenderingPipeline::get_samples() const
{
  return _samples;
}

void SSAO2RenderingPipeline::set_expensiveBlur(bool b)
{
  _blurHPostProcess->updateEffect(
    "#define BILATERAL_BLUR\n#define BILATERAL_BLUR_H\n#define SAMPLES "
    "16\n#define EXPENSIVE "
      + string_t(b ? "1" : "0") + "\n",
    {}, {"textureSampler", "depthSampler"});
  _blurVPostProcess->updateEffect(
    "#define BILATERAL_BLUR\n#define SAMPLES 16\n#define EXPENSIVE "
      + string_t(b ? "1" : "0") + "\n",
    {}, {"textureSampler", "depthSampler"});
  _expensiveBlur = b;
  _firstUpdate   = true;
}

bool SSAO2RenderingPipeline::get_expensiveBlur() const
{
  return _expensiveBlur;
}

bool SSAO2RenderingPipeline::IsSupported()
{
  auto engine = Engine::LastCreatedEngine();
  if (!engine) {
    return false;
  }
  return engine->getCaps().drawBuffersExtension;
}

void SSAO2RenderingPipeline::dispose(bool disableGeometryBufferRenderer,
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

  if (disableGeometryBufferRenderer) {
    _scene->disableGeometryBufferRenderer();
  }

  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
    _name, _scene->cameras);
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
    "BlurH", "ssao", {"outSize", "samplerOffsets"}, {"depthSampler"},
    ToVariant<float, PostProcessOptions>(ssaoRatio), nullptr,
    TextureConstants::TRILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
    "#define BILATERAL_BLUR\n#define BILATERAL_BLUR_H\n#define SAMPLES 16");
  _blurHPostProcess->setOnApply([&](Effect* effect, EventState&) {
    if (!_scene->activeCamera) {
      return;
    }
    effect->setFloat("outSize",
                     static_cast<float>(_ssaoCombinePostProcess->width > 0 ?
                                          _ssaoCombinePostProcess->width :
                                          _originalColorPostProcess->width));
    effect->setFloat("near", _scene->activeCamera->minZ);
    effect->setFloat("far", _scene->activeCamera->maxZ);
    effect->setFloat("radius", radius);
    effect->setTexture("depthSampler", _depthTexture);

    if (_firstUpdate) {
      effect->setArray("samplerOffsets", samplerOffsets);
    }
  });

  _blurVPostProcess = new PostProcess(
    "BlurV", "ssao", {"outSize", "samplerOffsets"}, {"depthSampler"},
    ToVariant<float, PostProcessOptions>(blurRatio), nullptr,
    TextureConstants::TRILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
    "#define BILATERAL_BLUR\n#define SAMPLES 16");
  _blurVPostProcess->setOnApply([&](Effect* effect, EventState&) {
    if (!_scene->activeCamera) {
      return;
    }
    effect->setFloat("outSize",
                     static_cast<float>(_ssaoCombinePostProcess->height > 0 ?
                                          _ssaoCombinePostProcess->height :
                                          _originalColorPostProcess->height));
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

void SSAO2RenderingPipeline::_rebuild()
{
  _firstUpdate = true;
  PostProcessRenderPipeline::_rebuild();
}

Float32Array SSAO2RenderingPipeline::_generateHemisphere()
{
  auto numSamples = samples();
  Float32Array result;
  Vector3 vector;
  float scale = 0.f;

  const auto rand
    = [](float min, float max) { return Math::random() * (max - min) + min; };

  for (unsigned int i = 0; i < numSamples; ++i) {
    vector = Vector3(rand(-1.f, 1.f), rand(-1.f, 1.f), rand(0.3f, 1.f));
    vector.normalize();
    scale = static_cast<float>(i) / static_cast<float>(numSamples);
    scale = Scalar::Lerp(0.1f, 1.f, scale * scale);
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
    {"randomSampler", "normalSampler"},
    ToVariant<float, PostProcessOptions>(ratio), nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
    "#define SAMPLES " + ::std::to_string(numSamples) + "\n#define SSAO");

  _ssaoPostProcess->setOnApply([&](Effect* effect, EventState&) {
    if (_firstUpdate) {
      effect->setArray3("sampleSphere", sampleSphere);
      effect->setFloat("randTextureTiles", 4.f);
    }

    if (!_scene->activeCamera) {
      return;
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
    effect->setFloat("xViewport", ::std::tan(_scene->activeCamera->fov / 2.f)
                                    * _scene->getEngine()->getAspectRatio(
                                        *_scene->activeCamera, true));
    effect->setFloat("yViewport", ::std::tan(_scene->activeCamera->fov / 2.f));
    effect->setMatrix("projection", _scene->getProjectionMatrix());

    effect->setTexture("textureSampler", _depthTexture);
    effect->setTexture("normalSampler", _normalTexture);
    effect->setTexture("randomSampler", _randomTexture);
  });
}

void SSAO2RenderingPipeline::_createSSAOCombinePostProcess(float ratio)
{
  _ssaoCombinePostProcess = new PostProcess(
    "ssaoCombine", "ssaoCombine", {}, {"originalColor", "viewport"},
    ToVariant<float, PostProcessOptions>(ratio), nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false);

  _ssaoCombinePostProcess->setOnApply([&](Effect* effect, EventState&) {
    const auto& viewport = _scene->activeCamera->viewport;
    effect->setVector4("viewport", Tmp::Vector4Array[0].copyFromFloats(
                                     static_cast<float>(viewport.x),
                                     static_cast<float>(viewport.y),
                                     static_cast<float>(viewport.width),
                                     static_cast<float>(viewport.height)));
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
    = DynamicTexture::New("SSAORandomTexture", options, _scene, false,
                          TextureConstants::TRILINEAR_SAMPLINGMODE);
  _randomTexture->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _randomTexture->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  auto context
    = ::std::static_pointer_cast<DynamicTexture>(_randomTexture)->getContext();

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
      randVector.x = ::std::floor(randVector.x);
      randVector.y = ::std::floor(randVector.y);

      context->fillStyle = "rgb(" + ::std::to_string(randVector.x) + ", "
                           + ::std::to_string(randVector.y) + ", "
                           + ::std::to_string(randVector.z) + ")";
      context->fillRect(static_cast<int>(x), static_cast<int>(y), 1, 1);
    }
  }

  _randomTexture->update(false);
}

Json::object SSAO2RenderingPipeline::serialize() const
{
  return Json::object();
}

unique_ptr_t<SSAO2RenderingPipeline>
SSAO2RenderingPipeline::Parse(const Json::value& /*source*/, Scene* /*scene*/,
                              const string_t& /*url*/)
{
  return nullptr;
}

} // end of namespace BABYLON
