#ifndef BABYLON_ACTIONS_DIRECTACTIONS_DO_NOTHING_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_DO_NOTHING_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/actions/action_manager.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT DoNothingAction : public Action {

public:
  DoNothingAction(unsigned int triggerOptions = ActionManager::NothingTrigger(),
                  Condition* condition        = nullptr);
  ~DoNothingAction() override;

  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

}; // end of class DoNothingAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_DO_NOTHING_ACTION_H
