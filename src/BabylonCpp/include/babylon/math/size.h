#ifndef BABYLON_MATH_SIZE_H
#define BABYLON_MATH_SIZE_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/math/isize.h>

namespace BABYLON {

/**
 * @brief Size containing widht and height.
 */
class BABYLON_SHARED_EXPORT Size : public ISize {

public:
  /**
   * @brief Creates a Size object from the given width and height (floats).
   * @param width width of the new size
   * @param height height of the new size
   */
  Size();
  Size(int size);
  Size(int width, int height);
  Size(const Size& otherSize);
  Size(Size&& otherSize);
  ~Size();
  Size& operator=(const Size& otherSize);
  Size& operator=(Size&& otherSize);
  Size copy() const;

  /**
   * @brief Clones the size.
   * @returns a new Size copied from the given one.
   */
  std::unique_ptr<Size> clone() const;

  friend std::ostream& operator<<(std::ostream& os, const Size& size);

  /**
   * @brief Returns a string with the Size width and height.
   * @returns a string with the Size width and height
   */
  std::string toString() const;

  /**
   * @brief "Size"
   * @returns the string "Size"
   */
  const char* getClassName() const;

  /**
   * @brief Returns the Size hash code.
   * @returns a hash code for a unique width and height
   */
  int getHashCode() const;

  /**
   * @brief Updates the current size from the given one.
   * @param src the given size
   */
  void copyFrom(const Size& src);

  /**
   * @brief Updates in place the current Size from the given floats.
   * @param width width of the new size
   * @param height height of the new size
   * @returns the updated Size.
   */
  Size& copyFromFloats(float width, float height);

  /**
   * @brief Updates in place the current Size from the given floats.
   * @param width width to set
   * @param height height to set
   * @returns the updated Size.
   */
  Size& set(float width, float height);

  /**
   * @brief Multiplies the width and height by numbers.
   * @param w factor to multiple the width by
   * @param h factor to multiple the height by
   * @returns a new Size set with the multiplication result of the current Size
   * and the given floats.
   */
  Size multiplyByFloats(float w, float h) const;

  /**
   * @brief True if the current Size and the given one width and height are
   * strictly equal.
   * @param other the other size to compare against
   * @returns True if the current Size and the given one width and height are
   * strictly equal.
   */
  bool equals(const Size& otherSize);

  /**
   * @brief Returns the surface of the Size : width * height (float).
   */
  int surface() const;

  /**
   * @brief Create a new size of zero.
   * @returns a new Size set to (0.0, 0.0)
   */
  static Size Zero();

  /**
   * @brief Sums the width and height of two sizes.
   * @param otherSize size to add to this size
   * @returns a new Size set as the addition result of the current Size and the
   * given one.
   */
  Size add(const Size& otherSize) const;

  /**
   *@brief Subtracts the width and height of two.
   * @param otherSize size to subtract to this size
   * @returns a new Size set as the subtraction result of  the given one from
   *the current Size.
   */
  Size subtract(const Size& otherSize) const;

  Size scale(float scale) const;

  /**
   * @brief Creates a new Size set at the linear interpolation "amount" between
   * "start" and "end"
   * @param start starting size to lerp between
   * @param end end size to lerp between
   * @param amount amount to lerp between the start and end values
   * @returns a new Size set at the linear interpolation "amount" between
   * "start" and "end"
   */
  static Size Lerp(const Size& start, const Size& end, float amount);

}; // end of struct Size

std::ostream& operator<<(std::ostream& os, const Size& size);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SIZE_H
