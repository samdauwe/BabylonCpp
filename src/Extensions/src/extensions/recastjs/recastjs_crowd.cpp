#include <babylon/extensions/recastjs/recastjs_crowd.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/extensions/recastjs/recastjs.h>
#include <babylon/extensions/recastjs/recastjs_plugin.h>
#include <babylon/maths/vector3.h>
#include <babylon/meshes/transform_node.h>
#include <babylon/navigation/iagent_parameters.h>

namespace BABYLON {
namespace Extensions {

RecastJSCrowd::RecastJSCrowd(RecastJSPlugin* plugin, size_t maxAgents, float maxAgentRadius,
                             Scene* scene)
    : ICrowd{}
{
  bjsRECASTPlugin = plugin;
  recastCrowd     = std::make_unique<Crowd>(static_cast<int>(maxAgents), maxAgentRadius,
                                        bjsRECASTPlugin->navMesh->getNavMesh());
  _scene          = scene;

  _onBeforeAnimationsObserver
    = scene->onBeforeAnimationsObservable.add([this](Scene* scene, EventState & /*es*/) -> void {
        update(scene->getEngine()->getDeltaTime() * 0.001f);
      });
}

RecastJSCrowd::~RecastJSCrowd()
{
  dispose();
}

int RecastJSCrowd::addAgent(const Vector3& pos, const IAgentParameters& parameters,
                            const TransformNodePtr& transform)
{
  dtCrowdAgentParams agentParams;
  agentParams.radius                = parameters.radius;
  agentParams.height                = parameters.height;
  agentParams.maxAcceleration       = parameters.maxAcceleration;
  agentParams.maxSpeed              = parameters.maxSpeed;
  agentParams.collisionQueryRange   = parameters.collisionQueryRange;
  agentParams.pathOptimizationRange = parameters.pathOptimizationRange;
  agentParams.separationWeight      = parameters.separationWeight;
  agentParams.updateFlags           = 7;
  agentParams.obstacleAvoidanceType = 0;
  agentParams.queryFilterType       = 0;
  agentParams.userData              = nullptr;

  auto agentIndex = recastCrowd->addAgent(Vec3(pos.x, pos.y, pos.z), &agentParams);
  transforms.emplace_back(transform);
  agents.emplace_back(agentIndex);
  return agentIndex;
}

Vector3 RecastJSCrowd::getAgentPosition(int index)
{
  const auto agentPos = recastCrowd->getAgentPosition(index);
  return Vector3(agentPos.x, agentPos.y, agentPos.z);
}

Vector3 RecastJSCrowd::getAgentVelocity(int index)
{
  const auto agentVel = recastCrowd->getAgentVelocity(index);
  return Vector3(agentVel.x, agentVel.y, agentVel.z);
}

void RecastJSCrowd::agentGoto(int index, const Vector3& destination)
{
  recastCrowd->agentGoto(index, Vec3(destination.x, destination.y, destination.z));
}

void RecastJSCrowd::agentTeleport(int index, const Vector3& destination)
{
  recastCrowd->agentTeleport(index, Vec3(destination.x, destination.y, destination.z));
}

void RecastJSCrowd::updateAgentParameters(int index, const IAgentParameters& parameters)
{
  auto agentParams = recastCrowd->getAgentParameters(index);

  if (parameters.radius >= 0.f) {
    agentParams.radius = parameters.radius;
  }
  if (parameters.height > 0.f) {
    agentParams.height = parameters.height;
  }
  if (parameters.maxAcceleration >= 0.f) {
    agentParams.maxAcceleration = parameters.maxAcceleration;
  }
  if (parameters.maxSpeed >= 0.f) {
    agentParams.maxSpeed = parameters.maxSpeed;
  }
  if (parameters.collisionQueryRange > 0.f) {
    agentParams.collisionQueryRange = parameters.collisionQueryRange;
  }
  if (parameters.pathOptimizationRange > 0.f) {
    agentParams.pathOptimizationRange = parameters.pathOptimizationRange;
  }
  if (parameters.separationWeight >= 0.f) {
    agentParams.separationWeight = parameters.separationWeight;
  }

  recastCrowd->setAgentParameters(index, &agentParams);
}

void RecastJSCrowd::removeAgent(int index)
{
  recastCrowd->removeAgent(index);

  auto item = stl_util::index_of(agents, index);
  if (item > -1) {
    stl_util::splice(agents, item, 1);
    stl_util::splice(transforms, item, 1);
  }
}

std::vector<int>& RecastJSCrowd::getAgents()
{
  return agents;
}

void RecastJSCrowd::update(float deltaTime)
{
  // update crowd
  recastCrowd->update(deltaTime);

  // update transforms
  for (size_t index = 0; index < agents.size(); index++) {
    transforms[index]->position = getAgentPosition(agents[index]);
  }
}

void RecastJSCrowd::setDefaultQueryExtent(const Vector3& extent)
{
  Vec3 ext(extent.x, extent.y, extent.z);
  recastCrowd->setDefaultQueryExtent(ext);
}

Vector3 RecastJSCrowd::getDefaultQueryExtent() const
{
  const auto p = recastCrowd->getDefaultQueryExtent();
  return Vector3(p.x, p.y, p.z);
}

void RecastJSCrowd::dispose()
{
  recastCrowd->destroy();
  // _scene->onBeforeAnimationsObservable.remove(_onBeforeAnimationsObserver);
  // _onBeforeAnimationsObserver = nullptr;
}

} // end of namespace Extensions
} // end of namespace BABYLON
