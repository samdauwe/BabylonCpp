#ifndef BABYLON_ANIMATIONS_ANIMATION_GROUP_H
#define BABYLON_ANIMATIONS_ANIMATION_GROUP_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
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
   * @brief Add an animation (with its target) in the group.
   * @param animation defines the animation we want to add
   * @param target defines the target of the animation
   * @returns the {BABYLON.TargetedAnimation} object
   */
  TargetedAnimation addTargetedAnimation(Animation* animation,
                                         IAnimatable* target);

  /**
   * @brief This function will normalize every animation in the group to make
   * sure they all go from beginFrame to endFrame. It can add constant keys at
   * begin or end
   * @param beginFrame defines the new begin frame for all animations or the
   * smallest begin frame of all animations if null (defaults to null)
   * @param endFrame defines the new end frame for all animations or the largest
   * end frame of all animations if null (defaults to null)
   */
  AnimationGroup& normalize(const Nullable<int>& beginFrame = nullptr,
                            const Nullable<int>& endFrame   = nullptr);

  /**
   * @brief Start all animations on given targets.
   * @param loop defines if animations must loop
   * @param speedRatio defines the ratio to apply to animation speed (1 by
   * default)
   * @param from defines the from key (optional)
   * @param to defines the to key (optional)
   * @returns the current animation group
   */
  AnimationGroup& start(bool loop = false, float speedRatio = 1.f,
                        Nullable<int> from = nullptr,
                        Nullable<int> to   = nullptr);

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
   * @brief Set animation weight for all animatables.
   * @param weight defines the weight to use
   * @return the animationGroup
   * @see http://doc.babylonjs.com/babylon101/animations#animation-weights
   */
  AnimationGroup& setWeightForAllAnimatables(float weight);

  /**
   * @brief Synchronize and normalize all animatables with a source animatable.
   * @param root defines the root animatable to synchronize with
   * @return the animationGroup
   * @see http://doc.babylonjs.com/babylon101/animations#animation-weights
   */
  AnimationGroup& syncAllAnimationsWith(Animatable* root);

  /**
   * @brief Goes to a specific frame in this animation group
   *
   * @param frame the frame number to go to
   * @return the animationGroup
   */
  AnimationGroup& goToFrame(int frame);

  /**
   * @brief Dispose all associated resources.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  AnimationGroup(const string_t& name, Scene* scene = nullptr);

private:
  /**
   * @brief Gets the first frame.
   */
  int get_from() const;

  /**
   * @brief Gets the last frame.
   */
  int get_to() const;

  /**
   * @brief Define if the animations are started.
   */
  bool get_isStarted() const;

  /**
   * @brief Gets or sets the speed ratio to use for all animations.
   */
  float get_speedRatio() const;

  /**
   * @brief Gets or sets the speed ratio to use for all animations.
   */
  void set_speedRatio(float value);

  /**
   * @brief Gets the targeted animations for this animation group.
   */
  vector_t<unique_ptr_t<TargetedAnimation>>& get_targetedAnimations();

  /**
   * @brief Returning the list of animatables controlled by this animation
   * group.
   */
  vector_t<Animatable*>& get_animatables();

  void _checkAnimationGroupEnded(Animatable* animatable);

public:
  string_t name;
  Observable<TargetedAnimation> onAnimationEndObservable;

  /**
   * This observable will notify when all animations have ended.
   */
  Observable<AnimationGroup> onAnimationGroupEndObservable;

  /**
   * Gets the first frame
   */
  ReadOnlyProperty<AnimationGroup, int> from;

  /**
   * Gets the last frame
   */
  ReadOnlyProperty<AnimationGroup, int> to;

  /**
   * Define if the animations are started.
   */
  ReadOnlyProperty<AnimationGroup, bool> isStarted;

  /**
   * Speed ratio to use for all animations.
   */
  Property<AnimationGroup, float> speedRatio;

  /**
   * Targeted animations for this animation group.
   */
  ReadOnlyProperty<AnimationGroup, vector_t<unique_ptr_t<TargetedAnimation>>>
    targetedAnimations;

  /**
   * Animatables controlled by this animation group.
   */
  ReadOnlyProperty<AnimationGroup, vector_t<Animatable*>> animatables;

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
