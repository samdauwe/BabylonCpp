#ifndef BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SetValueAction : public Action {

public:
  SetValueAction(unsigned int triggerOptions, IAnimatable* target,
                 const string_t& propertyPath, AnimationValue* value,
                 Condition* condition = nullptr);
  ~SetValueAction() override;

  void _prepare() override;
  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

private:
  IAnimatable* _target;
  IAnimatable* _effectiveTarget;
  string_t _property;
  string_t _propertyPath;
  AnimationValue* _value;

}; // end of class SetValueAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
