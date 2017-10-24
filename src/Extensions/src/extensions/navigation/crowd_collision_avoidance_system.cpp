#include <babylon/extensions/navigation/crowd_collision_avoidance_system.h>

#include <babylon/extensions/navigation/rvo2/rvo_simulator.h>

namespace BABYLON {
namespace Extensions {

CrowdCollisionAvoidanceSystem::CrowdCollisionAvoidanceSystem(
  RVO2::RVOSimulator* sim)
    : _sim{sim}
{
}

CrowdCollisionAvoidanceSystem::~CrowdCollisionAvoidanceSystem()
{
}

void CrowdCollisionAvoidanceSystem::update()
{
  // Setting the preferred velocity to be a vector of unit magnitude (speed) in
  // the direction of the goal.
  setPreferredVelocities();
  // Perform a simulation step and updates the two-dimensional position and
  // two-dimensional velocity of each agent.
  _sim->doStep();
}

void CrowdCollisionAvoidanceSystem::setPreferredVelocities()
{
  auto entities = getEntities();
  for (auto& entity : entities) {
    auto agent      = entity.getComponent<CrowdAgent>();
    auto goalVector = agent.goal() - agent.position();

    if (RVO2::absSq(goalVector) > 1.f) {
      goalVector = RVO2::normalize(goalVector);
    }

    agent.setAgentPrefVelocity(goalVector);
  }
}

} // end of namespace Extensions
} // end of namespace BABYLON
