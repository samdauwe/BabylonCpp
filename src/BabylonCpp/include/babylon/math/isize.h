#ifndef BABYLON_MATH_ISIZE_H
#define BABYLON_MATH_ISIZE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ISize {

  ISize();
  ISize(int width, int height);
  ~ISize();

  int width;
  int height;

}; // end of struct ISize

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_ISIZE_H
