#ifndef OIMO_CONSTRAINT_CONSTRAINT_H
#define OIMO_CONSTRAINT_CONSTRAINT_H

namespace OIMO {

class World;
class RigidBody;

/**
 * @brief The base class of all type of the constraints.
 */
class Constraint {

public:
  Constraint();
  virtual ~Constraint();

  /**
   * Prepare for solving the constraint.
   * @param timeStep
   * @param invTimeStep
   */
  virtual void preSolve(float timeStep, float invTimeStep);

  /**
   * Solve the constraint.
   * This is usually called iteratively.
   */
  virtual void solve();

  /**
   * Do the post-processing.
   */
  virtual void postSolve();

public:
  // The parent world of the constraint.
  World* parent;
  // The first body of the constraint.
  RigidBody* body1;
  // The second body of the constraint.
  RigidBody* body2;
  // Internal
  bool addedToIsland;

}; // end of class Constraint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_CONSTRAINT_H
