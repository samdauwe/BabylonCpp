#include <babylon/actions/directactions/set_state_action.h>

#include <babylon/animations/ianimatable.h>
#include <babylon/core/json.h>

namespace BABYLON {

SetStateAction::SetStateAction(unsigned int triggerOptions, IAnimatable* target,
                               const string_t& value, Condition* condition)
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

Json::object SetStateAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object(
      {Json::Pair("name", "SetStateAction"),
       Json::Pair(
         "properties",
         Json::array({{Json::value(Action::_GetTargetProperty(_target)),
                       Json::NameValuePair("value", _value)}}))}),
    parent);
}

} // end of namespace BABYLON
