#ifndef BABYLON_ACTIONS_CONDITIONS_STATE_CONDITION_H
#define BABYLON_ACTIONS_CONDITIONS_STATE_CONDITION_H

#include <babylon/actions/condition.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class AnimationValue;

/**
 * @brief Defines a state condition as an extension of Condition.
 */
class BABYLON_SHARED_EXPORT StateCondition : public Condition {

public:
  /**
   * @brief Creates a new StateCondition.
   * @param actionManager manager for the action the condition applies to
   * @param target of the condition
   * @param value to compare with target state
   */
  StateCondition(ActionManager* actionManager, const IAnimatablePtr& target, AnimationValue* value);
  ~StateCondition() override; // = default

  /** Methods **/

  /**
   * @brief Gets a boolean indicating if the current condition is met.
   * @returns the validity of the state
   */
  bool isValid() override;

  /**
   * @brief Serialize the StateCondition into a JSON compatible object.
   * @returns serialization object
   */
  [[nodiscard]] json serialize() const override;

public:
  /**
   * @brief Value to compare with target state.
   */
  AnimationValue* value;

private:
  /** Members **/

  /**
   * Internal only
   */
  IAnimatablePtr _target;

}; // end of class StateCondition

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_CONDITIONS_STATE_CONDITION_H
