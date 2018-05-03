#ifndef BABYLON_ACTIONS_CONDITIONS_STATE_CONDITION_H
#define BABYLON_ACTIONS_CONDITIONS_STATE_CONDITION_H

#include <babylon/actions/condition.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

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
  StateCondition(ActionManager* actionManager, IAnimatable* target,
                 AnimationValue* value);
  virtual ~StateCondition() override;

  /** Methods **/

  /**
   * @returns the validity of the state.
   */
  bool isValid() override;

  /**
   * @brief Serialize the StateCondition into a JSON compatible object.
   * @returns serialization object
   */
  Json::object serialize() const override;

public:
  AnimationValue* value;

private:
  /** Members **/

  /**
   * Internal only
   */
  IAnimatable* _target;

}; // end of class StateCondition

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_CONDITIONS_STATE_CONDITION_H
