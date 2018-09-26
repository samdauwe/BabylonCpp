#ifndef BABYLON_EXTENSIONS_NAVIGATION_CROWD_AGENT_H
#define BABYLON_EXTENSIONS_NAVIGATION_CROWD_AGENT_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/entitycomponentsystem/component.h>
#include <babylon/extensions/navigation/crowd_roadmap_vertex.h>
#include <babylon/extensions/navigation/rvo2/vector2.h>
#include <babylon/math/vector2.h>

namespace BABYLON {
namespace Extensions {

struct CrowdRoadmapVertex;

namespace RVO2 {
class RVOSimulator;
}

/**
 * @brief A component used to describe the agent properties of an entity.
 */
class BABYLON_SHARED_EXPORT CrowdAgent : public ECS::Component {

public:
  CrowdAgent(RVO2::RVOSimulator* sim,
             const BABYLON::Vector2& position = BABYLON::Vector2::Zero());
  ~CrowdAgent();

  size_t id() const;
  const RVO2::Vector2& position() const;
  void setRadius(float radius);
  const RVO2::Vector2& goal() const;
  void setGoal(const BABYLON::Vector2& goal);
  float radius() const;
  const RVO2::Vector2& getAgentPrefVelocity() const;
  void setAgentPrefVelocity(const BABYLON::Vector2& goalVector);
  void setAgentPrefVelocity(const RVO2::Vector2& goalVector);
  bool hasRoadMap() const;
  std::vector<CrowdRoadmapVertex>& roadmap();
  const std::vector<CrowdRoadmapVertex>& roadmap() const;
  void addWayPoint(const BABYLON::Vector2& wayPoint);
  void setAgentMaxNeighbors(size_t neighborsMax);
  void setAgentNeighborDist(float neighborDist);
  void setAgentTimeHorizon(float timeHorizon);
  void setAgentTimeHorizonObst(float timeHorizonObst);

  /* Checks if the agent has reached the pre-defined goal. */
  bool reachedGoal() const;

private:
  size_t _id;
  RVO2::Vector2 _goal;
  RVO2::RVOSimulator* _sim;
  // Holds the roadmap
  std::vector<CrowdRoadmapVertex> _roadmap;

}; // end of class CrowdAgent

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NAVIGATION_CROWD_AGENT_H
