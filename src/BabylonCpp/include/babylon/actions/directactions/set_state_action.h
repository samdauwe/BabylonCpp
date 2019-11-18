#ifndef BABYLON_ACTIONS_DIRECTACTIONS_SET_STATE_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_SET_STATE_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to set a the state field of the
 * target to a desired value once triggered.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT SetStateAction : public Action {

public:
  /**
   * @brief Instantiate the action.
   * @param triggerOptions defines the trigger options
   * @param target defines the object containing the state property
   * @param value defines the value to store in the state field
   * @param condition defines the trigger related conditions
   */
  SetStateAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                 const std::string& value, Condition* condition = nullptr);
  ~SetStateAction() override; // = default

  /**
   * @brief Execute the action and store the value on the target state property.
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
   * The value to store in the state field.
   */
  std::string value;

private:
  IAnimatablePtr _target;

}; // end of class SetStateAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_SET_STATE_ACTION_H
