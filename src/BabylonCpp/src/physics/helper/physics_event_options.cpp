#include <babylon/physics/helper/physics_event_options.h>

namespace BABYLON {

//------------------------------------------------------------------------------
// Physics radial explosion event options
//------------------------------------------------------------------------------

PhysicsRadialExplosionEventOptions::PhysicsRadialExplosionEventOptions()
    : radius{5.f}
    , strength{10.f}
    , falloff{PhysicsRadialImpulseFalloff::Constant}
    , affectedImpostorsCallback{nullptr}
{
  sphere.segments = 32;
  sphere.diameter = 1.f;
}

PhysicsRadialExplosionEventOptions::PhysicsRadialExplosionEventOptions(
  const PhysicsRadialExplosionEventOptions& other)
    : radius{other.radius}
    , strength{other.strength}
    , falloff{other.falloff}
    , sphere{other.sphere}
    , affectedImpostorsCallback{other.affectedImpostorsCallback}
{
}

PhysicsRadialExplosionEventOptions::PhysicsRadialExplosionEventOptions(
  PhysicsRadialExplosionEventOptions&& other)
    : radius{std::move(other.radius)}
    , strength{std::move(other.strength)}
    , falloff{std::move(other.falloff)}
    , sphere{std::move(other.sphere)}
    , affectedImpostorsCallback{std::move(other.affectedImpostorsCallback)}
{
}

PhysicsRadialExplosionEventOptions& PhysicsRadialExplosionEventOptions::
operator=(const PhysicsRadialExplosionEventOptions& other)
{
  if (&other != this) {
    radius                    = other.radius;
    strength                  = other.strength;
    falloff                   = other.falloff;
    sphere                    = other.sphere;
    affectedImpostorsCallback = other.affectedImpostorsCallback;
  }

  return *this;
}

PhysicsRadialExplosionEventOptions& PhysicsRadialExplosionEventOptions::
operator=(PhysicsRadialExplosionEventOptions&& other)
{
  if (&other != this) {
    radius                    = std::move(other.radius);
    strength                  = std::move(other.strength);
    falloff                   = std::move(other.falloff);
    sphere                    = std::move(other.sphere);
    affectedImpostorsCallback = std::move(other.affectedImpostorsCallback);
  }

  return *this;
}

PhysicsRadialExplosionEventOptions::~PhysicsRadialExplosionEventOptions()
{
}

} // end of namespace BABYLON
