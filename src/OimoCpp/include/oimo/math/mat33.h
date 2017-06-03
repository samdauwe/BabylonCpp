#ifndef OIMO_MATH_MAT33_H
#define OIMO_MATH_MAT33_H

#include <array>
#include <iostream>
#include <memory>
#include <utility>

namespace OIMO {

class Quat;
class Vec3;

class Mat33 {

public:
  Mat33(float e00 = 1.f, float e01 = 0.f, float e02 = 0.f, //
        float e10 = 0.f, float e11 = 1.f, float e12 = 0.f, //
        float e20 = 0.f, float e21 = 0.f, float e22 = 1.f);
  Mat33(const Mat33& m);
  Mat33(Mat33&& m);
  ~Mat33();
  Mat33& operator=(const Mat33& m);
  Mat33& operator=(Mat33&& m);
  Mat33 clone() const;
  std::unique_ptr<Mat33> cloneToNewObject() const;
  friend std::ostream& operator<<(std::ostream& os, const Mat33& m);

  Mat33& set(float e00 = 1.f, float e01 = 0.f, float e02 = 0.f, //
             float e10 = 0.f, float e11 = 1.f, float e12 = 0.f, //
             float e20 = 0.f, float e21 = 0.f, float e22 = 1.f);
  Mat33& init(float e00 = 1.f, float e01 = 0.f, float e02 = 0.f, //
              float e10 = 0.f, float e11 = 1.f, float e12 = 0.f, //
              float e20 = 0.f, float e21 = 0.f, float e22 = 1.f);
  Mat33& identity();
  Mat33& multiply(float s);
  Mat33& multiplyScalar(float s);
  Mat33& add(const Mat33& m);
  Mat33& add(const Mat33& m1, const Mat33& m2);
  Mat33& addEqual(const Mat33& m);
  Mat33& sub(const Mat33& m1, const Mat33& m2);
  Mat33& subEqual(const Mat33& m);
  Mat33& scale(const Mat33& m, float s);
  Mat33& scaleEqual(float s);
  Mat33& mul(const Mat33& m1, const Mat33& m2, bool transpose = false);
  Mat33& multiplyMatrices(const Mat33& m1, const Mat33& m2,
                          bool transpose = false);
  Mat33& mulScale(const Mat33& m, float sx, float sy, float sz,
                  bool prepend = false);
  Mat33& mulRotate(const Mat33& m, float rad, float ax, float ay, float az,
                   bool prepend = false);
  Mat33& transpose();
  Mat33& transpose(const Mat33& m);
  Mat33& setQuat(const Quat& q);
  Mat33& invert(const Mat33& m);
  Mat33& addOffset(float m, const Vec3& v);
  Mat33& subOffset(float m, const Vec3& v);
  Mat33& copy(const Mat33& m);
  float determinant() const;
  Mat33& fromArray(const std::array<float, 9>& array);
  std::array<float, 9> toArray() const;
  std::string toString() const;

public:
  std::array<float, 9> elements;

}; // end of class Mat33

} // end of namespace OIMO

#endif // end of OIMO_MATH_MAT33_H
