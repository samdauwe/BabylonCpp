#include <babylon/postprocess/standard_rendering_pipeline.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/renderpipeline/post_process_render_effect.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

StandardRenderingPipeline::StandardRenderingPipeline(
  const std::string& iName, Scene* scene, float ratio,
  PostProcess* iOriginalPostProcess, const std::vector<Camera*>& cameras)
    : PostProcessRenderPipeline{scene->getEngine(), iName}
    , originalPostProcess{nullptr}
    , downSampleX4PostProcess{nullptr}
    , brightPassPostProcess{nullptr}
    , textureAdderPostProcess{nullptr}
    , textureAdderFinalPostProcess{nullptr}
    , lensFlarePostProcess{nullptr}
    , lensFlareComposePostProcess{nullptr}
    , depthOfFieldPostProcess{nullptr}
    , brightThreshold{1.f}
    , blurWidth{2.f}
    , gaussianCoefficient{0.25f}
    , gaussianMean{1.f}
    , gaussianStandardDeviation{1.f}
    , exposure{1.f}
    , lensTexture{nullptr}
    , lensColorTexture{nullptr}
    , lensFlareStrength{20.f}
    , lensFlareGhostDispersal{1.4f}
    , lensFlareHaloWidth{0.7f}
    , lensFlareDistortionStrength{16.f}
    , lensStarTexture{nullptr}
    , lensFlareDirtTexture{nullptr}
    , depthOfFieldDistance{10.f}
    , _scene{scene}
    , _depthRenderer{nullptr}
    , _depthOfFieldEnabled{true}
    , _lensFlareEnabled{true}
{
  // Create pass post-process
  if (!originalPostProcess) {
    originalPostProcess = new PostProcess(
      "HDRPass", "standard", {}, {}, ratio, nullptr,
      Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), true,
      "#define PASS_POST_PROCESS", Engine::TEXTURETYPE_FLOAT);
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
    Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), true,
    "#define PASS_POST_PROCESS", Engine::TEXTURETYPE_UNSIGNED_INT);
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRDepthOfFieldSource",
    [&]() { return textureAdderFinalPostProcess; }, true));

  // Create lens flare post-process
  _createLensFlarePostProcess(scene, ratio);

  // Create gaussian blur used by depth-of-field
  _createGaussianBlurPostProcesses(scene, ratio / 2.f, 5);

  // Create depth-of-field post-process
  _createDepthOfFieldPostProcess(scene, ratio);

  // Finish
  scene->postProcessRenderPipelineManager->addPipeline(this);

  if (!cameras.empty()) {
    scene->postProcessRenderPipelineManager->attachCamerasToRenderPipeline(
      iName, cameras);
  }

  // Deactivate
  setLensFlareEnabled(false);
  setDepthOfFieldEnabled(false);
}

StandardRenderingPipeline::~StandardRenderingPipeline()
{
}

void StandardRenderingPipeline::setDepthOfFieldEnabled(bool enabled)
{
  auto blurIndexStr = std::to_string(gaussianBlurHPostProcesses.size() - 1);

  if (enabled && !_depthOfFieldEnabled) {
    _scene->postProcessRenderPipelineManager->enableEffectInPipeline(
      _name, "HDRGaussianBlurH" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager->enableEffectInPipeline(
      _name, "HDRGaussianBlurV" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager->enableEffectInPipeline(
      _name, "HDRDepthOfField", _scene->getCameras());
    _depthRenderer = _scene->enableDepthRenderer();
  }
  else if (!enabled && _depthOfFieldEnabled) {
    _scene->postProcessRenderPipelineManager->disableEffectInPipeline(
      _name, "HDRGaussianBlurH" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager->disableEffectInPipeline(
      _name, "HDRGaussianBlurV" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager->disableEffectInPipeline(
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
    _scene->postProcessRenderPipelineManager->enableEffectInPipeline(
      _name, "HDRLensFlare", _scene->getCameras());
    _scene->postProcessRenderPipelineManager->enableEffectInPipeline(
      _name, "HDRLensFlareShift", _scene->getCameras());
    _scene->postProcessRenderPipelineManager->enableEffectInPipeline(
      _name, "HDRGaussianBlurH" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager->enableEffectInPipeline(
      _name, "HDRGaussianBlurV" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager->enableEffectInPipeline(
      _name, "HDRLensFlareCompose", _scene->getCameras());
  }
  else if (!enabled && _lensFlareEnabled) {
    _scene->postProcessRenderPipelineManager->disableEffectInPipeline(
      _name, "HDRLensFlare", _scene->getCameras());
    _scene->postProcessRenderPipelineManager->disableEffectInPipeline(
      _name, "HDRLensFlareShift", _scene->getCameras());
    _scene->postProcessRenderPipelineManager->disableEffectInPipeline(
      _name, "HDRGaussianBlurH" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager->disableEffectInPipeline(
      _name, "HDRGaussianBlurV" + blurIndexStr, _scene->getCameras());
    _scene->postProcessRenderPipelineManager->disableEffectInPipeline(
      _name, "HDRLensFlareCompose", _scene->getCameras());
  }

  _lensFlareEnabled = enabled;
}

bool StandardRenderingPipeline::lensFlareEnabled() const
{
  return _lensFlareEnabled;
}

void StandardRenderingPipeline::_createDownSampleX4PostProcess(Scene* scene,
                                                               float ratio)
{
  downSampleX4PostProcess = new PostProcess(
    "HDRDownSampleX4", "standard", {"dsOffsets"}, {}, ratio, nullptr,
    Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define DOWN_SAMPLE_X4", Engine::TEXTURETYPE_UNSIGNED_INT);

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
    nullptr, Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define BRIGHT_PASS", Engine::TEXTURETYPE_UNSIGNED_INT);

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
    Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define GAUSSIAN_BLUR_H", Engine::TEXTURETYPE_UNSIGNED_INT);
  gaussianBlurHPostProcess->setOnApply(
    [&](Effect* /*effect*/) { callback(false); });

  // Create vertical gaussian blur post-process
  auto gaussianBlurVPostProcess = new PostProcess(
    "HDRGaussianBlurV" + ratioStr, "standard", uniforms, {}, ratio, nullptr,
    Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define GAUSSIAN_BLUR_V", Engine::TEXTURETYPE_UNSIGNED_INT);
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
    Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define TEXTURE_ADDER", Engine::TEXTURETYPE_UNSIGNED_INT);
  textureAdderPostProcess->setOnApply([&](Effect* effect) {
    effect->setTextureFromPostProcess("otherSampler", originalPostProcess);
    effect->setTexture("lensSampler", lensTexture);
    effect->setFloat("exposure", exposure);
  });

  // Add to pipeline
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRTextureAdder",
    [&]() { return textureAdderPostProcess; }, true));
}

void StandardRenderingPipeline::_createLensFlarePostProcess(Scene* scene,
                                                            float ratio)
{
  lensFlarePostProcess = new PostProcess(
    "HDRLensFlare", "standard", {"strength", "ghostDispersal", "haloWidth",
                                 "resolution", "distortionStrength"},
    {"lensColorSampler"}, ratio / 2.f, nullptr, Texture::BILINEAR_SAMPLINGMODE,
    scene->getEngine(), true, "#define LENS_FLARE",
    Engine::TEXTURETYPE_UNSIGNED_INT);
  addEffect(new PostProcessRenderEffect(scene->getEngine(), "HDRLensFlare",
                                        [&]() { return lensFlarePostProcess; },
                                        false));

  _createGaussianBlurPostProcesses(scene, ratio / 4.f, 4);

  lensFlareComposePostProcess = new PostProcess(
    "HDRLensFlareCompose", "standard", {"lensStarMatrix"},
    {"otherSampler", "lensDirtSampler", "lensStarSampler"}, ratio, nullptr,
    Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define LENS_FLARE_COMPOSE", Engine::TEXTURETYPE_UNSIGNED_INT);
  addEffect(new PostProcessRenderEffect(
    scene->getEngine(), "HDRLensFlareCompose",
    [&]() { return lensFlareComposePostProcess; }, false));

  // Lens flare
  lensFlarePostProcess->setOnApply([&](Effect* effect) {
    effect->setTextureFromPostProcess("textureSampler",
                                      gaussianBlurHPostProcesses[0]);
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
  });
}

void StandardRenderingPipeline::_createDepthOfFieldPostProcess(Scene* scene,
                                                               float ratio)
{
  depthOfFieldPostProcess = new PostProcess(
    "HDRDepthOfField", "standard", {"distance"},
    {"otherSampler", "depthSampler"}, ratio, nullptr,
    Texture::BILINEAR_SAMPLINGMODE, scene->getEngine(), false,
    "#define DEPTH_OF_FIELD", Engine::TEXTURETYPE_UNSIGNED_INT);
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
  }

  _scene->postProcessRenderPipelineManager->detachCamerasFromRenderPipeline(
    _name, _scene->getCameras());

  PostProcessRenderPipeline::dispose(doNotRecurse);
}

} // end of namespace BABYLON
