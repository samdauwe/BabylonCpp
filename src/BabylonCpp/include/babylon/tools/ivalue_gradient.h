#ifndef BABYLON_TOOLS_IVALUE_GRADIENT_H
#define BABYLON_TOOLS_IVALUE_GRADIENT_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Interface used by value gradients (color, factor, ...).
 */
struct BABYLON_SHARED_EXPORT IValueGradient {

  /**
   * Gets or sets the gradient value (between 0 and 1)
   */
  float gradient;

}; // end of struct IValueGradient

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_IVALUE_GRADIENT_H
