#ifndef BABYLON_MATERIALS_EFFECT_WRAPPER_H
#define BABYLON_MATERIALS_EFFECT_WRAPPER_H

#include <babylon/babylon_api.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class Effect;
struct IEffectWrapperCreationOptions;
using EffectPtr = std::shared_ptr<Effect>;

/**
 * @brief Wraps an effect to be used for rendering.
 */
struct BABYLON_SHARED_EXPORT EffectWrapper {

  /**
   * Event that is fired right before the effect is drawn (should be used to update uniforms)
   */
  Observable<void> onApplyObservable;

  /**
   * The underlying effect
   */
  EffectPtr effect;

  /**
   * @brief Creates an effect to be renderer.
   * @param creationOptions options to create the effect
   */
  EffectWrapper(IEffectWrapperCreationOptions& creationOptions);
  ~EffectWrapper();

  /**
   * @brief Disposes of the effect wrapper.
   */
  void dispose();

}; // end of struct EffectWrapper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_FALLBACKS_H
