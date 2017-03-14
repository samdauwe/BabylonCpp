#include <babylon/physics/plugins/oimo_physics_engine_plugin.h>

#include <babylon/core/logging.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/physics/iphysics_body.h>
#include <babylon/physics/physics_impostor.h>
#include <oimo/collision/broadphase/broad_phase.h>
#include <oimo/constraint/contact/contact.h>
#include <oimo/dynamics/world.h>
#include <oimo/math/vec3.h>

namespace BABYLON {

OimoPhysicsEnginePlugin::OimoPhysicsEnginePlugin(unsigned int iterations)
    : world{new OIMO::World(1.f / 60.f,
                            OIMO::BroadPhase::Type::BR_BOUNDING_VOLUME_TREE,
                            iterations, true)}
    , _tmpPositionVector{Vector3::Zero()}
{
  world->clear();
  // making sure no stats are calculated
  world->isNoStat = true;
}

OimoPhysicsEnginePlugin::~OimoPhysicsEnginePlugin()
{
}

void OimoPhysicsEnginePlugin::setGravity(const Vector3& gravity)
{
  world->gravity.copy(OIMO::Vec3(gravity.x, gravity.y, gravity.z));
}

void OimoPhysicsEnginePlugin::setTimeStep(float timeStep)
{
  world->timeStep = timeStep;
}

void OimoPhysicsEnginePlugin::executeStep(
  float /*delta*/, const std::vector<PhysicsImpostor*>& /*impostors*/)
{
#if 0
  for (auto& impostor : impostors) {
    impostor->beforeStep();
  }

  world->step();

  for (auto& impostor : impostors) {
    impostor->afterStep();
    // update the ordered impostors array
    _tmpImpostorsArray[std::to_string(impostor->uniqueId)] = impostor;
  }

  // check for collisions
  OIMO::Contact* contact = world->contacts;

  while (contact != nullptr) {
    if (contact->touching && !contact->body1->sleeping
        && !contact->body2->sleeping) {
      contact = contact->next;
      continue;
    }
    // is this body colliding with any other? get the impostor
    PhysicsImpostor* mainImpostor
      = (std::find(_tmpImpostorsArray.begin(), _tmpImpostorsArray.end(),
                   contact->body1->name)
         != _tmpImpostorsArray.end()) ?
          _tmpImpostorsArray[contact->body1->name] :
          nullptr;
    PhysicsImpostor* collidingImpostor
      = (std::find(_tmpImpostorsArray.begin(), _tmpImpostorsArray.end(),
                   contact->body2->name)
         != _tmpImpostorsArray.end()) ?
          _tmpImpostorsArray[contact->body2->name] :
          nullptr;

    if (!mainImpostor || !collidingImpostor) {
      contact = contact->next;
      continue;
    }

    mainImpostor->onCollide(collidingImpostor->physicsBody());
    collidingImpostor->onCollide(mainImpostor->physicsBody());
    contact = contact->next;
  }
#endif
}

void OimoPhysicsEnginePlugin::applyImpulse(PhysicsImpostor* impostor,
                                           const Vector3& force,
                                           const Vector3& contactPoint)
{
  auto mass = impostor->physicsBody()->getMass();
  impostor->physicsBody()->applyImpulse(
    contactPoint.scale(OIMO::World::INV_SCALE),
    force.scale(OIMO::World::INV_SCALE * mass));
}

void OimoPhysicsEnginePlugin::applyForce(PhysicsImpostor* impostor,
                                         const Vector3& force,
                                         const Vector3& contactPoint)
{
  BABYLON_LOG_WARN(
    "OimoPhysicsEnginePlugin",
    "Oimo doesn't support applying force. Using impule instead.");
  applyImpulse(impostor, force, contactPoint);
}

void OimoPhysicsEnginePlugin::generatePhysicsBody(PhysicsImpostor* /*impostor*/)
{
}

void OimoPhysicsEnginePlugin::removePhysicsBody(PhysicsImpostor* /*impostor*/)
{
}

void OimoPhysicsEnginePlugin::generateJoint(
  PhysicsImpostorJoint* /*impostorJoint*/)
{
}

void OimoPhysicsEnginePlugin::removeJoint(
  PhysicsImpostorJoint* /*impostorJoint*/)
{
}

bool OimoPhysicsEnginePlugin::isSupported() const
{
  return false;
}

void OimoPhysicsEnginePlugin::setTransformationFromPhysicsBody(
  PhysicsImpostor* /*impostor*/)
{
}

void OimoPhysicsEnginePlugin::setPhysicsBodyTransformation(
  PhysicsImpostor* /*impostor*/, const Vector3& /*newPosition*/,
  const Quaternion& /*newRotation*/)
{
}

OIMO::Shape* OimoPhysicsEnginePlugin::getLastShape(OIMO::Body* /*body*/)
{
  return nullptr;
}

void OimoPhysicsEnginePlugin::setLinearVelocity(PhysicsImpostor* /*impostor*/,
                                                const Vector3& /*velocity*/)
{
}

Vector3
OimoPhysicsEnginePlugin::getLinearVelocity(PhysicsImpostor* /*impostor*/)
{
  return Vector3();
}

Vector3
OimoPhysicsEnginePlugin::getAngularVelocity(PhysicsImpostor* /*impostor*/)
{
  return Vector3();
}

void OimoPhysicsEnginePlugin::setBodyMass(PhysicsImpostor* /*impostor*/,
                                          float /*mass*/)
{
}

void OimoPhysicsEnginePlugin::sleepBody(PhysicsImpostor* /*impostor*/)
{
}

void OimoPhysicsEnginePlugin::wakeUpBody(PhysicsImpostor* /*impostor*/)
{
}

void OimoPhysicsEnginePlugin::updateDistanceJoint(IMotorEnabledJoint* /*joint*/,
                                                  float /*maxDistance*/,
                                                  float /*minDistance*/)
{
}

void OimoPhysicsEnginePlugin::setMotor(IMotorEnabledJoint* /*joint*/,
                                       float /*speed*/, float /*maxForce*/,
                                       unsigned int /*motorIndex*/)
{
}

void OimoPhysicsEnginePlugin::setLimit(IMotorEnabledJoint* /*joint*/,
                                       float /*upperLimit*/,
                                       float /*lowerLimit*/,
                                       unsigned int /*motorIndex*/)
{
}

void OimoPhysicsEnginePlugin::dispose()
{
  world->clear();
}

} // end of namespace BABYLON
