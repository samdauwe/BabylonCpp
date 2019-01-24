#ifndef BABYLON_ACTIONS_DIRECTACTIONS_PLAY_ANIMATION_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_PLAY_ANIMATION_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to start an animation once
 * triggered.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT PlayAnimationAction : public Action {

public:
  /**
   * @brief Instantiate the action.
   * @param triggerOptions defines the trigger options
   * @param target defines the target animation or animation name
   * @param from defines from where the animation should start (animation frame)
   * @param end defines where the animation should stop (animation frame)
   * @param loop defines if the animation should loop or stop after the first
   * play
   * @param condition defines the trigger related conditions
   */
  PlayAnimationAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                      int from, int to, bool loop = false,
                      Condition* condition = nullptr);
  ~PlayAnimationAction() override;

  /**
   * @brief Hidden
   */
  void _prepare() override;

  /**
   * @brief Execute the action and play the animation.
   */
  void execute(const ActionEvent& evt) override;

  /**
   * @brief Serializes the actions and its related information.
   * @param parent defines the object to serialize in
   * @returns the serialized object
   */
  json serialize(json& parent) const override;

public:
  /**
   * Where the animation should start (animation frame)
   */
  int from;

  /**
   * Where the animation should stop (animation frame)
   */
  int to;

  /**
   * Define if the animation should loop or stop after the first play.
   */
  bool loop;

private:
  IAnimatablePtr _target;

}; // end of class PlayAnimationAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_PLAY_ANIMATION_ACTION_H
