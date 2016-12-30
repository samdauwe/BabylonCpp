#ifndef BABYLON_MATH_SIZE_H
#define BABYLON_MATH_SIZE_H

#include <babylon/babylon_global.h>
#include <babylon/math/isize.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Size : public ISize {

public:
  Size(int width = 0, int height = 0);
  Size(const Size& otherSize);
  Size(Size&& otherSize);
  ~Size();
  Size& operator=(const Size& otherSize);
  Size& operator=(Size&& otherSize);
  Size copy() const;
  std::unique_ptr<Size> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Size& size);
  std::string toString() const;
  const char* getClassName() const;
  int getHashCode() const;
  void copyFrom(const Size& src);
  void copyFromFloats(float width, float height);
  Size multiplyByFloats(float w, float h) const;

  bool equals(const Size& otherSize);
  int surface() const;
  static Size Zero();
  Size add(const Size& otherSize);
  Size subtract(const Size& otherSize);
  Size scale(float scale) const;
  static Size Lerp(const Size& start, const Size& end, float amount);

}; // end of struct Size

std::ostream& operator<<(std::ostream& os, const Size& size);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SIZE_H
