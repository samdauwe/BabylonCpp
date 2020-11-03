#include <babylon/rendering/sub_surface_scene_component.h>

#include <babylon/engines/scene.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

SubSurfaceSceneComponent::SubSurfaceSceneComponent(Scene* iScene) : scene{iScene}
{
}

SubSurfaceSceneComponent::~SubSurfaceSceneComponent() = default;

void SubSurfaceSceneComponent::_register()
{
}

void SubSurfaceSceneComponent::serialize(json& /*serializationObject*/)
{
}

void SubSurfaceSceneComponent::addFromContainer(AbstractScene& /*container*/)
{
  // Nothing to do
}

void SubSurfaceSceneComponent::removeFromContainer(AbstractScene& /*container*/, bool /*dispose*/)
{
  // Make sure nothing will be serialized
  if (!scene->prePassRenderer()) {
    return;
  }

  if (scene->subSurfaceConfiguration()) {
    scene->subSurfaceConfiguration()->clearAllDiffusionProfiles();
  }
}

void SubSurfaceSceneComponent::rebuild()
{
  // Nothing to do for this component
}

void SubSurfaceSceneComponent::dispose()
{
  // Nothing to do for this component
}

} // end of namespace BABYLON
