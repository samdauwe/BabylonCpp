#ifndef OIMO_DYNAMICS_RIGID_BODY_H
#define OIMO_DYNAMICS_RIGID_BODY_H

// -- Disable warnings -- //
// 'identifier' : class 'type' needs to have dll-interface to be used by clients
// of class 'type2'
#pragma warning(disable : 4251)

#include <oimo/collision/shape/mass_info.h>
#include <oimo/math/euler.h>
#include <oimo/math/mat33.h>
#include <oimo/math/mat44.h>
#include <oimo/math/quat.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_api.h>

namespace OIMO {

struct ContactLink;
struct JointLink;
class Shape;
class World;

/**
 * @brief The class of rigid body. Rigid body has the shape of a single or
 * multiple collision processing.
 */
class OIMO_SHARED_EXPORT RigidBody {

public:
  // The maximum number of shapes that can be added to a one rigid.
  static constexpr int MAX_SHAPES = 64;

public:
  // Body type
  enum class Type : unsigned int {
    BODY_NULL    = 0,
    BODY_DYNAMIC = 1,
    BODY_STATIC  = 2
  }; // end of enum class Body

public:
  RigidBody(float x = 0.f, float y = 0.f, float z = 0.f, float rad = 0.f,
            float ax = 0.f, float ay = 0.f, float az = 0.f);
  ~RigidBody();

  /**
   * Adds a shape to rigid body. If you add a shape, please call the setupMass
   * method to step up to the start of the next.
   * @param   shape shape to Add
   */
  void addShape(Shape* shape);

  /**
   * Deletes the shape from the rigid body. If you delete a shape, please call
   * the setupMass method to step up to the start of the next.
   * @param   shape shape to Delete
   */
  void removeShape(Shape* shape);

  void remove();
  void dispose();
  void checkContact(const std::string& name);

  /**
   * Calulates mass datas(center of gravity, mass, moment inertia, etc...).
   * If the parameter type is set to BODY_STATIC, the rigid body will be fixed
   * to the space.
   * If the parameter adjustPosition is set to true, the shapes' relative
   * positions and
   * the rigid body's position will be adjusted to the center of gravity.
   * @param   type
   * @param   adjustPosition
   */
  void setupMass(RigidBody::Type type = RigidBody::Type::BODY_DYNAMIC,
                 bool adjustPosition  = true);

  /**
   * Awake the rigid body.
   */
  void awake();

  /**
   * Sleep the rigid body.
   */
  void sleep();

  /**
   * Test wakeup rigid body.
   */
  void testWakeUp();

  /**
   * Get whether the rigid body has not any connection with others.
   * @return
   */
  bool isLonely() const;

  /**
   * The time integration of the motion of a rigid body, you can update the
   * information such as the shape. This method is invoked automatically when
   * calling the step of the World. There is no need to call from outside
   * usually.
   * @param  timeStep time
   */
  void updatePosition(float timeStep);

  Vec3 getAxis();
  void rotateInertia(const Mat33& rot, const Mat33& inertia, Mat33& out);
  Quat rotationVectToQuad(const Vec3& rot);
  Quat rotationAxisToQuad(float rad, float ax, float ay, float az);
  void syncShapes();

  //----------------------------------------------------------------------------
  // APPLY IMPULSE FORCE
  //----------------------------------------------------------------------------

  void applyImpulse(const Vec3& position, const Vec3& force);

  //----------------------------------------------------------------------------
  // SET DYNAMIC POSITION AND ROTATION
  //----------------------------------------------------------------------------
  void setPosition(const Vec3& pos);
  void setQuaternion(const Quat& q);
  void setRotation(const Vec3& rot);

  //----------------------------------------------------------------------------
  // RESET DYNAMIC POSITION AND ROTATION
  //----------------------------------------------------------------------------
  void resetPosition(float x, float y, float z);
  void resetQuaternion(const Quat& q);
  void resetRotation(float x, float y, float z);

  //----------------------------------------------------------------------------
  // GET POSITION AND ROTATION
  //----------------------------------------------------------------------------
  Vec3 getPosition() const;
  Quat getQuaternion() const;
  std::array<float, 16> getMatrix();

  //----------------------------------------------------------------------------
  // AUTO UPDATE MESH PROPERTIES
  //----------------------------------------------------------------------------
  void updateMesh();

public:
  std::string name;
  RigidBody* prev;
  RigidBody* next;
  Type type;
  MassInfo massInfo;
  // It is the world coordinate of the center of gravity.
  Vec3 position;
  Quat orientation;
  float scale, invScale;
  Vec3 newPosition;
  bool controlPos;
  Quat newOrientation;
  Vec3 newRotation;
  bool controlRot;
  bool controlRotInTime;
  Quat quaternion;
  Vec3 pos;
  // Is the translational velocity.
  Vec3 linearVelocity;
  // Is the angular velocity.
  Vec3 angularVelocity;
  // Return matrix
  Mat44 matrix;
  // It is a world that rigid body has been added.
  World* parent;
  ContactLink* contactLink;
  unsigned int numContacts;
  // An array of shapes that are included in the rigid body.
  Shape* shapes;
  // The number of shapes that are included in the rigid body.
  unsigned int numShapes;
  // It is the link array of joint that is connected to the rigid body.
  JointLink* jointLink;
  // The number of joints that are connected to the rigid body.
  unsigned int numJoints;
  // It is the world coordinate of the center of gravity in the sleep just
  // before.
  Vec3 sleepPosition;
  // It is a quaternion that represents the attitude of sleep just before.
  Quat sleepOrientation;
  // Determines whether this rigid body is static.
  bool isStatic;
  // Determines whether this rigid body is dynamic.
  bool isDynamic;
  // Determines whether this rigid body is kinematic.
  bool isKinematic;
  // It is a rotation matrix representing the orientation.
  Mat33 rotation;
  // This is the weight.
  float mass;
  // It is the reciprocal of the mass.
  float inverseMass;
  // It is the inverse of the inertia tensor in the world system.
  Mat33 inverseInertia;
  // It is the inertia tensor in the initial state.
  Mat33 localInertia;
  // Temporary inertia tensor.
  Mat33 tmpInertia;
  // It is the inverse of the inertia tensor in the initial state.
  Mat33 inverseLocalInertia;
  // I indicates rigid body whether it has been added to the simulation Island.
  bool addedToIsland;
  // It shows how to sleep rigid body.
  bool allowSleep;
  // This is the time from when the rigid body at rest.
  float sleepTime;
  // I shows rigid body to determine whether it is a sleep state.
  bool sleeping;
  // Mesh properties
  Vec3 meshPosition;
  Quat meshQuaternion;

}; // end of class RigidBody

} // end of namespace OIMO

#endif // end of OIMO_DYNAMICS_RIGID_BODY_H
