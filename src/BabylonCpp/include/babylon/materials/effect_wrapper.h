#ifndef BABYLON_MATERIALS_EFFECT_WRAPPER_H
#define BABYLON_MATERIALS_EFFECT_WRAPPER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

struct EffectWrapperCreationOptions;
FWD_CLASS_SPTR(Effect)
FWD_STRUCT_SPTR(DrawWrapper)

/**
 * @brief Wraps an effect to be used for rendering.
 */
class BABYLON_SHARED_EXPORT EffectWrapper {

public:
  /**
   * @brief Creates an effect to be renderer.
   * @param creationOptions options to create the effect
   */
  EffectWrapper(const EffectWrapperCreationOptions& creationOptions);
  ~EffectWrapper();

  /**
   * @brief Disposes of the effect wrapper.
   */
  void dispose();

protected:
  /**
   * @brief Gets the underlying effect.
   */
  EffectPtr& get_effect();

  /**
   * @brief Sets the underlying effect.
   */
  void set_effect(const EffectPtr& effect);

public:
  /**
   * Event that is fired right before the effect is drawn (should be used to update uniforms)
   */
  Observable<void> onApplyObservable;

  /**
   * The underlying effect
   */
  Property<EffectWrapper, EffectPtr> effect;

  /** @hidden */
  DrawWrapperPtr _drawWrapper;

}; // end of class EffectWrapper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_FALLBACKS_H
