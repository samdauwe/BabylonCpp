#ifndef BABYLON_ANIMATIONS_ANIMATABLE_H
#define BABYLON_ANIMATIONS_ANIMATABLE_H

#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief Class used to store an actual running animation.
 */
class BABYLON_SHARED_EXPORT Animatable
    : public ::std::enable_shared_from_this<Animatable> {

public:
  template <typename... Ts>
  static AnimatablePtr New(Ts&&... args)
  {
    auto animatable
      = shared_ptr_t<Animatable>(new Animatable(::std::forward<Ts>(args)...));
    animatable->addToScene(animatable);

    return animatable;
  }
  ~Animatable();

  void addToScene(const AnimatablePtr& newAnimatable);

  /** Methods **/

  /**
   * @brief Synchronize and normalize current Animatable with a source
   * Animatable. This is useful when using animation weights and when animations
   * are not of the same length.
   * @param root defines the root Animatable to synchronize with
   * @returns the current Animatable
   */
  Animatable& syncWith(Animatable* root);

  /**
   * Gets the list of runtime animations
   * @returns an array of RuntimeAnimation
   */
  vector_t<RuntimeAnimationPtr>& getAnimations();

  /**
   * @brief Adds more animations to the current animatable.
   * @param target defines the target of the animations
   * @param animations defines the new animations to add
   */
  void appendAnimations(const IAnimatablePtr& target,
                        const vector_t<AnimationPtr>& animations);

  /**
   * @brief Gets the source animation for a specific property.
   * @param property defines the propertyu to look for
   * @returns null or the source animation for the given property
   */
  AnimationPtr getAnimationByTargetProperty(const string_t& property) const;

  /**
   * @brief Gets the runtime animation for a specific property.
   * @param property defines the propertyu to look for
   * @returns null or the runtime animation for the given property
   */
  RuntimeAnimationPtr
  getRuntimeAnimationByTargetProperty(const string_t& property) const;

  /**
   * @brief Resets the animatable to its original state.
   */
  void reset();

  /**
   * @brief Allows the animatable to blend with current running animations.
   * @see http://doc.babylonjs.com/babylon101/animations#animation-blending
   * @param blendingSpeed defines the blending speed to use
   */
  void enableBlending(float blendingSpeed);

  /**
   * @brief Disable animation blending.
   * @see http://doc.babylonjs.com/babylon101/animations#animation-blending
   */
  void disableBlending();

  /**
   * @brief Jump directly to a given frame.
   * @param frame defines the frame to jump to
   */
  void goToFrame(int frame);

  /**
   * @brief Pause the animation.
   */
  void pause();

  /**
   * @brief Restart the animation.
   */
  void restart();

  /**
   * @brief Stop and delete the current animation.
   * @param animationName defines a string used to only stop some of the runtime
   * animations instead of all
   * @param targetMask - a function that determines if the animation should be
   * stopped based on its target (all animations will be stopped if both this
   * and animationName are empty)
   */
  void stop(const string_t& animationName = "",
            const ::std::function<bool(IAnimatable* target)>& targetMask
            = nullptr);

  /**
   * @brief Hidden
   */
  bool _animate(const millisecond_t& delay);

protected:
  /**
   * @brief Creates a new Animatable.
   * @param scene defines the hosting scene
   * @param target defines the target object
   * @param fromFrame defines the starting frame number (default is 0)
   * @param toFrame defines the ending frame number (default is 100)
   * @param loopAnimation defines if the animation must loop (default is false)
   * @param speedRatio defines the factor to apply to animation speed (default
   * is 1)
   * @param onAnimationEnd defines a callback to call when animation ends if it
   * is not looping
   * @param animations defines a group of animation to add to the new Animatable
   */
  Animatable(Scene* scene, const IAnimatablePtr& target, int fromFrame = 0,
             int toFrame = 100, bool loopAnimation = false,
             float speedRatio                              = 1.f,
             const ::std::function<void()>& onAnimationEnd = nullptr,
             const vector_t<AnimationPtr>& animations      = {});

private:
  /**
   * @brief Gets the root Animatable used to synchronize and normalize
   * animations.
   */
  Animatable*& get_syncRoot();

  /**
   * @brief Gets the current frame of the first RuntimeAnimation.
   * Used to synchronize Animatables
   */
  float get_masterFrame() const;

  /**
   * @brief Gets the animatable weight (-1.0 by default meaning not weighted).
   */
  float get_weight() const;

  /**
   * @brief Sets the animatable weight (-1.0 by default meaning not weighted).
   */
  void set_weight(float value);

  /**
   * @brief Gets the speed ratio to apply to the animatable (1.0 by default).
   */
  float get_speedRatio() const;

  /**
   * @brief Sets the speed ratio to apply to the animatable (1.0 by default).
   */
  void set_speedRatio(float value);

  void _raiseOnAnimationEnd();

public:
  /**
   * Defines the target object
   */
  IAnimatablePtr target;

  /**
   * Gets or sets a boolean indicating if the animatable must be disposed and
   * removed at the end of the animation. This will only apply for non looping
   * animation (default is true)
   */
  bool disposeOnEnd;

  /**
   * Gets a boolean indicating if the animation has started
   */
  bool animationStarted;

  /**
   * Defines the starting frame number (default is 0)
   */
  int fromFrame;

  /**
   * Defines the ending frame number (default is 100)
   */
  int toFrame;

  /**
   * Defines if the animation must loop (default is false)
   */
  bool loopAnimation;

  /**
   * Defines a callback to call when animation ends if it is not looping
   */
  ::std::function<void()> onAnimationEnd;

  /**
   * Observer raised when the animation ends
   */
  Observable<Animatable> onAnimationEndObservable;

  /**
   * Root Animatable used to synchronize and normalize animations
   */
  ReadOnlyProperty<Animatable, Animatable*> syncRoot;

  /**
   * Current frame of the first RuntimeAnimation
   */
  ReadOnlyProperty<Animatable, float> masterFrame;

  /**
   * Animatable weight (-1.0 by default meaning not weighted)
   */
  Property<Animatable, float> weight;

  /**
   * Speed ratio to apply to the animatable (1.0 by default)
   */
  Property<Animatable, float> speedRatio;

private:
  nullable_t<millisecond_t> _localDelayOffset;
  nullable_t<millisecond_t> _pausedDelay;
  vector_t<RuntimeAnimationPtr> _runtimeAnimations;
  bool _paused;
  Scene* _scene;
  float _speedRatio;
  float _weight;
  Animatable* _syncRoot;

}; // end of class Animatable

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATABLE_H
