#include <babylon/actions/directactions/increment_value_action.h>

#include <babylon/animations/ianimatable.h>
#include <nlohmann/json.hpp>

namespace BABYLON {

IncrementValueAction::IncrementValueAction(unsigned int triggerOptions,
                                           const IAnimatablePtr& target,
                                           const std::string& propertyPath,
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

json IncrementValueAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
