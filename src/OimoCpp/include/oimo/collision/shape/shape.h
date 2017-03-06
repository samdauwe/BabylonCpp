#ifndef OIMO_COLLISION_SHAPE_SHAPE_H
#define OIMO_COLLISION_SHAPE_SHAPE_H

#include <memory>

#include <oimo/collision/broadphase/proxy.h>
#include <oimo/math/mat33.h>
#include <oimo/math/vec3.h>

namespace OIMO {

class AABB;
struct ContactLink;
struct MassInfo;
class Proxy;
class RigidBody;
struct ShapeConfig;

/**
 * @brief A shape is used to detect collisions of rigid bodies.
 */
class Shape {

public:
  static unsigned int s_nextID;

public:
  // Shape type
  enum class Type : unsigned int {
    SHAPE_SPHERE   = 0,
    SHAPE_BOX      = 1,
    SHAPE_CYLINDER = 2,
    SHAPE_TETRA    = 3,
    SHAPE_POLYGON  = 4,
    SHAPE_PARTICLE = 5,
    SHAPE_PLANE    = 6,
    SHAPE_NULL     = 50
  }; // end of enum class Type

public:
  Shape(const ShapeConfig& config);
  virtual ~Shape();

  /**
   * Calculate the volume of the shape.
   */
  virtual float volume() const;

  /**
   * Calculate the mass information of the shape.
   */
  virtual void calculateMassInfo(MassInfo& out);

  /**
   * Update the proxy of the shape.
   */
  virtual void updateProxy();

public:
  Type type;
  unsigned int id;
  // The previous shape in parent rigid body.
  Shape* prev;
  // The next shape in parent rigid body.
  Shape* next;
  // The proxy of the shape used for broad-phase collision detection.
  std::unique_ptr<Proxy> proxy;
  // The parent rigid body of the shape.
  RigidBody* parent;
  // The linked list of the contacts with the shape.
  ContactLink* contactLink;
  // The number of the contacts with the shape.
  int numContacts;
  // The center of gravity of the shape in world coordinate system.
  Vec3 position;
  // The rotation matrix of the shape in world coordinate system
  Mat33 rotation;
  // The position of the shape in parent's coordinate system.
  Vec3 relativePosition;
  // The rotation matrix of the shape in parent's coordinate system.
  Mat33 relativeRotation;
  // The axis-aligned bounding box of the shape.
  AABB* aabb;
  // The density of the shape.
  float density;
  // The coefficient of friction of the shape.
  float friction;
  // The coefficient of restitution of the shape.
  float restitution;
  // The bits of the collision groups to which the shape belongs.
  int belongsTo;
  // The bits of the collision groups with which the shape collides.
  int collidesWith;

}; // end of class Shape

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_SHAPE_SHAPE_H
