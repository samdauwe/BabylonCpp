#include <babylon/postprocesses/screen_space_reflection_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/pre_pass_render_target.h>
#include <babylon/misc/string_tools.h>
#include <babylon/rendering/geometry_buffer_renderer.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/screen_space_reflections_configuration.h>

namespace BABYLON {

ScreenSpaceReflectionPostProcess::ScreenSpaceReflectionPostProcess(
  const std::string& iName, Scene* scene, float options, const CameraPtr& camera,
  unsigned int samplingMode, Engine* engine, bool reusable, unsigned int textureType,
  bool blockCompilation, bool forceGeometryBuffer)
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
    , _forceGeometryBuffer{false}
    , _geometryBufferRenderer{this, &ScreenSpaceReflectionPostProcess::get__geometryBufferRenderer}
    , _prePassRenderer{this, &ScreenSpaceReflectionPostProcess::get__prePassRenderer}
    , _enableSmoothReflections{false}
    , _reflectionSamples{64}
    , _smoothSteps{5}
    , _nullGeometryBufferRenderer{nullptr}
    , _nullPrePassRenderer{nullptr}
{
  _forceGeometryBuffer = forceGeometryBuffer;

  if (_forceGeometryBuffer) {
    // Get geometry buffer renderer and update effect
    auto geometryBufferRenderer = scene->enableGeometryBufferRenderer();
    if (geometryBufferRenderer) {
      if (geometryBufferRenderer->isSupported()) {
        geometryBufferRenderer->enablePosition     = true;
        geometryBufferRenderer->enableReflectivity = true;
      }
    }
  }
  else {
    const auto prePassRenderer = scene->enablePrePassRenderer();
    if (prePassRenderer) {
      prePassRenderer->markAsDirty();
    }
    _prePassEffectConfiguration = std::make_shared<ScreenSpaceReflectionsConfiguration>();
  }

  _updateEffectDefines();

  // On apply, send uniforms
  onApply = [this, scene](Effect* effect, EventState&) -> void {
    const auto& geometryBufferRenderer = _geometryBufferRenderer;
    const auto& prePassRenderer        = _prePassRenderer;

    if (!prePassRenderer() && !geometryBufferRenderer()) {
      return;
    }

    const auto iGeometryBufferRenderer = geometryBufferRenderer();
    const auto iPrePassRenderer        = prePassRenderer();

    if (iGeometryBufferRenderer) {
      // Samplers
      const auto positionIndex = static_cast<size_t>(
        iGeometryBufferRenderer->getTextureIndex(GeometryBufferRenderer::POSITION_TEXTURE_TYPE));
      const auto roughnessIndex = static_cast<size_t>(iGeometryBufferRenderer->getTextureIndex(
        GeometryBufferRenderer::REFLECTIVITY_TEXTURE_TYPE));

      effect->setTexture("normalSampler", iGeometryBufferRenderer->getGBuffer()->textures()[1]);
      effect->setTexture("positionSampler",
                         iGeometryBufferRenderer->getGBuffer()->textures()[positionIndex]);
      effect->setTexture("reflectivitySampler",
                         iGeometryBufferRenderer->getGBuffer()->textures()[roughnessIndex]);
    }
    else if (iPrePassRenderer) {
      // Samplers
      const auto positionIndex
        = iPrePassRenderer->getIndex(Constants::PREPASS_POSITION_TEXTURE_TYPE);
      const auto roughnessIndex
        = iPrePassRenderer->getIndex(Constants::PREPASS_REFLECTIVITY_TEXTURE_TYPE);
      const auto normalIndex = iPrePassRenderer->getIndex(Constants::PREPASS_NORMAL_TEXTURE_TYPE);

      effect->setTexture("normalSampler",
                         iPrePassRenderer->getRenderTarget()->textures()[normalIndex]);
      effect->setTexture("positionSampler",
                         iPrePassRenderer->getRenderTarget()->textures()[positionIndex]);
      effect->setTexture("reflectivitySampler",
                         iPrePassRenderer->getRenderTarget()->textures()[roughnessIndex]);
    }

    // Uniforms
    const auto camera = scene->activeCamera();
    if (!camera) {
      return;
    }

    const auto& viewMatrix       = camera->getViewMatrix(true);
    const auto& projectionMatrix = camera->getProjectionMatrix(true);

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

std::string ScreenSpaceReflectionPostProcess::getClassName() const
{
  return "ScreenSpaceReflectionPostProcess";
}

GeometryBufferRendererPtr& ScreenSpaceReflectionPostProcess::get__geometryBufferRenderer()
{
  if (!_forceGeometryBuffer) {
    return _nullGeometryBufferRenderer;
  }

  return _scene->geometryBufferRenderer();
}

PrePassRendererPtr& ScreenSpaceReflectionPostProcess::get__prePassRenderer()
{
  if (_forceGeometryBuffer) {
    return _nullPrePassRenderer;
  }

  return _scene->prePassRenderer();
}

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
  if (_geometryBufferRenderer() || _prePassRenderer()) {
    defines.emplace_back("#define SSR_SUPPORTED");
  }
  if (_enableSmoothReflections) {
    defines.emplace_back("#define ENABLE_SMOOTH_REFLECTIONS");
  }

  defines.emplace_back("#define REFLECTION_SAMPLES " + std::to_string(_reflectionSamples >> 0));
  defines.emplace_back("#define SMOOTH_STEPS " + std::to_string(_smoothSteps >> 0));

  updateEffect(StringTools::join(defines, '\n'));
}

ScreenSpaceReflectionPostProcessPtr
ScreenSpaceReflectionPostProcess::_Parse(const json& /*parsedPostProcess*/,
                                         const CameraPtr& /*targetCamera*/, Scene* /*scene*/,
                                         const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
