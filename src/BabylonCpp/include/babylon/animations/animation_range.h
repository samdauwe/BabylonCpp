#ifndef BABYLON_ANIMATIONS_ANIMATION_RANGE_H
#define BABYLON_ANIMATIONS_ANIMATION_RANGE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/animation_value.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT AnimationRange {

public:
  AnimationRange();
  AnimationRange(const std::string& name, int from, int to);
  AnimationRange(const AnimationRange& other);
  ~AnimationRange();

  AnimationRange copy() const;
  std::unique_ptr<AnimationRange> clone() const;

public:
  std::string name;
  int from;
  int to;

}; // end of class AnimationRange

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_RANGE_H
