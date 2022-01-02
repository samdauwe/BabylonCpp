#include <babylon/actions/directactions/do_nothing_action.h>

#include <nlohmann/json.hpp>

namespace BABYLON {

DoNothingAction::DoNothingAction(unsigned int iTtriggerOptions, Condition* condition)
    : Action(iTtriggerOptions, condition)
{
}

DoNothingAction::~DoNothingAction() = default;

void DoNothingAction::execute(const std::optional<IActionEvent>& /*evt*/)
{
}

json DoNothingAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
