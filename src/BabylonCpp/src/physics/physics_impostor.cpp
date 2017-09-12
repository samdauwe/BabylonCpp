#include <babylon/physics/physics_impostor.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/engine/scene.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/physics/iphysics_enabled_object.h>
#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/joint/physics_joint.h>
#include <babylon/physics/physics_engine.h>

namespace BABYLON {

const Vector3 PhysicsImpostor::DEFAULT_OBJECT_SIZE = Vector3::One();

PhysicsImpostor::PhysicsImpostor(IPhysicsEnabledObject* _object,
                                 unsigned int _type,
                                 PhysicsImpostorParameters& options,
                                 Scene* scene)
    : object{_object}
    , type{_type}
    , _options{options}
    , _scene{scene}
    , _bodyUpdateRequired{false}
    , _deltaPosition{Vector3::Zero()}
{
  // Sanity check!
  if (!object) {
    BABYLON_LOG_ERROR("PhysicsImpostor",
                      "No object was provided. A physics object is obligatory");
    return;
  }

  // legacy support for old syntax.
  if (!_scene && object->getScene()) {
    _scene = object->getScene();
  }

  _physicsEngine = _scene->getPhysicsEngine();
  if (!_physicsEngine) {
    BABYLON_LOG_ERROR("PhysicsImpostor",
                      "Physics not enabled. Please use "
                      "scene.enablePhysics(...) before creating impostors.");
  }
  else {
    // Set the object's quaternion, if not set
    if (!object->rotationQuaternionSet()) {
      if (object->rotation() == Vector3::Zero()) {
        object->setRotationQuaternion(Quaternion::RotationYawPitchRoll(
          object->rotation().y, object->rotation().x, object->rotation().z));
      }
      else {
        object->setRotationQuaternion(Quaternion());
      }
    }
    // If the mesh has a parent, don't initialize the physicsBody. Instead wait
    // for the parent to do that.
    if (!object->getParent()) {
      _init();
    }
  }
}

PhysicsImpostor::~PhysicsImpostor()
{
}

void PhysicsImpostor::_init()
{
  _physicsEngine->removeImpostor(this);
  _physicsBody = nullptr;
  _parent      = (_parent == nullptr) ? _getPhysicsParent() : _parent;
  if (!parent()) {
    _physicsEngine->addImpostor(this);
  }
}

PhysicsImpostor* PhysicsImpostor::_getPhysicsParent()
{
  return object->getParent()->physicsImpostor.get();
}

bool PhysicsImpostor::isBodyInitRequired() const
{
  return _bodyUpdateRequired || (!_physicsBody && !_parent);
}

void PhysicsImpostor::setScalingUpdated()
{
  forceUpdate();
}

void PhysicsImpostor::forceUpdate()
{
  _init();
  if (parent()) {
    parent()->forceUpdate();
  }
}

IPhysicsBody* PhysicsImpostor::physicsBody()
{
  return _parent ? _parent->physicsBody() : _physicsBody;
}

PhysicsImpostor* PhysicsImpostor::parent()
{
  return _parent;
}

void PhysicsImpostor::setParent(PhysicsImpostor* value)
{
  _parent = value;
}

void PhysicsImpostor::setPhysicsBody(IPhysicsBody* physicsBody)
{
  if (_physicsBody) {
    _physicsEngine->getPhysicsPlugin()->removePhysicsBody(this);
  }
  _physicsBody = physicsBody;
  resetUpdateFlags();
}

void PhysicsImpostor::resetUpdateFlags()
{
  _bodyUpdateRequired = false;
}

Vector3 PhysicsImpostor::getObjectExtendSize()
{
  if (object->getBoundingInfo()) {
    object->computeWorldMatrix(true);
    return object->getBoundingInfo()->boundingBox.extendSize.scale(2).multiply(
      object->scaling());
  }
  else {
    return PhysicsImpostor::DEFAULT_OBJECT_SIZE;
  }
}

Vector3 PhysicsImpostor::getObjectCenter()
{
  if (object->getBoundingInfo()) {
    return object->getBoundingInfo()->boundingBox.center;
  }
  else {
    return object->position();
  }
}

float PhysicsImpostor::getParam(const std::string& paramName) const
{
  if (_options.contains(paramName)) {
    return _options[paramName];
  }

  return 0.f;
}

void PhysicsImpostor::setParam(const std::string& paramName, float value)
{
  _options.setValue(paramName, value);
  _bodyUpdateRequired = true;
}

void PhysicsImpostor::setMass(float mass)
{
  if (stl_util::almost_equal(getParam("mass"), mass)) {
    setParam("mass", mass);
  }
  _physicsEngine->getPhysicsPlugin()->setBodyMass(this, mass);
}

Vector3 PhysicsImpostor::getLinearVelocity()
{
  return _physicsEngine->getPhysicsPlugin()->getLinearVelocity(this);
}

void PhysicsImpostor::setLinearVelocity(const Vector3& velocity)
{
  _physicsEngine->getPhysicsPlugin()->setLinearVelocity(this, velocity);
}

Vector3 PhysicsImpostor::getAngularVelocity()
{
  return _physicsEngine->getPhysicsPlugin()->getAngularVelocity(this);
}

void PhysicsImpostor::setAngularVelocity(const Vector3& velocity)
{
  return _physicsEngine->getPhysicsPlugin()->setAngularVelocity(this, velocity);
}

void PhysicsImpostor::executeNativeFunction(
  const FastFunc<void(Mesh* world, IPhysicsBody* physicsBody)>& func)
{
  func(_physicsEngine->getPhysicsPlugin()->world, physicsBody());
}

void PhysicsImpostor::registerBeforePhysicsStep(
  const FastFunc<void(PhysicsImpostor* impostor)>& func)
{
  _onBeforePhysicsStepCallbacks.emplace_back(func);
}

void PhysicsImpostor::unregisterBeforePhysicsStep(
  const FastFunc<void(PhysicsImpostor* impostor)>& func)
{
  _onBeforePhysicsStepCallbacks.erase(
    ::std::remove(_onBeforePhysicsStepCallbacks.begin(),
                  _onBeforePhysicsStepCallbacks.end(), func),
    _onBeforePhysicsStepCallbacks.end());
}

void PhysicsImpostor::registerAfterPhysicsStep(
  const FastFunc<void(PhysicsImpostor* impostor)>& func)
{
  _onAfterPhysicsStepCallbacks.emplace_back(func);
}

void PhysicsImpostor::unregisterAfterPhysicsStep(
  const FastFunc<void(PhysicsImpostor* impostor)>& func)
{
  _onAfterPhysicsStepCallbacks.erase(
    ::std::remove(_onAfterPhysicsStepCallbacks.begin(),
                  _onAfterPhysicsStepCallbacks.end(), func),
    _onAfterPhysicsStepCallbacks.end());
}

void PhysicsImpostor::registerOnPhysicsCollide()
{
}

void PhysicsImpostor::unregisterOnPhysicsCollide()
{
}

void PhysicsImpostor::beforeStep()
{
  object->position().subtractToRef(_deltaPosition, _tmpPositionWithDelta);
  // conjugate deltaRotation
  if (_deltaRotationConjugated) {
    object->rotationQuaternion().multiplyToRef(*_deltaRotationConjugated,
                                               _tmpRotationWithDelta);
  }
  else {
    _tmpRotationWithDelta.copyFrom(object->rotationQuaternion());
  }

  _physicsEngine->getPhysicsPlugin()->setPhysicsBodyTransformation(
    this, _tmpPositionWithDelta, _tmpRotationWithDelta);

  for (auto& func : _onBeforePhysicsStepCallbacks) {
    func(this);
  }
}

void PhysicsImpostor::afterStep()
{
  for (auto& func : _onAfterPhysicsStepCallbacks) {
    func(this);
  }

  _physicsEngine->getPhysicsPlugin()->setTransformationFromPhysicsBody(this);

  object->position().addInPlace(_deltaPosition);
  if (_deltaRotation) {
    object->rotationQuaternion().multiplyInPlace(*_deltaRotation);
  }
}

void PhysicsImpostor::onCollide(IPhysicsBody* /*body*/)
{
}

void PhysicsImpostor::applyForce(const Vector3& force,
                                 const Vector3& contactPoint)
{
  _physicsEngine->getPhysicsPlugin()->applyForce(this, force, contactPoint);
}

void PhysicsImpostor::applyImpulse(const Vector3& force,
                                   const Vector3& contactPoint)
{
  _physicsEngine->getPhysicsPlugin()->applyImpulse(this, force, contactPoint);
}

void PhysicsImpostor::createJoint(PhysicsImpostor* otherImpostor,
                                  unsigned int jointType,
                                  const PhysicsJointData& jointData)
{
  addJoint(otherImpostor,
           ::std::make_shared<PhysicsJoint>(jointType, jointData));
}

void PhysicsImpostor::addJoint(PhysicsImpostor* otherImpostor,
                               const std::shared_ptr<PhysicsJoint>& joint)
{
  Joint _joint;
  _joint.otherImpostor = otherImpostor;
  _joint.joint         = joint;
  _joints.emplace_back(_joint);

  _physicsEngine->addJoint(this, otherImpostor, joint);
}

void PhysicsImpostor::sleep()
{
  _physicsEngine->getPhysicsPlugin()->sleepBody(this);
}

void PhysicsImpostor::wakeUp()
{
  _physicsEngine->getPhysicsPlugin()->wakeUpBody(this);
}

std::unique_ptr<PhysicsImpostor>
PhysicsImpostor::clone(IPhysicsEnabledObject* newObject)
{
  if (!newObject) {
    return nullptr;
  }
  return ::std::make_unique<PhysicsImpostor>(newObject, type, _options, _scene);
}

void PhysicsImpostor::dispose(bool /*doNotRecurse*/)
{
  // No dispose if no physics engine is available.
  if (!_physicsEngine) {
    return;
  }

  for (auto& j : _joints) {
    _physicsEngine->removeJoint(this, j.otherImpostor, j.joint.get());
  }

  // Dispose the physics body
  _physicsEngine->removeImpostor(this);
  if (parent()) {
    parent()->forceUpdate();
  }
}

void PhysicsImpostor::setDeltaPosition(const Vector3& position)
{
  _deltaPosition.copyFrom(position);
}

void PhysicsImpostor::setDeltaRotation(const Quaternion& rotation)
{
  if (!_deltaRotation) {
    _deltaRotation = ::std::make_unique<Quaternion>();
  }
  _deltaRotation->copyFrom(rotation);
  _deltaRotationConjugated->copyFrom(_deltaRotation->conjugate());
}

} // end of namespace BABYLON
