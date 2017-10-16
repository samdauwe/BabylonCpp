#ifndef BABYLON_ACTIONS_ACTION_H
#define BABYLON_ACTIONS_ACTION_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief Actions are a simple way to add interactions in your scenes. An action
 * is launched when its trigger is fired.
 */
class BABYLON_SHARED_EXPORT Action
  : public ::std::enable_shared_from_this<Action> {

public:
  template <typename... Ts>
  static shared_ptr_t<Action> New(Ts&&... args)
  {
    return std::make_shared<Action>(std::forward<Ts>(args)...);
  }

  Action(unsigned int triggerOptions, Condition* condition = nullptr);
  Action(const TriggerOptions& triggerOptions, Condition* condition = nullptr);
  virtual ~Action();

  /** Methods **/
  virtual void _prepare();
  string_t getTriggerParameter();
  void _executeCurrent(const ActionEvent& evt);
  virtual void execute(const ActionEvent& evt);
  void skipToNextActiveAction();
  Action* then(Action* action);
  string_t _getProperty(const string_t& propertyPath);
  IAnimatable* _getEffectiveTarget(IAnimatable* target,
                                   const string_t& propertyPath);
  virtual Json::object serialize(Json::object& parent) const;

  /** Statics **/
  static string_t _SerializeValueAsString(const AnimationValue& value);
  static Json::object _GetTargetProperty(IAnimatable* target);

protected:
  // Called by BABYLON.Action objects in serialize(...). Internal use
  Json::object _serialize(const Json::object& serializedAction,
                          Json::object& parent) const;

public:
  unsigned int trigger;
  ActionManager* _actionManager;
  Observable<Action> onBeforeExecuteObservable;

private:
  Action* _nextActiveAction;
  Action* _child;
  Condition* _condition;
  string_t _triggerParameter;

}; // end of class Action

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTION_H
