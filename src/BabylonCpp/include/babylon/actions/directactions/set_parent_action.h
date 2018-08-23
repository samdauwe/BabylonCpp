#ifndef BABYLON_ACTIONS_DIRECTACTIONS_SET_PARENT_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_SET_PARENT_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SetParentAction : public Action {

public:
  SetParentAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                  IAnimatable* parent, Condition* condition = nullptr);
  ~SetParentAction() override;

  void _prepare() override;
  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

private:
  IAnimatablePtr _target;
  IAnimatablePtr _parent;

}; // end of class SetParentAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_SET_PARENT_ACTION_H
