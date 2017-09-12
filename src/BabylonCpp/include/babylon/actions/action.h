#ifndef BABYLON_ACTIONS_ACTION_H
#define BABYLON_ACTIONS_ACTION_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>

namespace BABYLON {

/**
 * @brief Actions are a simple way to add interactions in your scenes. An action
 * is launched when its trigger is fired.
 */
class BABYLON_SHARED_EXPORT Action
    : public ::std::enable_shared_from_this<Action> {

public:
  template <typename... Ts>
  static std::shared_ptr<Action> New(Ts&&... args)
  {
    return std::make_shared<Action>(std::forward<Ts>(args)...);
  }

  Action(unsigned int triggerOptions, Condition* condition = nullptr);
  Action(const TriggerOptions& triggerOptions, Condition* condition = nullptr);
  virtual ~Action();

  /** Methods **/
  virtual void _prepare();
  std::string getTriggerParameter();
  void _executeCurrent(const ActionEvent& evt);
  virtual void execute(const ActionEvent& evt);
  void skipToNextActiveAction();
  Action* then(Action* action);
  std::string _getProperty(const std::string& propertyPath);
  IAnimatable* _getEffectiveTarget(IAnimatable* target,
                                   const std::string& propertyPath);
  virtual Json::object serialize(Json::object& parent) const;

  /** Statics **/
  static std::string _SerializeValueAsString(const AnimationValue& value);
  static Json::object _GetTargetProperty(IAnimatable* target);

protected:
  // Called by BABYLON.Action objects in serialize(...). Internal use
  Json::object _serialize(const Json::object& serializedAction,
                          Json::object& parent) const;

public:
  unsigned int trigger;
  ActionManager* _actionManager;

private:
  Action* _nextActiveAction;
  Action* _child;
  Condition* _condition;
  std::string _triggerParameter;

}; // end of class Action

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTION_H
