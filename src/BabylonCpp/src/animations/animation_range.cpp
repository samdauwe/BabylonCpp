#include <babylon/animations/animation_range.h>

namespace BABYLON {

AnimationRange::AnimationRange() : name{""}, from{0.f}, to{0.f}
{
}

AnimationRange::AnimationRange(const std::string& iName, float iFrom, float iTo)
    : name{iName}, from{iFrom}, to{iTo}
{
}

AnimationRange::AnimationRange(const AnimationRange& other) = default;

AnimationRange::AnimationRange(AnimationRange&& other) = default;

AnimationRange& AnimationRange::operator=(const AnimationRange& other) = default;

AnimationRange& AnimationRange::operator=(AnimationRange&& other) = default;

AnimationRange::~AnimationRange() = default;

AnimationRange AnimationRange::copy() const
{
  return AnimationRange(name, from, to);
}

AnimationRange AnimationRange::clone() const
{
  return AnimationRange(name, from, to);
}

} // end of namespace BABYLON
