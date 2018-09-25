#include <babylon/actions/directactions/execute_code_action.h>

#include <babylon/core/json.h>

namespace BABYLON {

ExecuteCodeAction::ExecuteCodeAction(
  unsigned int triggerOptions,
  const std::function<void(const ActionEvent&)>& iFunc, Condition* condition)
    : Action(triggerOptions, condition), func{iFunc}
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
