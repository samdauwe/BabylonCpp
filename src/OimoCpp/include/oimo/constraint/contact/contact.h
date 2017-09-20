#ifndef OIMO_CONSTRAINT_JOINT_BASE_ANGULAR_CONSTRAINT_H
#define OIMO_CONSTRAINT_JOINT_BASE_ANGULAR_CONSTRAINT_H

#include <array>

#include <oimo/constraint/contact/impulse_data_buffer.h>
#include <oimo/constraint/contact/manifold_point.h>
#include <oimo/oimo_api.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

class CollisionDetector;
class ContactConstraint;
struct ContactLink;
class ContactManifold;
class Constraint;
class RigidBody;
class Shape;

/**
 * @brief A contact is a pair of shapes whose axis-aligned bounding boxes are
 * overlapping.
 */
class OIMO_SHARED_EXPORT Contact {

public:
  Contact();
  ~Contact();

  static float MixRestitution(float restitution1, float restitution2);
  static float MixFriction(float restitution1, float restitution2);

  /**
   * Update the contact manifold.
   */
  void updateManifold();

  /**
   * Attach the contact to the shapes.
   * @param   shape1
   * @param   shape2
   */
  void attach(Shape* shape1, Shape* shape2);

  /**
   * Detach the contact from the shapes.
   */
  void detach();

public:
  // The first shape.
  Shape* shape1;
  // The second shape.
  Shape* shape2;
  // The first rigid body.
  RigidBody* body1;
  // The second rigid body.
  RigidBody* body2;
  // The previous contact in the world.
  Contact* prev;
  // The next contact in the world.
  Contact* next;
  // Internal
  bool persisting;
  // Whether both the rigid bodies are sleeping or not.
  bool sleeping;
  // The collision detector between two shapes.
  CollisionDetector* detector;
  // Whether the shapes are touching or not.
  bool touching;
  // Shapes is very close and touching
  bool close;
  float dist;
  // Contact links
  std::unique_ptr<ContactLink> b1Link, b2Link;
  std::unique_ptr<ContactLink> s1Link, s2Link;
  // The contact manifold of the contact.
  std::unique_ptr<ContactManifold> manifold;
  // The contact constraint of the contact.
  std::unique_ptr<ContactConstraint> constraint;
  std::array<ManifoldPoint, 4> points;
  std::array<ImpulseDataBuffer, 4> buffer;

}; // end of class Contact

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_BASE_ANGULAR_CONSTRAINT_H
