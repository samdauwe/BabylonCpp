#include <babylon/actions/directactions/set_value_action.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/ianimatable.h>

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

json SetValueAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
