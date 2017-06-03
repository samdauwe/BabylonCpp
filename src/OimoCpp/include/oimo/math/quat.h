#ifndef OIMO_MATH_QUAT_H
#define OIMO_MATH_QUAT_H

#include <array>
#include <memory>
#include <vector>

namespace OIMO {

class Mat33;
class Vec3;

class Quat {

public:
  Quat(float x = 0.f, float y = 0.f, float z = 0.f, float w = 1.f);
  Quat(const Quat& q);
  Quat(Quat&& q);
  ~Quat();
  Quat& operator=(const Quat& q);
  Quat& operator=(Quat&& q);
  Quat clone() const;
  std::unique_ptr<Quat> cloneToNewObject() const;
  friend std::ostream& operator<<(std::ostream& os, const Quat& q);

  Quat& set(float x, float y, float z, float w);
  Quat& setFromEuler(float x, float y, float z);
  Quat& setFromAxis(Vec3& axis, float rad);
  Quat& setFromRotationMatrix(const Mat33& mtx);
  Quat& init(float x = 0.f, float y = 0.f, float z = 1.f, float w = 0.f);
  Quat& add(const Quat& q1, const Quat& q2);
  Quat& addTime(const Vec3& v, float t);
  Quat& sub(const Quat& q1, const Quat& q2);
  Quat& scale(const Quat& q, float s);
  Quat& mul(const Quat& a);
  Quat& mul(const Quat& a, const Quat& b);
  Quat& multiply(const Quat& a);
  Quat& multiplyQuaternions(const Quat& a, const Quat& b);
  Quat& setFromUnitVectors(const Vec3& v1, const Vec3& v2);
  Quat& arc(const Vec3& v1, const Vec3& v2);
  Quat& normalize();
  Quat& normalize(const Quat& q);
  Quat& inverse();
  Quat& invert(const Quat& q);
  Quat& conjugate();
  float length() const;
  float lengthSq() const;
  Quat& copy(const Quat& q);
  bool testDiff(const Quat& q) const;
  bool equals(const Quat& q) const;
  std::string toString() const;
  void toArray(std::vector<float> array, size_t offset = 0);
  Quat& fromArray(const std::vector<float> array, size_t offset = 0);

public:
  float x;
  float y;
  float z;
  float w;

}; // end of class Quat

} // end of namespace OIMO

#endif // end of OIMO_MATH_QUAT_H
