#ifndef BABYLON_ANIMATIONS_ANIMATION_GROUP_H
#define BABYLON_ANIMATIONS_ANIMATION_GROUP_H

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observable.h>

using json = nlohmann::json;

namespace BABYLON {

class Animatable;
class Animation;
class IAnimatable;
class TargetedAnimation;
class Scene;
using AnimatablePtr  = std::shared_ptr<Animatable>;
using AnimationPtr   = std::shared_ptr<Animation>;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

/**
 * @brief Use this class to create coordinated animations on multiple targets.
 */
class BABYLON_SHARED_EXPORT AnimationGroup : public IDisposable {

public:
  template <typename... Ts>
  static AnimationGroup* New(Ts&&... args)
  {
    auto animationGroup = new AnimationGroup(std::forward<Ts>(args)...);
    animationGroup->addToScene(
      static_cast<std::unique_ptr<AnimationGroup>>(animationGroup));

    return animationGroup;
  }
  virtual ~AnimationGroup();

  void addToScene(std::unique_ptr<AnimationGroup>&& newAnimationGroup);

  /**
   * @brief Add an animation (with its target) in the group.
   * @param animation defines the animation we want to add
   * @param target defines the target of the animation
   * @returns the TargetedAnimation object
   */
  TargetedAnimation addTargetedAnimation(const AnimationPtr& animation,
                                         const IAnimatablePtr& target);

  /**
   * @brief This function will normalize every animation in the group to make
   * sure they all go from beginFrame to endFrame It can add constant keys at
   * begin or end
   * @param beginFrame defines the new begin frame for all animations or the
   * smallest begin frame of all animations if null (defaults to null)
   * @param endFrame defines the new end frame for all animations or the largest
   * end frame of all animations if null (defaults to null)
   * @returns the animation group
   */
  AnimationGroup& normalize(const std::optional<int>& beginFrame = std::nullopt,
                            const std::optional<int>& endFrame = std::nullopt);

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
                        std::optional<float> from = std::nullopt,
                        std::optional<float> to   = std::nullopt);

  /**
   * @brief Pause all animations.
   * @returns the animation group
   */
  AnimationGroup& pause();

  /**
   * @brief Play all animations to initial state.
   * This function will start() the animations if they were not started or will
   * restart() them if they were paused
   * @param loop defines if animations must loop
   * @returns the animation group
   */
  AnimationGroup& play(bool loop = false);

  /**
   * @brief Reset all animations to initial state.
   * @returns the animation group
   */
  AnimationGroup& reset();

  /**
   * @brief Restart animations from key 0.
   * @returns the animation group
   */
  AnimationGroup& restart();

  /**
   * @brief Stop all animations.
   * @returns the animation group
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

  // Statics

  /**
   * @brief Returns a new AnimationGroup object parsed from the source provided.
   * @param parsedAnimationGroup defines the source
   * @param scene defines the scene that will receive the animationGroup
   * @returns a new AnimationGroup
   */
  static AnimationGroup* Parse(const json& parsedAnimationGroup, Scene* scene);

  /**
   * @brief Returns the string "AnimationGroup".
   * @returns "AnimationGroup"
   */
  std::string getClassName() const;

  /**
   * @brief Creates a detailled string about the object
   * @param fullDetails defines if the output string will support multiple
   * levels of logging within scene loading
   * @returns a string representing the object
   */
  std::string toString(bool fullDetails = false) const;

protected:
  /**
   * @brief Instantiates a new Animation Group.
   * This helps managing several animations at once.
   * @see http://doc.babylonjs.com/how_to/group
   * @param name Defines the name of the group
   * @param scene Defines the scene the group belongs to
   */
  AnimationGroup(const std::string& name, Scene* scene = nullptr);

private:
  /**
   * @brief Gets the first frame.
   */
  float get_from() const;

  /**
   * @brief Gets the last frame.
   */
  float get_to() const;

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
  std::vector<std::unique_ptr<TargetedAnimation>>& get_targetedAnimations();

  /**
   * @brief Returning the list of animatables controlled by this animation
   * group.
   */
  std::vector<AnimatablePtr>& get_animatables();

  void _checkAnimationGroupEnded(const AnimatablePtr& animatable);

public:
  /**
   * The name of the animation group
   */
  std::string name;

  /**
   * This observable will notify when one animation have ended.
   */
  Observable<TargetedAnimation> onAnimationEndObservable;

  /**
   * This observable will notify when all animations have ended.
   */
  Observable<AnimationGroup> onAnimationGroupEndObservable;

  /**
   * This observable will notify when all animations have paused.
   */
  Observable<AnimationGroup> onAnimationGroupPauseObservable;

  /**
   * Gets the first frame
   */
  ReadOnlyProperty<AnimationGroup, float> from;

  /**
   * Gets the last frame
   */
  ReadOnlyProperty<AnimationGroup, float> to;

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
  ReadOnlyProperty<AnimationGroup,
                   std::vector<std::unique_ptr<TargetedAnimation>>>
    targetedAnimations;

  /**
   * Animatables controlled by this animation group.
   */
  ReadOnlyProperty<AnimationGroup, std::vector<AnimatablePtr>> animatables;

private:
  Scene* _scene;
  std::vector<std::unique_ptr<TargetedAnimation>> _targetedAnimations;
  std::vector<AnimatablePtr> _animatables;
  float _from;
  float _to;
  bool _isStarted;
  float _speedRatio;

}; // end of class AnimationGroup

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_GROUP_H
