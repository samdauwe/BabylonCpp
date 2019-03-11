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

IAnimationKey::IAnimationKey(const IAnimationKey& other)
    : frame{other.frame}
    , value{other.value}
    , inTangent{other.inTangent}
    , outTangent{other.outTangent}
    , interpolation{other.interpolation}
{
}

IAnimationKey::IAnimationKey(IAnimationKey&& other)
    : frame{std::move(other.frame)}
    , value{std::move(other.value)}
    , inTangent{std::move(other.inTangent)}
    , outTangent{std::move(other.outTangent)}
    , interpolation{std::move(other.interpolation)}
{
}

IAnimationKey& IAnimationKey::operator=(const IAnimationKey& other)
{
  if (&other != this) {
    frame         = other.frame;
    value         = other.value;
    inTangent     = other.inTangent;
    outTangent    = other.outTangent;
    interpolation = other.interpolation;
  }

  return *this;
}

IAnimationKey& IAnimationKey::operator=(IAnimationKey&& other)
{
  if (&other != this) {
    frame         = std::move(other.frame);
    value         = std::move(other.value);
    inTangent     = std::move(other.inTangent);
    outTangent    = std::move(other.outTangent);
    interpolation = std::move(other.interpolation);
  }

  return *this;
}

IAnimationKey::~IAnimationKey()
{
}

} // end of namespace BABYLON
