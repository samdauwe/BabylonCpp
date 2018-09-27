#include <babylon/physics/physics_impostor_parameters.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

bool PhysicsImpostorParameters::contains(const std::string& key) const
{
  return (key == "mass") || (key == "friction") || (key == "restitution")
         || (key == "ignoreParent") || stl_util::contains(nativeOptions, key);
}

float PhysicsImpostorParameters::operator[](const std::string& key) const
{
  if (key == "mass") {
    return *mass;
  }
  else if (key == "friction") {
    return *friction;
  }
  else if (key == "restitution") {
    return *restitution;
  }
  else if (key == "ignoreParent") {
    return ignoreParent;
  }
  else {
    auto it = nativeOptions.find(key);
    if (it != nativeOptions.end()) {
      return it->second;
    }
  }

  return 0.f;
}

void PhysicsImpostorParameters::setValue(const std::string& key, float value)
{
  if (key == "mass") {
    mass = value;
  }
  else if (key == "friction") {
    friction = value;
  }
  else if (key == "restitution") {
    restitution = value;
  }
  else if (key == "ignoreParent") {
    ignoreParent = static_cast<bool>(value);
  }
  else {
    nativeOptions[key] = value;
  }
}

} // end of namespace BABYLON
