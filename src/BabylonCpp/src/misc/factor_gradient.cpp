#include <babylon/misc/factor_gradient.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/random.h>
#include <babylon/maths/scalar.h>

namespace BABYLON {

FactorGradient::FactorGradient(float iGradient, float iFactor1, std::optional<float> iFactor2)
    : IValueGradient{iGradient}, factor1{iFactor1}, factor2{iFactor2}
{
}

FactorGradient::~FactorGradient() = default;

float FactorGradient::getFactor() const
{
  if (!factor2.has_value() || stl_util::almost_equal(*factor2, factor1)) {
    return factor1;
  }

  return factor1 + ((*factor2 - factor1) * Math::random());
}

bool operator==(const FactorGradient& lhs, const FactorGradient& rhs)
{
  return stl_util::almost_equal(lhs.gradient, rhs.gradient)
         && stl_util::almost_equal(lhs.factor1, rhs.factor1) && lhs.factor2 == rhs.factor2;
}

bool operator!=(const FactorGradient& lhs, const FactorGradient& rhs)
{
  return !(lhs == rhs);
}

} // end of namespace BABYLON
