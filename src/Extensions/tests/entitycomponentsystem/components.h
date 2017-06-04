#ifndef ANAX_TESTS_COMPONENTS_H
#define ANAX_TESTS_COMPONENTS_H

#include <string>

#include <babylon/extensions/entitycomponentsystem/component.h>

struct vec3 {
  float x, y, z;
};

using namespace BABYLON::Extensions::ECS;

struct PositionComponent : Component, vec3 {
};
struct VelocityComponent : Component, vec3 {
};

struct PlayerComponent : Component {
  std::string name;
};

struct NPCComponent : Component {
  std::string name;

  enum Type { MOVING = 0, STILL, MAGICAL } type;
};

#endif // ANAX_TESTS_COMPONENTS_H
