#include <babylon/postprocess/renderpipeline/pipelines/standard_rendering_pipeline.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

unsigned int StandardRenderingPipeline::LuminanceSteps = 6;

StandardRenderingPipeline::StandardRenderingPipeline(
  const std::string& iName, Scene* scene, float ratio,
  PostProcess* iOriginalPostProcess, const std::vector<Camera*>& cameras)
    : PostProcessRenderPipeline{scene->getEngine(), iName}
    , originalPostProcess{nullptr}
    , downSampleX4PostProcess{nullptr}
    , brightPassPostProcess{nullptr}
    , textureAdderPostProcess{nullptr}
    , textureAdderFinalPostProcess{nullptr}
    , lensFlareFinalPostProcess{nullptr}
    , luminancePostProcess{nullptr}
    , hdrPostProcess{nullptr}
    , hdrFinalPostProcess{nullptr}
    , lensFlarePostProcess{nullptr}
    , lensFlareComposePostProcess{nullptr}
    , motionBlurPostProcess{nullptr}
    , depthOfFieldPostProcess{nullptr}
    , brightThreshold{1.f}
    , blurWidth{2.f}
    , gaussianCoefficient{0.25f}
    , gaussianMean{1.f}
    , gaussianStandardDeviation{1.f}
    , exposure{1.f}
    , hdrMinimumLuminance{1.f}
    , hdrDecreaseRate{0.5f}
    , hdrIncreaseRate{0.5f}
    , lensTexture{nullptr}
    , lensColorTexture{nullptr}
    , lensFlareStrength{20.f}
    , lensFlareGhostDispersal{1.4f}
    , lensFlareHaloWidth{0.7f}
    , lensFlareDistortionStrength{16.f}
    , lensStarTexture{nullptr}
    , lensFlareDirtTexture{nullptr}
    , depthOfFieldDistance{10.f}
    , depthOfFieldBlurWidth{2.f}
    , motionStrength{1.f}
    , _scene{scene}
    , _depthRenderer{nullptr}
    , _currentDepthOfFieldSource{nullptr}
    , _currentHDRSource{nullptr}
    , _hdrCurrentLuminance{1.f}
    , _motionBlurSamples{64}
    , _bloomEnabled{true}
    , _depthOfFieldEnabled{true}
    , _lensFlareEnabled{true}
    , _hdrEnabled{true}
    , _motionBlurEnabled{true}
{
  // Misc
  auto floatTextureType = scene->getEngine()->getCaps().textureFloatRender ?
                            EngineConstants::TEXTURETYPE_FLOAT :
                            EngineConstants::TEXTURETYPE_HALF_FLOAT;

  // Create pass post-process
  if (!originalPostProcess) {
    originalPostProcess = new PostProcess(
      "HDRPass", "standard", {}, {}, ratio, nullptr,
      TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), true,
      "#define PASS_POST_PROCESS", floatTextureType);
  }
  else {
    originalPostProcess = iOriginalPostProcess;
  }

  addEffect(
    new PostProcessRenderEffect(scene->getEngine(), "HDRPassPostProcess",
                                [&]() { return originalPostProcess; }, true));

  // Create down sample X4 post-process
  _createDownSampleX4PostProcess(scene, ratio / 2.f);

  // Create bright pass post-process
  _createBrightPassPostProcess(scene, ratio / 2.f);

  // Create gaussian blur post-processes (down sampling blurs)
  _createGaussianBlurPostProcesses(scene, ratio / 2.f, 0);
  _createGaussianBlurPostProcesses(scene, ratio / 4.f, 1);
  _createGaussianBlurPostProcesses(scene, ratio / 8.f, 2);
  _createGaussianBlurPostProcesses(scene, ratio / 16.f, 3);

  // Create texture adder post-process
  _createTextureAdderPostProcess(scene, ratio);

  // Create depth-of-field source post-process
  textureAdderFinalPostProcess = new PostProcess(
    "HDRDepthOfFieldSource", "standard", {}, {}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), true,
    "#define PASS_POST_PROCESS", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRDepthOfFieldSource",
    [&]() { return textureAdderFinalPostProcess; }, true));

  // Create lens flare post-process
  _createLensFlarePostProcess(scene, ratio);

  // Create depth-of-field source post-process post lens-flare and disable it
  // now
  lensFlareFinalPostProcess = new PostProcess(
    "HDRPostLensFlareDepthOfFieldSource", "standard", {}, {}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define PASS_POST_PROCESS", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRPostLensFlareDepthOfFieldSource",
    [&]() { return lensFlareFinalPostProcess; }, true));

  // Create luminance
  _createLuminancePostProcesses(scene, floatTextureType);

  // Create HDR
  _createHdrPostProcess(scene, ratio);

  // Create depth-of-field source post-process post hdr and disable it now
  hdrFinalPostProcess = new PostProcess(
    "HDRPostHDReDepthOfFieldSource", "standard", {}, {}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define PASS_POST_PROCESS", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRPostHDReDepthOfFieldSource",
    [&]() { return hdrFinalPostProcess; }, true));

  // Create gaussian blur used by depth-of-field
  _createGaussianBlurPostProcesses(scene, ratio / 2.f, 5);

  // Create depth-of-field post-process
  _createDepthOfFieldPostProcess(scene, ratio);

  // Create motion blur post-process
  _createMotionBlurPostProcess(scene, ratio);

  // Finish
  scene->postProcessRenderPipelineManager()->addPipeline(this);

  if (!cameras.empty()) {
    scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(
      iName, cameras);
  }

  // Deactivate
  setLensFlareEnabled(false);
  setDepthOfFieldEnabled(false);
  setHDREnabled(false);
  setMotionBlurEnabled(false);
}

StandardRenderingPipeline::~StandardRenderingPipeline()
{
}

void StandardRenderingPipeline::setBloomEnabled(bool enabled)
{
  const auto cameras = getCameras();

  if (enabled && !_bloomEnabled) {
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRDownSampleX4", cameras);
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRBrightPass", cameras);

    for (std::size_t i = 0; i < gaussianBlurHPostProcesses.size() - 1; ++i) {
      const std::string istr = std::to_string(i);
      _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
        _name, "HDRGaussianBlurH" + istr, cameras);
      _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
        _name, "HDRGaussianBlurV" + istr, cameras);
    }

    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRTextureAdder", cameras);
  }
  else if (!enabled && _bloomEnabled) {
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRDownSampleX4", cameras);
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRBrightPass", cameras);

    for (std::size_t i = 0; i < gaussianBlurHPostProcesses.size() - 1; ++i) {
      const std::string istr = std::to_string(i);
      _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
        _name, "HDRGaussianBlurH" + istr, cameras);
      _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
        _name, "HDRGaussianBlurV" + istr, cameras);
    }

    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRTextureAdder", cameras);
  }

  _bloomEnabled = enabled;
}

bool StandardRenderingPipeline::bloomEnabled() const
{
  return _bloomEnabled;
}

void StandardRenderingPipeline::setDepthOfFieldEnabled(bool enabled)
{
  auto blurIndexStr = std::to_string(gaussianBlurHPostProcesses.size() - 1);

  if (enabled && !_depthOfFieldEnabled) {
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRGaussianBlurH" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRGaussianBlurV" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRDepthOfField", _scene->getCameras());
    _depthRenderer = _scene->enableDepthRenderer();
  }
  else if (!enabled && _depthOfFieldEnabled) {
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRGaussianBlurH" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRGaussianBlurV" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRDepthOfField", _scene->getCameras());
  }

  _depthOfFieldEnabled = enabled;
}

bool StandardRenderingPipeline::depthOfFieldEnabled() const
{
  return _depthOfFieldEnabled;
}

void StandardRenderingPipeline::setLensFlareEnabled(bool enabled)
{
  auto blurIndexStr = std::to_string(gaussianBlurHPostProcesses.size() - 1);

  if (enabled && !_lensFlareEnabled) {
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRLensFlare", _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRLensFlareShift", _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRGaussianBlurH" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRGaussianBlurV" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRLensFlareCompose", _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRPostLensFlareDepthOfFieldSource", _scene->getCameras());
  }
  else if (!enabled && _lensFlareEnabled) {
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRLensFlare", _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRLensFlareShift", _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRGaussianBlurH" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRGaussianBlurV" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRLensFlareCompose", _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRPostLensFlareDepthOfFieldSource", _scene->getCameras());
  }

  _lensFlareEnabled = enabled;
}

bool StandardRenderingPipeline::lensFlareEnabled() const
{
  return _lensFlareEnabled;
}

void StandardRenderingPipeline::setHDREnabled(bool enabled)
{
  if (enabled && !_hdrEnabled) {
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRLuminance", _scene->getCameras());
    for (std::size_t i = 0; i < luminanceDownSamplePostProcesses.size(); ++i) {
      _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
        _name, "HDRLuminanceDownSample" + std::to_string(i),
        _scene->getCameras());
    }
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDR", _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRPostHDReDepthOfFieldSource", _scene->getCameras());
  }
  else if (!enabled && _hdrEnabled) {
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRLuminance", _scene->getCameras());
    for (std::size_t i = 0; i < luminanceDownSamplePostProcesses.size(); ++i) {
      _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
        _name, "HDRLuminanceDownSample" + std::to_string(i),
        _scene->getCameras());
    }
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDR", _scene->getCameras());
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRPostHDReDepthOfFieldSource", _scene->getCameras());
  }

  _hdrEnabled = enabled;
}

bool StandardRenderingPipeline::HDREnabled() const
{
  return _hdrEnabled;
}

void StandardRenderingPipeline::setMotionBlurEnabled(bool enabled)
{
  if (enabled && !_motionBlurEnabled) {
    _scene->postProcessRenderPipelineManager()->enableEffectInPipeline(
      _name, "HDRMotionBlur", _scene->getCameras());
    _depthRenderer = _scene->enableDepthRenderer();
  }
  else if (!enabled && _motionBlurEnabled) {
    _scene->postProcessRenderPipelineManager()->disableEffectInPipeline(
      _name, "HDRMotionBlur", _scene->getCameras());
  }

  _motionBlurEnabled = enabled;
}

bool StandardRenderingPipeline::motionBlurEnabled() const
{
  return _motionBlurEnabled;
}

unsigned int StandardRenderingPipeline::motionBlurSamples() const
{
  return _motionBlurSamples;
}

void StandardRenderingPipeline::setMotionBlurSamples(unsigned int samples)
{
  motionBlurPostProcess->updateEffect(
    "#define MOTION_BLUR\n#define MAX_MOTION_SAMPLES "
    + std::to_string(samples));
  _motionBlurSamples = samples;
}

void StandardRenderingPipeline::_createDownSampleX4PostProcess(Scene* scene,
                                                               float ratio)
{
  downSampleX4PostProcess = new PostProcess(
    "HDRDownSampleX4", "standard", {"dsOffsets"}, {}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define DOWN_SAMPLE_X4", EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  downSampleX4PostProcess->setOnApply([&](Effect* effect) {
    Float32Array downSampleX4Offsets(32);
    unsigned int id = 0;
    for (int i = -2; i < 2; ++i) {
      for (int j = -2; j < 2; ++j) {
        downSampleX4Offsets[id]
          = (i + 0.5f) * (1.f / downSampleX4PostProcess->width);
        downSampleX4Offsets[id + 1]
          = (j + 0.5f) * (1.f / downSampleX4PostProcess->height);
        id += 2;
      }
    }
    effect->setArray2("dsOffsets", downSampleX4Offsets);
  });

  // Add to pipeline
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRDownSampleX4",
    [&]() { return downSampleX4PostProcess; }, true));
}

void StandardRenderingPipeline::_createBrightPassPostProcess(Scene* scene,
                                                             float ratio)
{
  brightPassPostProcess = new PostProcess(
    "HDRBrightPass", "standard", {"dsOffsets", "brightThreshold"}, {}, ratio,
    nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define BRIGHT_PASS", EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  brightPassPostProcess->setOnApply([&](Effect* effect) {
    float sU = (1.f / brightPassPostProcess->width);
    float sV = (1.f / brightPassPostProcess->height);

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
  addEffect(new PostProcessRenderEffect(scene->getEngine(), "HDRBrightPass",
                                        [&]() { return brightPassPostProcess; },
                                        true));
}

void StandardRenderingPipeline::_createGaussianBlurPostProcesses(
  Scene* scene, float ratio, unsigned int indice)
{
  std::vector<std::string> uniforms{"blurOffsets", "blurWeights", "blurWidth"};

  auto callback = [&](bool height) {
    return [&](Effect& effect) {
      Float32Array blurOffsets(9);
      Float32Array blurWeights(9);
      // Weights
      float x = 0.f;
      for (unsigned int i = 0; i < 9; i++) {
        x = (i - 4.f) / 4.f;
        blurWeights[i]
          = gaussianCoefficient
            * (1.f / std::sqrt(2.f * Math::PI * gaussianStandardDeviation))
            * std::exp((-((x - gaussianMean) * (x - gaussianMean)))
                       / (2.f * gaussianStandardDeviation
                          * gaussianStandardDeviation));
      }

      ISize lastOutputDimensions(scene->getEngine()->getRenderWidth(),
                                 scene->getEngine()->getRenderHeight());

      for (unsigned int i = 0; i < 9; i++) {
        float value = (i - 4.f) * (1.f / (height ? lastOutputDimensions.height :
                                                   lastOutputDimensions.width));
        blurOffsets[i] = value;
      }

      effect.setArray("blurOffsets", blurOffsets);
      effect.setArray("blurWeights", blurWeights);
      effect.setFloat("blurWidth", blurWidth);
    };
  };

  // Create horizontal gaussian blur post-processes
  auto ratioStr                 = std::to_string(ratio);
  auto gaussianBlurHPostProcess = new PostProcess(
    "HDRGaussianBlurH" + ratioStr, "standard", uniforms, {}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define GAUSSIAN_BLUR_H", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  gaussianBlurHPostProcess->setOnApply(
    [&](Effect* /*effect*/) { callback(false); });

  // Create vertical gaussian blur post-process
  auto gaussianBlurVPostProcess = new PostProcess(
    "HDRGaussianBlurV" + ratioStr, "standard", uniforms, {}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define GAUSSIAN_BLUR_V", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  gaussianBlurVPostProcess->setOnApply(
    [&](Effect* /*effect*/) { callback(true); });

  // Add to pipeline
  auto indiceStr = std::to_string(indice);
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRGaussianBlurH" + indiceStr,
    [&]() { return gaussianBlurHPostProcess; }, true));
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRGaussianBlurV" + indiceStr,
    [&]() { return gaussianBlurVPostProcess; }, true));

  // Finish
  gaussianBlurHPostProcesses.emplace_back(gaussianBlurHPostProcess);
  gaussianBlurVPostProcesses.emplace_back(gaussianBlurVPostProcess);
}

void StandardRenderingPipeline::_createTextureAdderPostProcess(Scene* scene,
                                                               float ratio)
{
  textureAdderPostProcess = new PostProcess(
    "HDRTextureAdder", "standard", {"exposure"},
    {"otherSampler", "lensSampler"}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define TEXTURE_ADDER", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  textureAdderPostProcess->setOnApply([&](Effect* effect) {
    effect->setTextureFromPostProcess("otherSampler", originalPostProcess);
    effect->setTexture("lensSampler", lensTexture);
    effect->setFloat("exposure", exposure);

    _currentDepthOfFieldSource = textureAdderFinalPostProcess;
    _currentHDRSource          = textureAdderFinalPostProcess;
  });

  // Add to pipeline
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRTextureAdder",
    [&]() { return textureAdderPostProcess; }, true));
}

void StandardRenderingPipeline::_createLuminancePostProcesses(
  Scene* scene, unsigned int textureType)
{
  // Create luminance
  float size = static_cast<float>(
    std::pow(3, StandardRenderingPipeline::LuminanceSteps));
  luminancePostProcess = new PostProcess(
    "HDRLuminance", "standard", {"lumOffsets"}, {}, size, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define LUMINANCE", textureType);

  luminancePostProcess->setOnApply([&](Effect* effect) {
    float sU = (1.f / luminancePostProcess->width);
    float sV = (1.f / luminancePostProcess->height);

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
  addEffect(new PostProcessRenderEffect(scene->getEngine(), "HDRLuminance",
                                        [this] { return luminancePostProcess; },
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
      = new PostProcess("HDRLuminanceDownSample" + iStr, "standard",
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

    pp->setOnApply([&](Effect* effect) {
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
      pp->setOnAfterRender([&](Effect* /*effect*/) {
        auto pixel = scene->getEngine()->readPixels(0, 0, 1, 1);
        Vector4 bit_shift(1.f / (255.f * 255.f * 255.f), 1.f / (255.f * 255.f),
                          1.f / 255.f, 1.f);
        _hdrCurrentLuminance
          = (pixel[0] * bit_shift.x + pixel[1] * bit_shift.y
             + pixel[2] * bit_shift.z + pixel[3] * bit_shift.w)
            / 100.f;
      });
    }

    addEffect(new PostProcessRenderEffect(scene->getEngine(),
                                          "HDRLuminanceDownSample" + indexStr,
                                          [&]() { return pp; }, true));
    ++index;
  };
}

// Create HDR post-process
void StandardRenderingPipeline::_createHdrPostProcess(Scene* scene, float ratio)
{
  hdrPostProcess = new PostProcess(
    "HDR", "standard", {"averageLuminance"}, {"textureAdderSampler"}, ratio,
    nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define HDR", EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  float outputLiminance = 1.f;
  float time            = 0.f;
  float lastTime        = 0.f;

  hdrPostProcess->setOnApply([&](Effect* effect) {

    effect->setTextureFromPostProcess("textureAdderSampler",
                                      _bloomEnabled ? _currentHDRSource :
                                                      originalPostProcess);

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

  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDR", [&]() { return hdrPostProcess; }, true));
}

void StandardRenderingPipeline::_createLensFlarePostProcess(Scene* scene,
                                                            float ratio)
{
  lensFlarePostProcess = new PostProcess(
    "HDRLensFlare", "standard",
    {"strength", "ghostDispersal", "haloWidth", "resolution",
     "distortionStrength"},
    {"lensColorSampler"}, ratio / 2.f, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), true,
    "#define LENS_FLARE", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  addEffect(new PostProcessRenderEffect(scene->getEngine(), "HDRLensFlare",
                                        [&]() { return lensFlarePostProcess; },
                                        false));

  _createGaussianBlurPostProcesses(scene, ratio / 4.f, 4);

  lensFlareComposePostProcess = new PostProcess(
    "HDRLensFlareCompose", "standard", {"lensStarMatrix"},
    {"otherSampler", "lensDirtSampler", "lensStarSampler"}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define LENS_FLARE_COMPOSE", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRLensFlareCompose",
    [&]() { return lensFlareComposePostProcess; }, false));

  // Lens flare
  lensFlarePostProcess->setOnApply([&](Effect* effect) {
    effect->setTextureFromPostProcess(
      "textureSampler",
      _bloomEnabled ? gaussianBlurHPostProcesses[0] : originalPostProcess);
    effect->setTexture("lensColorSampler", lensColorTexture);
    effect->setFloat("strength", lensFlareStrength);
    effect->setFloat("ghostDispersal", lensFlareGhostDispersal);
    effect->setFloat("haloWidth", lensFlareHaloWidth);

    // Shift
    Vector2 resolution(0.f, 0.f);
    resolution.x = lensFlarePostProcess->width;
    resolution.y = lensFlarePostProcess->height;
    effect->setVector2("resolution", resolution);

    effect->setFloat("distortionStrength", lensFlareDistortionStrength);
  });

  // Compose
  lensFlareComposePostProcess->setOnApply([&](Effect* effect) {
    effect->setTextureFromPostProcess("otherSampler",
                                      textureAdderFinalPostProcess);
    effect->setTexture("lensDirtSampler", lensFlareDirtTexture);
    effect->setTexture("lensStarSampler", lensStarTexture);

    // Lens start rotation matrix
    auto camerax = _scene->activeCamera->getViewMatrix().getRow(0);
    auto cameraz = _scene->activeCamera->getViewMatrix().getRow(2);
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
    _currentHDRSource          = lensFlareFinalPostProcess;
  });
}

void StandardRenderingPipeline::_createDepthOfFieldPostProcess(Scene* scene,
                                                               float ratio)
{
  depthOfFieldPostProcess = new PostProcess(
    "HDRDepthOfField", "standard", {"distance"},
    {"otherSampler", "depthSampler"}, ratio, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define DEPTH_OF_FIELD", EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  depthOfFieldPostProcess->setOnApply([&](Effect* effect) {
    effect->setTextureFromPostProcess("otherSampler",
                                      textureAdderFinalPostProcess);
    effect->setTexture("depthSampler", _depthRenderer->getDepthMap());
    effect->setFloat("distance", depthOfFieldDistance);
  });

  // Add to pipeline
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRDepthOfField",
    [&]() { return depthOfFieldPostProcess; }, true));
}

// Create motion blur post-process
void StandardRenderingPipeline::_createMotionBlurPostProcess(Scene* scene,
                                                             float ratio)
{
  motionBlurPostProcess = new PostProcess(
    "HDRMotionBlur", "standard",
    {"inverseViewProjection", "prevViewProjection", "screenSize", "motionScale",
     "motionStrength"},
    {"depthSampler"}, ratio, nullptr, TextureConstants::BILINEAR_SAMPLINGMODE,
    scene->getEngine(), false,
    "#define MOTION_BLUR\n#define MAX_MOTION_SAMPLES "
      + std::to_string(motionBlurSamples()),
    EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  auto motionScale        = 0.f;
  auto prevViewProjection = Matrix::Identity();
  auto invViewProjection  = Matrix::Identity();
  auto viewProjection     = Matrix::Identity();
  auto screenSize         = Vector2::Zero();

  motionBlurPostProcess->setOnApply([&](Effect* effect) {
    viewProjection
      = scene->getProjectionMatrix().multiply(scene->getViewMatrix());

    viewProjection.invertToRef(invViewProjection);
    effect->setMatrix("inverseViewProjection", invViewProjection);

    effect->setMatrix("prevViewProjection", prevViewProjection);
    prevViewProjection = viewProjection;

    screenSize.x = motionBlurPostProcess->width;
    screenSize.y = motionBlurPostProcess->height;
    effect->setVector2("screenSize", screenSize);

    motionScale = scene->getEngine()->getFps() / 60.f;
    effect->setFloat("motionScale", motionScale);
    effect->setFloat("motionStrength", motionStrength);

    effect->setTexture("depthSampler", _depthRenderer->getDepthMap());
  });

  addEffect(new PostProcessRenderEffect(scene->getEngine(), "HDRMotionBlur",
                                        [&]() { return motionBlurPostProcess; },
                                        true));
}

void StandardRenderingPipeline::dispose(bool doNotRecurse)
{
  for (auto& _camera : _scene->cameras) {
    auto camera = _camera.get();
    originalPostProcess->dispose(camera);
    downSampleX4PostProcess->dispose(camera);
    brightPassPostProcess->dispose(camera);
    textureAdderPostProcess->dispose(camera);

    for (auto& gaussianBlurHPostProcess : gaussianBlurHPostProcesses) {
      gaussianBlurHPostProcess->dispose(camera);
    }

    for (auto& gaussianBlurVPostProcess : gaussianBlurVPostProcesses) {
      gaussianBlurVPostProcess->dispose(camera);
    }

    textureAdderFinalPostProcess->dispose(camera);
    lensFlarePostProcess->dispose(camera);
    lensFlareComposePostProcess->dispose(camera);
    depthOfFieldPostProcess->dispose(camera);
    motionBlurPostProcess->dispose(camera);
  }

  _scene->postProcessRenderPipelineManager()->detachCamerasFromRenderPipeline(
    _name, _scene->getCameras());

  PostProcessRenderPipeline::dispose(doNotRecurse);
}

} // end of namespace BABYLON
