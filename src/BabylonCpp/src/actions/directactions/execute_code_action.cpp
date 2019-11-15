#include <babylon/actions/directactions/execute_code_action.h>

#include <babylon/actions/iaction_event.h>

namespace BABYLON {

ExecuteCodeAction::ExecuteCodeAction(
  unsigned int iTriggerOptions,
  const std::function<void(const std::optional<IActionEvent>& evt)>& iFunc,
  Condition* condition)
    : Action(iTriggerOptions, condition), func{iFunc}
{
}

ExecuteCodeAction::~ExecuteCodeAction() = default;

void ExecuteCodeAction::execute(const std::optional<IActionEvent>& evt)
{
  func(evt);
}

} // end of namespace BABYLON
