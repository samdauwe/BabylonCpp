#ifndef OIMO_MATH_QUAT_H
#define OIMO_MATH_QUAT_H

#include <array>

namespace OIMO {

class Mat33;
class Vec3;

class Quat {

public:
  Quat(float s = 1.f, float x = 0.f, float y = 0.f, float z = 0.f);
  Quat(const Quat& q);
  Quat(Quat&& q);
  ~Quat();
  Quat& operator=(const Quat& q);
  Quat& operator=(Quat&& q);
  Quat clone() const;
  Quat* cloneToNewObject() const;
  friend std::ostream& operator<<(std::ostream& os, const Quat& q);

  Quat& set(float s, float x, float y, float z);
  Quat& setFromRotationMatrix(const Mat33& mtx);
  Quat& init(float s = 1.f, float x = 0.f, float y = 0.f, float z = 0.f);
  Quat& add(const Quat& q1, const Quat& q2);
  Quat& addTime(const Vec3& v, float t);
  Quat& sub(const Quat& q1, const Quat& q2);
  Quat& scale(const Quat& q, float s);
  Quat& mul(const Quat& q1, const Quat& q2);
  Quat& arc(const Vec3& v1, const Vec3& v2);
  Quat& normalize(const Quat& q);
  Quat& invert(const Quat& q);
  float length() const;
  Quat& copy(const Quat& q);
  bool testDiff(const Quat& q);
  std::string toString() const;

public:
  float s;
  float x;
  float y;
  float z;

}; // end of class Quat

} // end of namespace OIMO

#endif // end of OIMO_MATH_QUAT_H
