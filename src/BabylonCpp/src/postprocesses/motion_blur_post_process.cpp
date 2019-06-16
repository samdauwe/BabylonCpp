#include <babylon/postprocesses/motion_blur_post_process.h>

#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/rendering/geometry_buffer_renderer.h>

namespace BABYLON {

MotionBlurPostProcess::MotionBlurPostProcess(
  const std::string& iName, Scene* scene,
  const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
  bool reusable, unsigned int textureType, bool blockCompilation)
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
    , _geometryBufferRenderer{nullptr}
{
  _geometryBufferRenderer = scene->enableGeometryBufferRenderer();

  if (!_geometryBufferRenderer) {
    // Geometry buffer renderer is not supported. So, work as a passthrough.
    BABYLON_LOG_WARN("MotionBlurPostProcess",
                     "Multiple Render Target support needed to compute object "
                     "based motion blur")
    updateEffect();
  }
  else {
    // Geometry buffer renderer is supported.
    _geometryBufferRenderer->enableVelocity = true;

    onApply = [&](Effect* effect, EventState & /*es*/) -> void {
      effect->setVector2("screenSize", Vector2(static_cast<float>(width),
                                               static_cast<float>(height)));

      effect->setFloat("motionScale", scene->getAnimationRatio());
      effect->setFloat("motionStrength", motionStrength);

      if (_geometryBufferRenderer) {
        const auto velocityIndex = _geometryBufferRenderer->getTextureIndex(
          GeometryBufferRenderer::VELOCITY_TEXTURE_TYPE);
        if (velocityIndex > 0) {
          const auto& textures
            = _geometryBufferRenderer->getGBuffer()->textures();
          const auto& texture
            = textures.at(static_cast<unsigned>(velocityIndex));
          effect->setTexture("velocitySampler", texture);
        }
      }
    };
  }
}

MotionBlurPostProcess::~MotionBlurPostProcess()
{
}

unsigned int MotionBlurPostProcess::get_motionBlurSamples() const
{
  return _motionBlurSamples;
}

void MotionBlurPostProcess::set_motionBlurSamples(unsigned int iSamples)
{
  _motionBlurSamples = iSamples;

  if (_geometryBufferRenderer) {
    updateEffect("#define GEOMETRY_SUPPORTED\n#define SAMPLES "
                 + std::to_string(iSamples));
  }
}

void MotionBlurPostProcess::excludeSkinnedMesh(
  const AbstractMeshPtr& skinnedMesh)
{
  if (_geometryBufferRenderer && skinnedMesh->skeleton()) {
    _geometryBufferRenderer->excludedSkinnedMeshesFromVelocity.emplace_back(
      skinnedMesh);
  }
}

void MotionBlurPostProcess::removeExcludedSkinnedMesh(
  const AbstractMeshPtr& skinnedMesh)
{
  if (_geometryBufferRenderer && skinnedMesh->skeleton()) {
    _geometryBufferRenderer->excludedSkinnedMeshesFromVelocity.erase(
      std::remove(
        _geometryBufferRenderer->excludedSkinnedMeshesFromVelocity.begin(),
        _geometryBufferRenderer->excludedSkinnedMeshesFromVelocity.end(),
        skinnedMesh),
      _geometryBufferRenderer->excludedSkinnedMeshesFromVelocity.end());
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

} // end of namespace BABYLON
