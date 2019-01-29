#include <babylon/extensions/navigation/crowd_simulation.h>

#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/extensions/navigation/crowd_roadmap_vertex.h>
#include <babylon/extensions/navigation/rvo2/rvo_simulator.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {
namespace Extensions {

CrowdSimulation::CrowdSimulation()
    : _simulator{std::make_unique<RVO2::RVOSimulator>()}
    , _crowdCollisionAvoidanceSystem{
        CrowdCollisionAvoidanceSystem(_simulator.get())}
{
  initializeWorld();
  _simulator->setAgentDefaults(15.0f, 10, 5.0f, 5.0f, 2.0f, 2.0f);
}

CrowdSimulation::~CrowdSimulation()
{
}

void CrowdSimulation::initializeWorld()
{
  _world.addSystem(_crowdCollisionAvoidanceSystem);
  _world.addSystem(_crowdMeshUpdaterSystem);
}

void CrowdSimulation::setTimeStep(float timeStep)
{
  _simulator->setTimeStep(timeStep);
}

size_t CrowdSimulation::addAgent(const AbstractMeshPtr& mesh)
{
  BABYLON::Vector2 position(mesh->position().x, mesh->position().z);
  return addAgent(mesh, position);
}

size_t CrowdSimulation::addAgent(const AbstractMeshPtr& mesh,
                                 const Vector2& position)
{
  // Create the crowd agent entity
  auto agent = _world.createEntity();

  // Set the agent's mesh
  auto& agentMesh = agent.addComponent<CrowdMesh>().mesh;
  agentMesh       = mesh;

  // Set the agent's properties
  auto& agentComp = agent.addComponent<CrowdAgent>(_simulator.get(), position);

  // Set the agent radius
  const auto& bbox  = mesh->getBoundingInfo().boundingBox;
  const auto& min   = bbox.minimumWorld;
  const auto& max   = bbox.maximumWorld;
  const auto box    = max.subtract(min).scale(0.5f);
  const auto radius = (box.x + box.z) * 0.5f;
  agentComp.setRadius(radius);

  // Activate and store the crowd agent entity
  agent.activate();
  _agents.emplace_back(agent);

  return agentComp.id();
}

size_t CrowdSimulation::addAgent(const AbstractMeshPtr& mesh,
                                 const Vector3& position)
{
  return addAgent(mesh, Vector2(position.x, position.z));
}

void CrowdSimulation::setAgentGoal(size_t agentId, const BABYLON::Vector2& goal)
{
  _agents[agentId].getComponent<CrowdAgent>().setGoal(goal);
}

void CrowdSimulation::setAgentMaxSpeed(size_t agentId, float speed)
{
  _simulator->setAgentMaxSpeed(agentId, speed);
}

void CrowdSimulation::addObstacleByBoundingBox(const AbstractMeshPtr& mesh,
                                               const Vector3& position,
                                               bool isVisible)
{
  mesh->position  = position;
  mesh->isVisible = isVisible;

  mesh->getBoundingInfo().update(mesh->getWorldMatrix());
  const auto bbox = mesh->getBoundingInfo().boundingBox;
  const auto min  = bbox.minimumWorld;
  const auto max  = bbox.maximumWorld;

  // Add (polygonal) obstacle, specifying the vertices in counterclockwise
  // order.
  const std::vector<RVO2::Vector2> obstacle{
    RVO2::Vector2(max.x, max.z), //
    RVO2::Vector2(min.x, max.z), //
    RVO2::Vector2(min.x, min.z), //
    RVO2::Vector2(max.x, min.z)  //
  };

  _simulator->addObstacle(obstacle);
}

void CrowdSimulation::processObstacles()
{
  _simulator->processObstacles();
}

void CrowdSimulation::addWayPoint(const BABYLON::Vector2& waypoint)
{
  for (auto& agent : _agents) {
    agent.getComponent<CrowdAgent>().addWayPoint(waypoint);
  }
}

void CrowdSimulation::computeRoadMap()
{
  for (auto& agent : _agents) {
    // Check if there is a roadmap configured
    if (!agent.getComponent<CrowdAgent>().hasRoadMap()) {
      continue;
    }

    // Get the agent properties
    auto& roadmap    = agent.getComponent<CrowdAgent>().roadmap();
    auto agentRadius = agent.getComponent<CrowdAgent>().radius();

    // Connect the roadmap vertices by edges if mutually visible.
    for (size_t i = 0; i < roadmap.size(); ++i) {
      for (size_t j = 0; j < roadmap.size(); ++j) {
        if (_simulator->queryVisibility(roadmap[i].position,
                                        roadmap[j].position, agentRadius)) {
          roadmap[i].neighbors.push_back(static_cast<uint32_t>(j));
        }
      }

      // Initialize the distance to each of the four goal vertices at infinity
      // (9e9f).
      roadmap[i].distToGoal.resize(1, 9e9f);
    }

    // Compute the distance to each of the agent's goals (the first vertex)
    // for all vertices using Dijkstra's algorithm.
    const unsigned int i = 0;
    {
      std::multimap<float, unsigned int> Q;
      std::vector<std::multimap<float, unsigned int>::iterator> posInQ(
        roadmap.size(), Q.end());

      roadmap[i].distToGoal[i] = 0.0f;
      posInQ[i]                = Q.insert(std::make_pair(0.0f, i));

      while (!Q.empty()) {
        const auto u = Q.begin()->second;
        Q.erase(Q.begin());
        posInQ[u] = Q.end();

        for (size_t j = 0; j < roadmap[u].neighbors.size(); ++j) {
          const auto v = roadmap[u].neighbors[j];
          const float dist_uv
            = RVO2::abs(roadmap[v].position - roadmap[u].position);

          if (roadmap[v].distToGoal[i] > roadmap[u].distToGoal[i] + dist_uv) {
            roadmap[v].distToGoal[i] = roadmap[u].distToGoal[i] + dist_uv;

            if (posInQ[v] == Q.end()) {
              posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
            }
            else {
              Q.erase(posInQ[v]);
              posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
            }
          }
        }
      }
    }
  }
}

void CrowdSimulation::setPrecision(unsigned int precision)
{
  size_t neighborsMax   = 0;
  float neighborDist    = 0.f;
  float timeHorizon     = 0.f;
  float timeHorizonObst = 0.f;

  switch (precision) {
    case 1:
      // Low precision
      neighborsMax    = 10;
      neighborDist    = 15.f;
      timeHorizon     = 10.f;
      timeHorizonObst = 10.f;
      break;
    case 2:
      // Medium precision
      neighborsMax    = 100;
      neighborDist    = 200.f;
      timeHorizon     = 50.f;
      timeHorizonObst = 30.f;
      break;
    case 3:
      // High precision
      neighborsMax    = 100;
      neighborDist    = 100.f;
      timeHorizon     = 100.f;
      timeHorizonObst = 100.f;
      break;
  }

  for (auto& agent : _agents) {
    auto crowdAgent = agent.getComponent<CrowdAgent>();

    crowdAgent.setAgentMaxNeighbors(neighborsMax);
    crowdAgent.setAgentNeighborDist(neighborDist);
    crowdAgent.setAgentTimeHorizon(timeHorizon);
    crowdAgent.setAgentTimeHorizonObst(timeHorizonObst);
  }
}

bool CrowdSimulation::isRunning() const
{
  // Check if all agents reached their goal
  auto numEntitiesReachedGoal = std::count_if(
    _agents.begin(), _agents.end(), [](const ECS::Entity& entity) {
      return entity.getComponent<CrowdAgent>().reachedGoal();
    });

  return numEntitiesReachedGoal == static_cast<long>(_agents.size());
}

void CrowdSimulation::update()
{
  _world.refresh();
  // if (isRunning()) {
  _crowdCollisionAvoidanceSystem.update();
  _crowdMeshUpdaterSystem.update();
  //}
}

} // end of namespace Extensions
} // end of namespace BABYLON
