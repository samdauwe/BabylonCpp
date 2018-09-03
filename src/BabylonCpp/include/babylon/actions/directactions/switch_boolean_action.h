#ifndef BABYLON_ACTIONS_DIRECTACTIONS_SWITCH_BOOLEAN_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_SWITCH_BOOLEAN_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SwitchBooleanAction : public Action {

public:
  SwitchBooleanAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                      const string_t& propertyPath,
                      Condition* condition = nullptr);
  ~SwitchBooleanAction() override;

  /**
   * @brief Hidden
   */
  void _prepare() override;

  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

private:
  IAnimatablePtr _target;
  IAnimatablePtr _effectiveTarget;
  string_t _property;
  string_t _propertyPath;

}; // end of class SwitchBooleanAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_SWITCH_BOOLEAN_ACTION_H
