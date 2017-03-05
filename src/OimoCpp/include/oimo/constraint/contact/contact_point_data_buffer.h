#ifndef OIMO_CONSTRAINT_CONTACT_CONTACT_POINT_DATA_BUFFER_H
#define OIMO_CONSTRAINT_CONTACT_CONTACT_POINT_DATA_BUFFER_H

#include <memory>
#include <oimo/math/vec3.h>

namespace OIMO {

/**
 * @brief
 */
struct ContactPointDataBuffer {
  Vec3 nor;
  Vec3 tan;
  Vec3 bin;

  Vec3 norU1;
  Vec3 tanU1;
  Vec3 binU1;

  Vec3 norU2;
  Vec3 tanU2;
  Vec3 binU2;

  Vec3 norT1;
  Vec3 tanT1;
  Vec3 binT1;

  Vec3 norT2;
  Vec3 tanT2;
  Vec3 binT2;

  Vec3 norTU1;
  Vec3 tanTU1;
  Vec3 binTU1;

  Vec3 norTU2;
  Vec3 tanTU2;
  Vec3 binTU2;

  float norImp = 0.f;
  float tanImp = 0.f;
  float binImp = 0.f;

  float norDen = 0.f;
  float tanDen = 0.f;
  float binDen = 0.f;

  float norTar = 0.f;

  std::unique_ptr<ContactPointDataBuffer> next;
  bool last = false;
}; // end of struct ContactPointDataBuffer

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_CONTACT_CONTACT_POINT_DATA_BUFFER_H
