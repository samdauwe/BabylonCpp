#include <babylon/postprocesses/depth_of_field_blur_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

DepthOfFieldBlurPostProcess::DepthOfFieldBlurPostProcess(
  const std::string& iName, Scene* scene, const Vector2& iDirection, float iKernel,
  const std::variant<float, PostProcessOptions>& options, const CameraPtr& camera,
  PostProcess* circleOfConfusion, PostProcess* imageToBlur,
  const std::optional<unsigned int>& samplingMode, Engine* engine, bool reusable,
  unsigned int textureType, bool blockCompilation)
    : BlurPostProcess{
      iName,           iDirection,
      iKernel,         options,
      camera,          samplingMode.value_or(Constants::TEXTURE_BILINEAR_SAMPLINGMODE),
      engine,          reusable,
      textureType,     "#define DOF 1\r\n",
      blockCompilation}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    if (imageToBlur != nullptr) {
      effect->setTextureFromPostProcess("textureSampler", imageToBlur);
    }
    effect->setTextureFromPostProcessOutput("circleOfConfusionSampler", circleOfConfusion);
    if (scene->activeCamera()) {
      effect->setFloat2("cameraMinMaxZ", scene->activeCamera()->minZ, scene->activeCamera()->maxZ);
    }
  });
}

DepthOfFieldBlurPostProcess::~DepthOfFieldBlurPostProcess() = default;

} // end of namespace BABYLON
