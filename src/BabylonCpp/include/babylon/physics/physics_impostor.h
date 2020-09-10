#ifndef BABYLON_PHYSICS_PHYSICS_IMPOSTER_H
#define BABYLON_PHYSICS_PHYSICS_IMPOSTER_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/vector3.h>
#include <babylon/physics/physics_impostor_parameters.h>

namespace BABYLON {

class AbstractMesh;
class Bone;
struct IPhysicsBody;
struct IPhysicsEnabledObject;
struct IPhysicsEngine;
class Mesh;
class PhysicsEngine;
class PhysicsImpostor;
class PhysicsJoint;
struct PhysicsJointData;
class Scene;
using IPhysicsEnginePtr  = std::shared_ptr<IPhysicsEngine>;
using PhysicsImpostorPtr = std::shared_ptr<PhysicsImpostor>;

struct Joint {
  std::shared_ptr<PhysicsJoint> joint = nullptr;
  PhysicsImpostorPtr otherImpostor= nullptr;
}; // end of class JointElement

/**
 * @brief Represents a physics imposter.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT PhysicsImpostor {

public:
  /**
   * The default object size of the imposter
   */
  static const Vector3 DEFAULT_OBJECT_SIZE;

  /**
   * The identity quaternion of the imposter
   */
  static Quaternion IDENTITY_QUATERNION;

  // Impostor types
  /**
   * No-Imposter type
   */
  static constexpr unsigned int NoImpostor = 0;
  /**
   * Sphere-Imposter type
   */
  static constexpr unsigned int SphereImpostor = 1;
  /**
   * Box-Imposter type
   */
  static constexpr unsigned int BoxImpostor = 2;
  /**
   * Plane-Imposter type
   */
  static constexpr unsigned int PlaneImpostor = 3;
  /**
   * Mesh-imposter type
   */
  static constexpr unsigned int MeshImpostor = 4;
  /**
   * Capsule-Impostor type (Ammo.js plugin only)
   */
  static constexpr unsigned int CapsuleImpostor = 6;
  /**
   * Cylinder-Imposter type
   */
  static constexpr unsigned int CylinderImpostor = 7;
  /**
   * Particle-Imposter type
   */
  static constexpr unsigned int ParticleImpostor = 8;
  /**
   * Heightmap-Imposter type
   */
  static constexpr unsigned int HeightmapImpostor = 9;
  /**
   * ConvexHull-Impostor type (Ammo.js plugin only)
   */
  static constexpr unsigned int ConvexHullImpostor = 10;
  /**
   * Custom-Imposter type (Ammo.js plugin only)
   */
  static constexpr unsigned int CustomImpostor = 100;
  /**
   * Rope-Imposter type
   */
  static constexpr unsigned int RopeImpostor = 101;
  /**
   * Cloth-Imposter type
   */
  static constexpr unsigned int ClothImpostor = 102;
  /**
   * Softbody-Imposter type
   */
  static constexpr unsigned int SoftbodyImpostor = 103;

public:
  PhysicsImpostor(IPhysicsEnabledObject* object, unsigned int type,
                  PhysicsImpostorParameters& options, Scene* scene = nullptr);
  virtual ~PhysicsImpostor(); // = default

  /**
   * @brief This function will completly initialize this impostor.
   * It will create a new body - but only if this mesh has no parent.
   * If it has, this impostor will not be used other than to define the impostor
   * of the child mesh.
   * Hidden
   */
  void _init();

  /**
   * @brief Should a new body be generated.
   * @returns boolean specifying if body initialization is required
   */
  [[nodiscard]] bool isBodyInitRequired() const;

  /**
   * @brief Sets the updated scaling.
   * @param updated Specifies if the scaling is updated
   */
  void setScalingUpdated();

  /**
   * @brief Force a regeneration of this or the parent's impostor's body.
   * Use under cautious - This will remove all joints already implemented.
   */
  void forceUpdate();

  /**
   * @brief Resets the update flags.
   */
  void resetUpdateFlags();

  /**
   * @brief Gets the object extend size.
   * @returns the object extend size
   */
  Vector3 getObjectExtendSize();

  /**
   * @brief Gets the object center.
   * @returns The object center
   */
  Vector3 getObjectCenter();

  /**
   * @brief Get a specific parameter from the options parameters.
   * @param paramName The object parameter name
   * @returns The object parameter
   */
  [[nodiscard]] float getParam(const std::string& paramName) const;

  /**
   * @brief Sets a specific parameter in the options given to the physics
   * plugin.
   * @param paramName The parameter name
   * @param value The value of the parameter
   */
  void setParam(const std::string& paramName, float value);

  /**
   * @brief Specifically change the body's mass option. Won't recreate the
   * physics body object
   * @param mass The mass of the physics imposter
   */
  void setMass(float mass);

  /**
   * @brief Gets the linear velocity.
   * @returns  linear velocity or null
   */
  std::optional<Vector3> getLinearVelocity();

  /**
   * @brief Sets the linear velocity.
   * @param velocity linear velocity or null
   */
  void setLinearVelocity(const std::optional<Vector3>& velocity);

  /**
   * @brief Gets the angular velocity.
   * @returns angular velocity or null
   */
  std::optional<Vector3> getAngularVelocity();

  /**
   * @brief Sets the angular velocity.
   * @param velocity The velocity or null
   */
  void setAngularVelocity(const std::optional<Vector3>& velocity);

  /**
   * @brief Execute a function with the physics plugin native code
   * Provide a function the will have two variables - the world object and the
   * physics body object.
   * @param func The function to execute with the physics plugin native code
   */
  void
  executeNativeFunction(const std::function<void(Mesh* world, IPhysicsBody* physicsBody)>& func);

  /**
   * @brief Register a function that will be executed before the physics world
   * is stepping forward.
   * @param func The function to execute before the physics world is stepped
   * forward
   */
  void registerBeforePhysicsStep(const std::function<void(PhysicsImpostor* impostor)>& func);

  /**
   * @brief Unregister a function that will be executed before the physics world
   * is stepping forward.
   * @param func The function to execute before the physics world is stepped
   * forward
   */
  void unregisterBeforePhysicsStep(const std::function<void(PhysicsImpostor* impostor)>& func);

  /**
   * @brief Register a function that will be executed after the physics step.
   * @param func The function to execute after physics step
   */
  void registerAfterPhysicsStep(const std::function<void(PhysicsImpostor* impostor)>& func);

  /**
   * @brief Unregisters a function that will be executed after the physics step.
   * @param func The function to execute after physics step
   */
  void unregisterAfterPhysicsStep(const std::function<void(PhysicsImpostor* impostor)>& func);

  /**
   * @brief Register a function that will be executed when this impostor
   * collides against a different body.
   * @param collideAgainst Physics imposter, or array of physics imposters to
   * collide against
   * @param func Callback that is executed on collision
   */
  void registerOnPhysicsCollide();

  /**
   * @brief Unregisters the physics imposter on contact.
   * @param collideAgainst The physics object to collide against
   * @param func Callback to execute on collision
   */
  void unregisterOnPhysicsCollide();

  /**
   * @brief Get the parent rotation.
   * @returns The parent rotation
   */
  Quaternion& getParentsRotation();

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
   * @brief Apply a force.
   * @param force The force to apply
   * @param contactPoint The contact point for the force
   * @returns The physics imposter
   */
  PhysicsImpostor& applyForce(const Vector3& force, const Vector3& contactPoint);

  /**
   * @brief Apply an impulse.
   * @param force The impulse force
   * @param contactPoint The contact point for the impulse force
   * @returns The physics imposter
   */
  PhysicsImpostor& applyImpulse(const Vector3& force, const Vector3& contactPoint);

  /**
   * @brief A help function to create a joint.
   * @param otherImpostor A physics imposter used to create a joint
   * @param jointType The type of joint
   * @param jointData The data for the joint
   * @returns The physics imposter
   */
  PhysicsImpostor& createJoint(const PhysicsImpostorPtr& otherImpostor, unsigned int jointType,
                               const PhysicsJointData& jointData);

  /**
   * @brief Add a joint to this impostor with a different impostor.
   * @param otherImpostor A physics imposter used to add a joint
   * @param joint The joint to add
   * @returns The physics imposter
   */
  PhysicsImpostor& addJoint(const PhysicsImpostorPtr& otherImpostor,
                            const std::shared_ptr<PhysicsJoint>& joint);

  /**
   * @brief Add an anchor to a cloth impostor.
   * @param otherImpostor rigid impostor to anchor to
   * @param width ratio across width from 0 to 1
   * @param height ratio up height from 0 to 1
   * @param influence the elasticity between cloth impostor and anchor from 0,
   * very stretchy to 1, little strech
   * @param noCollisionBetweenLinkedBodies when true collisions between cloth
   * impostor and anchor are ignored; default false
   * @returns impostor the soft imposter
   */
  PhysicsImpostor& addAnchor(const PhysicsImpostorPtr& otherImpostor, int width, int height,
                             float influence, bool noCollisionBetweenLinkedBodies);

  /**
   * @brief Add a hook to a rope impostor.
   * @param otherImpostor rigid impostor to anchor to
   * @param length ratio across rope from 0 to 1
   * @param influence the elasticity between rope impostor and anchor from 0,
   * very stretchy to 1, little strech
   * @param noCollisionBetweenLinkedBodies when true collisions between soft
   * impostor and anchor are ignored; default false
   * @returns impostor the rope imposter
   */
  PhysicsImpostor& addHook(const PhysicsImpostorPtr& otherImpostor, float length, float influence,
                           bool noCollisionBetweenLinkedBodies);

  /**
   * @brief Will keep this body still, in a sleep mode.
   * @returns the physics imposter
   */
  PhysicsImpostor& sleep();

  /**
   * @brief Wake the body up.
   * @returns The physics imposter
   */
  PhysicsImpostor& wakeUp();

  /**
   * @brief Clones the physics imposter.
   * @param newObject The physics imposter clones to this physics-enabled object
   * @returns A nullable physics imposter
   */
  std::unique_ptr<PhysicsImpostor> clone(IPhysicsEnabledObject* newObject);

  /**
   * @brief Disposes the physics imposter.
   */
  void dispose();

  /**
   * @brief Sets the delta position.
   * @param position The delta position amount
   */
  void setDeltaPosition(const Vector3& position);

  /**
   * @brief Sets the delta rotation.
   * @param rotation The delta rotation amount
   */
  void setDeltaRotation(const Quaternion& rotation);

  /**
   * @brief Gets the box size of the physics imposter and stores the result in
   * the input parameter.
   * @param result Stores the box size
   * @returns The physics imposter
   */
  PhysicsImpostor& getBoxSizeToRef(Vector3& result);

  /**
   * @brief Gets the radius of the physics imposter.
   * @returns Radius of the physics imposter
   */
  [[nodiscard]] float getRadius() const;

  /**
   * @brief Sync a bone with this impostor.
   * @param bone The bone to sync to the impostor.
   * @param boneMesh The mesh that the bone is influencing.
   * @param jointPivot The pivot of the joint / bone in local space.
   * @param distToJoint Optional distance from the impostor to the joint.
   * @param adjustRotation Optional quaternion for adjusting the local rotation
   * of the bone.
   */
  void syncBoneWithImpostor(Bone* bone, AbstractMesh* boneMesh,
                            const std::optional<Vector3>& jointPivot,
                            std::optional<float> distToJoint,
                            const std::optional<Quaternion>& adjustRotation);

  /**
   * @brief Sync impostor to a bone.
   * @param bone The bone that the impostor will be synced to.
   * @param boneMesh The mesh that the bone is influencing.
   * @param jointPivot The pivot of the joint / bone in local space.
   * @param distToJoint Optional distance from the impostor to the joint.
   * @param adjustRotation Optional quaternion for adjusting the local rotation
   * of the bone.
   * @param boneAxis Optional vector3 axis the bone is aligned with
   */
  void syncImpostorWithBone(Bone* bone, AbstractMesh* boneMesh,
                            const std::optional<Vector3>& jointPivot,
                            std::optional<float> distToJoint,
                            const std::optional<Quaternion>& adjustRotation,
                            std::optional<Vector3>& boneAxis);

private:
  static std::array<Vector3, 3> _tmpVecs;
  static Quaternion _tmpQuat;

private:
  /**
   * @brief Specifies if the physics imposter is disposed.
   */
  [[nodiscard]] bool get_isDisposed() const;

  /**
   * @brief Gets the mass of the physics imposter.
   */
  [[nodiscard]] float get_mass() const;

  /**
   * @brief Sets the mass of the physics imposter.
   */
  void set_mass(float value);

  /**
   * @brief Gets the coefficient of friction.
   */
  [[nodiscard]] float get_friction() const;

  /**
   * @brief Sets the coefficient of friction.
   */
  void set_friction(float value);

  /**
   * @brief Gets the coefficient of restitution.
   */
  [[nodiscard]] float get_restitution() const;

  /**
   * @brief Sets the coefficient of restitution.
   */
  void set_restitution(float value);

  /**
   * @brief Gets the pressure of a soft body; only supported by the
   * AmmoJSPlugin.
   */
  [[nodiscard]] float get_pressure() const;

  /**
   * @brief Sets the pressure of a soft body; only supported by the
   * AmmoJSPlugin.
   */
  void set_pressure(float value);

  /**
   * @brief Gets the stiffness of a soft body; only supported by the
   * AmmoJSPlugin.
   */
  [[nodiscard]] float get_stiffness() const;

  /**
   * @brief Sets the stiffness of a soft body; only supported by the
   * AmmoJSPlugin.
   */
  void set_stiffness(float value);

  /**
   * @brief Gets the velocityIterations of a soft body; only supported by the
   * AmmoJSPlugin.
   */
  [[nodiscard]] size_t get_velocityIterations() const;

  /**
   * @brief Sets the velocityIterations of a soft body; only supported by the
   * AmmoJSPlugin.
   */
  void set_velocityIterations(size_t value);

  /**
   * @brief Gets the positionIterations of a soft body; only supported by the
   * AmmoJSPlugin.
   */
  [[nodiscard]] size_t get_positionIterations() const;

  /**
   * @brief Sets the positionIterations of a soft body; only supported by the
   * AmmoJSPlugin.
   */
  void set_positionIterations(size_t value);

  /**
   * @brief Gets the body that holds this impostor. Either its own, or its.
   * parent.
   */
  IPhysicsBody*& get_physicsBody();

  /**
   * @brief Set the physics body. Used mainly by the physics engine/plugin.
   */
  void set_physicsBody(IPhysicsBody* const& physicsBody);

  /**
   * @brief Get the parent of the physics imposter.
   * @returns Physics imposter or null
   */
  PhysicsImpostor*& get_parent();

  /**
   * @brief Sets the parent of the physics imposter.
   */
  void set_parent(PhysicsImpostor* const& value);

  /**
   * @brief Hidden
   */
  PhysicsImpostorPtr _getPhysicsParent();

public:
  /**
   * Hidden
   */
  bool _isFromLin;

  /**
   * Specifies if the physics imposter is disposed
   */
  ReadOnlyProperty<PhysicsImpostor, bool> isDisposed;

  /**
   * Gets or sets the mass of the physics imposter
   */
  Property<PhysicsImpostor, float> mass;

  /**
   * Gets or sets the coefficient of friction
   */
  Property<PhysicsImpostor, float> friction;

  /**
   * Gets or sets the coefficient of restitution
   */
  Property<PhysicsImpostor, float> restitution;

  /**
   * Gets or sets the pressure of a soft body; only supported by the
   * AmmoJSPlugin
   */
  Property<PhysicsImpostor, float> pressure;

  /**
   * Gets or sets the stiffness of a soft body; only supported by the
   * AmmoJSPlugin
   */
  Property<PhysicsImpostor, float> stiffness;

  /**
   * Gets or sets the velocityIterations of a soft body; only supported by the
   * AmmoJSPlugin
   */
  Property<PhysicsImpostor, size_t> velocityIterations;

  /**
   * Gets or sets the positionIterations of a soft body; only supported by the
   * AmmoJSPlugin
   */
  Property<PhysicsImpostor, size_t> positionIterations;

  /**
   * The unique id of the physics imposter
   * set by the physics engine when adding this impostor to the array
   */
  size_t uniqueId;

  /**
   * Hidden
   */
  bool soft;

  /**
   * Hidden
   */
  unsigned int segments;

  /**
   * The physics-enabled object used as the physics imposter
   */
  IPhysicsEnabledObject* object;

  /**
   * The type of the physics imposter
   */
  unsigned int physicsImposterType;

  /**
   * Gets or sets the body that holds this impostor. Either its own, or its
   * parent
   */
  Property<PhysicsImpostor, IPhysicsBody*> physicsBody;

  /**
   * Gets or sets the parent of the physics imposter
   */
  Property<PhysicsImpostor, PhysicsImpostor*> parent;

private:
  PhysicsImpostorParameters _options;
  Scene* _scene;
  IPhysicsEnginePtr _physicsEngine;
  // The native cannon/oimo/energy physics body object.
  IPhysicsBody* _physicsBody;
  bool _bodyUpdateRequired;
  std::vector<std::function<void(PhysicsImpostor* impostor)>> _onBeforePhysicsStepCallbacks;
  std::vector<std::function<void(PhysicsImpostor* imposter)>> _onAfterPhysicsStepCallbacks;
  Vector3 _deltaPosition;
  std::unique_ptr<Quaternion> _deltaRotation;
  std::unique_ptr<Quaternion> _deltaRotationConjugated;
  // If set, this is this impostor's parent
  PhysicsImpostor* _parent;
  bool _isDisposed;
  std::vector<Joint> _joints;
  Vector3 _tmpPositionWithDelta;
  Quaternion _tmpRotationWithDelta;

  // Temp variables for parent rotation calculations
  Quaternion _tmpQuat2;

  PhysicsImpostor* nullPhysicsImpostor;

}; // end of class PhysicsImpostor

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_IMPOSTER_H
