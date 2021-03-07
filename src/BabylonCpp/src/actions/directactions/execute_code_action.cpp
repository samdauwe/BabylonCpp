#include <babylon/actions/directactions/execute_code_action.h>

#include <babylon/actions/iaction_event.h>

namespace BABYLON {

ExecuteCodeAction::ExecuteCodeAction(unsigned int iTriggerOptions,
                                     const std::function<void(const IActionEventPtr& evt)>& iFunc,
                                     Condition* condition)
    : Action(iTriggerOptions, condition), func{iFunc}
{
}

ExecuteCodeAction::~ExecuteCodeAction() = default;

void ExecuteCodeAction::execute(const IActionEventPtr& evt)
{
  func(evt);
}

} // end of namespace BABYLON
