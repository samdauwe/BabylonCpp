#ifndef BABYLON_ACTIONS_DIRECTACTIONS_SET_PARENT_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_SET_PARENT_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SetParentAction : public Action {

public:
  SetParentAction(unsigned int triggerOptions, IAnimatable* target,
                  IAnimatable* parent, Condition* condition = nullptr);
  ~SetParentAction();

  void _prepare() override;
  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

private:
  IAnimatable* _target;
  IAnimatable* _parent;

}; // end of class SetParentAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_SET_PARENT_ACTION_H
