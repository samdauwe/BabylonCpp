#ifndef BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to stop an animation once
 * triggered.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT StopAnimationAction : public Action {

public:
  /**
   * Instantiate the action
   * @param triggerOptions defines the trigger options
   * @param target defines the target animation or animation name
   * @param condition defines the trigger related conditions
   */
  StopAnimationAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                      Condition* condition = nullptr);
  ~StopAnimationAction(); // = default

  /**
   * @brief Hidden
   */
  void _prepare() override;

  /**
   * @brief Execute the action and stop the animation.
   */
  void execute(const std::optional<IActionEvent>& evt) override;

  /**
   * @brief Serializes the actions and its related information.
   * @param parent defines the object to serialize in
   * @returns the serialized object
   */
  json serialize(json& parent) const override;

private:
  IAnimatablePtr _target;

}; // end of class StopAnimationAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
