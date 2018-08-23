#include <babylon/actions/directactions/switch_boolean_action.h>

#include <babylon/animations/animation_value.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/core/json.h>

namespace BABYLON {

SwitchBooleanAction::SwitchBooleanAction(unsigned int triggerOptions,
                                         const IAnimatablePtr& target,
                                         const string_t& propertyPath,
                                         Condition* condition)
    : Action(triggerOptions, condition)
    , _target{target}
    , _effectiveTarget{target}
    , _propertyPath{propertyPath}
{
}

SwitchBooleanAction::~SwitchBooleanAction()
{
}

void SwitchBooleanAction::_prepare()
{
  _effectiveTarget = _getEffectiveTarget(_effectiveTarget, _propertyPath);
  _property        = _getProperty(_propertyPath);
}

void SwitchBooleanAction::execute(const ActionEvent& /*evt*/)
{
  (*_effectiveTarget)[_property] = !(*_effectiveTarget)[_property];
}

Json::object SwitchBooleanAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object(
      {Json::Pair("name", "SwitchBooleanAction"),
       Json::Pair(
         "properties",
         Json::array({{Json::value(Action::_GetTargetProperty(_target)),
                       Json::NameValuePair("propertyPath", _propertyPath)}}))}),
    parent);
}

} // end of namespace BABYLON
