#ifndef BABYLON_EXTENSIONS_NAVIGATION_CROWD_COLLISION_AVOIDANCE_SYSTEM_H
#define BABYLON_EXTENSIONS_NAVIGATION_CROWD_COLLISION_AVOIDANCE_SYSTEM_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/entitycomponentsystem/system.h>
#include <babylon/extensions/navigation/crowd_agent.h>

namespace BABYLON {
namespace Extensions {

namespace RVO2 {
class RVOSimulator;
}

class BABYLON_SHARED_EXPORT CrowdCollisionAvoidanceSystem
    : public ECS::System<ECS::Requires<CrowdAgent>> {

public:
  CrowdCollisionAvoidanceSystem(RVO2::RVOSimulator* sim);
  ~CrowdCollisionAvoidanceSystem() override; // = default

  void update();

private:
  /**
   * Updates the collision avoidance system by setting the preferred velocity to
   * be a vector of unit magnitude (speed) in the direction of the goal.
   */
  void setPreferredVelocities();

private:
  RVO2::RVOSimulator* _sim;

}; // end of class CrowdCollisionAvoidanceSystem

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_NAVIGATION_CROWD_COLLISION_AVOIDANCE_SYSTEM_H
