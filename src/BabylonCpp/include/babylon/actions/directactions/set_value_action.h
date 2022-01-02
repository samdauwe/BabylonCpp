#ifndef BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to set a property of the target
 *  to a desired value once triggered.
 * @see https://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT SetValueAction : public Action {

public:
  /**
   * @brief Instantiate the action.
   * @param triggerOptions defines the trigger options
   * @param target defines the object containing the property
   * @param propertyPath defines the path of the property to set in the target
   * @param value defines the value to set in the property
   * @param condition defines the trigger related conditions
   */
  SetValueAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                 const std::string& propertyPath, AnimationValue* value,
                 Condition* condition = nullptr);
  ~SetValueAction() override; // = default

  /**
   * @brief Hidden
   */
  void _prepare() override;

  /**
   * @brief Execute the action and set the targeted property to the desired value.
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
   * The path of the property to set in the target.
   */
  std::string propertyPath;

  /**
   * The value to set in the property
   */
  AnimationValue* value;

private:
  IAnimatablePtr _target;
  IAnimatablePtr _effectiveTarget;
  std::string _property;

}; // end of class SetValueAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
