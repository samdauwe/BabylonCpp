#include <babylon/extensions/navigation/crowd_collision_avoidance_system.h>

#include <babylon/extensions/navigation/crowd_roadmap_vertex.h>
#include <babylon/extensions/navigation/rvo2/rvo_simulator.h>

namespace BABYLON {
namespace Extensions {

CrowdCollisionAvoidanceSystem::CrowdCollisionAvoidanceSystem(
  RVO2::RVOSimulator* sim)
    : _sim{sim}
{
}

CrowdCollisionAvoidanceSystem::~CrowdCollisionAvoidanceSystem() = default;

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
  const auto& entities = getEntities();
  for (auto& entity : entities) {
    auto agent = entity.getComponent<CrowdAgent>();
    if (!agent.hasRoadMap()) {
      // Set the preferred velocity to be a vector of unit magnitude (speed) in
      // the direction of the goal
      auto goalVector = agent.goal() - agent.position();

      if (RVO2::absSq(goalVector) > 1.f) {
        goalVector = RVO2::normalize(goalVector);
      }

      agent.setAgentPrefVelocity(goalVector);
    }
    else {
      // Set the preferred velocity to be a vector of unit magnitude (speed) in
      // the direction of the visible roadmap vertex that is on the shortest
      // path to the goal.
      const auto& roadmap = agent.roadmap();
      float minDist       = 9e9f;
      int minVertex       = -1;

      for (unsigned int j = 0; j < roadmap.size(); ++j) {
        if (RVO2::abs(roadmap[j].position - agent.position())
                + roadmap[j].distToGoal[0]
              < minDist
            && _sim->queryVisibility(agent.position(), roadmap[j].position,
                                     agent.radius())) {

          minDist = RVO2::abs(roadmap[j].position - agent.position())
                    + roadmap[j].distToGoal[0];
          minVertex = static_cast<int>(j);
        }
      }

      if (minVertex == -1) {
        // No roadmap vertex is visible; should not happen.
        agent.setAgentPrefVelocity(RVO2::Vector2(0, 0));
      }
      else {
        const auto _minVertex = static_cast<size_t>(minVertex);
        if (RVO2::absSq(roadmap[_minVertex].position - agent.position())
            == 0.0f) {
          if (_minVertex == 0) {
            agent.setAgentPrefVelocity(RVO2::Vector2());
          }
          else {
            agent.setAgentPrefVelocity(
              RVO2::normalize(roadmap[0].position - agent.position()));
          }
        }
        else {
          agent.setAgentPrefVelocity(
            RVO2::normalize(roadmap[_minVertex].position - agent.position()));
        }
      }

      // Perturb a little to avoid deadlocks due to perfect symmetry.
      const float angle = std::rand() * 2.0f * Math::PI / RAND_MAX;
      const float dist  = std::rand() * 0.0001f / RAND_MAX;

      agent.setAgentPrefVelocity(
        agent.getAgentPrefVelocity()
        + dist * RVO2::Vector2(std::cos(angle), std::sin(angle)));
    }
  }
}

} // end of namespace Extensions
} // end of namespace BABYLON
