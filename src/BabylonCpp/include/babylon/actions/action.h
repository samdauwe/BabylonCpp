#ifndef BABYLON_ACTIONS_ACTION_H
#define BABYLON_ACTIONS_ACTION_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief The action to be carried out following a trigger.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions#available-actions
 */
class BABYLON_SHARED_EXPORT Action
    : public ::std::enable_shared_from_this<Action> {

public:
  template <typename... Ts>
  static shared_ptr_t<Action> New(Ts&&... args)
  {
    return std::make_shared<Action>(std::forward<Ts>(args)...);
  }

  /**
   * @brief Creates a new Action.
   * @param triggerOptions the trigger, with or without parameters, for the
   * action
   * @param condition an optional determinant of action
   */
  Action(unsigned int triggerOptions, Condition* condition = nullptr);
  Action(const TriggerOptions& triggerOptions, Condition* condition = nullptr);
  virtual ~Action();

  /** Methods **/

  /**
   * @brief Internal only.
   */
  virtual void _prepare();

  /**
   * @brief Gets the trigger parameters.
   * @returns the trigger parameters
   */
  string_t getTriggerParameter() const;

  /**
   * @brief Internal only - executes current action event.
   */
  void _executeCurrent(const ActionEvent& evt);

  /**
   * @brief Execute placeholder for child classes.
   * @param evt optional action event
   */
  virtual void execute(const ActionEvent& evt);

  /**
   * @brief Skips to next active action.
   */
  void skipToNextActiveAction();

  /**
   * @brief Adds action to chain of actions, may be a DoNothingAction.
   * @param action defines the next action to execute
   * @returns The action passed in
   * @see https://www.babylonjs-playground.com/#1T30HR#0
   */
  Action* then(Action* action);

  /**
   * @brief Internal only.
   */
  string_t _getProperty(const string_t& propertyPath);

  /**
   * @brief Internal only.
   */
  IAnimatablePtr _getEffectiveTarget(const IAnimatablePtr& target,
                                     const string_t& propertyPath);

  /**
   * @brief Serialize placeholder for child classes.
   * @param parent of child
   * @returns the serialized object
   */
  virtual Json::object serialize(Json::object& parent) const;

  /** Statics **/

  /**
   * @brief Internal only.
   */
  static string_t _SerializeValueAsString(const AnimationValue& value);

  /**
   * @brief Internal only.
   */
  static Json::object _GetTargetProperty(const IAnimatablePtr& target);

protected:
  /**
   * @brief Internal only called by serialize.
   */
  Json::object _serialize(const Json::object& serializedAction,
                          Json::object& parent) const;

public:
  /**
   * Trigger for the action
   */
  unsigned int trigger;

  /**
   * Internal only - manager for action
   */
  ActionManager* _actionManager;

  /**
   * An event triggered prior to action being executed.
   */
  Observable<Action> onBeforeExecuteObservable;

private:
  Action* _nextActiveAction;
  Action* _child;
  Condition* _condition;
  string_t _triggerParameter;

}; // end of class Action

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTION_H
