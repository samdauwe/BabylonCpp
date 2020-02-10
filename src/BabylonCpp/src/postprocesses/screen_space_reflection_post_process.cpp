#include <babylon/postprocesses/screen_space_reflection_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/misc/string_tools.h>
#include <babylon/rendering/geometry_buffer_renderer.h>

namespace BABYLON {

ScreenSpaceReflectionPostProcess::ScreenSpaceReflectionPostProcess(
  const std::string& iName, Scene* scene, float options, const CameraPtr& camera,
  unsigned int samplingMode, Engine* engine, bool reusable, unsigned int textureType,
  bool blockCompilation)
    : PostProcess{iName,
                  "screenSpaceReflection",
                  {"projection", "view", "threshold", "reflectionSpecularFalloffExponent",
                   "strength", "step", "roughnessFactor"},
                  {"textureSampler", "normalSampler", "positionSampler", "reflectivitySampler"},
                  options,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "#define SSR_SUPPORTED\n#define REFLECTION_SAMPLES 64\n#define SMOOTH_STEPS 5\n",
                  textureType,
                  "",
                  {},
                  blockCompilation}
    , threshold{1.2f}
    , strength{1.f}
    , reflectionSpecularFalloffExponent{3.f}
    , step{1.f}
    , roughnessFactor{0.2f}
    , enableSmoothReflections{this, &ScreenSpaceReflectionPostProcess::get_enableSmoothReflections,
                              &ScreenSpaceReflectionPostProcess::set_enableSmoothReflections}
    , reflectionSamples{this, &ScreenSpaceReflectionPostProcess::get_reflectionSamples,
                        &ScreenSpaceReflectionPostProcess::set_reflectionSamples}
    , smoothSteps{this, &ScreenSpaceReflectionPostProcess::get_smoothSteps,
                  &ScreenSpaceReflectionPostProcess::set_smoothSteps}
    , _geometryBufferRenderer{nullptr}
    , _enableSmoothReflections{false}
    , _reflectionSamples{64}
    , _smoothSteps{5}
{
  // Get geometry buffer renderer and update effect
  auto geometryBufferRenderer = scene->enableGeometryBufferRenderer();
  if (geometryBufferRenderer) {
    if (geometryBufferRenderer->isSupported()) {
      geometryBufferRenderer->enablePosition     = true;
      geometryBufferRenderer->enableReflectivity = true;
      _geometryBufferRenderer                    = geometryBufferRenderer;
    }
  }

  _updateEffectDefines();

  // On apply, send uniforms
  onApply = [this, geometryBufferRenderer, scene, camera](Effect* effect, EventState&) -> void {
    if (!geometryBufferRenderer) {
      return;
    }

    // Samplers
    const auto positionIndex = static_cast<size_t>(
      geometryBufferRenderer->getTextureIndex(GeometryBufferRenderer::POSITION_TEXTURE_TYPE));
    const auto roughnessIndex = static_cast<size_t>(
      geometryBufferRenderer->getTextureIndex(GeometryBufferRenderer::REFLECTIVITY_TEXTURE_TYPE));

    effect->setTexture("normalSampler", geometryBufferRenderer->getGBuffer()->textures()[1]);
    effect->setTexture("positionSampler",
                       geometryBufferRenderer->getGBuffer()->textures()[positionIndex]);
    effect->setTexture("reflectivitySampler",
                       geometryBufferRenderer->getGBuffer()->textures()[roughnessIndex]);

    // Uniforms
    auto camera = scene->activeCamera();
    if (!camera) {
      return;
    }

    const auto viewMatrix       = camera->getViewMatrix();
    const auto projectionMatrix = camera->getProjectionMatrix();

    effect->setMatrix("projection", projectionMatrix);
    effect->setMatrix("view", viewMatrix);
    effect->setFloat("threshold", threshold);
    effect->setFloat("reflectionSpecularFalloffExponent", reflectionSpecularFalloffExponent);
    effect->setFloat("strength", strength);
    effect->setFloat("step", step);
    effect->setFloat("roughnessFactor", roughnessFactor);
  };
}

ScreenSpaceReflectionPostProcess::~ScreenSpaceReflectionPostProcess() = default;

bool ScreenSpaceReflectionPostProcess::get_enableSmoothReflections() const
{
  return _enableSmoothReflections;
}

void ScreenSpaceReflectionPostProcess::set_enableSmoothReflections(bool enabled)
{
  if (enabled == _enableSmoothReflections) {
    return;
  }

  _enableSmoothReflections = enabled;
  _updateEffectDefines();
}

unsigned int ScreenSpaceReflectionPostProcess::get_reflectionSamples() const
{
  return _reflectionSamples;
}

void ScreenSpaceReflectionPostProcess::set_reflectionSamples(unsigned int iSamples)
{
  if (iSamples == _reflectionSamples) {
    return;
  }

  _reflectionSamples = iSamples;
  _updateEffectDefines();
}

unsigned int ScreenSpaceReflectionPostProcess::get_smoothSteps() const
{
  return _smoothSteps;
}

void ScreenSpaceReflectionPostProcess::set_smoothSteps(unsigned int steps)
{
  if (steps == _smoothSteps) {
    return;
  }

  _smoothSteps = steps;
  _updateEffectDefines();
}

void ScreenSpaceReflectionPostProcess::_updateEffectDefines()
{
  std::vector<std::string> defines;
  if (_geometryBufferRenderer) {
    defines.emplace_back("#define SSR_SUPPORTED");
  }
  if (_enableSmoothReflections) {
    defines.emplace_back("#define ENABLE_SMOOTH_REFLECTIONS");
  }

  defines.emplace_back("#define REFLECTION_SAMPLES " + std::to_string(_reflectionSamples >> 0));
  defines.emplace_back("#define SMOOTH_STEPS " + std::to_string(_smoothSteps >> 0));

  updateEffect(StringTools::join(defines, '\n'));
}

} // end of namespace BABYLON
