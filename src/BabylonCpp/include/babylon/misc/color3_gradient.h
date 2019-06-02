#ifndef BABYLON_MISC_COLOR3_GRADIENT_H
#define BABYLON_MISC_COLOR3_GRADIENT_H

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>
#include <babylon/misc/ivalue_gradient.h>

namespace BABYLON {

/**
 * @brief Class used to store color3 gradient.
 */
struct BABYLON_SHARED_EXPORT Color3Gradient : public IValueGradient {

  Color3Gradient();
  ~Color3Gradient();

  /**
   * Gets or sets the associated color
   */
  Color3 color;

}; // end of struct Color3Gradient

bool operator==(const Color3Gradient& lhs, const Color3Gradient& rhs);
bool operator!=(const Color3Gradient& lhs, const Color3Gradient& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_COLOR3_GRADIENT_H
