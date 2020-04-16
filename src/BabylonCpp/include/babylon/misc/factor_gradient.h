#ifndef BABYLON_MISC_FACTOR_GRADIENT_H
#define BABYLON_MISC_FACTOR_GRADIENT_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/misc/ivalue_gradient.h>

namespace BABYLON {

/**
 * @brief Class used to store factor gradient.
 */
struct BABYLON_SHARED_EXPORT FactorGradient : public IValueGradient {

  /**
   * @brief Creates a new factor gradient.
   * @param gradient gets or sets the gradient value (between 0 and 1)
   * @param factor1 gets or sets first associated factor
   * @param factor2 gets or sets second associated factor
   */
  FactorGradient(float gradient, float factor1, std::optional<float> factor2 = std::nullopt);
  ~FactorGradient(); // = default

  /**
   * Gets or sets first associated factor
   */
  float factor1;

  /**
   * Gets or sets second associated factor
   */
  std::optional<float> factor2;

  /**
   * @brief Will get a number picked randomly between factor1 and factor2.
   * If factor2 is undefined then factor1 will be used
   * @returns the picked number
   */
  [[nodiscard]] float getFactor() const;

}; // end of struct FactorGradient

bool operator==(const FactorGradient& lhs, const FactorGradient& rhs);
bool operator!=(const FactorGradient& lhs, const FactorGradient& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_FACTOR_GRADIENT_H
