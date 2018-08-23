#include <babylon/actions/directactions/increment_value_action.h>

#include <babylon/animations/ianimatable.h>
#include <babylon/core/json.h>

namespace BABYLON {

IncrementValueAction::IncrementValueAction(unsigned int triggerOptions,
                                           const IAnimatablePtr& target,
                                           const string_t& propertyPath,
                                           AnimationValue* value,
                                           Condition* condition)
    : Action(triggerOptions, condition)
    , _target{target}
    , _effectiveTarget{target}
    , _propertyPath{propertyPath}
    , _value{value}
{
}

IncrementValueAction::~IncrementValueAction()
{
}

void IncrementValueAction::_prepare()
{
  _effectiveTarget = _getEffectiveTarget(_effectiveTarget, _propertyPath);
  _property        = _getProperty(_propertyPath);
}

void IncrementValueAction::execute(const ActionEvent& /*evt*/)
{
  // _effectiveTarget[_property] = value;

  /*if (_target->markAsDirty()) {
    _target->markAsDirty(_property);
  }*/
}

Json::object IncrementValueAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object(
      {Json::Pair("name", "IncrementValueAction"),
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
