#ifndef BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT StopAnimationAction : public Action {

public:
  StopAnimationAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                      Condition* condition = nullptr);
  ~StopAnimationAction() override;

  void _prepare() override;
  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

private:
  IAnimatablePtr _target;

}; // end of class StopAnimationAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_STOP_ANIMATION_ACTION_H
