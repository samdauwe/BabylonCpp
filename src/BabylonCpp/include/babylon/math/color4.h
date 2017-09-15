#ifndef BABYLON_MATH_COLOR4_H
#define BABYLON_MATH_COLOR4_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a color4.
 */
class BABYLON_SHARED_EXPORT Color4 {

public:
  /**
   * @brief Creates a new Color4 object from the passed float values ( < 1) :
   * red, green, blue, alpha.
   */
  Color4(float red = 0.f, float green = 0.f, float blue = 0.f,
         float alpha = 1.f);
  Color4(const Color4& otherColor);
  Color4(Color4&& otherColor);
  Color4& operator=(const Color4& otherColor);
  Color4& operator=(Color4&& otherColor);
  ~Color4();
  Color4 copy() const;

  /**
   * @brief Creates a new Color4 copied from the current one.
   */
  unique_ptr_t<Color4> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Color4& color);

  /**
   * @brief Returns the string "Color4"
   */
  const char* getClassName() const;

  /**
   * @brief Return the Color4 hash code as a number.
   */
  int getHashCode() const;

  /** Operators **/

  /**
   * @brief Adds in place the passed Color4 values to the current Color4.
   * @returns The updated Color4.
   */
  Color4& addInPlace(const Color4& right);

  /**
   * @brief Returns a new array populated with 4 numeric elements : red, green,
   * blue, alpha values.
   */
  const Float32Array asArray() const;

  /**
   * @brief Stores from the starting index in the passed array the Color4
   * successive values.
   * @returns The Color4.
   */
  const Color4& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Returns a new Color4 set with the added values of the current Color4
   * and of the passed one.
   */
  Color4 add(const Color4& right) const;

  /**
   * @brief Returns a new Color4 set with the subtracted values of the passed
   * one from the current Color4.
   */
  Color4 subtract(const Color4& right) const;

  /**
   * @brief Subtracts the passed ones from the current Color4 values and stores
   * the results in "result".
   * @returns The Color4.
   */
  const Color4& subtractToRef(const Color4& right, Color4& result) const;

  /**
   * @brief Creates a new Color4 with the current Color4 values multiplied by
   * scale.
   */
  Color4 scale(float iscale) const;

  /**
   * @brief Multiplies the current Color4 values by scale and stores the result
   * in "result".
   * @returns The Color4.
   */
  const Color4& scaleToRef(int iscale, Color4& result) const;
  const Color4& scaleToRef(float iscale, Color4& result) const;

  /**
   * @brief Multipy an RGBA Color4 value by another and return a new Color4
   * object
   * @param color The Color4 (RGBA) value to multiply by
   * @returns A new Color4.
   */
  Color4 multiply(const Color4& color) const;

  /**
   * @brief Multipy an RGBA Color4 value by another and push the result in a
   * reference value
   * @param color The Color4 (RGBA) value to multiply by
   * @param result The Color4 (RGBA) to fill the result in
   * @returns The result Color4.
   */
  const Color4 multiplyToRef(const Color4& color, Color4& result) const;

  /**
   * @brief Returns true if the rgb values are equal to the passed ones.
   */
  bool equals(const Color4& otherColor) const;

  /**
   * @brief Returns a string with the Color4 values.
   */
  string_t toString() const;

  /**
   * @brief Copies the passed Color4 values into the current one.
   * @returns The updated Color4.
   */
  Color4& copyFrom(const Color4& source);

  /**
   * @brief Copies the passed float values into the current one.
   * @returns The updated Color4.
   */
  Color4& copyFromFloats(float red, float green, float blue, float alpha);

  /**
   * @brief Copies the passed float values into the current one.
   * @returns The updated Color4.
   */
  Color4& set(float red, float green, float blue, float alpha);

  /**
   * @brief Returns a string containing the hexadecimal Color4 code.
   */
  string_t toHexString() const;

  /**
   * @brief Returns a new Color4 converted to linear space.
   */
  Color4 toLinearSpace() const;

  /**
   * @brief Converts the Color4 values to linear space and stores the result in
   * "convertedColor".
   * @returns The unmodified Color4.
   */
  const Color4& toLinearSpaceToRef(Color4& convertedColor) const;

  /**
   * @brief Returns a new Color4 converted to gamma space.
   */
  Color4 toGammaSpace() const;

  /**
   * @brief Converts the Color4 values to gamma space and stores the result in
   * "convertedColor".
   * @returns The unmodified Color4.
   */
  const Color4& toGammaSpaceToRef(Color4& convertedColor) const;

  /** Operator overloading **/
  friend std::ostream& operator<<(std::ostream& os, const Color4& color);
  friend bool operator==(const Color4& lhs, const Color4& rhs);
  friend bool operator!=(const Color4& lhs, const Color4& rhs);
  Color4 operator+(const Color4& right) const;
  Color4& operator+=(const Color4& right);
  Color4 operator-(const Color4& right) const;
  Color4 operator*(float iscale) const;

  /** Statics **/

  /**
   * @brief Creates a new Color4 from the valid hexadecimal value contained in
   * the passed string.
   */
  static Color4 FromHexString(const string_t& hex);

  /**
   * @brief Creates a new Color4 object set with the linearly interpolated
   * values of "amount" between the left Color4 and the right Color4.
   */
  static Color4 Lerp(const Color4& left, const Color4& right, float amount);

  /**
   * @brief Set the passed "result" with the linearly interpolated values of
   * "amount" between the left Color4 and the right Color4.
   */
  static void LerpToRef(const Color4& left, const Color4& right, float amount,
                        Color4& result);

  /**
   * @brief Creates a new Color4 from the starting index element of the passed
   * array.
   */
  static Color4 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Creates a new Color4 from the passed integers ( < 256 ).
   */
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
bool operator==(const Color4& lhs, const Color4& rhs);
bool operator!=(const Color4& lhs, const Color4& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_COLOR4_H
