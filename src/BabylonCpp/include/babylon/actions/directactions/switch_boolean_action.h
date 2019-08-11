#ifndef BABYLON_ACTIONS_DIRECTACTIONS_SWITCH_BOOLEAN_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_SWITCH_BOOLEAN_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to toggle a boolean once triggered.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT SwitchBooleanAction : public Action {

public:
  /**
   * @brief Instantiate the action.
   * @param triggerOptions defines the trigger options
   * @param target defines the object containing the boolean
   * @param propertyPath defines the path to the boolean property in the target
   * object
   * @param condition defines the trigger related conditions
   */
  SwitchBooleanAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                      const std::string& propertyPath,
                      Condition* condition = nullptr);
  ~SwitchBooleanAction() override;

  /**
   * @brief Hidden
   */
  void _prepare() override;

  /**
   * @brief Execute the action toggle the boolean value.
   */
  void execute(const std::optional<IActionEvent>& evt) override;

  /**
   * @brief Serializes the actions and its related information.
   * @param parent defines the object to serialize in
   * @returns the serialized object
   */
  json serialize(json& parent) const override;

public:
  /**
   * The path to the boolean property in the target object
   */
  std::string propertyPath;

private:
  IAnimatablePtr _target;
  IAnimatablePtr _effectiveTarget;
  std::string _property;

}; // end of class SwitchBooleanAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_SWITCH_BOOLEAN_ACTION_H
