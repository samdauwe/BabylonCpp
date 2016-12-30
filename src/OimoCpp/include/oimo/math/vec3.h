#ifndef OIMO_MATH_VEC3_H
#define OIMO_MATH_VEC3_H

#include <iostream>
#include <utility>

namespace OIMO {

class Mat33;
class Quat;

class Vec3 {

public:
  Vec3(float x = 0.f, float y = 0.f, float z = 0.f);
  Vec3(const Vec3& v);
  Vec3(Vec3&& v);
  ~Vec3();
  Vec3& operator=(const Vec3& v);
  Vec3& operator=(Vec3&& v);
  Vec3 clone() const;
  Vec3* cloneToNewObject() const;
  friend std::ostream& operator<<(std::ostream& os, const Vec3& v);

  Vec3& init(float x = 0.f, float y = 0.f, float z = 0.f);
  Vec3& set(float x, float y, float z);
  Vec3& add(const Vec3& v);
  Vec3& add(const Vec3& v1, const Vec3& v2);
  Vec3& addEqual(const Vec3& v);
  Vec3& addTime(const Vec3& v, float t);
  Vec3& sub(const Vec3& v1, const Vec3& v2);
  Vec3& subEqual(const Vec3& v);
  Vec3& addScale(const Vec3& v, float s);
  Vec3& scale(const Quat& q, float s);
  Vec3& scale(const Vec3& v, float s);
  Vec3& scaleEqual(float s);
  Vec3& cross(const Vec3& v);
  Vec3& cross(const Vec3& v1, const Vec3& v2);
  Vec3& mul(const Vec3& o, const Vec3& v, const Mat33& m);
  Vec3& mulMat(const Mat33 m, const Vec3& v);
  Vec3& normalize(const Vec3& v);
  Vec3& invert(const Vec3& v);
  Vec3& negate();
  float dot(const Vec3& v) const;
  float lengthSq() const;
  float length() const;
  Vec3& copy(const Vec3& v);
  Vec3& applyQuaternion(const Quat& q);
  bool testZero() const;
  bool testDiff(const Vec3& v) const;
  bool equals(const Vec3& v) const;
  std::string toString() const;
  Vec3& multiplyScalar(float scalar);
  Vec3& divideScalar(float scalar);
  Vec3& norm();
  float angleTo(const Vec3& v) const;

public:
  float x;
  float y;
  float z;

}; // end of class Vec3

} // end of namespace OIMO

#endif // end of OIMO_MATH_VEC3_H
