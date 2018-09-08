#ifndef BABYLON_ANIMATIONS_ANIMATION_PROPERTIES_OVERRIDE_H
#define BABYLON_ANIMATIONS_ANIMATION_PROPERTIES_OVERRIDE_H

#include <babylon/animations/animation.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Class used to override all child animations of a given target.
 */
struct BABYLON_SHARED_EXPORT AnimationPropertiesOverride {

  /**
   * Gets or sets a value indicating if animation blending must be used.
   */
  bool enableBlending = false;

  /**
   * Gets or sets the blending speed to use when enableBlending is true.
   */
  float blendingSpeed = 0.01f;

  /**
   * Gets or sets the default loop mode to use.
   */
  unsigned int loopMode = Animation::ANIMATIONLOOPMODE_CYCLE();

}; // end of class AnimationPropertiesOverride

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_PROPERTIES_OVERRIDE_H
