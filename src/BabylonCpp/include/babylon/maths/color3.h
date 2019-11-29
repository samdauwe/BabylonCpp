#ifndef BABYLON_MATHS_COLOR3_H
#define BABYLON_MATHS_COLOR3_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Color4;

/**
 * @brief Class used to hold a RBG color.
 */
class BABYLON_SHARED_EXPORT Color3 {

private:
  static const Color3 _BlackReadOnly;

public:
  /**
   * @brief Creates a new Color3 object from red, green, blue values, all
   * between 0 and 1.
   * @param r defines the red component (between 0 and 1, default is 0)
   * @param g defines the green component (between 0 and 1, default is 0)
   * @param b defines the blue component (between 0 and 1, default is 0)
   */
  Color3(float red = 0.f, float green = 0.f, float blue = 0.f);
  Color3(const Color3& otherColor);            // Copy constructor
  Color3(Color3&& otherColor);                 // Move constructor
  Color3& operator=(const Color3& otherColor); // Copy assignment operator
  Color3& operator=(const Color4& otherColor); // Copy assignment operator
  Color3& operator=(Color3&& otherColor);      // Move assignment operator
  ~Color3();                                   // = default
  [[nodiscard]] Color3 copy() const;

  /**
   * @brief Copy the current object.
   * @returns a new Color3 copied the current one
   */
  [[nodiscard]] std::unique_ptr<Color3> clone() const;

  /**
   * @brief Creates a string with the Color3 current values.
   * @returns the string representation of the Color3 object
   */
  [[nodiscard]] std::string toString() const;

  /**
   * @brief Returns the string "Color3".
   */
  [[nodiscard]] const char* getClassName() const;

  /**
   * @brief Compute the Color3 hash code.
   * @returns an unique number that can be used to hash Color3 objects
   */
  [[nodiscard]] int getHashCode() const;

  /** Operators **/

  /**
   * @brief Stores in the given array from the given starting index the red,
   * green, blue values as successive elements.
   * @param array defines the array where to store the r,g,b components
   * @param index defines an optional index in the target array to define where
   * to start storing values
   * @returns the current Color3 object
   */
  const Color3& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Returns a new Color4 object from the current Color3 and the given
   * alpha
   * @param alpha defines the alpha component on the new Color4 object (default
   * is 1)
   * @returns a new Color4 object
   */
  [[nodiscard]] Color4 toColor4(float alpha = 1.f) const;

  /**
   * @brief Returns a new array populated with 3 numeric elements : red, green
   * and blue values.
   * @returns the new array
   */
  [[nodiscard]] Float32Array asArray() const;

  /**
   * @brief Returns the luminance value.
   * @returns a float value
   */
  [[nodiscard]] float toLuminance() const;

  /**
   * @brief Multiply each Color3 rgb values by the given Color3 rgb values in a
   * new Color3 object.
   * @param otherColor defines the second operand
   * @returns the new Color3 object
   */
  [[nodiscard]] Color3 multiply(const Color3& otherColor) const;

  /**
   * @brief Multiply the rgb values of the Color3 and the given Color3 and
   * stores the result in the object "result".
   * @param otherColor defines the second operand
   * @param result defines the Color3 object where to store the result
   * @returns the current Color3
   */
  const Color3& multiplyToRef(const Color3& otherColor, Color3& result) const;

  /**
   * @brief Determines equality between Color3 objects
   * @param otherColor defines the second operand
   * @returns true if the rgb values are equal to the given ones
   */
  [[nodiscard]] bool equals(const Color3& otherColor) const;

  /**
   * @brief Determines equality between the current Color3 object and a set of
   * r,b,g values.
   * @param r defines the red component to check
   * @param g defines the green component to check
   * @param b defines the blue component to check
   * @returns true if the rgb values are equal to the given ones
   */
  [[nodiscard]] bool equalsFloats(float red, float green, float blue) const;

  /**
   * @brief Multiplies in place each rgb value by scale.
   * @param scale defines the scaling factor
   * @returns the updated Color3.
   */
  [[nodiscard]] Color3 scale(float iscale) const;

  /**
   * @brief Multiplies the rgb values by scale and stores the result into
   * "result".
   * @param scale defines the scaling factor
   * @param result defines the Color3 object where to store the result
   * @returns the unmodified current Color3.
   */
  const Color3& scaleToRef(float iscale, Color3& result) const;

  /**
   * @brief Scale the current Color3 values by a factor and add the result to a
   * given Color3.
   * @param scale defines the scale factor
   * @param result defines color to store the result into
   * @returns the unmodified current Color3
   */
  const Color3& scaleAndAddToRef(float scale, Color3& result) const;

  /**
   * @brief Clamps the rgb values by the min and max values and stores the
   * result into "result".
   * @param result defines color to store the result into
   * @param min defines minimum clamping value (default is 0)
   * @param max defines maximum clamping value (default is 1)
   * @returns the original Color3
   */
  const Color3& clampToRef(Color3& result, float min = 0.f, float max = 1.f) const;

  /**
   * @brief Creates a new Color3 set with the added values of the current Color3
   * and of the given one.
   * @param otherColor defines the second operand
   * @returns the new Color3
   */
  [[nodiscard]] Color3 add(const Color3& otherColor) const;

  /**
   * @brief Stores the result of the addition of the current Color3 and given
   * one rgb values into "result".
   * @param otherColor defines the second operand
   * @param result defines Color3 object to store the result into
   * @returns the unmodified current Color3
   */
  const Color3& addToRef(const Color3& otherColor, Color3& result) const;

  /**
   * @brief Returns a new Color3 set with the subtracted values of the given one
   * from the current Color3.
   * @param otherColor defines the second operand
   * @returns the new Color3
   */
  [[nodiscard]] Color3 subtract(const Color3& otherColor) const;

  /**
   * @brief Stores the result of the subtraction of given one from the current
   * Color3 rgb values into "result".
   * @param otherColor defines the second operand
   * @param result defines Color3 object to store the result into
   * @returns the unmodified current Color3
   */
  const Color3& subtractToRef(const Color3& otherColor, Color3& result) const;

  /**
   * @brief Copies the rgb values from the source in the current Color3.
   * @param source defines the source Color3 object
   * @returns the updated Color3 object
   */
  Color3& copyFrom(const Color3& source);

  /**
   * @brief Updates the Color3 rgb values from the given floats.
   * @param r defines the red component to read from
   * @param g defines the green component to read from
   * @param b defines the blue component to read from
   * @returns the current Color3 object
   */
  Color3& copyFromFloats(float red, float green, float blue);

  /**
   * @brief Updates the Color3 rgb values from the given floats.
   * @param r defines the red component to read from
   * @param g defines the green component to read from
   * @param b defines the blue component to read from
   * @returns the current Color3 object
   */
  Color3& set(float red, float green, float blue);

  /**
   * @brief Compute the Color3 hexadecimal code as a string.
   * @returns a string containing the hexadecimal representation of the Color3
   * object
   */
  [[nodiscard]] std::string toHexString() const;

  /**
   * @brief Computes a new Color3 converted from the current one to linear
   * space.
   * @returns a new Color3 object
   */
  Color3 toLinearSpace();

  /**
   * @brief Converts the Color3 values to linear space and stores the result in
   * "convertedColor".
   * @param convertedColor defines the Color3 object where to store the linear
   * space version
   * @returns the unmodified Color3
   */
  Color3& toLinearSpaceToRef(Color3& convertedColor);

  /**
   * @brief Computes a new Color3 converted from the current one to gamma space.
   * @returns a new Color3 object
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
  friend bool operator==(const Color3& lhs, const Color3& rhs);
  friend bool operator!=(const Color3& lhs, const Color3& rhs);
  Color3 operator+(const Color3& otherColor) const;
  Color3 operator-(const Color3& otherColor) const;
  Color3 operator*(float iscale) const;
  Color3 operator*(const Color3& otherColor) const;
  Color3 operator/(float iscale) const;

  /** Statics **/

  /**
   * @brief Creates a new Color3 from the string containing valid hexadecimal
   * values.
   * @param hex defines a string containing valid hexadecimal values
   * @returns a new Color3 object
   */
  static Color3 FromHexString(const std::string& hex);

  /**
   * @brief Creates a new Color3 from the starting index of the given array.
   * @param array defines the source array
   * @param offset defines an offset in the source array
   * @returns a new Color3 object
   */
  static Color3 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Creates a new Color3 from an integer value.
   * @param rgb defines the rgb component to read from
   * @returns a new Color3 object
   */
  static Color3 FromInt(int rgb);

  /**
   * @brief Creates a new Color3 from integer values (< 256).
   * @param r defines the red component to read from (value between 0 and 255)
   * @param g defines the green component to read from (value between 0 and 255)
   * @param b defines the blue component to read from (value between 0 and 255)
   * @returns a new Color3 object
   */
  static Color3 FromInts(int r, int g, int b);

  /**
   * @brief Creates a new Color3 with values linearly interpolated of "amount"
   * between the start Color3 and the end Color3.
   * @param start defines the start Color3 value
   * @param end defines the end Color3 value
   * @param amount defines the gradient value between start and end
   * @returns a new Color3 object
   */
  static Color3 Lerp(const Color3& start, const Color3& end, float amount);

  /**
   * @brief Creates a new Color3 with values linearly interpolated of "amount"
   * between the start Color3 and the end Color3.
   * @param left defines the start value
   * @param right defines the end value
   * @param amount defines the gradient factor
   * @param result defines the Color3 object where to store the result
   */
  static void LerpToRef(const Color3& left, const Color3& right, float amount, Color3& result);

  /**
   * @brief Returns a Color3 value containing a red color.
   * @returns a new Color3 object
   */
  static Color3 Red();

  /**
   * @brief Returns a Color3 value containing a green color.
   * @returns a new Color3 object
   */
  static Color3 Green();

  /**
   * @brief Returns a Color3 value containing a blue color.
   * @returns a new Color3 object
   */
  static Color3 Blue();

  /**
   * @brief Returns a Color3 value containing a black color.
   * @returns a new Color3 object
   */
  static Color3 Black();

  /**
   * @brief Gets a Color3 value containing a black color that must not be
   * updated.
   */
  static Color3 BlackReadOnly();

  /**
   * @brief Returns a Color3 value containing a white color.
   * @returns a new Color3 object
   */
  static Color3 White();

  /**
   * @brief Returns a Color3 value containing a purple color.
   * @returns a new Color3 object
   */
  static Color3 Purple();

  /**
   * @brief Returns a Color3 value containing a magenta color.
   * @returns a new Color3 object
   */
  static Color3 Magenta();

  /**
   * @brief Returns a Color3 value containing a yellow color.
   * @returns a new Color3 object
   */
  static Color3 Yellow();

  /**
   * @brief Returns a Color3 value containing a cyan color.
   * @returns a new Color3 object
   */
  static Color3 Cyan();

  /**
   * @brief Returns a Color3 value containing a gray color.
   * @returns a new Color3 object
   */
  static Color3 Gray();

  /**
   * @brief Returns a Color3 value containing a teal color.
   * @returns a new Color3 object
   */
  static Color3 Teal();

  /**
   * @brief Returns a Color3 value containing a random color.
   * @returns a new Color3 object
   */
  static Color3 Random();

public:
  /**
   * Defines the red component (between 0 and 1, default is 0).
   */
  float r;

  /**
   * Defines the green component (between 0 and 1, default is 0) .
   */
  float g;

  /**
   * Defines the blue component (between 0 and 1, default is 0).
   */
  float b;

}; // end of class Color3

std::ostream& operator<<(std::ostream& os, const Color3& color);
bool operator==(const Color3& lhs, const Color3& rhs);
bool operator!=(const Color3& lhs, const Color3& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MATHS_COLOR3_H
