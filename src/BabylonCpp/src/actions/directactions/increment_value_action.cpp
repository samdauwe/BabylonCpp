#include <babylon/actions/directactions/increment_value_action.h>

#include <babylon/animations/ianimatable.h>
#include <nlohmann/json.hpp>

namespace BABYLON {

IncrementValueAction::IncrementValueAction(unsigned int iTtriggerOptions,
                                           const IAnimatablePtr& target,
                                           const std::string& iPropertyPath,
                                           AnimationValue* iValue,
                                           Condition* condition)
    : Action(iTtriggerOptions, condition)
    , propertyPath{iPropertyPath}
    , value{iValue}
    , _target{target}
    , _effectiveTarget{target}
{
}

IncrementValueAction::~IncrementValueAction()
{
}

void IncrementValueAction::_prepare()
{
  _effectiveTarget = _getEffectiveTarget(_effectiveTarget, propertyPath);
  _property        = _getProperty(propertyPath);
}

void IncrementValueAction::execute(const std::optional<IActionEvent>& /*evt*/)
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
