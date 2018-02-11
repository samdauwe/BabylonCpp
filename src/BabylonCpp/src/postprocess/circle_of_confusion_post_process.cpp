#include <babylon/postprocess/circle_of_confusion_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/render_target_texture.h>

namespace BABYLON {

CircleOfConfusionPostProcess::CircleOfConfusionPostProcess(
  const string_t& name, Scene* scene, RenderTargetTexture* depthTexture,
  const Variant<float, PostProcessOptions>& options, Camera* camera,
  unsigned int samplingMode, Engine* engine, bool reusable,
  unsigned int textureType)
    : PostProcess{name,
                  "circleOfConfusion",
                  {"cameraMinMaxZ", "focusDistance", "cocPrecalculation"},
                  {"depthSampler"},
                  options,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  nullptr,
                  textureType}
    , lensSize{50.f}
    , fStop{1.4f}
    , focusDistance{2000.f}
    , focalLength{50.f}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    effect->setTexture("depthSampler", depthTexture);

    // Circle of confusion calculation, See
    // https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch23.html
    float aperture = lensSize / fStop;
    float cocPrecalculation
      = ((aperture * focalLength)
         / ((focusDistance - focalLength))); // * ((focusDistance -
                                             // pixelDistance)/pixelDistance)
                                             // [This part is done in shader]

    effect->setFloat("focusDistance", focusDistance);
    effect->setFloat("cocPrecalculation", cocPrecalculation);

    if (scene->activeCamera) {
      effect->setFloat2("cameraMinMaxZ", scene->activeCamera->minZ,
                        scene->activeCamera->maxZ);
    }
  });
}

CircleOfConfusionPostProcess::~CircleOfConfusionPostProcess()
{
}

} // end of namespace BABYLON
