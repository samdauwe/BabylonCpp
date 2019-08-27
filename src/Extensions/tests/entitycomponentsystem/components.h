#ifndef ANAX_TESTS_COMPONENTS_H
#define ANAX_TESTS_COMPONENTS_H

#include <string>

#include <babylon/extensions/entitycomponentsystem/component.h>

struct vec3 {
  float x, y, z;
};

struct PositionComponent : BABYLON::Extensions::ECS::Component, vec3 {
};
struct VelocityComponent : BABYLON::Extensions::ECS::Component, vec3 {
};

struct PlayerComponent : BABYLON::Extensions::ECS::Component {
  std::string name;
};

struct NPCComponent : BABYLON::Extensions::ECS::Component {
  std::string name;

  enum Type { MOVING = 0, STILL, MAGICAL } type;
};

#endif // ANAX_TESTS_COMPONENTS_H
