#ifndef OIMO_CONSTRAINT_CONTACT_IMPULSE_DATA_BUFFER_H
#define OIMO_CONSTRAINT_CONTACT_IMPULSE_DATA_BUFFER_H

#include <oimo/math/vec3.h>

namespace OIMO {

/**
 * @brief
 */
struct ImpulseDataBuffer {
  float lp1X;
  float lp1Y;
  float lp1Z;
  float lp2X;
  float lp2Y;
  float lp2Z;
  float impulse;
}; // end of struct ImpulseDataBuffer

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_CONTACT_IMPULSE_DATA_BUFFER_H