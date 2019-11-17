#include <babylon/physics/physics_engine.h>

#include <babylon/core/logging.h>
#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/joint/physics_joint.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/physics/physics_impostor_joint.h>
#include <babylon/physics/physics_raycast_result.h>

namespace BABYLON {

float PhysicsEngine::Epsilon = 0.001f;

PhysicsEngine::PhysicsEngine(const std::optional<Vector3>& iGravity,
                             IPhysicsEnginePlugin* physicsPlugin)
    : _initialized{false}, _physicsPlugin{physicsPlugin}
{
  if (_physicsPlugin && _physicsPlugin->isSupported()) {
    setGravity(iGravity.has_value() ? *iGravity : Vector3(0.f, -9.807f, 0.f));
    setTimeStep();
    _initialized = true;
  }
  else {
    if (_physicsPlugin) {
      BABYLON_LOGF_ERROR(
          "PhysicsEngine",
          "Physics Engine %s cannot be found. Please make sure it is included.",
          _physicsPlugin->name.c_str());
    }
  }
}

PhysicsEngine::~PhysicsEngine() = default;

void PhysicsEngine::setGravity(const Vector3& _gravity)
{
  gravity = _gravity;
  _physicsPlugin->setGravity(gravity);
}

void PhysicsEngine::setTimeStep(float newTimeStep)
{
  _physicsPlugin->setTimeStep(newTimeStep);
}

float PhysicsEngine::getTimeStep() const
{
  return _physicsPlugin->getTimeStep();
}

void PhysicsEngine::dispose()
{
  for (const auto& impostor : _impostors) {
    impostor->dispose();
  }
  _physicsPlugin->dispose();
}

std::string PhysicsEngine::getPhysicsPluginName() const
{
  return _physicsPlugin->name;
}

void PhysicsEngine::addImpostor(PhysicsImpostor* impostor)
{
  _impostors.emplace_back(impostor);
  impostor->uniqueId = _impostors.size();
  // if no parent, generate the body
  if (!impostor->parent()) {
    _physicsPlugin->generatePhysicsBody(*impostor);
  }
}

void PhysicsEngine::removeImpostor(PhysicsImpostor* impostor)
{
  auto it = std::find_if(_impostors.begin(), _impostors.end(),
                         [&impostor](const PhysicsImpostorPtr& _imposter) {
                           return _imposter.get() == impostor;
                         });
  if (it != _impostors.end()) {
    _impostors.erase(it);
    getPhysicsPlugin()->removePhysicsBody(*impostor);
  }
}

void PhysicsEngine::addJoint(PhysicsImpostor* mainImpostor,
                             PhysicsImpostor* connectedImpostor,
                             const std::shared_ptr<PhysicsJoint>& joint)
{
  auto impostorJoint               = std::make_shared<PhysicsImpostorJoint>();
  impostorJoint->mainImpostor      = mainImpostor;
  impostorJoint->connectedImpostor = connectedImpostor;
  impostorJoint->joint             = joint;

  joint->physicsPlugin = _physicsPlugin;
  _joints.emplace_back(impostorJoint);
  _physicsPlugin->generateJoint(impostorJoint.get());
}

void PhysicsEngine::removeJoint(PhysicsImpostor* mainImpostor,
                                PhysicsImpostor* connectedImpostor,
                                PhysicsJoint* joint)
{
  std::vector<PhysicsImpostorJointPtr> matchingJoints(_joints.size());

  auto it = std::copy_if(
    _joints.begin(), _joints.end(), matchingJoints.begin(),
    [&connectedImpostor, &joint,
     &mainImpostor](const PhysicsImpostorJointPtr& impostorJoint) {
      return (impostorJoint->connectedImpostor == connectedImpostor
              && impostorJoint->joint.get() == joint
              && impostorJoint->mainImpostor == mainImpostor);
    });

  // shrink container to new size
  matchingJoints.resize(
    static_cast<size_t>(std::distance(matchingJoints.begin(), it)));

  if (!matchingJoints.empty()) {
    _physicsPlugin->removeJoint(matchingJoints[0].get());
  }
}

void PhysicsEngine::_step(float delta)
{
  // check if any mesh has no body / requires an update
  for (auto& impostor : _impostors) {
    if (impostor->isBodyInitRequired()) {
      _physicsPlugin->generatePhysicsBody(*impostor);
    }
  }

  if (delta > 0.1f) {
    delta = 0.1f;
  }
  else if (delta <= 0.f) {
    delta = 1.f / 60.f;
  }

  // TODO FIXME
  // _physicsPlugin->executeStep(delta, _impostors);
}

IPhysicsEnginePlugin* PhysicsEngine::getPhysicsPlugin()
{
  return _physicsPlugin;
}

std::vector<PhysicsImpostorPtr>& PhysicsEngine::getImpostors()
{
  return _impostors;
}

PhysicsImpostor*
PhysicsEngine::getImpostorForPhysicsObject(IPhysicsEnabledObject* object)
{
  auto it = std::find_if(_impostors.begin(), _impostors.end(),
                         [&object](const PhysicsImpostorPtr& impostor) {
                           return impostor->object == object;
                         });
  return (it == _impostors.end()) ? nullptr : (*it).get();
}

PhysicsImpostor* PhysicsEngine::getImpostorWithPhysicsBody(IPhysicsBody* body)
{
  auto it = std::find_if(_impostors.begin(), _impostors.end(),
                         [&body](const PhysicsImpostorPtr& impostor) {
                           return impostor->physicsBody() == body;
                         });
  return (it == _impostors.end()) ? nullptr : (*it).get();
}

bool PhysicsEngine::isInitialized() const
{
  return _initialized;
}

PhysicsRaycastResult PhysicsEngine::raycast(const Vector3& from,
                                            const Vector3& to)
{
  return _physicsPlugin->raycast(from, to);
}

} // end of namespace BABYLON
