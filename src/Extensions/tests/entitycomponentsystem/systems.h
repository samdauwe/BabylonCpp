#ifndef ANAX_TESTS_SYSTEMS_H
#define ANAX_TESTS_SYSTEMS_H

#include <stdexcept>

#include <babylon/extensions/entitycomponentsystem/system.h>

#include "components.h"

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wheader-hygiene"
#elif defined(__GNUC__) || defined(__GNUG__)
#endif

using namespace BABYLON::Extensions::ECS;

class MovementSystem
  : public System<Requires<PositionComponent, VelocityComponent>> {
public:
  void update()
  {
    auto entities = getEntities();
    for (auto& e : entities) {
      auto& position = e.getComponent<PositionComponent>();
      auto& velocity = e.getComponent<VelocityComponent>();

      position.x += velocity.x;
      position.y += velocity.y;
      position.z += velocity.z;
    }
  }

private:
  virtual void onEntityAdded(Entity& /*e*/) override
  {
  }

  virtual void onEntityRemoved(Entity& /*e*/) override
  {
  }
};

class PlayerSystem
  : public System<Requires<PlayerComponent>, Excludes<NPCComponent>> {
private:
  virtual void onEntityAdded(Entity& e) override
  {
    if (e.hasComponent<NPCComponent>()) {
      throw std::logic_error(
        "Player contains NPCComponent, this goes against the component filter");
    }
  }

  virtual void onEntityRemoved(Entity& /*e*/) override
  {
  }
};

#endif // ANAX_TESTS_SYSTEMS_H
