#include <babylon/animations/ianimation_key.h>

namespace BABYLON {

IAnimationKey::IAnimationKey()
    : frame{0}
    , value{0}
    , inTangent{std::nullopt}
    , outTangent{std::nullopt}
    , interpolation{std::nullopt}
{
}

IAnimationKey::IAnimationKey(float iFrame, AnimationValue iValue)
    : frame{iFrame}
    , value{iValue}
    , inTangent{std::nullopt}
    , outTangent{std::nullopt}
    , interpolation{std::nullopt}
{
}

IAnimationKey::IAnimationKey(
  float iFrame, AnimationValue iValue,
  const std::optional<AnimationValue>& iInTangent,
  const std::optional<AnimationValue>& iOutTangent,
  const std::optional<AnimationValue>& iInterpolation)
    : frame{iFrame}
    , value{iValue}
    , inTangent{iInTangent}
    , outTangent{iOutTangent}
    , interpolation{iInterpolation}
{
}

IAnimationKey::IAnimationKey(const IAnimationKey& other) = default;

IAnimationKey::IAnimationKey(IAnimationKey&& other) = default;

IAnimationKey& IAnimationKey::operator=(const IAnimationKey& other) = default;

IAnimationKey& IAnimationKey::operator=(IAnimationKey&& other) = default;

IAnimationKey::~IAnimationKey() = default;

} // end of namespace BABYLON
