#ifndef BABYLON_TOOLS_FACTOR_GRADIENT_H
#define BABYLON_TOOLS_FACTOR_GRADIENT_H

#include <babylon/babylon_global.h>
#include <babylon/tools/ivalue_gradient.h>

namespace BABYLON {

/**
 * @brief Class used to store factor gradient.
 */
struct BABYLON_SHARED_EXPORT FactorGradient : public IValueGradient {

  float getFactor() const;

  /**
   * Gets or sets associated factor
   */
  float factor;

}; // end of struct FactorGradient

bool operator==(const FactorGradient& lhs, const FactorGradient& rhs);
bool operator!=(const FactorGradient& lhs, const FactorGradient& rhs);

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_FACTOR_GRADIENT_H
