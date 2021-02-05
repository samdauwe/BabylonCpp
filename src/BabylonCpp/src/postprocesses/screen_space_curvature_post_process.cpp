#include <babylon/postprocesses/screen_space_curvature_post_process.h>

#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/engine_store.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/rendering/geometry_buffer_renderer.h>

namespace BABYLON {

ScreenSpaceCurvaturePostProcess::ScreenSpaceCurvaturePostProcess(
  const std::string& iName, Scene* scene, const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, const std::optional<unsigned int>& samplingMode, Engine* engine,
  bool reusable, unsigned int textureType, bool blockCompilation)
    : PostProcess{iName,
                  "screenSpaceCurvature",
                  {"curvature_ridge", "curvature_valley"},
                  {"textureSampler", "normalSampler"},
                  options,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "",
                  textureType,
                  "",
                  {},
                  blockCompilation}
    , ridge{1.f}
    , valley{1.f}
{
  _geometryBufferRenderer = scene->enableGeometryBufferRenderer();

  if (!_geometryBufferRenderer) {
    // Geometry buffer renderer is not supported. So, work as a passthrough.
    BABYLON_LOG_ERROR("ScreenSpaceCurvaturePostProcess",
                      "Multiple Render Target support needed for screen space curvature post "
                      "process. Please use IsSupported test first.");
  }
  else {
    // Geometry buffer renderer is supported.
    onApply = [this](Effect* effect, EventState& /*es*/) -> void {
      effect->setFloat("curvature_ridge", 0.5f / std::max(ridge * ridge, 1e-4f));
      effect->setFloat("curvature_valley", 0.7f / std::max(valley * valley, 1e-4f));

      const auto normalTexture = _geometryBufferRenderer->getGBuffer()->textures()[1];
      effect->setTexture("normalSampler", normalTexture);
    };
  }
}

ScreenSpaceCurvaturePostProcess::~ScreenSpaceCurvaturePostProcess() = default;

std::string ScreenSpaceCurvaturePostProcess::getClassName() const
{
  return "ScreenSpaceCurvaturePostProcess";
}

bool ScreenSpaceCurvaturePostProcess::IsSupported()
{
  const auto engine = EngineStore::LastCreatedEngine();
  if (!engine) {
    return false;
  }

  return engine->getCaps().drawBuffersExtension;
}

ScreenSpaceCurvaturePostProcessPtr
ScreenSpaceCurvaturePostProcess::_Parse(const json& /*parsedPostProcess*/,
                                        const CameraPtr& /*targetCamera*/, Scene* /*scene*/,
                                        const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
