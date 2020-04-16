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
   * @param updateFunc defines the callback function used to get the final value from the selected
   * gradients
   */
  template <typename T>
  static void GetCurrentGradient(
    float ratio, const std::vector<T>& gradients,
    const std::function<void(const T& current, const T& next, float scale)>& updateFunc)
  {
    // Use last index if over
    if (gradients[0].gradient > ratio) {
      updateFunc(gradients[0], gradients[0], 1.f);
      return;
    }

    for (size_t gradientIndex = 0; gradientIndex < gradients.size() - 1; ++gradientIndex) {
      auto currentGradient = gradients[gradientIndex];
      auto nextGradient    = gradients[gradientIndex + 1];

      if (ratio >= currentGradient.gradient && ratio <= nextGradient.gradient) {
        const auto scale
          = (ratio - currentGradient.gradient) / (nextGradient.gradient - currentGradient.gradient);
        updateFunc(currentGradient, nextGradient, scale);
        return;
      }
    }

    // Use last index if over
    auto lastGradient = gradients.back();
    updateFunc(lastGradient, lastGradient, 1.f);
  }

}; // end of struct GradientHelper

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_GRADIENT_HELPER_H
