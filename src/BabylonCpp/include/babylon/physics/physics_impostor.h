#ifndef BABYLON_PHYSICS_PHYSICS_IMPOSTER_H
#define BABYLON_PHYSICS_PHYSICS_IMPOSTER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>
#include <babylon/physics/physics_impostor_parameters.h>

namespace BABYLON {

struct Joint {
  shared_ptr_t<PhysicsJoint> joint;
  PhysicsImpostor* otherImpostor;
}; // end of class JointElement

class BABYLON_SHARED_EXPORT PhysicsImpostor : public IDisposable {

public:
  static const Vector3 DEFAULT_OBJECT_SIZE;

  static Quaternion IDENTITY_QUATERNION;

  // Impostor types
  static constexpr unsigned int NoImpostor        = 0;
  static constexpr unsigned int SphereImpostor    = 1;
  static constexpr unsigned int BoxImpostor       = 2;
  static constexpr unsigned int PlaneImpostor     = 3;
  static constexpr unsigned int MeshImpostor      = 4;
  static constexpr unsigned int CylinderImpostor  = 7;
  static constexpr unsigned int ParticleImpostor  = 8;
  static constexpr unsigned int HeightmapImpostor = 9;

public:
  PhysicsImpostor(IPhysicsEnabledObject* object, unsigned int type,
                  PhysicsImpostorParameters& _options, Scene* scene = nullptr);
  virtual ~PhysicsImpostor();

  /**
   * @brief This function will completly initialize this impostor.
   * It will create a new body - but only if this mesh has no parent.
   * If it has, this impostor will not be used other than to define the impostor
   * of the child mesh.
   */
  void _init();

  /**
   * @brief Should a new body be generated.
   */
  bool isBodyInitRequired() const;

  void setScalingUpdated();

  /**
   * @brief Force a regeneration of this or the parent's impostor's body.
   * Use under cautious - This will remove all joints already implemented.
   */
  void forceUpdate();

  /**
   * @brief Gets the body that holds this impostor. Either its own, or its
   * parent.
   */
  IPhysicsBody* physicsBody();

  PhysicsImpostor* parent();
  void setParent(PhysicsImpostor* value);
  unsigned int type() const;

  /**
   * @brief Set the physics body. Used mainly by the physics engine/plugin
   */
  void setPhysicsBody(IPhysicsBody* physicsBody);

  void resetUpdateFlags();
  Vector3 getObjectExtendSize();
  Vector3 getObjectCenter();

  /**
   * @brief Get a specific parametes from the options parameter.
   */
  float getParam(const string_t& paramName) const;

  /**
   * @brief Sets a specific parameter in the options given to the physics plugin
   */
  void setParam(const string_t& paramName, float value);

  /**
   * @brief Specifically change the body's mass option. Won't recreate the
   * physics body object
   */
  void setMass(float mass);

  Vector3 getLinearVelocity();

  /**
   * @brief Set the body's linear velocity.
   */
  void setLinearVelocity(const Vector3& velocity);

  Vector3 getAngularVelocity();

  /**
   * @brief Set the body's angular velocity.
   */
  void setAngularVelocity(const Vector3& velocity);

  /**
   * @brief Execute a function with the physics plugin native code.
   * Provide a function the will have two variables - the world object and the
   * physics body object.
   */
  void executeNativeFunction(
    const ::std::function<void(Mesh* world, IPhysicsBody* physicsBody)>& func);

  /**
   * @brief Register a function that will be executed before the physics world
   * is stepping forward.
   */
  void registerBeforePhysicsStep(
    const ::std::function<void(PhysicsImpostor* impostor)>& func);

  void unregisterBeforePhysicsStep(
    const ::std::function<void(PhysicsImpostor* impostor)>& func);

  /**
   * @brief Register a function that will be executed after the physics step
   */
  void registerAfterPhysicsStep(
    const ::std::function<void(PhysicsImpostor* impostor)>& func);

  void unregisterAfterPhysicsStep(
    const ::std::function<void(PhysicsImpostor* impostor)>& func);

  /**
   * @brief Register a function that will be executed when this impostor
   * collides against a different body.
   */
  void registerOnPhysicsCollide();

  void unregisterOnPhysicsCollide();

  void getParentsRotation();

  /**
   * @brief This function is executed by the physics engine.
   */
  void beforeStep();

  /**
   * @brief This function is executed by the physics engine.
   */
  void afterStep();

  /**
   * @brief Event and body object due to cannon's event-based architecture.
   */
  void onCollide(IPhysicsBody* body);

  /**
   * @brief Apply a force
   */
  void applyForce(const Vector3& force, const Vector3& contactPoint);

  /**
   * @brief Apply an impulse
   */
  void applyImpulse(const Vector3& force, const Vector3& contactPoint);

  /**
   * @brief A help function to create a joint.
   */
  void createJoint(PhysicsImpostor* otherImpostor, unsigned int jointType,
                   const PhysicsJointData& jointData);

  /**
   * @brief Add a joint to this impostor with a different impostor.
   */
  void addJoint(PhysicsImpostor* otherImpostor,
                const shared_ptr_t<PhysicsJoint>& joint);

  /**
   * @brief Will keep this body still, in a sleep mode.
   */
  void sleep();

  /**
   * @brief Wake the body up.
   */
  void wakeUp();

  unique_ptr_t<PhysicsImpostor> clone(IPhysicsEnabledObject* newObject);
  void dispose(bool doNotRecurse = false) override;
  void setDeltaPosition(const Vector3& position);
  void setDeltaRotation(const Quaternion& rotation);
  PhysicsImpostor& getBoxSizeToRef(Vector3& result);
  float getRadius() const;

private:
  bool get_isDisposed() const;
  float get_mass() const;
  void set_mass(float value);
  float get_friction() const;
  void set_friction(float value);
  float get_restitution() const;
  void set_restitution(float value);
  PhysicsImpostor* _getPhysicsParent();

public:
  IPhysicsEnabledObject* object;
  // set by the physics engine when adding this impostor to the array.
  size_t uniqueId;

  ReadOnlyProperty<PhysicsImpostor, bool> isDisposed;
  Property<PhysicsImpostor, float> mass;
  Property<PhysicsImpostor, float> friction;
  Property<PhysicsImpostor, float> restitution;

private:
  unsigned int _type;
  PhysicsImpostorParameters _options;
  Scene* _scene;
  PhysicsEngine* _physicsEngine;
  // The native cannon/oimo/energy physics body object.
  IPhysicsBody* _physicsBody;
  bool _bodyUpdateRequired;
  vector_t<::std::function<void(PhysicsImpostor* impostor)>>
    _onBeforePhysicsStepCallbacks;
  vector_t<::std::function<void(PhysicsImpostor* imposter)>>
    _onAfterPhysicsStepCallbacks;
  Vector3 _deltaPosition;
  unique_ptr_t<Quaternion> _deltaRotation;
  unique_ptr_t<Quaternion> _deltaRotationConjugated;
  // If set, this is this impostor's parent
  PhysicsImpostor* _parent;
  bool _isDisposed;
  vector_t<Joint> _joints;
  Vector3 _tmpPositionWithDelta;
  Quaternion _tmpRotationWithDelta;

  // Temp variables for parent rotation calculations
  Quaternion _tmpQuat;
  Quaternion _tmpQuat2;

}; // end of class PhysicsImpostor

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_IMPOSTER_H
