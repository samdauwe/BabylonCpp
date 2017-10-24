#include <babylon/extensions/navigation/crowd_simulation.h>

#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/extensions/navigation/rvo2/rvo_simulator.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {
namespace Extensions {

CrowdSimulation::CrowdSimulation()
    : _simulator{::std::make_unique<RVO2::RVOSimulator>()}
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

size_t CrowdSimulation::addAgent(AbstractMesh* mesh)
{
  BABYLON::Vector2 position(mesh->position().x, mesh->position().z);
  return addAgent(mesh, position);
}

size_t CrowdSimulation::addAgent(AbstractMesh* mesh,
                                 const BABYLON::Vector2& position)
{
  // Create the crowd agent entity
  auto agent = _world.createEntity();

  // Set the agent's mesh
  auto& agentMesh = agent.addComponent<CrowdMesh>().mesh;
  agentMesh       = mesh;

  // Set the agent's properties
  auto& agentComp = agent.addComponent<CrowdAgent>(_simulator.get(), position);

  // Set the agent radius
  const auto& bbox  = mesh->getBoundingInfo()->boundingBox;
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

void CrowdSimulation::setAgentGoal(size_t agentId, const BABYLON::Vector2& goal)
{
  _agents[agentId].getComponent<CrowdAgent>().setGoal(goal);
}

void CrowdSimulation::setAgentMaxSpeed(size_t agentId, float speed)
{
  _simulator->setAgentMaxSpeed(agentId, speed);
}

void CrowdSimulation::processObstacles()
{
  _simulator->processObstacles();
}

void CrowdSimulation::computeRoadMap()
{
}

bool CrowdSimulation::isRunning() const
{
  // Check if all agents reached their goal
  long numEntitiesReachedGoal = std::count_if(
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
