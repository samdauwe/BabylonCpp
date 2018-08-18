#include <babylon/tools/factor_gradient.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

float FactorGradient::getFactor() const
{
  return factor;
}

bool operator==(const FactorGradient& lhs, const FactorGradient& rhs)
{
  return stl_util::almost_equal(lhs.factor, rhs.factor);
}

bool operator!=(const FactorGradient& lhs, const FactorGradient& rhs)
{
  return !(lhs == rhs);
}

} // end of namespace BABYLON
