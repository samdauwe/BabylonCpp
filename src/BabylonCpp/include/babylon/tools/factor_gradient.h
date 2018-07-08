#ifndef BABYLON_TOOLS_FACTOR_GRADIENT_H
#define BABYLON_TOOLS_FACTOR_GRADIENT_H

#include <babylon/babylon_global.h>
#include <babylon/tools/ivalue_gradient.h>

namespace BABYLON {

/**
 * @brief Class used to store factor gradient.
 */
struct BABYLON_SHARED_EXPORT FactorGradient : public IValueGradient {

  /**
   * Gets or sets associated factor
   */
  float factor;

}; // end of struct FactorGradient

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_FACTOR_GRADIENT_H
