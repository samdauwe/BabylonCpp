#include <babylon/postprocess/renderpipeline/pipelines/standard_rendering_pipeline.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/rendering/depth_renderer.h>
#include <babylon/rendering/geometry_buffer_renderer.h>

namespace BABYLON {

unsigned int StandardRenderingPipeline::LuminanceSteps = 6;

StandardRenderingPipeline::StandardRenderingPipeline(
  const std::string& iName, Scene* scene, float ratio,
  PostProcess* iOriginalPostProcess, const std::vector<CameraPtr>& cameras)
    : PostProcessRenderPipeline{scene->getEngine(), iName}
    , originalPostProcess{iOriginalPostProcess}
    , downSampleX4PostProcess{nullptr}
    , brightPassPostProcess{nullptr}
    , textureAdderPostProcess{nullptr}
    , volumetricLightPostProcess{nullptr}
    , volumetricLightSmoothXPostProcess{nullptr}
    , volumetricLightSmoothYPostProcess{nullptr}
    , volumetricLightMergePostProces{nullptr}
    , volumetricLightFinalPostProcess{nullptr}
    , luminancePostProcess{nullptr}
    , hdrPostProcess{nullptr}
    , textureAdderFinalPostProcess{nullptr}
    , lensFlareFinalPostProcess{nullptr}
    , hdrFinalPostProcess{nullptr}
    , lensFlarePostProcess{nullptr}
    , lensFlareComposePostProcess{nullptr}
    , motionBlurPostProcess{nullptr}
    , depthOfFieldPostProcess{nullptr}
    , brightThreshold{1.f}
    , blurWidth{512.f}
    , horizontalBlur{false}
    , exposure{1.f}
    , lensTexture{nullptr}
    , volumetricLightCoefficient{0.2f}
    , volumetricLightPower{4.f}
    , volumetricLightBlurScale{64.f}
    , sourceLight{nullptr}
    , hdrMinimumLuminance{1.f}
    , hdrDecreaseRate{0.5f}
    , hdrIncreaseRate{0.5f}
    , lensColorTexture{nullptr}
    , lensFlareStrength{20.f}
    , lensFlareGhostDispersal{1.4f}
    , lensFlareHaloWidth{0.7f}
    , lensFlareDistortionStrength{16.f}
    , lensStarTexture{nullptr}
    , lensFlareDirtTexture{nullptr}
    , depthOfFieldDistance{10.f}
    , depthOfFieldBlurWidth{64.f}
    , motionStrength{1.f}
    , _scene{scene}
    , _currentDepthOfFieldSource{nullptr}
    , _basePostProcess{iOriginalPostProcess}
    , _hdrCurrentLuminance{1.f}
    , _floatTextureType{scene->getEngine()->getCaps().textureFloatRender ?
                          EngineConstants::TEXTURETYPE_FLOAT :
                          EngineConstants::TEXTURETYPE_HALF_FLOAT}
    , _ratio{ratio}
    , _bloomEnabled{true}
    , _depthOfFieldEnabled{false}
    , _vlsEnabled{false}
    , _lensFlareEnabled{false}
    , _hdrEnabled{false}
    , _motionBlurEnabled{false}
    , _motionBlurSamples{64.f}
    , _volumetricLightStepsCount{50.f}
{
  for (auto& camera : cameras) {
    _cameras[camera->name] = camera;
  }
}

StandardRenderingPipeline::~StandardRenderingPipeline()
{
}

void StandardRenderingPipeline::addToScene(
  const StandardRenderingPipelinePtr& renderingPipeline)
{
  // Finish
  _scene->postProcessRenderPipelineManager()->addPipeline(renderingPipeline);
  renderingPipeline->_buildPipeline();
}

IReflect::Type StandardRenderingPipeline::type() const
{
  return IReflect::Type::STANDARDRENDERINGPIPLINE;
}

float StandardRenderingPipeline::operator[](const std::string& key) const
{
  if (key == "brightThreshold") {
    return brightThreshold;
  }
  else if (key == "blurWidth") {
    return blurWidth;
  }
  else if (key == "exposure") {
    return exposure;
  }
  else if (key == "volumetricLightCoefficient") {
    return volumetricLightCoefficient;
  }
  else if (key == "volumetricLightPower") {
    return volumetricLightPower;
  }
  else if (key == "volumetricLightBlurScale") {
    return volumetricLightBlurScale;
  }
  else if (key == "hdrMinimumLuminance") {
    return hdrMinimumLuminance;
  }
  else if (key == "hdrDecreaseRate") {
    return hdrDecreaseRate;
  }
  else if (key == "hdrIncreaseRate") {
    return hdrIncreaseRate;
  }
  else if (key == "lensFlareStrength") {
    return lensFlareStrength;
  }
  else if (key == "lensFlareGhostDispersal") {
    return lensFlareGhostDispersal;
  }
  else if (key == "lensFlareHaloWidth") {
    return lensFlareHaloWidth;
  }
  else if (key == "lensFlareDistortionStrength") {
    return lensFlareDistortionStrength;
  }
  else if (key == "depthOfFieldDistance") {
    return depthOfFieldDistance;
  }
  else if (key == "depthOfFieldBlurWidth") {
    return depthOfFieldBlurWidth;
  }
  else if (key == "motionStrength") {
    return motionStrength;
  }

  return 0.f;
}

bool StandardRenderingPipeline::bloomEnabled() const
{
  return _bloomEnabled;
}

void StandardRenderingPipeline::setBloomEnabled(bool enabled)
{
  if (_bloomEnabled == enabled) {
    return;
  }

  _bloomEnabled = enabled;
  _buildPipeline();
}

bool StandardRenderingPipeline::depthOfFieldEnabled() const
{
  return _depthOfFieldEnabled;
}

void StandardRenderingPipeline::setDepthOfFieldEnabled(bool enabled)
{
  if (_depthOfFieldEnabled == enabled) {
    return;
  }

  _depthOfFieldEnabled = enabled;
  _buildPipeline();
}

bool StandardRenderingPipeline::lensFlareEnabled() const
{
  return _lensFlareEnabled;
}

void StandardRenderingPipeline::setLensFlareEnabled(bool enabled)
{
  if (_lensFlareEnabled == enabled) {
    return;
  }

  _lensFlareEnabled = enabled;
  _buildPipeline();
}

bool StandardRenderingPipeline::HDREnabled() const
{
  return _hdrEnabled;
}

void StandardRenderingPipeline::setHDREnabled(bool enabled)
{
  if (_hdrEnabled == enabled) {
    return;
  }

  _hdrEnabled = enabled;
  _buildPipeline();
}

bool StandardRenderingPipeline::VLSEnabled() const
{
  return _vlsEnabled;
}

void StandardRenderingPipeline::setVLSEnabled(bool enabled)
{
  if (_vlsEnabled == enabled) {
    return;
  }

  if (enabled) {
    auto geometry = _scene->enableGeometryBufferRenderer();
    if (!geometry) {
      BABYLON_LOG_WARN("StandardRenderingPipeline",
                       "Geometry renderer is not supported, cannot create "
                       "volumetric lights in Standard Rendering Pipeline");
      return;
    }
  }

  _vlsEnabled = enabled;
  _buildPipeline();
}

bool StandardRenderingPipeline::motionBlurEnabled() const
{
  return _motionBlurEnabled;
}

void StandardRenderingPipeline::setMotionBlurEnabled(bool enabled)
{
  if (_motionBlurEnabled == enabled) {
    return;
  }

  _motionBlurEnabled = enabled;
  _buildPipeline();
}

float StandardRenderingPipeline::volumetricLightStepsCount() const
{
  return _volumetricLightStepsCount;
}

void StandardRenderingPipeline::setVolumetricLightStepsCount(float count)
{
  if (volumetricLightPostProcess) {
    volumetricLightPostProcess->updateEffect(
      "#define VLS\n#define NB_STEPS "
      + std::to_string(std::round(count * 10.f) / 10.f));
  }

  _volumetricLightStepsCount = count;
}

float StandardRenderingPipeline::motionBlurSamples() const
{
  return _motionBlurSamples;
}

void StandardRenderingPipeline::setMotionBlurSamples(float samples)
{
  if (motionBlurPostProcess) {
    motionBlurPostProcess->updateEffect(
      "#define MOTION_BLUR\n#define MAX_MOTION_SAMPLES "
      + std::to_string(std::round(samples * 10.f) / 10.f));
  }

  _motionBlurSamples = samples;
}

void StandardRenderingPipeline::_buildPipeline()
{
  auto ratio = _ratio;
  auto scene = _scene;

  _disposePostProcesses();
  _reset();

  // Create pass post-process
  if (!_basePostProcess) {
    originalPostProcess = PostProcess::New(
      "HDRPass", "standard", {}, {}, ratio, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
      "#define PASS_POST_PROCESS", _floatTextureType);
    originalPostProcess->setOnApply(
      [&](Effect* /*effect*/, EventState& /*es*/) {
        _currentDepthOfFieldSource = originalPostProcess;
      });
  }
  else {
    originalPostProcess = _basePostProcess;
  }

  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRPassPostProcess",
    [&]() -> std::vector<PostProcessPtr> { return {originalPostProcess}; },
    true));

  _currentDepthOfFieldSource = originalPostProcess;

  if (_bloomEnabled) {
    // Create down sample X4 post-process
    _createDownSampleX4PostProcess(scene, ratio / 2.f);

    // Create bright pass post-process
    _createBrightPassPostProcess(scene, ratio / 2.f);

    // Create gaussian blur post-processes (down sampling blurs)
    _createBlurPostProcesses(scene, ratio / 4.f, 1);

    // Create texture adder post-process
    _createTextureAdderPostProcess(scene, ratio);

    // Create depth-of-field source post-process
    textureAdderFinalPostProcess = PostProcess::New(
      "HDRDepthOfFieldSource", "standard", {}, {}, ratio, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
      "#define PASS_POST_PROCESS", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
    addEffect(PostProcessRenderEffect::New(
      scene->getEngine(), "HDRBaseDepthOfFieldSource",
      [this]() -> std::vector<PostProcessPtr> {
        return {textureAdderFinalPostProcess};
      },
      true));
  }

  if (_vlsEnabled) {
    // Create volumetric light
    _createVolumetricLightPostProcess(scene, ratio);

    // Create volumetric light final post-process
    volumetricLightFinalPostProcess = PostProcess::New(
      "HDRVLSFinal", "standard", {}, {}, ratio, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
      "#define PASS_POST_PROCESS", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
    addEffect(
      PostProcessRenderEffect::New(scene->getEngine(), "HDRVLSFinal",
                                   [this]() -> std::vector<PostProcessPtr> {
                                     return {volumetricLightFinalPostProcess};
                                   },
                                   true));
  }

  if (_lensFlareEnabled) {
    // Create lens flare post-process
    _createLensFlarePostProcess(scene, ratio);

    // Create depth-of-field source post-process post lens-flare and disable it
    // now
    lensFlareFinalPostProcess = PostProcess::New(
      "HDRPostLensFlareDepthOfFieldSource", "standard", {}, {}, ratio, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
      "#define PASS_POST_PROCESS", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
    addEffect(PostProcessRenderEffect::New(
      scene->getEngine(), "HDRPostLensFlareDepthOfFieldSource",
      [this]() -> std::vector<PostProcessPtr> {
        return {lensFlareFinalPostProcess};
      },
      true));
  }

  if (_hdrEnabled) {
    // Create luminance
    _createLuminancePostProcesses(scene, _floatTextureType);

    // Create HDR
    _createHdrPostProcess(scene, ratio);

    // Create depth-of-field source post-process post hdr and disable it now
    hdrFinalPostProcess = PostProcess::New(
      "HDRPostHDReDepthOfFieldSource", "standard", {}, {}, ratio, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
      "#define PASS_POST_PROCESS", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
    addEffect(PostProcessRenderEffect::New(
      scene->getEngine(), "HDRPostHDReDepthOfFieldSource",
      [this]() -> std::vector<PostProcessPtr> { return {hdrFinalPostProcess}; },
      true));
  }

  if (_depthOfFieldEnabled) {
    // Create gaussian blur used by depth-of-field
    _createBlurPostProcesses(scene, ratio / 2.f, 3, "depthOfFieldBlurWidth");

    // Create depth-of-field post-process
    _createDepthOfFieldPostProcess(scene, ratio);
  }

  if (_motionBlurEnabled) {
    // Create motion blur post-process
    _createMotionBlurPostProcess(scene, ratio);
  }

  if (!_cameras.empty()) {
    auto cameras = stl_util::extract_values(_cameras);
    _scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(
      _name, cameras);
  }
}

void StandardRenderingPipeline::_createDownSampleX4PostProcess(Scene* scene,
                                                               float ratio)
{
  downSampleX4PostProcess = PostProcess::New(
    "HDRDownSampleX4", "standard", {"dsOffsets"}, {}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define DOWN_SAMPLE_X4", EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  downSampleX4PostProcess->setOnApply([&](Effect* effect, EventState&) {
    Float32Array downSampleX4Offsets(32);
    unsigned int id   = 0;
    const auto width  = static_cast<float>(downSampleX4PostProcess->width);
    const auto height = static_cast<float>(downSampleX4PostProcess->height);
    for (int i = -2; i < 2; ++i) {
      for (int j = -2; j < 2; ++j) {
        downSampleX4Offsets[id]     = (i + 0.5f) * (1.f / width);
        downSampleX4Offsets[id + 1] = (j + 0.5f) * (1.f / height);
        id += 2;
      }
    }
    effect->setArray2("dsOffsets", downSampleX4Offsets);
  });

  // Add to pipeline
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRDownSampleX4",
    [&]() -> std::vector<PostProcessPtr> { return {downSampleX4PostProcess}; },
    true));
}

void StandardRenderingPipeline::_createBrightPassPostProcess(Scene* scene,
                                                             float ratio)
{
  brightPassPostProcess = PostProcess::New(
    "HDRBrightPass", "standard", {"dsOffsets", "brightThreshold"}, {}, ratio,
    nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define BRIGHT_PASS", EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  brightPassPostProcess->setOnApply([&](Effect* effect, EventState&) {
    const float sU = (1.f / brightPassPostProcess->width);
    const float sV = (1.f / brightPassPostProcess->height);

    Float32Array brightOffsets(8);
    brightOffsets[0] = -0.5f * sU;
    brightOffsets[1] = 0.5f * sV;
    brightOffsets[2] = 0.5f * sU;
    brightOffsets[3] = 0.5f * sV;
    brightOffsets[4] = -0.5f * sU;
    brightOffsets[5] = -0.5f * sV;
    brightOffsets[6] = 0.5f * sU;
    brightOffsets[7] = -0.5f * sV;

    effect->setArray2("dsOffsets", brightOffsets);
    effect->setFloat("brightThreshold", brightThreshold);
  });

  // Add to pipeline
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRBrightPass",
    [&]() -> std::vector<PostProcessPtr> { return {brightPassPostProcess}; },
    true));
}

void StandardRenderingPipeline::_createBlurPostProcesses(
  Scene* scene, float ratio, unsigned int indice,
  const std::string& blurWidthKey)
{
  auto engine = scene->getEngine();

  const std::string underscore = "_";
  const auto indiceStr         = std::to_string(indice);
  auto blurX                   = BlurPostProcess::New(
    "HDRBlurH" + underscore + indiceStr, Vector2(1.f, 0.f),
    (*this)[blurWidthKey], ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  auto blurY = BlurPostProcess::New(
    "HDRBlurV" + underscore + indiceStr, Vector2(0.f, 1.f),
    (*this)[blurWidthKey], ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  blurX->onActivateObservable.add([&](Camera* /*camera*/, EventState& /*es*/) {
    auto dw = static_cast<float>(blurX->width)
              / static_cast<float>(engine->getRenderWidth());
    blurX->kernel = (*this)[blurWidthKey] * dw;
  });

  blurY->onActivateObservable.add([&](Camera* /*camera*/, EventState& /*es*/) {
    auto dw = static_cast<float>(blurY->height)
              / static_cast<float>(engine->getRenderHeight());
    blurY->kernel = horizontalBlur ? 64.f * dw : (*this)[blurWidthKey] * dw;
  });

  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRBlurH" + indiceStr,
    [&]() -> std::vector<PostProcessPtr> { return {blurX}; }, true));
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRBlurV" + indiceStr,
    [&]() -> std::vector<PostProcessPtr> { return {blurY}; }, true));

  blurHPostProcesses.emplace_back(blurX);
  blurVPostProcesses.emplace_back(blurY);
}

void StandardRenderingPipeline::_createTextureAdderPostProcess(Scene* scene,
                                                               float ratio)
{
  textureAdderPostProcess = PostProcess::New(
    "HDRTextureAdder", "standard", {"exposure"},
    {"otherSampler", "lensSampler"}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define TEXTURE_ADDER", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  textureAdderPostProcess->setOnApply([&](Effect* effect, EventState& /*es*/) {
    effect->setTextureFromPostProcess(
      "otherSampler", _vlsEnabled ? _currentDepthOfFieldSource.get() :
                                    originalPostProcess.get());
    effect->setTexture("lensSampler", lensTexture);

    effect->setFloat("exposure", exposure);

    _currentDepthOfFieldSource = textureAdderFinalPostProcess;
  });

  // Add to pipeline
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRTextureAdder",
    [&]() -> std::vector<PostProcessPtr> { return {textureAdderPostProcess}; },
    true));
}

void StandardRenderingPipeline::_createVolumetricLightPostProcess(Scene* scene,
                                                                  float ratio)
{
  auto geometryRenderer            = scene->enableGeometryBufferRenderer();
  geometryRenderer->enablePosition = true;

  auto geometry = geometryRenderer->getGBuffer();

  // Base post-process
  volumetricLightPostProcess = PostProcess::New(
    "HDRVLS", "standard",
    {"shadowViewProjection", "cameraPosition", "sunDirection", "sunColor",
     "scatteringCoefficient", "scatteringPower", "depthValues"},
    {"shadowMapSampler", "positionSampler"}, ratio / 8.f, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define VLS\n#define NB_STEPS "
      + std::to_string(std::round(_volumetricLightStepsCount * 10.f) / 10.f));

  volumetricLightPostProcess->setOnApply(
    [&](Effect* effect, EventState& /*es*/) {
      if (sourceLight && sourceLight->getShadowGenerator()
          && _scene->activeCamera) {
        auto depthValues = Vector2::Zero();
        auto generator   = sourceLight->getShadowGenerator();

        effect->setTexture("shadowMapSampler", generator->getShadowMap());
        effect->setTexture("positionSampler", geometry->textures()[2]);

        effect->setColor3("sunColor", sourceLight->diffuse);
        effect->setVector3(
          "sunDirection",
          static_cast<IShadowLight*>(sourceLight)->getShadowDirection());

        effect->setVector3("cameraPosition",
                           scene->activeCamera->globalPosition());
        effect->setMatrix("shadowViewProjection",
                          generator->getTransformMatrix());

        effect->setFloat("scatteringCoefficient", volumetricLightCoefficient);
        effect->setFloat("scatteringPower", volumetricLightPower);

        depthValues.x
          = generator->getLight()->getDepthMinZ(*_scene->activeCamera);
        depthValues.y
          = generator->getLight()->getDepthMaxZ(*_scene->activeCamera);
        effect->setVector2("depthValues", depthValues);
      }
    });

  addEffect(
    PostProcessRenderEffect::New(scene->getEngine(), "HDRVLS",
                                 [this]() -> std::vector<PostProcessPtr> {
                                   return {volumetricLightPostProcess};
                                 },
                                 true));

  // Smooth
  _createBlurPostProcesses(scene, ratio / 4.f, 0.f, "volumetricLightBlurScale");

  // Merge
  volumetricLightMergePostProces
    = PostProcess::New("HDRVLSMerge", "standard", {}, {"originalSampler"},
                       ratio, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
                       scene->getEngine(), false, "#define VLSMERGE");

  volumetricLightMergePostProces->setOnApply(
    [this](Effect* effect, EventState& /*es*/) {
      effect->setTextureFromPostProcess(
        "originalSampler", _bloomEnabled ? textureAdderFinalPostProcess.get() :
                                           originalPostProcess.get());

      _currentDepthOfFieldSource = volumetricLightFinalPostProcess;
    });

  addEffect(
    PostProcessRenderEffect::New(scene->getEngine(), "HDRVLSMerge",
                                 [this]() -> std::vector<PostProcessPtr> {
                                   return {volumetricLightMergePostProces};
                                 },
                                 true));
}

void StandardRenderingPipeline::_createLuminancePostProcesses(
  Scene* scene, unsigned int textureType)
{
  // Create luminance
  float size = static_cast<float>(
    std::pow(3, StandardRenderingPipeline::LuminanceSteps));
  luminancePostProcess = PostProcess::New(
    "HDRLuminance", "standard", {"lumOffsets"}, {}, size, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define LUMINANCE", textureType);

  luminancePostProcess->setOnApply([&](Effect* effect, EventState&) {
    const float sU = (1.f / luminancePostProcess->width);
    const float sV = (1.f / luminancePostProcess->height);

    Float32Array offsets(8);
    offsets[0] = -0.5f * sU;
    offsets[1] = 0.5f * sV;
    offsets[2] = 0.5f * sU;
    offsets[3] = 0.5f * sV;
    offsets[4] = -0.5f * sU;
    offsets[5] = -0.5f * sV;
    offsets[6] = 0.5f * sU;
    offsets[7] = -0.5f * sV;

    effect->setArray2("lumOffsets", offsets);
  });

  // Add to pipeline
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRLuminance",
    [this]() -> std::vector<PostProcessPtr> { return {luminancePostProcess}; },
    true));

  // Create down sample luminance
  for (unsigned int i = StandardRenderingPipeline::LuminanceSteps; i-- > 0;) {
    const std::string iStr = std::to_string(i);
    float size             = static_cast<float>(std::pow(3, i));

    std::string defines = "#define LUMINANCE_DOWN_SAMPLE\n";
    if (i == 0) {
      defines += "#define FINAL_DOWN_SAMPLER";
    }

    auto postProcess
      = PostProcess::New("HDRLuminanceDownSample" + iStr, "standard",
                         {"dsOffsets", "halfDestPixelSize"}, {}, size, nullptr,
                         TextureConstants::BILINEAR_SAMPLINGMODE,
                         scene->getEngine(), false, defines, textureType);
    luminanceDownSamplePostProcesses.emplace_back(postProcess);
  }

  // Create callbacks and add effects
  auto lastLuminance = luminancePostProcess;

  std::size_t index = 0;
  for (auto& pp : luminanceDownSamplePostProcesses) {
    const std::string indexStr = std::to_string(index);
    Float32Array downSampleOffsets(18);

    pp->setOnApply([&](Effect* effect, EventState&) {
      if (!lastLuminance) {
        return;
      }

      unsigned int id = 0;
      for (float x = -1; x < 2; ++x) {
        for (float y = -1; y < 2; ++y) {
          downSampleOffsets[id]     = x / lastLuminance->width;
          downSampleOffsets[id + 1] = y / lastLuminance->height;
          id += 2;
        }
      }

      effect->setArray2("dsOffsets", downSampleOffsets);
      effect->setFloat("halfDestPixelSize", 0.5f / lastLuminance->width);

      if (index == luminanceDownSamplePostProcesses.size() - 1) {
        lastLuminance = luminancePostProcess;
      }
      else {
        lastLuminance = pp;
      }
    });

    if (index == luminanceDownSamplePostProcesses.size() - 1) {
      pp->setOnAfterRender([&](Effect*, EventState&) {
        auto pixel = scene->getEngine()->readPixels(0, 0, 1, 1);
        Vector4 bit_shift(1.f / (255.f * 255.f * 255.f), 1.f / (255.f * 255.f),
                          1.f / 255.f, 1.f);
        _hdrCurrentLuminance
          = (pixel[0] * bit_shift.x + pixel[1] * bit_shift.y
             + pixel[2] * bit_shift.z + pixel[3] * bit_shift.w)
            / 100.f;
      });
    }

    addEffect(PostProcessRenderEffect::New(
      scene->getEngine(), "HDRLuminanceDownSample" + indexStr,
      [&]() -> std::vector<PostProcessPtr> { return {pp}; }, true));
    ++index;
  };
}

void StandardRenderingPipeline::_createHdrPostProcess(Scene* scene, float ratio)
{
  hdrPostProcess = PostProcess::New(
    "HDR", "standard", {"averageLuminance"}, {"textureAdderSampler"}, ratio,
    nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define HDR", EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  float outputLiminance = 1.f;
  float time            = 0.f;
  float lastTime        = 0.f;

  hdrPostProcess->setOnApply([&](Effect* effect, EventState&) {
    effect->setTextureFromPostProcess("textureAdderSampler",
                                      _currentDepthOfFieldSource.get());

    time += scene->getEngine()->getDeltaTime();

    if (outputLiminance < 0) {
      outputLiminance = _hdrCurrentLuminance;
    }
    else {
      float dt = (lastTime - time) / 1000.f;

      if (_hdrCurrentLuminance < outputLiminance + hdrDecreaseRate * dt) {
        outputLiminance += hdrDecreaseRate * dt;
      }
      else if (_hdrCurrentLuminance > outputLiminance - hdrIncreaseRate * dt) {
        outputLiminance -= hdrIncreaseRate * dt;
      }
      else {
        outputLiminance = _hdrCurrentLuminance;
      }
    }

    outputLiminance
      = Scalar::Clamp(outputLiminance, hdrMinimumLuminance, 1e20f);

    effect->setFloat("averageLuminance", outputLiminance);

    lastTime = time;

    _currentDepthOfFieldSource = hdrFinalPostProcess;
  });

  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDR",
    [&]() -> std::vector<PostProcessPtr> { return {hdrPostProcess}; }, true));
}

void StandardRenderingPipeline::_createLensFlarePostProcess(Scene* scene,
                                                            float ratio)
{
  lensFlarePostProcess = PostProcess::New(
    "HDRLensFlare", "standard",
    {"strength", "ghostDispersal", "haloWidth", "resolution",
     "distortionStrength"},
    {"lensColorSampler"}, ratio / 2.f, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), true,
    "#define LENS_FLARE", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRLensFlare",
    [&]() -> std::vector<PostProcessPtr> { return {lensFlarePostProcess}; },
    false));

  _createBlurPostProcesses(scene, ratio / 4.f, 2);

  lensFlareComposePostProcess = PostProcess::New(
    "HDRLensFlareCompose", "standard", {"lensStarMatrix"},
    {"otherSampler", "lensDirtSampler", "lensStarSampler"}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define LENS_FLARE_COMPOSE", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  addEffect(PostProcessRenderEffect::New(scene->getEngine(),
                                         "HDRLensFlareCompose",
                                         [&]() -> std::vector<PostProcessPtr> {
                                           return {lensFlareComposePostProcess};
                                         },
                                         false));

  // Lens flare
  lensFlarePostProcess->setOnApply([&](Effect* effect, EventState&) {
    effect->setTextureFromPostProcess(
      "textureSampler",
      _bloomEnabled ? blurHPostProcesses[0].get() : originalPostProcess.get());
    effect->setTexture("lensColorSampler", lensColorTexture);
    effect->setFloat("strength", lensFlareStrength);
    effect->setFloat("ghostDispersal", lensFlareGhostDispersal);
    effect->setFloat("haloWidth", lensFlareHaloWidth);

    // Shift
    Vector2 resolution(0.f, 0.f);
    resolution.x = static_cast<float>(lensFlarePostProcess->width);
    resolution.y = static_cast<float>(lensFlarePostProcess->height);
    effect->setVector2("resolution", resolution);

    effect->setFloat("distortionStrength", lensFlareDistortionStrength);
  });

  // Compose
  lensFlareComposePostProcess->setOnApply([&](Effect* effect, EventState&) {
    if (!_scene->activeCamera) {
      return;
    }

    effect->setTextureFromPostProcess("otherSampler",
                                      textureAdderFinalPostProcess.get());
    effect->setTexture("lensDirtSampler", lensFlareDirtTexture);
    effect->setTexture("lensStarSampler", lensStarTexture);

    // Lens start rotation matrix
    auto camerax = *(_scene->activeCamera->getViewMatrix().getRow(0));
    auto cameraz = *(_scene->activeCamera->getViewMatrix().getRow(2));
    auto camRot  = Vector3::Dot(camerax.toVector3(), Vector3(1.f, 0.f, 0.f))
                  + Vector3::Dot(cameraz.toVector3(), Vector3(0.f, 0.f, 1.f));
    camRot *= 4.f;

    auto scaleBias1 = Matrix::FromValues(2.f, 0.f, -1.f, 0.f, //
                                         0.f, 2.f, -1.f, 0.f, //
                                         0.f, 0.f, 1.f, 0.f,  //
                                         0.f, 0.f, 0.f, 1.f   //
    );

    auto scaleBias2 = Matrix::FromValues(0.5f, 0.f, 0.5f, 0.f, //
                                         0.f, 0.5f, 0.5f, 0.f, //
                                         0.f, 0.f, 1.f, 0.f,   //
                                         0.f, 0.f, 0.f, 1.f    //
    );

    auto starRotation = Matrix::FromValues(
      std::cos(camRot) * 0.5f, -std::sin(camRot), 0.f, 0.f, //
      std::sin(camRot), std::cos(camRot) * 0.5f, 0.f, 0.f,  //
      0.f, 0.f, 1.f, 0.f,                                   //
      0.f, 0.f, 0.f, 1.f                                    //
    );

    auto lensStarMatrix
      = scaleBias2.multiply(starRotation).multiply(scaleBias1);

    effect->setMatrix("lensStarMatrix", lensStarMatrix);

    _currentDepthOfFieldSource = lensFlareFinalPostProcess;
  });
}

void StandardRenderingPipeline::_createDepthOfFieldPostProcess(Scene* scene,
                                                               float ratio)
{
  depthOfFieldPostProcess = PostProcess::New(
    "HDRDepthOfField", "standard", {"distance"},
    {"otherSampler", "depthSampler"}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define DEPTH_OF_FIELD", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  depthOfFieldPostProcess->setOnApply([&](Effect* effect, EventState&) {
    effect->setTextureFromPostProcess("otherSampler",
                                      textureAdderFinalPostProcess.get());
    effect->setTexture("depthSampler", _getDepthTexture());

    effect->setFloat("distance", depthOfFieldDistance);
  });

  // Add to pipeline
  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRDepthOfField",
    [&]() -> std::vector<PostProcessPtr> { return {depthOfFieldPostProcess}; },
    true));
}

// Create motion blur post-process
void StandardRenderingPipeline::_createMotionBlurPostProcess(Scene* scene,
                                                             float ratio)
{
  motionBlurPostProcess = PostProcess::New(
    "HDRMotionBlur", "standard",
    {"inverseViewProjection", "prevViewProjection", "screenSize", "motionScale",
     "motionStrength"},
    {"depthSampler"}, ratio, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
    scene->getEngine(), false,
    "#define MOTION_BLUR\n#define MAX_MOTION_SAMPLES "
      + std::to_string(motionBlurSamples()),
    EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  motionBlurPostProcess->setOnApply([&](Effect* effect, EventState&) {
    auto motionScale        = 0.f;
    auto prevViewProjection = Matrix::Identity();
    auto invViewProjection  = Matrix::Identity();
    auto viewProjection     = Matrix::Identity();
    auto screenSize         = Vector2::Zero();

    viewProjection
      = scene->getProjectionMatrix().multiply(scene->getViewMatrix());

    viewProjection.invertToRef(invViewProjection);
    effect->setMatrix("inverseViewProjection", invViewProjection);

    effect->setMatrix("prevViewProjection", prevViewProjection);
    prevViewProjection = viewProjection;

    screenSize.x = static_cast<float>(motionBlurPostProcess->width);
    screenSize.y = static_cast<float>(motionBlurPostProcess->height);
    effect->setVector2("screenSize", screenSize);

    motionScale = scene->getEngine()->getFps() / 60.f;
    effect->setFloat("motionScale", motionScale);
    effect->setFloat("motionStrength", motionStrength);

    effect->setTexture("depthSampler", _getDepthTexture());
  });

  addEffect(PostProcessRenderEffect::New(
    scene->getEngine(), "HDRMotionBlur",
    [&]() -> std::vector<PostProcessPtr> { return {motionBlurPostProcess}; },
    true));
}

TexturePtr StandardRenderingPipeline::_getDepthTexture()
{
  if (_scene->getEngine()->getCaps().drawBuffersExtension) {
    auto renderer = _scene->enableGeometryBufferRenderer();
    return renderer->getGBuffer()->textures()[0];
  }

  return _scene->enableDepthRenderer()->getDepthMap();
}

void StandardRenderingPipeline::_disposePostProcesses()
{
  for (auto& item : _cameras) {
    auto camera = item.second.get();

    if (originalPostProcess) {
      originalPostProcess->dispose(camera);
    }

    if (downSampleX4PostProcess) {
      downSampleX4PostProcess->dispose(camera);
    }
    if (brightPassPostProcess) {
      brightPassPostProcess->dispose(camera);
    }
    if (textureAdderPostProcess) {
      textureAdderPostProcess->dispose(camera);
    }
    if (textureAdderFinalPostProcess) {
      textureAdderFinalPostProcess->dispose(camera);
    }

    if (volumetricLightPostProcess) {
      volumetricLightPostProcess->dispose(camera);
    }
    if (volumetricLightSmoothXPostProcess) {
      volumetricLightSmoothXPostProcess->dispose(camera);
    }
    if (volumetricLightSmoothYPostProcess) {
      volumetricLightSmoothYPostProcess->dispose(camera);
    }
    if (volumetricLightMergePostProces) {
      volumetricLightMergePostProces->dispose(camera);
    }
    if (volumetricLightFinalPostProcess) {
      volumetricLightFinalPostProcess->dispose(camera);
    }

    if (lensFlarePostProcess) {
      lensFlarePostProcess->dispose(camera);
    }
    if (lensFlareComposePostProcess) {
      lensFlareComposePostProcess->dispose(camera);
    }

    for (auto& luminanceDownSamplePostProcess :
         luminanceDownSamplePostProcesses) {
      luminanceDownSamplePostProcess->dispose(camera);
    }

    if (luminancePostProcess) {
      luminancePostProcess->dispose(camera);
    }
    if (hdrPostProcess) {
      hdrPostProcess->dispose(camera);
    }
    if (hdrFinalPostProcess) {
      hdrFinalPostProcess->dispose(camera);
    }

    if (depthOfFieldPostProcess) {
      depthOfFieldPostProcess->dispose(camera);
    }

    if (motionBlurPostProcess) {
      motionBlurPostProcess->dispose(camera);
    }

    for (auto& blurHPostProcess : blurHPostProcesses) {
      blurHPostProcess->dispose(camera);
    }

    for (auto& blurVPostProcess : blurVPostProcesses) {
      blurVPostProcess->dispose(camera);
    }
  }

  originalPostProcess               = nullptr;
  downSampleX4PostProcess           = nullptr;
  brightPassPostProcess             = nullptr;
  textureAdderPostProcess           = nullptr;
  textureAdderFinalPostProcess      = nullptr;
  volumetricLightPostProcess        = nullptr;
  volumetricLightSmoothXPostProcess = nullptr;
  volumetricLightSmoothYPostProcess = nullptr;
  volumetricLightMergePostProces    = nullptr;
  volumetricLightFinalPostProcess   = nullptr;
  lensFlarePostProcess              = nullptr;
  lensFlareComposePostProcess       = nullptr;
  luminancePostProcess              = nullptr;
  hdrPostProcess                    = nullptr;
  hdrFinalPostProcess               = nullptr;
  depthOfFieldPostProcess           = nullptr;
  motionBlurPostProcess             = nullptr;

  luminanceDownSamplePostProcesses.clear();
  blurHPostProcesses.clear();
  blurVPostProcesses.clear();
}

void StandardRenderingPipeline::dispose(bool doNotRecurse,
                                        bool /*disposeMaterialAndTextures*/)
{
  _disposePostProcesses();

  auto cameras = stl_util::extract_values(_cameras);
  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
    _name, cameras);

  PostProcessRenderPipeline::dispose(doNotRecurse);
}

Json::object StandardRenderingPipeline::serialize() const
{
  return Json::object();
}

std::unique_ptr<StandardRenderingPipeline>
StandardRenderingPipeline::Parse(const Json::value& /*source*/,
                                 Scene* /*scene*/, const std::string& /*url*/)
{
  return nullptr;
}

} // end of namespace BABYLON
