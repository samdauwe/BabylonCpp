#include <babylon/actions/directactions/set_state_action.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/ianimatable.h>

namespace BABYLON {

SetStateAction::SetStateAction(unsigned int iTtriggerOptions,
                               const IAnimatablePtr& target,
                               const std::string& iValue, Condition* condition)
    : Action(iTtriggerOptions, condition), value{iValue}, _target{target}
{
}

SetStateAction::~SetStateAction() = default;

void SetStateAction::execute(const std::optional<IActionEvent>& /*evt*/)
{
  _target->state = value;
}

json SetStateAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
