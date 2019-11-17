#ifndef BABYLON_ANIMATIONS_TARGETED_ANIMATION_H
#define BABYLON_ANIMATIONS_TARGETED_ANIMATION_H

#include <nlohmann/json_fwd.hpp>

#include <memory>

#include <babylon/babylon_api.h>

using json = nlohmann::json;

namespace BABYLON {

class Animation;
class IAnimatable;
using AnimationPtr   = std::shared_ptr<Animation>;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

/**
 * @brief This class defines the direct association between an animation and a
 * target.
 */
struct BABYLON_SHARED_EXPORT TargetedAnimation {
  /**
   * Animation to perform
   */
  AnimationPtr animation = nullptr;
  /**
   * Target to animate
   */
  IAnimatablePtr target = nullptr;

  /**
   * @brief Serialize the object.
   * @returns the JSON object representing the current entity
   */
  [[nodiscard]] json serialize() const;
}; // end of struct TargetedAnimation

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_TARGETED_ANIMATION_H
