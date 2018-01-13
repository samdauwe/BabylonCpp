#ifndef BABYLON_MATH_ISIZE_H
#define BABYLON_MATH_ISIZE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT ISize {

  ISize();
  ISize(int width, int height);
  ISize(const ISize& other);            // Copy constructor
  ISize(ISize&& other);                 // Move constructor
  ISize& operator=(const ISize& other); // Copy assignment operator
  ISize& operator=(ISize&& other);      // Move assignment operator
  ~ISize();                             // Destructor

  /** Operator overloading **/
  friend std::ostream& operator<<(std::ostream& os, const ISize& isize);
  friend bool operator==(const ISize& lhs, const ISize& rhs);
  friend bool operator!=(const ISize& lhs, const ISize& rhs);
  ISize operator+(const ISize& other) const;
  ISize& operator+=(const ISize& other);
  ISize operator-(const ISize& other) const;
  ISize& operator-=(const ISize& other);
  ISize operator*(float scale) const;
  ISize& operator*=(float scale);
  ISize operator/(float scale) const;
  ISize& operator/=(float scale);

  int width;
  int height;

}; // end of struct ISize

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_ISIZE_H
