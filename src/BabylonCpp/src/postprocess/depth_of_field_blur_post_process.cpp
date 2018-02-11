#include <babylon/postprocess/depth_of_field_blur_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

DepthOfFieldBlurPostProcess::DepthOfFieldBlurPostProcess(
  const string_t& name, Scene* scene, const Vector2& direction, float kernel,
  const Variant<float, PostProcessOptions>& options, Camera* camera,
  PostProcess* circleOfConfusion, PostProcess* imageToBlur,
  unsigned int samplingMode, Engine* engine, bool reusable,
  unsigned int textureType)
    : BlurPostProcess{name,        direction,          kernel, options,
                      camera,      samplingMode,       engine, reusable,
                      textureType, "#define DOF 1\r\n"}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    if (imageToBlur != nullptr) {
      effect->setTextureFromPostProcess("textureSampler", imageToBlur);
    }
    effect->setTextureFromPostProcess("circleOfConfusionSampler",
                                      circleOfConfusion);
    if (scene->activeCamera) {
      effect->setFloat2("cameraMinMaxZ", scene->activeCamera->minZ,
                        scene->activeCamera->maxZ);
    }
  });
}

DepthOfFieldBlurPostProcess::~DepthOfFieldBlurPostProcess()
{
}

} // end of namespace BABYLON
