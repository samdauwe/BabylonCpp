#ifndef BABYLON_ANIMATIONS_ANIMATABLE_H
#define BABYLON_ANIMATIONS_ANIMATABLE_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Animatable {

public:
  Animatable(Scene* scene, IAnimatable* target, int fromFrame = 0,
             int toFrame = 100, bool loopAnimation = false,
             float speedRatio                              = 1.f,
             const ::std::function<void()>& onAnimationEnd = nullptr,
             const vector_t<Animation*>& animations = vector_t<Animation*>());
  ~Animatable();

  /** Methods **/
  /**
   * @brief Synchronize and normalize current Animatable with a source
   * Animatable. This is useful when using animation weights and when animations
   * are not of the same length.
   * @param root defines the root Animatable to synchronize with
   * @returns the current Animatable
   */
  Animatable& syncWith(Animatable* root);

  vector_t<RuntimeAnimation*>& getAnimations();
  void appendAnimations(IAnimatable* target,
                        const vector_t<Animation*>& animations);
  Animation* getAnimationByTargetProperty(const string_t& property) const;
  RuntimeAnimation*
  getRuntimeAnimationByTargetProperty(const string_t& property) const;
  void reset();
  void enableBlending(float blendingSpeed);
  void disableBlending();
  void goToFrame(int frame);
  void pause();
  void restart();
  void stop(const string_t& animationName = "");
  bool _animate(const millisecond_t& delay);

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
  int get_masterFrame() const;

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

public:
  IAnimatable* target;
  bool animationStarted;
  int fromFrame;
  int toFrame;
  bool loopAnimation;
  ::std::function<void()> onAnimationEnd;
  ReadOnlyProperty<Animatable, Animatable*> syncRoot;
  ReadOnlyProperty<Animatable, int> masterFrame;
  Property<Animatable, float> weight;
  Property<Animatable, float> speedRatio;

private:
  Nullable<millisecond_t> _localDelayOffset;
  Nullable<millisecond_t> _pausedDelay;
  vector_t<RuntimeAnimation*> _runtimeAnimations;
  bool _paused;
  Scene* _scene;
  float _speedRatio;
  float _weight;
  Animatable* _syncRoot;

}; // end of class Animatable

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATABLE_H
