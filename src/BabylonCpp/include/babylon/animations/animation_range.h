#ifndef BABYLON_ANIMATIONS_ANIMATION_RANGE_H
#define BABYLON_ANIMATIONS_ANIMATION_RANGE_H

#include <babylon/animations/animation_value.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Represents the range of an animation.
 */
class BABYLON_SHARED_EXPORT AnimationRange {

public:
  /**
   * @brief Initializes the range of an animation.
   * @param name The name of the animation range
   * @param from The starting frame of the animation
   * @param to The ending frame of the animation
   */
  AnimationRange();
  AnimationRange(const std::string& name, float from, float to);
  AnimationRange(const AnimationRange& other);
  AnimationRange(AnimationRange&& other);
  AnimationRange& operator=(const AnimationRange& other);
  AnimationRange& operator=(AnimationRange&& other);
  ~AnimationRange(); // = default

  AnimationRange copy() const;

  /**
   * @brief Makes a copy of the animation range.
   * @returns A copy of the animation range
   */
  AnimationRange clone() const;

public:
  /** The name of the animation range **/
  std::string name;
  /** The starting frame of the animation */
  float from;
  /** The ending frame of the animation */
  float to;

}; // end of class AnimationRange

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_RANGE_H
