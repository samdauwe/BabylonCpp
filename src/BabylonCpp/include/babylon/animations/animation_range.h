#ifndef BABYLON_ANIMATIONS_ANIMATION_RANGE_H
#define BABYLON_ANIMATIONS_ANIMATION_RANGE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/animation_value.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT AnimationRange {

public:
  AnimationRange();
  AnimationRange(const string_t& name, float from, float to);
  AnimationRange(const AnimationRange& other);
  ~AnimationRange();

  AnimationRange copy() const;
  std::unique_ptr<AnimationRange> clone() const;

public:
  string_t name;
  float from;
  float to;

}; // end of class AnimationRange

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_RANGE_H
