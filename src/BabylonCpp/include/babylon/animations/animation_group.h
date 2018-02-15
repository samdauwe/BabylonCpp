#ifndef BABYLON_ANIMATIONS_ANIMATION_GROUP_H
#define BABYLON_ANIMATIONS_ANIMATION_GROUP_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief Use this class to create coordinated animations on multiple targets.
 */
class BABYLON_SHARED_EXPORT AnimationGroup : public IDisposable {

public:
  template <typename... Ts>
  static AnimationGroup* New(Ts&&... args)
  {
    auto animationGroup = new AnimationGroup(::std::forward<Ts>(args)...);
    animationGroup->addToScene(
      static_cast<unique_ptr_t<AnimationGroup>>(animationGroup));

    return animationGroup;
  }
  virtual ~AnimationGroup();

  void addToScene(unique_ptr_t<AnimationGroup>&& newAnimationGroup);

  /**
   * @brief Define if the animations are started.
   */
  bool isStarted() const;

  /**
   * @brief Gets or sets the speed ratio to use for all animations.
   */
  float speedRatio() const;

  /**
   * @brief Gets or sets the speed ratio to use for all animations.
   */
  void setSpeedRatio(float value);

  /**
   * @brief Gets the targeted animations for this animation group.
   */
  vector_t<unique_ptr_t<TargetedAnimation>>& targetedAnimations();

  /**
   * @brief Add an animation (with its target) in the group.
   * @param animation defines the animation we want to add
   * @param target defines the target of the animation
   * @returns the {BABYLON.TargetedAnimation} object
   */
  TargetedAnimation addTargetedAnimation(Animation* animation,
                                         IAnimatable* target);

  /**
   * @brief This function will normalize every animation in the group to make
   * sure they all go from beginFrame to endFrame It can add constant keys at
   * begin or end.
   * @param beginFrame defines the new begin frame for all animations. It can't
   * be bigger than the smallest begin frame of all animations
   * @param endFrame defines the new end frame for all animations. It can't be
   * smaller than the largest end frame of all animations
   */
  AnimationGroup& normalize(int beginFrame = numeric_limits_t<int>::max(),
                            int endFrame   = numeric_limits_t<int>::lowest());

  /**
   * @brief Start all animations on given targets.
   * @param loop defines if animations must loop
   * @param speedRatio defines the ratio to apply to animation speed (1 by
   * default)
   */
  AnimationGroup& start(bool loop = false, float speedRatio = 1.f);

  /**
   * @brief Pause all animations.
   */
  AnimationGroup& pause();

  /**
   * @brief Play all animations to initial state.
   * This function will start() the animations if they were not started or
   * will restart() them if they were paused
   * @param loop defines if animations must loop
   */
  AnimationGroup& play(bool loop = false);

  /**
   * @brief Reset all animations to initial state.
   */
  AnimationGroup& reset();

  /**
   * @brief Restart animations from key 0.
   */
  AnimationGroup& restart();

  /**
   * @brief Stop all animations.
   */
  AnimationGroup& stop();

  /**
   * @brief Dispose all associated resources.
   */
  void dispose(bool doNotRecurse = false) override;

protected:
  AnimationGroup(const string_t& name, Scene* scene = nullptr);

public:
  string_t name;
  Observable<TargetedAnimation> onAnimationEndObservable;

private:
  Scene* _scene;

  vector_t<unique_ptr_t<TargetedAnimation>> _targetedAnimations;
  vector_t<Animatable*> _animatables;
  int _from;
  int _to;
  bool _isStarted;
  float _speedRatio;

}; // end of class AnimationGroup

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_GROUP_H
