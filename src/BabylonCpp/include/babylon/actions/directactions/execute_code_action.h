#ifndef BABYLON_ACTIONS_DIRECTACTIONS_EXECUTE_CODE_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_EXECUTE_CODE_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ExecuteCodeAction : public Action {

public:
  ExecuteCodeAction(unsigned int triggerOptions,
                    const ::std::function<void(const ActionEvent&)>& func,
                    Condition* condition = nullptr);
  ~ExecuteCodeAction() override;

  void execute(const ActionEvent& evt) override;

public:
  ::std::function<void(const ActionEvent&)> func;

}; // end of class ExecuteCodeAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_EXECUTE_CODE_ACTION_H
