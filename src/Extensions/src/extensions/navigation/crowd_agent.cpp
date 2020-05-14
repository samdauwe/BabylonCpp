#include <babylon/extensions/navigation/crowd_agent.h>

#include <babylon/extensions/navigation/rvo2/rvo_simulator.h>

namespace BABYLON {
namespace Extensions {

CrowdAgent::CrowdAgent(RVO2::RVOSimulator* sim, const BABYLON::Vector2& position)
    : _goal{RVO2::Vector2(0.f, 0.f)}, _sim{sim}
{
  _id = _sim->addAgent(RVO2::Vector2(position.x, position.y));
}

CrowdAgent::~CrowdAgent() = default;

size_t CrowdAgent::id() const
{
  return _id;
}

const RVO2::Vector2& CrowdAgent::position() const
{
  return _sim->getAgentPosition(_id);
}

void CrowdAgent::setRadius(float radius)
{
  _sim->setAgentRadius(_id, radius);
}

const RVO2::Vector2& CrowdAgent::goal() const
{
  return _goal;
}

void CrowdAgent::setGoal(const BABYLON::Vector2& goal)
{
  _goal = RVO2::Vector2(goal.x, goal.y);
}

float CrowdAgent::radius() const
{
  return _sim->getAgentRadius(_id);
}

const RVO2::Vector2& CrowdAgent::getAgentPrefVelocity() const
{
  return _sim->getAgentPrefVelocity(_id);
}

void CrowdAgent::setAgentPrefVelocity(const BABYLON::Vector2& goalVector)
{
  _sim->setAgentPrefVelocity(_id, RVO2::Vector2(goalVector.x, goalVector.y));
}

void CrowdAgent::setAgentPrefVelocity(const RVO2::Vector2& goalVector)
{
  _sim->setAgentPrefVelocity(_id, goalVector);
}

bool CrowdAgent::hasRoadMap() const
{
  return _roadmap.size() > 1;
}

std::vector<CrowdRoadmapVertex>& CrowdAgent::roadmap()
{
  return _roadmap;
}

const std::vector<CrowdRoadmapVertex>& CrowdAgent::roadmap() const
{
  return _roadmap;
}

void CrowdAgent::addWayPoint(const BABYLON::Vector2& wayPoint)
{
  if (_roadmap.empty()) {
    // Add the goal positions of the agent
    CrowdRoadmapVertex p;
    p.position = _goal;
    _roadmap.emplace_back(p);
  }

  // Add waypoint to the roadmap vertices
  CrowdRoadmapVertex p;
  p.position = RVO2::Vector2(wayPoint.x, wayPoint.y);
  _roadmap.emplace_back(p);
}

void CrowdAgent::setAgentMaxNeighbors(size_t neighborsMax)
{
  _sim->setAgentMaxNeighbors(_id, neighborsMax);
}

void CrowdAgent::setAgentNeighborDist(float neighborDist)
{
  _sim->setAgentNeighborDist(_id, neighborDist);
}

void CrowdAgent::setAgentTimeHorizon(float timeHorizon)
{
  _sim->setAgentTimeHorizon(_id, timeHorizon);
}

void CrowdAgent::setAgentTimeHorizonObst(float timeHorizonObst)
{
  _sim->setAgentTimeHorizonObst(_id, timeHorizonObst);
}

bool CrowdAgent::reachedGoal() const
{
  const float agentRadius = _sim->getAgentRadius(_id);
  return RVO2::absSq(_sim->getAgentPosition(_id) - _goal) > (agentRadius * agentRadius);
}

} // end of namespace Extensions
} // end of namespace BABYLON
