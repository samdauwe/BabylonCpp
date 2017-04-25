#include <babylon/postprocess/hdr_rendering_pipeline.h>

#include <babylon/core/time.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/math_tools.h>
#include <babylon/math/vector4.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>

namespace BABYLON {

HDRRenderingPipeline::HDRRenderingPipeline(const std::string& iName,
                                           Scene* scene, float ratio,
                                           PostProcess* originalPostProcess,
                                           const std::vector<Camera*>& cameras)
    : PostProcessRenderPipeline(scene->getEngine(), iName)
    , gaussCoeff{0.3f}
    , gaussMean{1.f}
    , gaussStandDev{0.8f}
    , gaussMultiplier{4.f}
    , exposure{1.f}
    , minimumLuminance{1.f}
    , maximumLuminance{1e20f}
    , luminanceIncreaserate{0.5f}
    , luminanceDecreaseRate{0.5f}
    , brightThreshold{0.8f}
    , _scene{scene}
    , _needUpdate{true}
{
  // Bright pass
  _createBrightPassPostProcess(scene, ratio);

  // Down sample X4
  _createDownSampleX4PostProcess(scene, ratio);

  // Create gaussian blur post-processes
  _createGaussianBlurPostProcess(scene, ratio);

  // Texture adder
  _createTextureAdderPostProcess(scene, ratio);

  // Luminance generator
  _createLuminanceGeneratorPostProcess(scene);

  // HDR
  _createHDRPostProcess(scene, ratio);

  // Pass postprocess
  if (originalPostProcess == nullptr) {
    _originalPostProcess = new PassPostProcess("hdr", ratio, nullptr,
                                               Texture::BILINEAR_SAMPLINGMODE,
                                               scene->getEngine(), false);
  }
  else {
    _originalPostProcess = originalPostProcess;
  }

  // Configure pipeline
  addEffect(
    new PostProcessRenderEffect(scene->getEngine(), "HDRPassPostProcess",
                                [&]() { return _originalPostProcess; }, true));
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRBrightPass",
    [&]() { return _brightPassPostProcess; }, true));
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRDownSampleX4",
    [&]() { return _downSampleX4PostProcess; }, true));
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRGaussianBlurH",
    [&]() { return _guassianBlurHPostProcess; }, true));
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRGaussianBlurV",
    [&]() { return _guassianBlurVPostProcess; }, true));
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRTextureAdder",
    [&]() { return _textureAdderPostProcess; }, true));

  auto addDownSamplerPostProcess = [&](unsigned int id) {
    addEffect(new PostProcessRenderEffect(
      scene->getEngine(), "HDRDownSampler" + std::to_string(id),
      [&]() { return _downSamplePostProcesses[id]; }, true));
  };
  for (unsigned int i = HDRRenderingPipeline::LUM_STEPS; i > 0; --i) {
    addDownSamplerPostProcess(i - 1);
  }

  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDR", [&]() { return _hdrPostProcess; }, true));

  // Finish
  scene->postProcessRenderPipelineManager()->addPipeline(this);

  if (!cameras.empty()) {
    scene->postProcessRenderPipelineManager()->attachCamerasToRenderPipeline(
      _name, cameras);
  }

  update();
}

HDRRenderingPipeline::~HDRRenderingPipeline()
{
}

void HDRRenderingPipeline::update()
{
  _needUpdate = true;
}

float HDRRenderingPipeline::getCurrentLuminance() const
{
  return _hdrCurrentLuminance;
}

float HDRRenderingPipeline::getOutputLuminance() const
{
  return _hdrOutputLuminance;
}

void HDRRenderingPipeline::dispose(bool /*doNotRecurse*/)
{
  _originalPostProcess      = nullptr;
  _brightPassPostProcess    = nullptr;
  _downSampleX4PostProcess  = nullptr;
  _guassianBlurHPostProcess = nullptr;
  _guassianBlurVPostProcess = nullptr;
  _textureAdderPostProcess  = nullptr;
  for (unsigned i = HDRRenderingPipeline::LUM_STEPS; i > 0; --i) {
    _downSamplePostProcesses[i - 1] = nullptr;
  }
  _hdrPostProcess = nullptr;

  // TODO FIXME
  //_scene->postProcessRenderPipelineManager->detachCamerasFromRenderPipeline(
  //  _name, _scene->cameras);
}

void HDRRenderingPipeline::_createHDRPostProcess(Scene* scene, float ratio)
{
  float hdrLastLuminance = 0.f;
  _hdrOutputLuminance    = -1.f;
  _hdrCurrentLuminance   = 1.f;
  _hdrPostProcess = new PostProcess("hdr", "hdr", {"exposure", "avgLuminance"},
                                    {"otherSampler"}, ratio, nullptr,
                                    Texture::BILINEAR_SAMPLINGMODE,
                                    scene->getEngine(), false, "#define HDR");

  _hdrPostProcess->setOnApply([&](Effect* effect) {
    if (_hdrOutputLuminance < 0.f) {
      _hdrOutputLuminance = _hdrCurrentLuminance;
    }
    else {
      float dt = (hdrLastLuminance
                  - (hdrLastLuminance + Time::fpMillisecondsDuration<float>(
                                          scene->getEngine()->getDeltaTime())))
                 / 1000.f;

      if (_hdrCurrentLuminance
          < _hdrOutputLuminance + luminanceDecreaseRate * dt) {
        _hdrOutputLuminance += luminanceDecreaseRate * dt;
      }
      else if (_hdrCurrentLuminance
               > _hdrOutputLuminance - luminanceIncreaserate * dt) {
        _hdrOutputLuminance -= luminanceIncreaserate * dt;
      }
      else {
        _hdrOutputLuminance = _hdrCurrentLuminance;
      }
    }

    _hdrOutputLuminance = MathTools::Clamp(_hdrOutputLuminance,
                                           minimumLuminance, maximumLuminance);
    hdrLastLuminance += Time::fpMillisecondsDuration<float>(
      scene->getEngine()->getDeltaTime());

    effect->setTextureFromPostProcess("textureSampler",
                                      _textureAdderPostProcess);
    effect->setTextureFromPostProcess("otherSampler", _originalPostProcess);
    effect->setFloat("exposure", exposure);
    effect->setFloat("avgLuminance", _hdrOutputLuminance);

    _needUpdate = false;
  });
}

void HDRRenderingPipeline::_createTextureAdderPostProcess(Scene* scene,
                                                          float ratio)
{
  _textureAdderPostProcess
    = new PostProcess("hdr", "hdr", {}, {"otherSampler"}, ratio, nullptr,
                      Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
                      "#define TEXTURE_ADDER");

  _textureAdderPostProcess->setOnApply([&](Effect* effect) {
    effect->setTextureFromPostProcess("otherSampler", _originalPostProcess);
  });
}

void HDRRenderingPipeline::_createDownSampleX4PostProcess(Scene* scene,
                                                          float ratio)
{
  Float32Array downSampleX4Offsets(32);
  _downSampleX4PostProcess
    = new PostProcess("hdr", "hdr", {"dsOffsets"}, {}, ratio / 4.f, nullptr,
                      Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
                      "#define DOWN_SAMPLE_X4");

  _downSampleX4PostProcess->setOnApply([&](Effect* effect) {
    if (_needUpdate) {
      unsigned int id = 0;
      for (int i = -2; i < 2; ++i) {
        for (int j = -2; j < 2; ++j) {
          downSampleX4Offsets[id]
            = (static_cast<float>(i) + 0.5f)
              * (1.f / static_cast<float>(_downSampleX4PostProcess->width));
          downSampleX4Offsets[id + 1]
            = (static_cast<float>(j) + 0.5f)
              * (1.f / static_cast<float>(_downSampleX4PostProcess->height));
          id += 2;
        }
      }
    }

    effect->setArray2("dsOffsets", downSampleX4Offsets);
  });
}

void HDRRenderingPipeline::_createBrightPassPostProcess(Scene* scene,
                                                        float ratio)
{
  Float32Array brightOffsets(8);

  auto brightPassCallback = [&](Effect* effect) {
    if (_needUpdate) {
      float sU = (1.f / static_cast<float>(_brightPassPostProcess->width));
      float sV = (1.f / static_cast<float>(_brightPassPostProcess->height));

      brightOffsets[0] = -0.5f * sU;
      brightOffsets[1] = 0.5f * sV;
      brightOffsets[2] = 0.5f * sU;
      brightOffsets[3] = 0.5f * sV;
      brightOffsets[4] = -0.5f * sU;
      brightOffsets[5] = -0.5f * sV;
      brightOffsets[6] = 0.5f * sU;
      brightOffsets[7] = -0.5f * sV;
    }

    effect->setArray2("dsOffsets", brightOffsets);
    effect->setFloat("brightThreshold", brightThreshold);
  };

  _brightPassPostProcess
    = new PostProcess("hdr", "hdr", {"dsOffsets", "brightThreshold"}, {}, ratio,
                      nullptr, Texture::BILINEAR_SAMPLINGMODE,
                      scene->getEngine(), false, "#define BRIGHT_PASS");
  _brightPassPostProcess->setOnApply(brightPassCallback);
}

void HDRRenderingPipeline::_createLuminanceGeneratorPostProcess(Scene* scene)
{
  unsigned int lumSteps = HDRRenderingPipeline::LUM_STEPS;
  Float32Array luminanceOffsets(8);
  Float32Array downSampleOffsets(18);
  float halfDestPixelSize;
  _downSamplePostProcesses.resize(lumSteps);

  // Utils for luminance
  auto luminanceUpdateSourceOffsets = [&](int width, int height) {
    float sU = (1.f / static_cast<float>(width));
    float sV = (1.f / static_cast<float>(height));

    luminanceOffsets[0] = -0.5f * sU;
    luminanceOffsets[1] = 0.5f * sV;
    luminanceOffsets[2] = 0.5f * sU;
    luminanceOffsets[3] = 0.5f * sV;
    luminanceOffsets[4] = -0.5f * sU;
    luminanceOffsets[5] = -0.5f * sV;
    luminanceOffsets[6] = 0.5f * sU;
    luminanceOffsets[7] = -0.5f * sV;
  };

  auto luminanceUpdateDestOffsets = [&](int width, int height) {
    unsigned id = 0;
    for (int x = -1; x < 2; ++x) {
      for (int y = -1; y < 2; ++y) {
        downSampleOffsets[id]
          = static_cast<float>(x) / static_cast<float>(width);
        downSampleOffsets[id + 1]
          = static_cast<float>(y) / static_cast<float>(height);
        id += 2;
      }
    }
  };

  // Luminance callback
  auto luminanceCallback = [&](Effect* effect) {
    if (_needUpdate) {
      luminanceUpdateSourceOffsets(_textureAdderPostProcess->width,
                                   _textureAdderPostProcess->height);
    }

    effect->setTextureFromPostProcess("textureSampler",
                                      _textureAdderPostProcess);
    effect->setArray2("lumOffsets", luminanceOffsets);
  };

  // Down sample callbacks
  auto downSampleCallback = [&](unsigned int indice) {
    unsigned int i = indice;
    return [&](Effect* effect) {
      luminanceUpdateSourceOffsets(_downSamplePostProcesses[i]->width,
                                   _downSamplePostProcesses[i]->height);
      luminanceUpdateDestOffsets(_downSamplePostProcesses[i]->width,
                                 _downSamplePostProcesses[i]->height);
      halfDestPixelSize
        = 0.5f / static_cast<float>(_downSamplePostProcesses[i]->width);

      effect->setTextureFromPostProcess("textureSampler",
                                        _downSamplePostProcesses[i + 1]);
      effect->setFloat("halfDestPixelSize", halfDestPixelSize);
      effect->setArray2("dsOffsets", downSampleOffsets);
    };
  };

  auto downSampleAfterRenderCallback = [&](Effect* /*effect*/) {
    // Unpack result
    Uint8Array pixel = scene->getEngine()->readPixels(0, 0, 1, 1);
    Vector4 bit_shift(1.f / (255.f * 255.f * 255.f), 1.f / (255.f * 255.f),
                      1.f / 255.f, 1.f);
    const std::array<float, 4> _pixel{
      {static_cast<float>(pixel[0]), static_cast<float>(pixel[1]),
       static_cast<float>(pixel[2]), static_cast<float>(pixel[3])}};
    _hdrCurrentLuminance = (_pixel[0] * bit_shift.x + _pixel[1] * bit_shift.y
                            + _pixel[2] * bit_shift.z + _pixel[3] * bit_shift.w)
                           / 100.f;
  };

  // Create luminance post-process
  int width  = static_cast<int>(std::pow(3, lumSteps - 1));
  int height = static_cast<int>(std::pow(3, lumSteps - 1));
  PostProcessOptions ratio{width, height};
  _downSamplePostProcesses[lumSteps - 1]
    = new PostProcess("hdr", "hdr", {"lumOffsets"}, {}, ratio, nullptr,
                      Texture::NEAREST_SAMPLINGMODE, scene->getEngine(), false,
                      "#define LUMINANCE_GENERATOR", Engine::TEXTURETYPE_FLOAT);
  _downSamplePostProcesses[lumSteps - 1]->setOnApply(luminanceCallback);

  // Create down sample post-processes
  for (unsigned int i = lumSteps - 1; i > 0; --i) {
    unsigned int _i = i - 1;
    int length      = static_cast<int>(std::pow(3, i));
    ratio           = PostProcessOptions{length, length};

    std::string defines = "#define DOWN_SAMPLE\n";
    if (_i == 0) {
      defines += "#define FINAL_DOWN_SAMPLE\n"; // To pack the result
    }

    _downSamplePostProcesses[_i] = new PostProcess(
      "hdr", "hdr", {"dsOffsets", "halfDestPixelSize"}, {}, ratio, nullptr,
      Texture::NEAREST_SAMPLINGMODE, scene->getEngine(), false, defines,
      Engine::TEXTURETYPE_FLOAT);
    _downSamplePostProcesses[_i]->setOnApply(downSampleCallback(i));

    if (_i == 0) {
      _downSamplePostProcesses[_i]->setOnAfterRender(
        downSampleAfterRenderCallback);
    }
  }
}

void HDRRenderingPipeline::_createGaussianBlurPostProcess(Scene* scene,
                                                          float ratio)
{
  Float32Array blurOffsetsW(9);
  Float32Array blurOffsetsH(9);
  Float32Array blurWeights(9);
  std::vector<std::string> uniforms{"blurOffsets", "blurWeights", "multiplier"};

  // Utils for gaussian blur
  auto calculateBlurOffsets = [&](bool height) {
    ISize lastOutputDimensions{scene->getEngine()->getRenderWidth(),
                               scene->getEngine()->getRenderHeight()};

    for (unsigned int i = 0; i < 9; ++i) {
      float value
        = static_cast<float>(i - 4)
          * (1.f / static_cast<float>(height ? lastOutputDimensions.height :
                                               lastOutputDimensions.width));
      if (height) {
        blurOffsetsH[i] = value;
      }
      else {
        blurOffsetsW[i] = value;
      }
    }
  };

  auto calculateWeights = [&]() {
    float x = 0.f;

    for (unsigned i = 0; i < 9; i++) {
      x              = static_cast<float>(i) - 4.f / 4.f;
      blurWeights[i] = gaussCoeff
                       * (1.f / std::sqrt(2.f * Math::PI * gaussStandDev))
                       * std::exp((-((x - gaussMean) * (x - gaussMean)))
                                  / (2.f * gaussStandDev * gaussStandDev));
    }
  };

  // Callback
  auto gaussianBlurCallback = [&](bool height) {
    return [&](Effect* effect) {
      if (_needUpdate) {
        calculateWeights();
        calculateBlurOffsets(height);
      }
      effect->setArray("blurOffsets", height ? blurOffsetsH : blurOffsetsW);
      effect->setArray("blurWeights", blurWeights);
      effect->setFloat("multiplier", gaussMultiplier);
    };
  };

  // Create horizontal gaussian blur post-processes
  _guassianBlurHPostProcess
    = new PostProcess("hdr", "hdr", uniforms, {}, ratio / 4.f, nullptr,
                      Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
                      "#define GAUSSIAN_BLUR_H");
  _guassianBlurHPostProcess->setOnApply(gaussianBlurCallback(false));

  // Create vertical gaussian blur post-process
  _guassianBlurVPostProcess
    = new PostProcess("hdr", "hdr", uniforms, {}, ratio / 4.f, nullptr,
                      Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
                      "#define GAUSSIAN_BLUR_V");
  _guassianBlurVPostProcess->setOnApply(gaussianBlurCallback(true));
}

} // end of namespace BABYLON
