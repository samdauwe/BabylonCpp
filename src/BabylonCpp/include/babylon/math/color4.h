#ifndef BABYLON_MATH_COLOR4_H
#define BABYLON_MATH_COLOR4_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Color3;

/**
 * @brief Class used to hold a RBGA color.
 */
class BABYLON_SHARED_EXPORT Color4 {

public:
  /**
   * @brief Creates a new Color4 object from red, green, blue values, all
   * between 0 and 1.
   * @param r defines the red component (between 0 and 1, default is 0)
   * @param g defines the green component (between 0 and 1, default is 0)
   * @param b defines the blue component (between 0 and 1, default is 0)
   * @param a defines the alpha component (between 0 and 1, default is 1)
   */
  Color4(float red = 0.f, float green = 0.f, float blue = 0.f,
         float alpha = 1.f);
  Color4(const Color3& otherColor);
  Color4(const Color4& otherColor);
  Color4(Color3&& otherColor);
  Color4(Color4&& otherColor);
  Color4& operator=(const Color3& otherColor);
  Color4& operator=(Color3&& otherColor);
  Color4& operator=(const Color4& otherColor);
  Color4& operator=(Color4&& otherColor);
  ~Color4(); // = default
  Color4 copy() const;

  /**
   * @brief Creates a new Color4 copied from the current one.
   * @returns a new Color4 object
   */
  std::unique_ptr<Color4> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Color4& color);

  /**
   * Returns the string "Color4".
   * @returns "Color4"
   */
  const char* getClassName() const;

  /**
   * @brief Compute the Color4 hash code.
   * @returns an unique number that can be used to hash Color4 objects
   */
  int getHashCode() const;

  /** Operators **/

  /**
   * @brief Adds in place the given Color4 values to the current Color4 object.
   * @param right defines the second operand
   * @returns the current updated Color4 object
   */
  Color4& addInPlace(const Color4& right);

  /**
   * @brief Creates a new array populated with 4 numeric elements : red, green,
   * blue, alpha values.
   * @returns the new array
   */
  Float32Array asArray() const;

  /**
   * @brief Stores from the starting index in the given array the Color4
   * successive values.
   * @param array defines the array where to store the r,g,b components
   * @param index defines an optional index in the target array to define where
   * to start storing values
   * @returns the current Color4 object
   */
  const Color4& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Determines equality between Color4 objects.
   * @param otherColor defines the second operand
   * @returns true if the rgba values are equal to the given ones
   */
  bool equals(const Color4& otherColor) const;

  /**
   * @brief Creates a new Color4 set with the added values of the current Color4
   * and of the given one.
   * @param right defines the second operand
   * @returns a new Color4 object
   */
  Color4 add(const Color4& right) const;

  /**
   * @brief Creates a new Color4 set with the subtracted values of the given one
   * from the current Color4.
   * @param right defines the second operand
   * @returns a new Color4 object
   */
  Color4 subtract(const Color4& right) const;

  /**
   * @brief Subtracts the given ones from the current Color4 values and stores
   * the results in "result".
   * @param right defines the second operand
   * @param result defines the Color4 object where to store the result
   * @returns the current Color4 object
   */
  const Color4& subtractToRef(const Color4& right, Color4& result) const;

  /**
   * @brief Creates a new Color4 with the current Color4 values multiplied by
   * scale.
   * @param scale defines the scaling factor to apply
   * @returns a new Color4 object
   */
  Color4 scale(float iscale) const;

  /**
   * @brief Multiplies the current Color4 values by scale and stores the result
   * in "result".
   * @param scale defines the scaling factor to apply
   * @param result defines the Color4 object where to store the result
   * @returns the current unmodified Color4
   */
  const Color4& scaleToRef(int iscale, Color4& result) const;

  /**
   * @brief Multiplies the current Color4 values by scale and stores the result
   * in "result".
   * @param scale defines the scaling factor to apply
   * @param result defines the Color4 object where to store the result
   * @returns the current unmodified Color4
   */
  const Color4& scaleToRef(float iscale, Color4& result) const;

  /**
   * @brief Scale the current Color4 values by a factor and add the result to a
   * given Color4
   * @param scale defines the scale factor
   * @param result defines the Color4 object where to store the result
   * @returns the unmodified current Color4
   */
  const Color4& scaleAndAddToRef(float scale, Color4& result) const;

  /**
   * @brief Clamps the rgb values by the min and max values and stores the
   * result into "result".
   * @param min defines minimum clamping value (default is 0)
   * @param max defines maximum clamping value (default is 1)
   * @param result defines color to store the result into.
   * @returns the cuurent Color4
   */
  const Color4& clampToRef(Color4& result, float min = 0.f,
                           float max = 1.f) const;

  /**
   * @brief Multipy an Color4 value by another and return a new Color4 object.
   * @param color defines the Color4 value to multiply by
   * @returns a new Color4 object
   */
  Color4 multiply(const Color4& color) const;

  /**
   * @brief Multipy an RGBA Color4 value by another and push the result in a
   * reference value.
   * @param color The Color4 (RGBA) value to multiply by
   * @param result The Color4 (RGBA) to fill the result in
   * @returns The result Color4.
   */
  Color4 multiplyToRef(const Color4& color, Color4& result) const;

  /**
   * @brief Creates a string with the Color4 current values.
   * @returns the string representation of the Color4 object
   */
  std::string toString() const;

  /**
   * @brief Copies the given Color4 values into the current one.
   * @param source defines the source Color4 object
   * @returns the current updated Color4 object
   */
  Color4& copyFrom(const Color4& source);

  /**
   * @brief Copies the given float values into the current one.
   * @param r defines the red component to read from
   * @param g defines the green component to read from
   * @param b defines the blue component to read from
   * @param a defines the alpha component to read from
   * @returns the current updated Color4 object
   */
  Color4& copyFromFloats(float red, float green, float blue, float alpha);

  /**
   * Copies the given float values into the current one.
   * @param r defines the red component to read from
   * @param g defines the green component to read from
   * @param b defines the blue component to read from
   * @param a defines the alpha component to read from
   * @returns the current updated Color4 object
   */
  Color4& set(float red, float green, float blue, float alpha);

  /**
   * @brief Compute the Color4 hexadecimal code as a string.
   * @returns a string containing the hexadecimal representation of the Color4
   * object
   */
  std::string toHexString() const;

  /**
   * @brief Computes a new Color4 converted from the current one to linear
   * space.
   * @returns a new Color4 object
   */
  Color4 toLinearSpace() const;

  /**
   * @brief Converts the Color4 values to linear space and stores the result in
   * "convertedColor".
   * @param convertedColor defines the Color4 object where to store the linear
   * space version
   * @returns the unmodified Color4
   */
  const Color4& toLinearSpaceToRef(Color4& convertedColor) const;

  /**
   * @brief Computes a new Color4 converted from the current one to gamma space.
   * @returns a new Color4 object
   */
  Color4 toGammaSpace() const;

  /**
   * @brief Converts the Color4 values to gamma space and stores the result in
   * "convertedColor".
   * @param convertedColor defines the Color4 object where to store the gamma
   * space version
   * @returns the unmodified Color4
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
   * @brief Creates a new Color4 from the string containing valid hexadecimal
   * values.
   * @param hex defines a string containing valid hexadecimal values
   * @returns a new Color4 object
   */
  static Color4 FromHexString(const std::string& hex);

  /**
   * @brief Creates a new Color4 object set with the linearly interpolated
   * values of "amount" between the left Color4 object and the right Color4
   * object.
   * @param left defines the start value
   * @param right defines the end value
   * @param amount defines the gradient factor
   * @returns a new Color4 object
   */
  static Color4 Lerp(const Color4& left, const Color4& right, float amount);

  /**
   * @brief Set the given "result" with the linearly interpolated values of
   * "amount" between the left Color4 object and the right Color4 object.
   * @param left defines the start value
   * @param right defines the end value
   * @param amount defines the gradient factor
   * @param result defines the Color4 object where to store data
   */
  static void LerpToRef(const Color4& left, const Color4& right, float amount,
                        Color4& result);

  /**
   * @brief Creates a new Color4 from a Color3 and an alpha value.
   * @param color3 defines the source Color3 to read from
   * @param alpha defines the alpha component (1.0 by default)
   * @returns a new Color4 object
   */
  static Color4 FromColor3(const Color3& color3, float alpha = 1.f);

  /**
   * @brief Creates a new Color4 from the starting index element of the given
   * array.
   * @param array defines the source array to read from
   * @param offset defines the offset in the source array
   * @returns a new Color4 object
   */
  static Color4 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Creates a new Color3 from integer values (< 256).
   * @param r defines the red component to read from (value between 0 and 255)
   * @param g defines the green component to read from (value between 0 and 255)
   * @param b defines the blue component to read from (value between 0 and 255)
   * @param a defines the alpha component to read from (value between 0 and 255)
   * @returns a new Color3 object
   */
  static Color4 FromInts(int red, int green, int blue, int alpha);

  /**
   * @brief Check the content of a given array and convert it to an array
   * containing RGBA data. If the original array was already containing count *
   * 4 values then it is returned directly
   * @param colors defines the array to check
   * @param count defines the number of RGBA data to expect
   * @returns an array containing count * 4 values (RGBA)
   */
  static Float32Array CheckColors4(const Float32Array& colors, size_t count);

public:
  /**
   * Defines the red component (between 0 and 1, default is 0)
   */
  float r;

  /**
   * Defines the green component (between 0 and 1, default is 0)
   */
  float g;

  /**
   * Defines the blue component (between 0 and 1, default is 0)
   */
  float b;

  /**
   * Defines the alpha component (between 0 and 1, default is 1)
   */
  float a;

}; // end of class Color4

std::ostream& operator<<(std::ostream& os, const Color4& color);
bool operator==(const Color4& lhs, const Color4& rhs);
bool operator!=(const Color4& lhs, const Color4& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_COLOR4_H
