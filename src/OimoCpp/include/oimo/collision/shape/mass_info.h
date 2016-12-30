#ifndef OIMO_COLLISION_SHAPE_MASS_INFO_H
#define OIMO_COLLISION_SHAPE_MASS_INFO_H

#include <oimo/math/mat33.h>

namespace OIMO {

/**
 * @brief This class holds mass information of a shape.
 */
struct MassInfo {
  // Mass of the shape.
  float mass = 0.f;
  // The moment inertia of the shape.
  Mat33 inertia;
}; // end of struct MassInfo

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_SHAPE_MASS_INFO_H
