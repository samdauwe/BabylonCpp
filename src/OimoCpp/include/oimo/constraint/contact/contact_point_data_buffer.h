#ifndef OIMO_CONSTRAINT_CONTACT_CONTACT_POINT_DATA_BUFFER_H
#define OIMO_CONSTRAINT_CONTACT_CONTACT_POINT_DATA_BUFFER_H

namespace OIMO {

/**
 * @brief
 */
struct ContactPointDataBuffer {
  float norX;
  float norY;
  float norZ;
  float tanX;
  float tanY;
  float tanZ;
  float binX;
  float binY;
  float binZ;

  float rp1X;
  float rp1Y;
  float rp1Z;
  float rp2X;
  float rp2Y;
  float rp2Z;

  float norU1X;
  float norU1Y;
  float norU1Z;
  float norU2X;
  float norU2Y;
  float norU2Z;
  float tanU1X;
  float tanU1Y;
  float tanU1Z;
  float tanU2X;
  float tanU2Y;
  float tanU2Z;
  float binU1X;
  float binU1Y;
  float binU1Z;
  float binU2X;
  float binU2Y;
  float binU2Z;

  float norT1X;
  float norT1Y;
  float norT1Z;
  float norT2X;
  float norT2Y;
  float norT2Z;
  float tanT1X;
  float tanT1Y;
  float tanT1Z;
  float tanT2X;
  float tanT2Y;
  float tanT2Z;
  float binT1X;
  float binT1Y;
  float binT1Z;
  float binT2X;
  float binT2Y;
  float binT2Z;

  float norTU1X;
  float norTU1Y;
  float norTU1Z;
  float norTU2X;
  float norTU2Y;
  float norTU2Z;
  float tanTU1X;
  float tanTU1Y;
  float tanTU1Z;
  float tanTU2X;
  float tanTU2Y;
  float tanTU2Z;
  float binTU1X;
  float binTU1Y;
  float binTU1Z;
  float binTU2X;
  float binTU2Y;
  float binTU2Z;

  float norImp;
  float tanImp;
  float binImp;
  float norDen;
  float tanDen;
  float binDen;
  float norTar;
  ContactPointDataBuffer* next;
  bool last = false;
}; // end of struct ContactPointDataBuffer

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_CONTACT_CONTACT_POINT_DATA_BUFFER_H