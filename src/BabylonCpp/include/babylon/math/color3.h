#ifndef BABYLON_MATH_COLOR3_H
#define BABYLON_MATH_COLOR3_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a Color3.
 */
class BABYLON_SHARED_EXPORT Color3 {

public:
  Color3(float red = 0.f, float green = 0.f, float blue = 0.f);
  Color3(const Color3& otherColor);            // Copy constructor
  Color3(Color3&& otherColor);                 // Move constructor
  Color3& operator=(const Color3& otherColor); // Copy assignment operator
  Color3& operator=(Color3&& otherColor);      // Move assignment operator
  ~Color3();                                   // Destructor
  Color3 copy() const;
  std::unique_ptr<Color3> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Color3& color);
  const char* getClassName() const;
  int getHashCode() const;

  /** Operators **/
  Color3& toArray(Float32Array& array, unsigned int index = 0);
  Color4 toColor4(float alpha = 1) const;
  Float32Array asArray() const;
  float toLuminance() const;
  Color3 multiply(const Color3& otherColor) const;
  Color3& multiplyToRef(const Color3& otherColor, Color3& result);
  bool equals(const Color3& otherColor) const;
  bool equalsFloats(float red, float green, float blue) const;
  Color3 scale(float iscale) const;
  Color3& scaleToRef(float iscale, Color3& result);
  Color3 add(const Color3& otherColor) const;
  Color3& addToRef(const Color3& otherColor, Color3& result);
  Color3 subtract(const Color3& otherColor) const;
  Color3& subtractToRef(const Color3& otherColor, Color3& result);
  Color3& copyFrom(const Color3& source);
  Color3& copyFromFloats(float red, float green, float blue);
  std::string toHexString() const;
  Color3 toLinearSpace();
  Color3& toLinearSpaceToRef(Color3& convertedColor);
  Color3 toGammaSpace();
  Color3& toGammaSpaceToRef(Color3& convertedColor);

  /** Operator overloading **/
  Color3 operator+(const Color3& otherColor) const;
  Color3 operator-(const Color3& otherColor) const;
  Color3 operator*(float iscale) const;
  Color3 operator*(const Color3& otherColor) const;
  bool operator==(const Color3& otherColor) const;
  bool operator!=(const Color3& otherColor) const;

  /** Statics **/
  static Color3 FromHexString(const std::string& hex);
  static Color3 FromArray(const Float32Array& array, unsigned int offset = 0);
  static Color3 FromInts(int r, int g, int b);
  static Color3 Lerp(const Color3& start, const Color3& end, float amount);
  static Color3 Red();
  static Color3 Green();
  static Color3 Blue();
  static Color3 Black();
  static Color3 White();
  static Color3 Purple();
  static Color3 Magenta();
  static Color3 Yellow();
  static Color3 Gray();

public:
  // Red channel value between 0 and 1. Default is 0.
  float r;
  // Green channel value between 0 and 1. Default is 0.
  float g;
  // Blue channel value between 0 and 1. Default is 0.
  float b;

}; // end of class Color3

std::ostream& operator<<(std::ostream& os, const Color3& color);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_COLOR3_H
