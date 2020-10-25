#include <babylon/postprocesses/motion_blur_post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/rendering/geometry_buffer_renderer.h>
#include <babylon/rendering/motion_blur_configuration.h>
#include <babylon/rendering/pre_pass_renderer.h>

namespace BABYLON {

MotionBlurPostProcess::MotionBlurPostProcess(
  const std::string& iName, Scene* scene, const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, const std::optional<unsigned int>& samplingMode, Engine* engine,
  bool reusable, unsigned int textureType, bool blockCompilation, bool forceGeometryBuffer)
    : PostProcess{iName,
                  "motionBlur",
                  {"motionStrength", "motionScale", "screenSize"},
                  {"velocitySampler"},
                  options,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "#define GEOMETRY_SUPPORTED\n#define SAMPLES 64.0",
                  textureType,
                  "",
                  {},
                  blockCompilation}
    , motionStrength{1.f}
    , motionBlurSamples{this, &MotionBlurPostProcess::get_motionBlurSamples,
                        &MotionBlurPostProcess::set_motionBlurSamples}
    , _motionBlurSamples{32}
    , _forceGeometryBuffer{false}
    , _geometryBufferRenderer{nullptr}
    , _prePassRenderer{nullptr}
{
  _forceGeometryBuffer = forceGeometryBuffer;

  // Set up assets
  if (_forceGeometryBuffer) {
    _geometryBufferRenderer = scene->enableGeometryBufferRenderer();
    if (_geometryBufferRenderer) {
      _geometryBufferRenderer->enableVelocity = true;
    }
  }
  else {
    _prePassRenderer = scene->enablePrePassRenderer();
    _prePassRenderer->markAsDirty();
    _prePassEffectConfiguration = std::make_shared<MotionBlurConfiguration>();
  }

  if (!_geometryBufferRenderer && !_prePassRenderer) {
    // We can't get a velocity texture. So, work as a passthrough.
    BABYLON_LOG_WARN("MotionBlurPostProcess",
                     "Multiple Render Target support needed to compute object "
                     "based motion blur")
    updateEffect();
  }
  else {
    onApply = [this, scene](Effect* effect, EventState & /*es*/) -> void {
      effect->setVector2("screenSize",
                         Vector2(static_cast<float>(width), static_cast<float>(height)));

      effect->setFloat("motionScale", scene->getAnimationRatio());
      effect->setFloat("motionStrength", motionStrength);

      if (_geometryBufferRenderer) {
        const auto velocityIndex
          = _geometryBufferRenderer->getTextureIndex(GeometryBufferRenderer::VELOCITY_TEXTURE_TYPE);
        if (velocityIndex > 0) {
          const auto& textures      = _geometryBufferRenderer->getGBuffer()->textures();
          const auto _velocityIndex = static_cast<unsigned>(velocityIndex);
          if (_velocityIndex < textures.size()) {
            const auto& texture = textures.at(_velocityIndex);
            effect->setTexture("velocitySampler", texture);
          }
        }
      }
      else {
        const auto velocityIndex
          = _geometryBufferRenderer->getTextureIndex(Constants::PREPASS_VELOCITY_TEXTURE_TYPE);
        if (velocityIndex > 0) {
          const auto _velocityIndex = static_cast<unsigned>(velocityIndex);
          effect->setTexture("velocitySampler",
                             _prePassRenderer->prePassRT->textures()[_velocityIndex]);
        }
      }
    };
  }
}

MotionBlurPostProcess::~MotionBlurPostProcess() = default;

std::string MotionBlurPostProcess::getClassName() const
{
  return "MotionBlurPostProcess";
}

unsigned int MotionBlurPostProcess::get_motionBlurSamples() const
{
  return _motionBlurSamples;
}

void MotionBlurPostProcess::set_motionBlurSamples(unsigned int iSamples)
{
  _motionBlurSamples = iSamples;

  if (_geometryBufferRenderer) {
    updateEffect("#define GEOMETRY_SUPPORTED\n#define SAMPLES " + std::to_string(iSamples));
  }
}

void MotionBlurPostProcess::excludeSkinnedMesh(const AbstractMeshPtr& skinnedMesh)
{
  if (skinnedMesh->skeleton()) {
    std::vector<AbstractMeshPtr>* list = nullptr;
    if (_geometryBufferRenderer) {
      list = &_geometryBufferRenderer->excludedSkinnedMeshesFromVelocity;
    }
    else if (_prePassRenderer) {
      list = &_prePassRenderer->excludedSkinnedMesh;
    }
    else {
      return;
    }
    list->emplace_back(skinnedMesh);
  }
}

void MotionBlurPostProcess::removeExcludedSkinnedMesh(const AbstractMeshPtr& skinnedMesh)
{
  if (skinnedMesh->skeleton()) {
    std::vector<AbstractMeshPtr>* list = nullptr;
    if (_geometryBufferRenderer) {
      list = &_geometryBufferRenderer->excludedSkinnedMeshesFromVelocity;
    }
    else if (_prePassRenderer) {
      list = &_prePassRenderer->excludedSkinnedMesh;
    }
    else {
      return;
    }

    const auto index = stl_util::index_of(*list, skinnedMesh);
    if (index != -1) {
      stl_util::splice(*list, index, 1);
    }
  }
}

void MotionBlurPostProcess::dispose(Camera* camera)
{
  if (_geometryBufferRenderer) {
    // Clear previous transformation matrices dictionary used to compute objects
    // velocities
    _geometryBufferRenderer->_previousTransformationMatrices.clear();
    _geometryBufferRenderer->_previousBonesTransformationMatrices.clear();
    _geometryBufferRenderer->excludedSkinnedMeshesFromVelocity.clear();
  }

  PostProcess::dispose(camera);
}

MotionBlurPostProcessPtr MotionBlurPostProcess::_Parse(const json& /*parsedPostProcess*/,
                                                       const CameraPtr& /*targetCamera*/,
                                                       Scene* /*scene*/,
                                                       const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
