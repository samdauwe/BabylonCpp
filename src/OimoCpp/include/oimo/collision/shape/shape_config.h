#ifndef OIMO_COLLISION_SHAPE_SHAPE_CONFIG_H
#define OIMO_COLLISION_SHAPE_SHAPE_CONFIG_H

#include <oimo/math/mat33.h>
#include <oimo/math/vec3.h>

namespace OIMO {

/**
 * @brief A shape configuration holds common configuration data for constructing
 * a shape. Shape configurations can be reused safely.
 */
struct ShapeConfig {
  // The position of the shape in parent's coordinate system.
  Vec3 relativePosition;
  // The rotation matrix of the shape in parent's coordinate system.
  Mat33 relativeRotation;
  // The coefficient of friction of the shape.
  float friction = 0.4f;
  // The coefficient of restitution of the shape.
  float restitution = 0.2f;
  // The density of the shape.
  float density = 1.f;
  // The bits of the collision groups to which the shape belongs.
  int belongsTo = 1;
  // The bits of the collision groups with which the shape collides.
  int collidesWith = 0xffff;
}; // end of struct ShapeConfig

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_SHAPE_SHAPE_CONFIG_H
