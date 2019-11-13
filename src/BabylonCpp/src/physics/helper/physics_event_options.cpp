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
  const PhysicsRadialExplosionEventOptions& other) = default;

PhysicsRadialExplosionEventOptions::PhysicsRadialExplosionEventOptions(
  PhysicsRadialExplosionEventOptions&& other) = default;

PhysicsRadialExplosionEventOptions& PhysicsRadialExplosionEventOptions::
operator=(const PhysicsRadialExplosionEventOptions& other) = default;

PhysicsRadialExplosionEventOptions& PhysicsRadialExplosionEventOptions::
operator=(PhysicsRadialExplosionEventOptions&& other) = default;

PhysicsRadialExplosionEventOptions::~PhysicsRadialExplosionEventOptions() = default;

} // end of namespace BABYLON
