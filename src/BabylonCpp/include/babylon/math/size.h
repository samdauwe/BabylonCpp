#ifndef BABYLON_MATH_SIZE_H
#define BABYLON_MATH_SIZE_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/math/isize.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Size : public ISize {

public:
  /**
   * @brief Creates a Size object from the given width and height (floats).
   */
  Size(int width = 0, int height = 0);
  Size(const Size& otherSize);
  Size(Size&& otherSize);
  ~Size();
  Size& operator=(const Size& otherSize);
  Size& operator=(Size&& otherSize);
  Size copy() const;

  /**
   * @brief Returns a new Size copied from the given one.
   */
  std::unique_ptr<Size> clone() const;

  friend std::ostream& operator<<(std::ostream& os, const Size& size);

  /**
   * @brief Returns a string with the Size width and height.
   */
  std::string toString() const;

  /**
   * @brief Returns the string "Size"
   */
  const char* getClassName() const;

  /**
   * @brief Returns the Size hash code.
   */
  int getHashCode() const;

  /**
   * @brief Updates the current size from the given one.
   * @returns the updated Size.
   */
  void copyFrom(const Size& src);

  /**
   * @brief Updates in place the current Size from the given floats.
   * @returns the updated Size.
   */
  Size& copyFromFloats(float width, float height);

  /**
   * @brief Updates in place the current Size from the given floats.
   * @returns the updated Size.
   */
  Size& set(float width, float height);

  /**
   * @brief Returns a new Size set with the multiplication result of the current
   * Size and the given floats.
   */
  Size multiplyByFloats(float w, float h) const;

  /**
   * @brief Boolean : True if the current Size and the given one width and
   * height are strictly equal.
   */
  bool equals(const Size& otherSize);

  /**
   * @brief Returns the surface of the Size : width * height (float).
   */
  int surface() const;

  /**
   * @brief Returns a new Size set to (0.0, 0.0)
   */
  static Size Zero();

  /**
   * @brief Returns a new Size set as the addition result of the current Size
   * and the given one.
   */
  Size add(const Size& otherSize) const;

  /**
   *@brief  Returns a new Size set as the subtraction result of  the given one
   *from the current Size.
   */
  Size subtract(const Size& otherSize) const;

  Size scale(float scale) const;

  /**
   * @brief Returns a new Size set at the linear interpolation "amount" between
   * "start" and "end".
   */
  static Size Lerp(const Size& start, const Size& end, float amount);

}; // end of struct Size

std::ostream& operator<<(std::ostream& os, const Size& size);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SIZE_H
