#include <babylon/physics/physics_impostor_parameters.h>

namespace BABYLON {

bool PhysicsImpostorParameters::contains(const std::string& key) const
{
  return (key == "mass") || (key == "friction") || (key == "restitution")
         || std_util::contains(nativeOptions, key);
}

float PhysicsImpostorParameters::operator[](const std::string& key) const
{
  if (key == "mass") {
    return mass;
  }
  else if (key == "friction") {
    return friction;
  }
  else if (key == "restitution") {
    return restitution;
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
  else {
    nativeOptions[key] = value;
  }
}

} // end of namespace BABYLON
