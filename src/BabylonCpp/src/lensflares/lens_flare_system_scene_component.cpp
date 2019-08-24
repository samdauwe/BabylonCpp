#include <babylon/lensflares/lens_flare_system_scene_component.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/abstract_scene.h>
#include <babylon/engines/scene.h>
#include <babylon/lensflares/lens_flare_system.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

LensFlareSystemSceneComponent::LensFlareSystemSceneComponent(Scene* iScene)
{
  ISceneComponent::name = LensFlareSystemSceneComponent::name;
  scene                 = iScene;
}

LensFlareSystemSceneComponent::~LensFlareSystemSceneComponent()
{
}

void LensFlareSystemSceneComponent::_register()
{
  scene->_afterCameraDrawStage.registerStep(
    SceneComponentConstants::STEP_AFTERCAMERADRAW_LENSFLARESYSTEM, this,
    [this](Camera* camera) -> bool {
      _draw(camera);
      return true;
    });
}

void LensFlareSystemSceneComponent::rebuild()
{
  // Nothing to do for lens flare
}

void LensFlareSystemSceneComponent::addFromContainer(AbstractScene* container)
{
  if (container->lensFlareSystems.empty()) {
    return;
  }
  for (const auto& o : container->lensFlareSystems) {
    scene->addLensFlareSystem(o);
  }
}

void LensFlareSystemSceneComponent::removeFromContainer(
  AbstractScene* container, bool dispose)
{
  if (container->lensFlareSystems.empty()) {
    return;
  }
  for (const auto& o : container->lensFlareSystems) {
    scene->removeLensFlareSystem(o);
    if (dispose) {
      o->dispose();
    }
  }
}

void LensFlareSystemSceneComponent::serialize(json& /*serializationObject*/)
{
}

void LensFlareSystemSceneComponent::dispose()
{
  auto& lensFlareSystems = scene->lensFlareSystems;
  for (const auto& lensFlareSystem : lensFlareSystems) {
    lensFlareSystem->dispose();
  }
  lensFlareSystems.clear();
}

void LensFlareSystemSceneComponent::_draw(Camera* camera)
{
  // Lens flares
  if (scene->lensFlaresEnabled) {
    const auto& lensFlareSystems = scene->lensFlareSystems;
    Tools::StartPerformanceCounter("Lens flares", !lensFlareSystems.empty());
    for (const auto& lensFlareSystem : lensFlareSystems) {
      if ((camera->layerMask & lensFlareSystem->layerMask) != 0) {
        lensFlareSystem->render();
      }
    }
    Tools::EndPerformanceCounter("Lens flares", !lensFlareSystems.empty());
  }
}

} // end of namespace BABYLON
