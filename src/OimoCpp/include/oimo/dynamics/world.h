#ifndef OIMO_DYNAMICS_RIGID_WORLD_H
#define OIMO_DYNAMICS_RIGID_WORLD_H

#include <array>
#include <string>
#include <vector>

#include <oimo/collision/broadphase/broad_phase.h>
#include <oimo/collision/narrowphase/collision_detector.h>
#include <oimo/constraint/constraint.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>
#include <oimo/util/performance.h>

namespace OIMO {

class Contact;
class Joint;
class RigidBody;
class Shape;

/**
 * @brief The class of physical computing world.
 */
class World {

public:
  // this world scale defaut is 0.1 to 10 meters max for dynamique body
  // scale all by 100 so object is between 10 to 10000 three unit.
  static float WORLD_SCALE;
  static float INV_SCALE;

public:
  World(float timeStep                  = 0.01666f, // 1/60
        BroadPhase::Type broadPhaseType = BroadPhase::Type::BR_SWEEP_AND_PRUNE,
        unsigned int iterations = 8, bool noStat = false);
  ~World();

  /**
   * Reset the randomizer and remove all rigid bodies, shapes, joints and any
   * object from the world.
   */
  void clear();

  /**
   * Adds a rigid body to the world.
   * Rigid body that has been added will be the operands of each step.
   * @param  rigidBody  Rigid body that you want to add
   */
  void addRigidBody(RigidBody* rigidBody);

  /**
   * I will remove the rigid body from the world.
   * Rigid body that has been deleted is excluded from the calculation on a
   * step-by-step basis.
   * @param  rigidBody  Rigid body to be removed
   */
  void removeRigidBody(RigidBody* rigidBody);

  /**
   * Adds a shape to the world.
   * Add to the rigid world, and if you add a shape to a rigid body that has
   * been added to the world,
   * Shape will be added to the world automatically, please do not call from
   * outside this method.
   * @param  shape  Shape you want to add
   */
  void addShape(Shape* shape);

  /**
   * Removes the shape from the world.
   * Add to the rigid world, and if you add a shape to a rigid body that has
   * been added to the world,
   * Shape will be added to the world automatically, please do not call from
   * outside this method.
   * @param  shape  Shape you want to delete
   */
  void removeShape(Shape* shape);

  /**
   * Adds a joint to the world.
   * Joint that has been added will be the operands of each step.
   * @param  shape Joint to be added
   */
  void addJoint(Joint* joint);

  /**
   * Removes the joint from the world.
   * Joint that has been added will be the operands of each step.
   * @param  shape Joint to be deleted
   */
  void removeJoint(Joint* joint);

  void setWorldscale(float scale = 100.f);
  void addContact(Shape* s1, Shape* s2);
  void removeContact(Contact* contact);
  bool checkContact(const std::string& name1, const std::string& name2);
  bool callSleep(RigidBody* body);

  /**
   * Proceed only time step seconds time of World.
   */
  void step();

public:
  // The time between each step
  float timeStep;
  // The number of iterations for constraint solvers.
  unsigned int numIterations;
  // It is a wide-area collision judgment that is used in order to reduce as
  // much as possible a detailed collision judgment.
  std::unique_ptr<BroadPhase> broadPhase;
  // This is the detailed information of the performance.
  Performance performance;
  bool isNoStat;
  // Whether the constraints randomizer is enabled or not.
  bool enableRandomizer;
  // The rigid body list
  RigidBody* rigidBodies;
  // number of rigid body
  unsigned int numRigidBodies;
  // The contact list
  Contact* contacts;
  Contact* unusedContacts;
  // The number of contact
  unsigned int numContacts;
  // The number of contact points
  unsigned int numContactPoints;
  //  The joint list
  Joint* joints;
  // The number of joints.
  unsigned int numJoints;
  // The number of simulation islands.
  unsigned int numIslands;
  // The gravity in the world.
  Vec3 gravity;
  unsigned int numShapeTypes;
  // Collision detectors (5 shape types)
  std::array<std::array<std::unique_ptr<CollisionDetector>, 5>, 5> detectors;
  // Rand
  unsigned int randX, randA, randB;
  std::vector<RigidBody*> islandRigidBodies;
  std::vector<RigidBody*> islandStack;
  std::vector<Constraint*> islandConstraints;

}; // end of struct World

} // end of namespace OIMO

#endif // end of OIMO_DYNAMICS_RIGID_WORLD_H
