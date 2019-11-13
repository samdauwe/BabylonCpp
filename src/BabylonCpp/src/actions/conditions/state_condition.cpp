#include <babylon/actions/conditions/state_condition.h>

#include <nlohmann/json.hpp>

#include <babylon/actions/action.h>

namespace BABYLON {

StateCondition::StateCondition(ActionManager* actionManager,
                               const IAnimatablePtr& target,
                               AnimationValue* iValue)
    : Condition{actionManager}, value{iValue}, _target{target}
{
}

StateCondition::~StateCondition() = default;

bool StateCondition::isValid()
{
  return false; // _target->state == value;
}

json StateCondition::serialize() const
{
  return nullptr;
}

} // end of namespace BABYLON
