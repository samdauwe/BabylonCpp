#include <babylon/actions/directactions/set_value_action.h>

#include <babylon/animations/ianimatable.h>
#include <babylon/core/json.h>

namespace BABYLON {

SetValueAction::SetValueAction(unsigned int triggerOptions,
                               const IAnimatablePtr& target,
                               const std::string& propertyPath,
                               AnimationValue* value, Condition* condition)
    : Action(triggerOptions, condition)
    , _target{target}
    , _effectiveTarget{target}
    , _propertyPath{propertyPath}
    , _value{value}
{
}

SetValueAction::~SetValueAction()
{
}

void SetValueAction::_prepare()
{
  _effectiveTarget = _getEffectiveTarget(_effectiveTarget, _propertyPath);
  _property        = _getProperty(_propertyPath);
}

void SetValueAction::execute(const ActionEvent& /*evt*/)
{
  // _effectiveTarget[_property] = value;

  // if (_target->markAsDirty()) {
  //  _target->markAsDirty(_property);
  //}
}

Json::object SetValueAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object(
      {Json::Pair("name", "SetValueAction"),
       Json::Pair("properties",
                  Json::array({{
                    Json::value(Action::_GetTargetProperty(_target)),
                    Json::NameValuePair("propertyPath", _propertyPath),
                    Json::NameValuePair(
                      "value", Action::_SerializeValueAsString(_value)),
                  }}))}),
    parent);
}

} // end of namespace BABYLON
