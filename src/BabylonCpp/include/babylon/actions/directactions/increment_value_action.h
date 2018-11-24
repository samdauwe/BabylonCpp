#ifndef BABYLON_ACTIONS_DIRECTACTIONS_INCREMENT_VALUE_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_INCREMENT_VALUE_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT IncrementValueAction : public Action {

public:
  IncrementValueAction(unsigned int triggerOptions,
                       const IAnimatablePtr& target,
                       const std::string& propertyPath, AnimationValue* value,
                       Condition* condition = nullptr);
  ~IncrementValueAction() override;

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

}; // end of class IncrementValueAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_INCREMENT_VALUE_ACTION_H
