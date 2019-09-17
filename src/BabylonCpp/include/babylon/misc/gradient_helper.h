#ifndef BABYLON_MISC_GRADIENT_HELPER_H
#define BABYLON_MISC_GRADIENT_HELPER_H

#include <functional>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/misc/ivalue_gradient.h>

namespace BABYLON {

/**
 * @brief Helper used to simplify some generic gradient tasks.
 */
struct BABYLON_SHARED_EXPORT GradientHelper {

  /**
   * @brief Gets the current gradient from an array of IValueGradient.
   * @param ratio defines the current ratio to get
   * @param gradients defines the array of IValueGradient
   * @param updateFunc defines the callback function used to get the final value
   * from the selected gradients
   */
  static void GetCurrentGradient(
    float ratio, const std::vector<IValueGradient>& gradients,
    const std::function<void(const IValueGradient& current,
                             const IValueGradient& next, float scale)>&
      updateFunc);

}; // end of struct GradientHelper

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_GRADIENT_HELPER_H
