#ifndef BABYLON_TOOLS_FACTOR_GRADIENT_H
#define BABYLON_TOOLS_FACTOR_GRADIENT_H

#include <babylon/babylon_global.h>
#include <babylon/tools/ivalue_gradient.h>

namespace BABYLON {

/**
 * @brief Class used to store factor gradient.
 */
struct BABYLON_SHARED_EXPORT FactorGradient : public IValueGradient {

  FactorGradient();
  ~FactorGradient();

  /**
   * Gets or sets first associated factor
   */
  float factor1;

  /**
   * Gets or sets second associated factor
   */
  nullable_t<float> factor2;

  /**
   * Will get a number picked randomly between factor1 and factor2.
   * If factor2 is undefined then factor1 will be used
   * @returns the picked number
   */
  float getFactor() const;

}; // end of struct FactorGradient

bool operator==(const FactorGradient& lhs, const FactorGradient& rhs);
bool operator!=(const FactorGradient& lhs, const FactorGradient& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_FACTOR_GRADIENT_H
