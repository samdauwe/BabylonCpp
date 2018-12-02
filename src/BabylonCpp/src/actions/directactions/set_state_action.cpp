#include <babylon/actions/directactions/set_state_action.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/ianimatable.h>

namespace BABYLON {

SetStateAction::SetStateAction(unsigned int triggerOptions,
                               const IAnimatablePtr& target,
                               const std::string& iValue, Condition* condition)
    : Action(triggerOptions, condition), value{iValue}, _target{target}
{
}

SetStateAction::~SetStateAction()
{
}

void SetStateAction::execute(const ActionEvent& /*evt*/)
{
  _target->state = value;
}

json SetStateAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
