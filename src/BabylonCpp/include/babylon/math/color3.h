#ifndef BABYLON_MATH_COLOR3_H
#define BABYLON_MATH_COLOR3_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a Color3.
 */
class BABYLON_SHARED_EXPORT Color3 {

public:
  /**
   * @brief Creates a new Color3 object from red, green, blue values, all
   * between 0 and 1.
   */
  Color3(float red = 0.f, float green = 0.f, float blue = 0.f);
  Color3(const Color3& otherColor);            // Copy constructor
  Color3(Color3&& otherColor);                 // Move constructor
  Color3& operator=(const Color3& otherColor); // Copy assignment operator
  Color3& operator=(Color3&& otherColor);      // Move assignment operator
  ~Color3();                                   // Destructor
  Color3 copy() const;

  /**
   * @brief Returns a new Color3 copied the current one.
   */
  std::unique_ptr<Color3> clone() const;

  /**
   * @brief Returns a string with the Color3 current values.
   */
  std::string toString() const;

  /**
   * @brief Returns the string "Color3".
   */
  const char* getClassName() const;

  /**
   * @brief Returns the Color3 hash code.
   */
  int getHashCode() const;

  /** Operators **/

  /**
   * @brief Stores in the passed array from the passed starting index the red,
   * green, blue values as successive elements.
   * @returns The Color3.
   */
  const Color3& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Returns a new Color4 object from the current Color3 and the passed
   * alpha.
   */
  Color4 toColor4(float alpha = 1) const;
  Float32Array asArray() const;

  /**
   * @brief Returns the luminance value (float).
   */
  float toLuminance() const;

  /**
   * @brief Multiply each Color3 rgb values by the passed Color3 rgb values in a
   * new Color3 object.
   * @returns This new object.
   */
  Color3 multiply(const Color3& otherColor) const;

  /**
   * @brief Multiply the rgb values of the Color3 and the passed Color3 and
   * stores the result in the object "result".
   * @returns The current Color3.
   */
  const Color3& multiplyToRef(const Color3& otherColor, Color3& result) const;

  /**
   * @brief Returns if the rgb values are equal to the passed ones.
   */
  bool equals(const Color3& otherColor) const;

  /**
   * @brief Returns if the rgb values are equal to the passed ones.
   */
  bool equalsFloats(float red, float green, float blue) const;

  /**
   * @brief Multiplies in place each rgb value by scale.
   * @returns The updated Color3.
   */
  Color3 scale(float iscale) const;

  /**
   * @brief Multiplies the rgb values by scale and stores the result into
   * "result".
   * @returns the unmodified current Color3.
   */
  const Color3& scaleToRef(float iscale, Color3& result) const;

  /**
   * @brief Returns a new Color3 set with the added values of the current Color3
   * and of the passed one.
   */
  Color3 add(const Color3& otherColor) const;

  /**
   * @brief Stores the result of the addition of the current Color3 and passed
   * one rgb values into "result".
   * @returns The unmodified current Color3.
   */
  const Color3& addToRef(const Color3& otherColor, Color3& result) const;

  /**
   * @brief Returns a new Color3 set with the subtracted values of the passed
   * one from the current Color3.
   */
  Color3 subtract(const Color3& otherColor) const;

  /**
   * @brief Stores the result of the subtraction of passed one from the current
   * Color3 rgb values into "result".
   * @returns The unmodified current Color3.
   */
  const Color3& subtractToRef(const Color3& otherColor, Color3& result) const;

  /**
   * @brief Copies the rgb values from the source in the current Color3.
   * @returns the updated Color3.
   */
  Color3& copyFrom(const Color3& source);

  /**
   * @brief Updates the Color3 rgb values from the passed floats.
   * @returns The Color3.
   */
  Color3& copyFromFloats(float red, float green, float blue);

  /**
   * @brief Updates the Color3 rgb values from the passed floats.
   * @returns The Color3.
   */
  Color3& set(float red, float green, float blue);

  /**
   * @brief Returns the Color3 hexadecimal code as a string.
   */
  std::string toHexString() const;

  /**
   * @brief Returns a new Color3 converted to linear space.
   */
  Color3 toLinearSpace();

  /**
   * @brief Converts the Color3 values to linear space and stores the result in
   * "convertedColor".
   * @returns The unmodified Color3.
   */
  Color3& toLinearSpaceToRef(Color3& convertedColor);

  /**
   * @brief Returns a new Color3 converted to gamma space.
   */
  Color3 toGammaSpace();

  /**
   * @brief Converts the Color3 values to gamma space and stores the result in
   * "convertedColor".
   * @returns The unmodified Color3.
   */
  Color3& toGammaSpaceToRef(Color3& convertedColor);

  /** Operator overloading **/
  friend std::ostream& operator<<(std::ostream& os, const Color3& color);
  Color3 operator+(const Color3& otherColor) const;
  Color3 operator-(const Color3& otherColor) const;
  Color3 operator*(float iscale) const;
  Color3 operator*(const Color3& otherColor) const;
  bool operator==(const Color3& otherColor) const;
  bool operator!=(const Color3& otherColor) const;

  /** Statics **/

  /**
   * @brief Creates a new Color3 from the string containing valid hexadecimal
   * values.
   */
  static Color3 FromHexString(const std::string& hex);

  /**
   * @brief Creates a new Vector3 from the start index of the passed array.
   */
  static Color3 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Creates a new Color3 from integer values ( < 256).
   */
  static Color3 FromInts(int r, int g, int b);

  /**
   * @brief Creates a new Color3 with values linearly interpolated of "amount"
   * between the start Color3 and the end Color3.
   */
  static Color3 Lerp(const Color3& start, const Color3& end, float amount);

  static Color3 Red();
  static Color3 Green();
  static Color3 Blue();
  static Color3 Black();
  static Color3 White();
  static Color3 Purple();
  static Color3 Magenta();
  static Color3 Yellow();
  static Color3 Cyan();
  static Color3 Gray();
  static Color3 Random();

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
