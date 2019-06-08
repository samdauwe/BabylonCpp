#include <babylon/actions/directactions/execute_code_action.h>

namespace BABYLON {

ExecuteCodeAction::ExecuteCodeAction(
  unsigned int iTriggerOptions,
  const std::function<void(const ActionEvent&)>& iFunc, Condition* condition)
    : Action(iTriggerOptions, condition), func{iFunc}
{
}

ExecuteCodeAction::~ExecuteCodeAction()
{
}

void ExecuteCodeAction::execute(const ActionEvent& evt)
{
  func(evt);
}

} // end of namespace BABYLON
