#include <babylon/tools/factor_gradient.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/random.h>
#include <babylon/math/scalar.h>

namespace BABYLON {

FactorGradient::FactorGradient() : factor2{std::nullopt}
{
}

FactorGradient::~FactorGradient()
{
}

float FactorGradient::getFactor() const
{
  if (!factor2.has_value()) {
    return factor1;
  }

  return Scalar::Lerp(factor1, *factor2, Math::random());
}

bool operator==(const FactorGradient& lhs, const FactorGradient& rhs)
{
  return stl_util::almost_equal(lhs.gradient, rhs.gradient)
         && stl_util::almost_equal(lhs.factor1, rhs.factor1)
         && lhs.factor2 == rhs.factor2;
}

bool operator!=(const FactorGradient& lhs, const FactorGradient& rhs)
{
  return !(lhs == rhs);
}

} // end of namespace BABYLON
