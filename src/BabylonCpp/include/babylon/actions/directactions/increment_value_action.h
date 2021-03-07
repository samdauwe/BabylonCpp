#ifndef BABYLON_ACTIONS_DIRECTACTIONS_INCREMENT_VALUE_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_INCREMENT_VALUE_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to increment the target value  to a desired value once
 * triggered.
 * @see https://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT IncrementValueAction : public Action {

public:
  /**
   * @brief Instantiate the action
   * @param triggerOptions defines the trigger options
   * @param target defines the object containing the property
   * @param propertyPath defines the path of the property to increment in the target
   * @param value defines the value value we should increment the property by
   * @param condition defines the trigger related conditions
   */
  IncrementValueAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                       const std::string& propertyPath, AnimationValue* value,
                       Condition* condition = nullptr);
  ~IncrementValueAction() override; // = default

  /**
   * @brief Hidden
   */
  void _prepare() override;

  /**
   * @brief Execute the action and increment the target of the value amount.
   */
  void execute(const IActionEventPtr& evt) override;

  /**
   * @brief Serializes the actions and its related information.
   * @param parent defines the object to serialize in
   * @returns the serialized object
   */
  json serialize(json& parent) const override;

public:
  /**
   * The path of the property to increment in the target.
   */
  std::string propertyPath;

  /**
   * The value we should increment the property by.
   */
  AnimationValue* value;

private:
  IAnimatablePtr _target;
  IAnimatablePtr _effectiveTarget;
  std::string _property;

}; // end of class IncrementValueAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_INCREMENT_VALUE_ACTION_H
