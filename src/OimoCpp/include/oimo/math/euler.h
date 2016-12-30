#ifndef OIMO_MATH_EULER_H
#define OIMO_MATH_EULER_H

#include <functional>

namespace OIMO {

class Mat33;
class Quat;

class Euler {

public:
  enum class RotationOrder {
    XYZ = 0,
    YZX = 1,
    ZXY = 2,
    XZY = 3,
    YXZ = 4,
    ZYX = 5
  };

public:
  Euler(float x = 0.f, float y = 0.f, float z = 0.f,
        const RotationOrder& order = RotationOrder::XYZ);
  Euler(const Euler& e);
  Euler(Euler&& e);
  ~Euler();
  Euler& operator=(const Euler& e);
  Euler& operator=(Euler&& e);
  Euler clone() const;
  Euler* cloneToNewObject() const;

  float x() const;
  void setX(float value);
  float Y() const;
  void setY(float value);
  float Z() const;
  void setZ(float value);
  RotationOrder order() const;
  void setOrder(const RotationOrder& value);

  Euler& set(float x, float y, float z);
  Euler& set(float x, float y, float z, const RotationOrder& order);
  Euler& copy(const Euler& euler);
  Euler& setFromRotationMatrix(const Mat33& m);
  Euler& setFromRotationMatrix(const Mat33& m, const RotationOrder& order);
  Euler& setFromQuaternion(const Quat& q, bool update = false);
  Euler& setFromQuaternion(const Quat& q, const RotationOrder& order,
                           bool update = false);
  bool equals(const Euler& euler) const;
  Euler& fromArray3(std::array<float, 3> array);
  Euler& fromArray4(std::array<float, 4> array);
  std::array<float, 4> toArray() const;
  Euler& onChange(const std::function<void()>& callback);

  static int RotationOrderToNumber(const RotationOrder& order);
  static RotationOrder NumberToRotationOrder(int number);

public:
  std::function<void()> onChangeCallback;

private:
  float _x;
  float _y;
  float _z;
  RotationOrder _order;

}; // end of struct Axis

} // end of namespace OIMO

#endif // end of OIMO_MATH_EULER_H
