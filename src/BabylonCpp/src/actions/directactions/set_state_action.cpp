#include <babylon/actions/directactions/set_state_action.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/ianimatable.h>

namespace BABYLON {

SetStateAction::SetStateAction(unsigned int triggerOptions,
                               const IAnimatablePtr& target,
                               const std::string& value, Condition* condition)
    : Action(triggerOptions, condition), _target{target}, _value{value}
{
}

SetStateAction::~SetStateAction()
{
}

void SetStateAction::execute(const ActionEvent& /*evt*/)
{
  _target->state = _value;
}

json SetStateAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
