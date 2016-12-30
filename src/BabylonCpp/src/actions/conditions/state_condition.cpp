#include <babylon/actions/conditions/state_condition.h>

#include <babylon/actions/action.h>
#include <babylon/core/json.h>

namespace BABYLON {

StateCondition::StateCondition(ActionManager* actionManager,
                               IAnimatable* target, AnimationValue* iValue)
    : Condition{actionManager}, value{iValue}, _target{target}
{
}

StateCondition::~StateCondition()
{
}

bool StateCondition::isValid()
{
  return false; // _target->state == value;
}

Json::object StateCondition::serialize() const
{
  auto object = Json::object(
    {Json::Pair("name", "StateCondition"),
     Json::Pair(
       "properties",
       Json::array({{Json::value(Action::_GetTargetProperty(_target)),
                     Json::NameValuePair(
                       "value", Action::_SerializeValueAsString(*value))}}))});
  return _serialize(object);
}

} // end of namespace BABYLON
