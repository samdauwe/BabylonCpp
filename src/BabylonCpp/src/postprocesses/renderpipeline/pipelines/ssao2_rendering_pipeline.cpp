#include <babylon/postprocesses/renderpipeline/pipelines/ssao2_rendering_pipeline.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/core/random.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas_rendering_context2D.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/dynamic_texture.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/maths/vector4.h>
#include <babylon/postprocesses/blur_post_process.h>
#include <babylon/postprocesses/pass_post_process.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/rendering/depth_renderer.h>
#include <babylon/rendering/geometry_buffer_renderer.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/ssao2_configuration.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

SSAO2RenderingPipeline::SSAO2RenderingPipeline(const std::string& iName, Scene* scene, float ratio,
                                               const std::vector<CameraPtr>& iCameras,
                                               bool forceGeometryBuffer)
    : SSAO2RenderingPipeline(iName, scene, {ratio, ratio}, iCameras, forceGeometryBuffer)
{
}

SSAO2RenderingPipeline::SSAO2RenderingPipeline(const std::string& iName, Scene* scene,
                                               const SSAO2Ratio& iRatio,
                                               const std::vector<CameraPtr>& iCameras,
                                               bool forceGeometryBuffer)
    : PostProcessRenderPipeline(scene->getEngine(), iName)
    , totalStrength{1.f}
    , maxZ{100.f}
    , minZAspect{0.2f}
    , samples{this, &SSAO2RenderingPipeline::get_samples, &SSAO2RenderingPipeline::set_samples}
    , textureSamples{this, &SSAO2RenderingPipeline::get_textureSamples,
                     &SSAO2RenderingPipeline::set_textureSamples}
    , expensiveBlur{this, &SSAO2RenderingPipeline::get_expensiveBlur,
                    &SSAO2RenderingPipeline::set_expensiveBlur}
    , radius{2.f}
    , area{0.0075f}
    , fallOff{0.000001f}
    , base{0.f}
    , _samples{8}
    , _textureSamples{1}
    , _forceGeometryBuffer{false}
    , _expensiveBlur{true}
    , _cameraList{iCameras}
    , _randomTexture{nullptr}
    , _originalColorPostProcess{nullptr}
    , _ssaoPostProcess{nullptr}
    , _blurHPostProcess{nullptr}
    , _blurVPostProcess{nullptr}
    , _ssaoCombinePostProcess{nullptr}
    , _prePassRenderer{nullptr}
{
  _scene               = scene;
  _ratio               = iRatio;
  _forceGeometryBuffer = forceGeometryBuffer;

  if (!isSupported()) {
    BABYLON_LOG_ERROR("SSAO2RenderingPipeline", "SSAO 2 needs WebGL 2 support.")
    return;
  }

  _bits.resize(1);

  auto ssaoRatio = _ratio.ssaoRatio;
  auto blurRatio = _ratio.blurRatio;

  // Set up assets
  if (_forceGeometryBuffer) {
    scene->enableGeometryBufferRenderer();
  }
  else {
    _prePassRenderer = scene->enablePrePassRenderer();
  }

  _createRandomTexture();

  _originalColorPostProcess
    = PassPostProcess::New("SSAOOriginalSceneColor", 1.f, nullptr,
                           TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false);
  _originalColorPostProcess->samples = textureSamples;
  _createSSAOPostProcess(1.f);
  _createBlurPostProcess(ssaoRatio, blurRatio);
  _createSSAOCombinePostProcess(blurRatio);

  // Set up pipeline
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAOOriginalSceneColorEffect,
    [=]() -> std::vector<PostProcessPtr> { return {_originalColorPostProcess}; }, true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAORenderEffect,
    [=]() -> std::vector<PostProcessPtr> { return {_ssaoPostProcess}; }, true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAOBlurHRenderEffect,
    [=]() -> std::vector<PostProcessPtr> { return {_blurHPostProcess}; }, true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAOBlurVRenderEffect,
    [=]() -> std::vector<PostProcessPtr> { return {_blurVPostProcess}; }, true));

  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), SSAOCombineRenderEffect,
    [=]() -> std::vector<PostProcessPtr> { return {_ssaoCombinePostProcess}; }, true));
}

SSAO2RenderingPipeline::~SSAO2RenderingPipeline() = default;

void SSAO2RenderingPipeline::addToScene(const SSAO2RenderingPipelinePtr& ssao2RenderingPipeline)
{
  // Finish
  _scene->postProcessRenderPipelineManager()->addPipeline(ssao2RenderingPipeline);
  if (!_cameraList.empty()) {
    _scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(_name, _cameraList);
  }
}

Scene* SSAO2RenderingPipeline::scene() const
{
  return _scene;
}

std::string SSAO2RenderingPipeline::getClassName() const
{
  return "SSAO2RenderingPipeline";
}

void SSAO2RenderingPipeline::set_samples(unsigned int n)
{
  _samples = n;
  _ssaoPostProcess->updateEffect(_getDefinesForSSAO());
  _sampleSphere = _generateHemisphere();
}

unsigned int SSAO2RenderingPipeline::get_samples() const
{
  return _samples;
}

void SSAO2RenderingPipeline::set_textureSamples(unsigned int n)
{
  _textureSamples = n;

  _originalColorPostProcess->samples = n;
  _blurHPostProcess->samples         = n;
  _blurVPostProcess->samples         = n;
  _ssaoPostProcess->samples          = n;
  _ssaoCombinePostProcess->samples   = n;
}

unsigned int SSAO2RenderingPipeline::get_textureSamples() const
{
  return _textureSamples;
}

void SSAO2RenderingPipeline::set_expensiveBlur(bool b)
{
  _blurHPostProcess->updateEffect(
    "#define BILATERAL_BLUR\n#define BILATERAL_BLUR_H\n#define SAMPLES "
    "16\n#define EXPENSIVE "
      + std::string(b ? "1" : "0") + "\n",
    {}, {"textureSampler", "depthNormalSampler"});
  _blurVPostProcess->updateEffect("#define BILATERAL_BLUR\n#define SAMPLES 16\n#define EXPENSIVE "
                                    + std::string(b ? "1" : "0") + "\n",
                                  {}, {"textureSampler", "depthNormalSampler"});
  _expensiveBlur = b;
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
  return engine->webGLVersion() >= 2.f;
}

void SSAO2RenderingPipeline::dispose(bool disableGeometryBufferRenderer,
                                     bool disposeMaterialAndTextures)
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

  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(_name,
                                                                              _scene->cameras);

  PostProcessRenderPipeline::dispose(disableGeometryBufferRenderer, disposeMaterialAndTextures);
}

void SSAO2RenderingPipeline::_createBlurPostProcess(float ssaoRatio, float blurRatio)
{
  Float32Array samplerOffsets;
  samplerOffsets.reserve(16);
  auto expensive = expensiveBlur();

  for (int i = -8; i < 8; ++i) {
    samplerOffsets.emplace_back(static_cast<float>(i) * 2.f + 0.5f);
  }

  _blurHPostProcess = PostProcess::New(
    "BlurH", "ssao", {"outSize", "samplerOffsets"}, {"depthNormalSampler"}, ssaoRatio, nullptr,
    TextureConstants::TRILINEAR_SAMPLINGMODE, _scene->getEngine(), false,
    "#define BILATERAL_BLUR\n#define BILATERAL_BLUR_H\n#define SAMPLES 16\n#define EXPENSIVE "
      + std::string(expensive ? "1" : "0") + "\n");
  _blurHPostProcess->onApply = [=](Effect* effect, EventState&) {
    if (!_scene->activeCamera()) {
      return;
    }
    effect->setFloat("outSize", static_cast<float>(_ssaoCombinePostProcess->width > 0 ?
                                                     _ssaoCombinePostProcess->width :
                                                     _originalColorPostProcess->width));
    effect->setFloat("near", _scene->activeCamera()->minZ);
    effect->setFloat("far", _scene->activeCamera()->maxZ);
    effect->setFloat("radius", radius);
    if (_forceGeometryBuffer) {
      effect->setTexture("depthNormalSampler",
                         _scene->enableGeometryBufferRenderer()->getGBuffer()->textures()[0]);
    }
    else {
      effect->setTexture(
        "depthNormalSampler",
        _prePassRenderer->prePassRT
          ->textures()[_prePassRenderer->getIndex(Constants::PREPASS_DEPTH_TEXTURE_TYPE)]);
    }
    effect->setArray("samplerOffsets", samplerOffsets);
  };

  _blurVPostProcess
    = PostProcess::New("BlurV", "ssao", {"outSize", "samplerOffsets"}, {"depthSampler"}, blurRatio,
                       nullptr, TextureConstants::TRILINEAR_SAMPLINGMODE, _scene->getEngine(),
                       false, "#define BILATERAL_BLUR\n#define SAMPLES 16");
  _blurVPostProcess->onApply = [=](Effect* effect, EventState&) {
    if (!_scene->activeCamera()) {
      return;
    }
    effect->setFloat("outSize", static_cast<float>(_ssaoCombinePostProcess->height > 0 ?
                                                     _ssaoCombinePostProcess->height :
                                                     _originalColorPostProcess->height));
    effect->setFloat("near", _scene->activeCamera()->minZ);
    effect->setFloat("far", _scene->activeCamera()->maxZ);
    effect->setFloat("radius", radius);
    if (_forceGeometryBuffer) {
      effect->setTexture("depthNormalSampler",
                         _scene->enableGeometryBufferRenderer()->getGBuffer()->textures()[0]);
    }
    else {
      effect->setTexture(
        "depthNormalSampler",
        _prePassRenderer->prePassRT
          ->textures()[_prePassRenderer->getIndex(Constants::PREPASS_DEPTH_TEXTURE_TYPE)]);
    }
    effect->setArray("samplerOffsets", samplerOffsets);
  };

  _blurHPostProcess->samples = textureSamples;
  _blurVPostProcess->samples = textureSamples;
}

void SSAO2RenderingPipeline::_rebuild()
{
  PostProcessRenderPipeline::_rebuild();
}

float SSAO2RenderingPipeline::_radicalInverse_VdC(uint32_t i)
{
  _bits[0] = i;
  _bits[0] = ((_bits[0] << 16) | (_bits[0] >> 16)) >> 0;
  _bits[0] = ((_bits[0] & 0x55555555) << 1) | ((_bits[0] & 0xAAAAAAAA) >> 1) >> 0;
  _bits[0] = ((_bits[0] & 0x33333333) << 2) | ((_bits[0] & 0xCCCCCCCC) >> 2) >> 0;
  _bits[0] = ((_bits[0] & 0x0F0F0F0F) << 4) | ((_bits[0] & 0xF0F0F0F0) >> 4) >> 0;
  _bits[0] = ((_bits[0] & 0x00FF00FF) << 8) | ((_bits[0] & 0xFF00FF00) >> 8) >> 0;
  return _bits[0] * 2.3283064365386963e-10f; // / 0x100000000 or / 4294967296
}

std::array<float, 2> SSAO2RenderingPipeline::_hammersley(uint32_t i, uint32_t n)
{
  return {{static_cast<float>(i) / static_cast<float>(n), _radicalInverse_VdC(i)}};
}

Vector3 SSAO2RenderingPipeline::_hemisphereSample_uniform(float u, float v)
{
  const auto phi = v * 2.f * Math::PI;
  // rejecting samples that are close to tangent plane to avoid z-fighting artifacts
  const auto cosTheta = 1.f - (u * 0.85f + 0.15f);
  const auto sinTheta = std::sqrt(1.f - cosTheta * cosTheta);
  return Vector3(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta, cosTheta);
}

Float32Array SSAO2RenderingPipeline::_generateHemisphere()
{
  auto numSamples = samples();
  Float32Array result;
  Vector3 vector;

  for (unsigned int i = 0; i < numSamples; ++i) {
    if (numSamples < 16) {
      vector = _hemisphereSample_uniform(Math::random(), Math::random());
    }
    else {
      auto rand = _hammersley(i, numSamples);
      vector    = _hemisphereSample_uniform(rand[0], rand[1]);
    }

    stl_util::concat(result, {vector.x, vector.y, vector.z});
  }

  return result;
}

std::string SSAO2RenderingPipeline::_getDefinesForSSAO()
{
  std::string defines = "#define SAMPLES " + std::to_string(samples()) + "\n#define SSAO";

  if (_forceGeometryBuffer) {
    defines = defines + "\n#define GEOMETRYBUFFER";
  }

  return defines;
}

void SSAO2RenderingPipeline::_createSSAOPostProcess(float ratio)
{
  const auto sampleSphere = _generateHemisphere();

  const auto defines = _getDefinesForSSAO();
  std::vector<std::string> samplers;

  if (_forceGeometryBuffer) {
    samplers = {"randomSampler", "depthSampler", "normalSampler"};
  }
  else {
    samplers = {"randomSampler", "depthNormalSampler"};
  }

  _ssaoPostProcess
    = PostProcess::New("ssao2", "ssao2",
                       {"sampleSphere", "samplesFactor", "randTextureTiles", "totalStrength",
                        "radius", "base", "range", "projection", "near", "far", "texelSize",
                        "xViewport", "yViewport", "maxZ", "minZAspect"},
                       samplers, ratio, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
                       _scene->getEngine(), false, defines);

  _ssaoPostProcess->onApply = [&](Effect* effect, EventState&) {
    if (!_scene->activeCamera()) {
      return;
    }

    effect->setArray3("sampleSphere", sampleSphere);
    effect->setFloat("randTextureTiles", 32.f);
    effect->setFloat("samplesFactor", 1.f / static_cast<float>(samples()));
    effect->setFloat("totalStrength", totalStrength);
    effect->setFloat2("texelSize", 1.f / static_cast<float>(_ssaoPostProcess->width),
                      1.f / static_cast<float>(_ssaoPostProcess->height));
    effect->setFloat("radius", radius);
    effect->setFloat("maxZ", maxZ);
    effect->setFloat("minZAspect", minZAspect);
    effect->setFloat("base", base);
    effect->setFloat("near", _scene->activeCamera()->minZ);
    effect->setFloat("far", _scene->activeCamera()->maxZ);
    effect->setFloat("xViewport",
                     std::tan(_scene->activeCamera()->fov / 2.f)
                       * _scene->getEngine()->getAspectRatio(*_scene->activeCamera(), true));
    effect->setFloat("yViewport", std::tan(_scene->activeCamera()->fov / 2.f));
    effect->setMatrix("projection", _scene->getProjectionMatrix());

    if (_forceGeometryBuffer) {
      effect->setTexture("depthSampler",
                         _scene->enableGeometryBufferRenderer()->getGBuffer()->textures()[0]);
      effect->setTexture("normalSampler",
                         _scene->enableGeometryBufferRenderer()->getGBuffer()->textures()[1]);
    }
    else {
      effect->setTexture(
        "depthNormalSampler",
        _prePassRenderer->prePassRT
          ->textures()[_prePassRenderer->getIndex(Constants::PREPASS_DEPTH_TEXTURE_TYPE)]);
    }
    effect->setTexture("randomSampler", _randomTexture);
  };
  _ssaoPostProcess->samples = textureSamples;
}

void SSAO2RenderingPipeline::_createSSAOCombinePostProcess(float ratio)
{
  _ssaoCombinePostProcess = PostProcess::New(
    "ssaoCombine", "ssaoCombine", {}, {"originalColor", "viewport"}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false);

  _ssaoCombinePostProcess->onApply = [&](Effect* effect, EventState&) {
    const auto& viewport = _scene->activeCamera()->viewport;
    effect->setVector4("viewport",
                       TmpVectors::Vector4Array[0].copyFromFloats(
                         static_cast<float>(viewport.x), static_cast<float>(viewport.y),
                         static_cast<float>(viewport.width), static_cast<float>(viewport.height)));
    effect->setTextureFromPostProcessOutput("originalColor", _originalColorPostProcess);
  };
  _ssaoCombinePostProcess->samples = textureSamples;

  if (!_forceGeometryBuffer) {
    _ssaoCombinePostProcess->_prePassEffectConfiguration = std::make_shared<SSAO2Configuration>();
  }
}

void SSAO2RenderingPipeline::_createRandomTexture()
{
  size_t size = 128;

  DynamicTextureOptions options;
  options.width  = static_cast<int>(size);
  options.height = static_cast<int>(size);

  _randomTexture        = DynamicTexture::New("SSAORandomTexture", options, _scene, false,
                                       TextureConstants::TRILINEAR_SAMPLINGMODE);
  _randomTexture->wrapU = TextureConstants::WRAP_ADDRESSMODE;
  _randomTexture->wrapV = TextureConstants::WRAP_ADDRESSMODE;

  auto context = std::static_pointer_cast<DynamicTexture>(_randomTexture)->getContext();

  const auto rand = [](float min, float max) { return Math::random() * (max - min) + min; };

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

      context->fillStyle = "rgb(" + std::to_string(randVector.x) + ", "
                           + std::to_string(randVector.y) + ", " + std::to_string(randVector.z)
                           + ")";
      context->fillRect(static_cast<int>(x), static_cast<int>(y), 1, 1);
    }
  }

  _randomTexture->update(false);
}

json SSAO2RenderingPipeline::serialize() const
{
  return nullptr;
}

std::unique_ptr<SSAO2RenderingPipeline>
SSAO2RenderingPipeline::Parse(const json& /*source*/, Scene* /*scene*/, const std::string& /*url*/)
{
  return nullptr;
}

} // end of namespace BABYLON
