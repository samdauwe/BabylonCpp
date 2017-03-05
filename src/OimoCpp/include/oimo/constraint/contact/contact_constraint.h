#ifndef OIMO_CONSTRAINT_JOINT_BASE_CONTACT_CONTACT_CONSTRAINT_H
#define OIMO_CONSTRAINT_JOINT_BASE_CONTACT_CONTACT_CONSTRAINT_H

#include <vector>

#include <oimo/constraint/constraint.h>
#include <oimo/constraint/contact/manifold_point.h>
#include <oimo/math/mat33.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

class ContactManifold;
struct ContactPointDataBuffer;

/**
 * @brief
 */
class ContactConstraint : public Constraint {

public:
  ContactConstraint(ContactManifold* _manifold);
  ~ContactConstraint();

  /**
   * Attach the constraint to the bodies.
   */
  void attach();

  /**
   * Detach the constraint from the bodies.
   */
  void detach();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;
  void postSolve() override;

public:
  // The coefficient of restitution of the constraint.
  float restitution;
  // The coefficient of friction of the constraint.
  float friction;

private:
  // The contact manifold of the constraint.
  ContactManifold* _manifold;
  Vec3 _p1, _p2;
  Vec3 _lv1, _lv2;
  Vec3 _av1, _av2;
  Mat33 _i1, _i2;
  Vec3 _tmp, _tmpC1, _tmpC2;
  Vec3 _tmpP1, _tmpP2;
  Vec3 _tmplv1, _tmplv2;
  Vec3 _tmpav1, _tmpav2;
  float _m1, _m2;
  unsigned int _num;
  std::array<ManifoldPoint, 4> _ps;
  std::unique_ptr<ContactPointDataBuffer> _cs;

}; // end of class ContactConstraint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_BASE_CONTACT_CONTACT_CONSTRAINT_H
