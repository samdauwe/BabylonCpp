#include <babylon/physics/physics_engine_component.h>

#include <babylon/engine/scene.h>
#include <babylon/physics/physics_engine.h>

namespace BABYLON {

PhysicsEngineSceneComponent::PhysicsEngineSceneComponent(Scene* iScene)
{
  ISceneComponent::name = PhysicsEngineSceneComponent::name;
  scene                 = iScene;

  // Replace the function used to get the deterministic frame time
  scene->getDeterministicFrameTime = [this]() -> float {
    if (scene->_physicsEngine) {
      return scene->_physicsEngine->getTimeStep() * 1000.f;
    }

    return 1000.f / 60.f;
  };
}

PhysicsEngineSceneComponent::~PhysicsEngineSceneComponent()
{
}

void PhysicsEngineSceneComponent::_register()
{
}

void PhysicsEngineSceneComponent::rebuild()
{
  // Nothing to do for this component
}

void PhysicsEngineSceneComponent::dispose()
{
  scene->onBeforePhysicsObservable.clear();
  scene->onAfterPhysicsObservable.clear();

  if (scene->_physicsEngine) {
    scene->disablePhysicsEngine();
  }
}

} // end of namespace BABYLON
