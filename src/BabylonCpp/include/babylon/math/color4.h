#ifndef BABYLON_MATH_COLOR4_H
#define BABYLON_MATH_COLOR4_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a color4.
 */
class BABYLON_SHARED_EXPORT Color4 {

public:
  Color4(float red = 0.f, float green = 0.f, float blue = 0.f,
         float alpha = 0.f);
  Color4(const Color4& otherColor);
  Color4(Color4&& otherColor);
  Color4& operator=(const Color4& otherColor);
  Color4& operator=(Color4&& otherColor);
  ~Color4();
  Color4 copy() const;
  std::unique_ptr<Color4> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Color4& color);
  const char* getClassName() const;
  int getHashCode() const;

  /** Operators **/
  Color4& addInPlace(const Color4& right);
  Float32Array asArray();
  Color4& toArray(Float32Array& array, unsigned int index = 0);
  Color4 add(const Color4& right) const;
  Color4 subtract(const Color4& right) const;
  Color4& subtractToRef(const Color4& right, Color4& result);
  Color4 scale(float iscale) const;
  Color4& scaleToRef(int iscale, Color4& result);
  Color4& scaleToRef(float iscale, Color4& result);

  /**
   * Multipy an RGBA Color4 value by another and return a new Color4
   * object
   * @param color The Color4 (RGBA) value to multiply by
   * @returns A new Color4.
   */
  Color4 multiply(const Color4& color) const;

  /**
   * Multipy an RGBA Color4 value by another and push the result in a reference
   * value
   * @param color The Color4 (RGBA) value to multiply by
   * @param result The Color4 (RGBA) to fill the result in
   * @returns the result Color4.
   */
  Color4 multiplyToRef(const Color4& color, Color4& result);

  std::string toString() const;
  Color4& copyFrom(const Color4& source);
  std::string toHexString() const;

  /** Operator overloading **/
  Color4 operator+(const Color4& right) const;
  Color4& operator+=(const Color4& right);
  Color4 operator-(const Color4& right) const;
  Color4 operator*(float iscale) const;

  /** Statics **/
  static Color4 FromHexString(const std::string& hex);
  static Color4 Lerp(const Color4& left, const Color4& right, float amount);
  static void LerpToRef(const Color4& left, const Color4& right, float amount,
                        Color4& result);
  static Color4 FromArray(const Float32Array& array, unsigned int offset = 0);
  static Color4 FromInts(int red, int green, int blue, int alpha);
  static Float32Array CheckColors4(const Float32Array& colors, size_t count);

public:
  // Red channel value between 0 and 1. Default is 0.
  float r;
  // Green channel value between 0 and 1. Default is 0.
  float g;
  // Blue channel value between 0 and 1. Default is 0.
  float b;
  // Alpha channel value between 0 and 1. Default is 0.
  float a;

}; // end of class Color4

std::ostream& operator<<(std::ostream& os, const Color4& color);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_COLOR4_H
