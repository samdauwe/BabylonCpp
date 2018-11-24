#ifndef BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SetValueAction : public Action {

public:
  SetValueAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                 const std::string& propertyPath, AnimationValue* value,
                 Condition* condition = nullptr);
  ~SetValueAction() override;

  /**
   * @brief Hidden
   */
  void _prepare() override;

  void execute(const ActionEvent& evt) override;
  json serialize(json& parent) const override;

private:
  IAnimatablePtr _target;
  IAnimatablePtr _effectiveTarget;
  std::string _property;
  std::string _propertyPath;
  AnimationValue* _value;

}; // end of class SetValueAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
