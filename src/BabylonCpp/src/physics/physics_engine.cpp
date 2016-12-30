#include <babylon/physics/physics_engine.h>

#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/joint/physics_joint.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/physics/physics_impostor_joint.h>

namespace BABYLON {

PhysicsEngine::PhysicsEngine(const Vector3& _gravity,
                             IPhysicsEnginePlugin* physicsPlugin)
    : _initialized{false}, _physicsPlugin{physicsPlugin}
{
  if (_physicsPlugin && _physicsPlugin->isSupported()) {
    setGravity(_gravity);
    setTimeStep();
    _initialized = true;
  }
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::setGravity(const Vector3& _gravity)
{
  gravity = _gravity;
  _physicsPlugin->setGravity(gravity);
}

void PhysicsEngine::setTimeStep(float newTimeStep)
{
  _physicsPlugin->setTimeStep(newTimeStep);
}

void PhysicsEngine::dispose(bool /*doNotRecurse*/)
{
  for (auto& impostor : _impostors) {
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
  impostor->uniqueId = _impostors.size();
  _impostors.emplace_back(impostor);
  // if no parent, generate the body
  if (!impostor->parent()) {
    _physicsPlugin->generatePhysicsBody(impostor);
  }
}

void PhysicsEngine::removeImpostor(PhysicsImpostor* impostor)
{
  auto it = std::find(_impostors.begin(), _impostors.end(), impostor);
  if (it != _impostors.end()) {
    // this will also remove it from the world.
    (*it)->setPhysicsBody(nullptr);
    _impostors.erase(it);
  }
}

void PhysicsEngine::addJoint(PhysicsImpostor* mainImpostor,
                             PhysicsImpostor* connectedImpostor,
                             PhysicsJoint* joint)
{
  PhysicsImpostorJoint* impostorJoint = new PhysicsImpostorJoint();
  impostorJoint->mainImpostor         = mainImpostor;
  impostorJoint->connectedImpostor    = connectedImpostor;
  impostorJoint->joint                = joint;

  joint->setPhysicsPlugin(_physicsPlugin);
  _joints.emplace_back(impostorJoint);
  _physicsPlugin->generateJoint(impostorJoint);
}

void PhysicsEngine::removeJoint(PhysicsImpostor* mainImpostor,
                                PhysicsImpostor* connectedImpostor,
                                PhysicsJoint* joint)
{
  std::vector<PhysicsImpostorJoint*> matchingJoints(_joints.size());

  auto it = std::copy_if(
    _joints.begin(), _joints.end(), matchingJoints.begin(),
    [&connectedImpostor, &joint,
     &mainImpostor](const PhysicsImpostorJoint* impostorJoint) {
      return (impostorJoint->connectedImpostor == connectedImpostor
              && impostorJoint->joint == joint
              && impostorJoint->mainImpostor == mainImpostor);
    });

  // shrink container to new size
  matchingJoints.resize(
    static_cast<size_t>(std::distance(matchingJoints.begin(), it)));

  if (!matchingJoints.empty()) {
    _physicsPlugin->removeJoint(matchingJoints[0]);
  }
}

void PhysicsEngine::_step(float delta)
{
  // check if any mesh has no body / requires an update
  for (auto& impostor : _impostors) {
    if (impostor->isBodyInitRequired()) {
      _physicsPlugin->generatePhysicsBody(impostor);
    }
  }

  if (delta > 0.1f) {
    delta = 0.1f;
  }
  else if (delta <= 0.f) {
    delta = 1.f / 60.f;
  }

  _physicsPlugin->executeStep(delta, _impostors);
}

IPhysicsEnginePlugin* PhysicsEngine::getPhysicsPlugin()
{
  return _physicsPlugin;
}

PhysicsImpostor*
PhysicsEngine::getImpostorForPhysicsObject(IPhysicsEnabledObject* object)
{
  auto it = std::find_if(_impostors.begin(), _impostors.end(),
                         [&object](PhysicsImpostor* impostor) {
                           return impostor->object == object;
                         });
  return (it == _impostors.end()) ? nullptr : (*it);
}

PhysicsImpostor* PhysicsEngine::getImpostorWithPhysicsBody(AbstractMesh* body)
{
  auto it = std::find_if(_impostors.begin(), _impostors.end(),
                         [&body](PhysicsImpostor* impostor) {
                           return impostor->physicsBody() == body;
                         });
  return (it == _impostors.end()) ? nullptr : (*it);
}

bool PhysicsEngine::isInitialized() const
{
  return _initialized;
}

} // end of namespace BABYLON
