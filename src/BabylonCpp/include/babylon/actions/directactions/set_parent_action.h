#ifndef BABYLON_ACTIONS_DIRECTACTIONS_SET_PARENT_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_SET_PARENT_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to set the parent property of the target once
 * triggered.
 * @see https://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT SetParentAction : public Action {

public:
  /**
   * @brief Instantiate the action.
   * @param triggerOptions defines the trigger options
   * @param target defines the target containing the parent property
   * @param parent defines from where the animation should start (animation frame)
   * @param condition defines the trigger related conditions
   */
  SetParentAction(unsigned int triggerOptions, const IAnimatablePtr& target, IAnimatable* parent,
                  Condition* condition = nullptr);
  ~SetParentAction() override; // = default

  /**
   * @brief Hidden
   */
  void _prepare() override;

  /**
   * @brief Execute the action and set the parent property.
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
  IAnimatablePtr _parent;

}; // end of class SetParentAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_SET_PARENT_ACTION_H
