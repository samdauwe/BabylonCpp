#include <babylon/physics/plugins/oimo_physics_engine_plugin.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/physics/iphysics_body.h>
#include <babylon/physics/iworld.h>
#include <babylon/physics/joint/physics_joint.h>
#include <babylon/physics/physics_impostor.h>
#include <babylon/physics/physics_impostor_joint.h>
#include <oimo/collision/broadphase/broad_phase.h>
#include <oimo/collision/shape/shape.h>
#include <oimo/constraint/contact/contact.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/dynamics/world.h>
#include <oimo/math/vec3.h>

namespace BABYLON {

OimoPhysicsEnginePlugin::OimoPhysicsEnginePlugin(unsigned int iterations)
    : world{nullptr}, _tmpPositionVector{Vector3::Zero()}
{
  world->create(1.f / 60.f, static_cast<unsigned int>(
                              OIMO::BroadPhase::Type::BR_BOUNDING_VOLUME_TREE),
                iterations, true);
  world->clear();
  // making sure no stats are calculated
  world->setNoStat(true);
}

OimoPhysicsEnginePlugin::~OimoPhysicsEnginePlugin()
{
}

void OimoPhysicsEnginePlugin::setGravity(const Vector3& gravity)
{
  world->setGravity(gravity);
}

void OimoPhysicsEnginePlugin::setTimeStep(float timeStep)
{
  world->setTimeStep(timeStep);
}

void OimoPhysicsEnginePlugin::executeStep(
  float /*delta*/, const std::vector<PhysicsImpostor*>& impostors)
{
  for (auto& impostor : impostors) {
    impostor->beforeStep();
  }

  world->step();

  for (auto& impostor : impostors) {
    impostor->afterStep();
    // update the ordered impostors array
    _tmpImpostorsArray[std::to_string(impostor->uniqueId)] = impostor;
  }
#if 0
  // check for collisions
  auto contact = world->contacts;

  while (contact != nullptr) {
    if (contact->touching && !contact->body1->sleeping
        && !contact->body2->sleeping) {
      contact = contact->next;
      continue;
    }
    // is this body colliding with any other? get the impostor
    auto mainImpostor
      = (std::find(_tmpImpostorsArray.begin(), _tmpImpostorsArray.end(),
                   contact->body1->name)
         != _tmpImpostorsArray.end()) ?
          _tmpImpostorsArray[contact->body1->name] :
          nullptr;
    auto collidingImpostor
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
  auto mass = impostor->physicsBody()->mass();
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

void OimoPhysicsEnginePlugin::generatePhysicsBody(PhysicsImpostor* impostor)
{
  // parent-child relationship. Does this impostor has a parent impostor?
  if (impostor->parent()) {
    if (impostor->physicsBody()) {
      removePhysicsBody(impostor);
      impostor->forceUpdate();
    }
    return;
  }
}

void OimoPhysicsEnginePlugin::removePhysicsBody(PhysicsImpostor* impostor)
{
  world->removeRigidBody(impostor->physicsBody());
}

void OimoPhysicsEnginePlugin::generateJoint(PhysicsImpostorJoint* impostorJoint)
{
  auto mainBody      = impostorJoint->mainImpostor->physicsBody();
  auto connectedBody = impostorJoint->connectedImpostor->physicsBody();

  if (!mainBody || !connectedBody) {
    return;
  }
}

void OimoPhysicsEnginePlugin::removeJoint(PhysicsImpostorJoint* impostorJoint)
{
  world->removeJoint(impostorJoint->joint->physicsJoint());
}

bool OimoPhysicsEnginePlugin::isSupported() const
{
  return true;
}

void OimoPhysicsEnginePlugin::setTransformationFromPhysicsBody(
  PhysicsImpostor* /*impostor*/)
{
}

void OimoPhysicsEnginePlugin::setPhysicsBodyTransformation(
  PhysicsImpostor* impostor, const Vector3& newPosition,
  const Quaternion& newRotation)
{
  auto body = impostor->physicsBody();
  body->setPosition(newPosition.scale(OIMO::World::INV_SCALE));
  body->setOrientation(newRotation);
  body->syncShapes();
  body->awake();
}

OIMO::Shape* OimoPhysicsEnginePlugin::getLastShape(OIMO::RigidBody* body)
{
  auto lastShape = body->shapes;
  while (lastShape->next) {
    lastShape = lastShape->next;
  }
  return lastShape;
}

void OimoPhysicsEnginePlugin::setLinearVelocity(PhysicsImpostor* impostor,
                                                const Vector3& velocity)
{
  impostor->physicsBody()->setLinearVelocity(velocity);
}

void OimoPhysicsEnginePlugin::setAngularVelocity(PhysicsImpostor* impostor,
                                                 const Vector3& velocity)
{
  impostor->physicsBody()->setAngularVelocity(velocity);
}

Vector3 OimoPhysicsEnginePlugin::getLinearVelocity(PhysicsImpostor* impostor)
{
  return impostor->physicsBody()->linearVelocity();
}

Vector3 OimoPhysicsEnginePlugin::getAngularVelocity(PhysicsImpostor* impostor)
{
  return impostor->physicsBody()->angularVelocity();
}

void OimoPhysicsEnginePlugin::setBodyMass(PhysicsImpostor* impostor, float mass)
{
  bool staticBody = stl_util::almost_equal(mass, 0.f);
  // This will actually set the body's density and not its mass.
  // But this is how oimo treats the mass variable.
  impostor->physicsBody()->setShapesDensity(staticBody ? 1.f : mass);
  impostor->physicsBody()->setupMass(staticBody ? 0x2 : 0x1);
}

void OimoPhysicsEnginePlugin::sleepBody(PhysicsImpostor* impostor)
{
  impostor->physicsBody()->sleep();
}

void OimoPhysicsEnginePlugin::wakeUpBody(PhysicsImpostor* impostor)
{
  impostor->physicsBody()->awake();
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
