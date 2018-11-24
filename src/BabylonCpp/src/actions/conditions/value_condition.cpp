#include <babylon/actions/conditions/value_condition.h>

#include <nlohmann/json.hpp>

#include <babylon/actions/action.h>

namespace BABYLON {

ValueCondition::ValueCondition(ActionManager* actionManager,
                               const IAnimatablePtr& target,
                               const std::string& propertyPath,
                               AnimationValue* value, unsigned int operatorType)
    : Condition{actionManager}
    , _propertyPath{propertyPath}
    , _operatorType{operatorType}
    , _target{target}
    , _value{value}
{
  _effectiveTarget = _getEffectiveTarget(target, propertyPath);
  _property        = _getProperty(propertyPath);
}

ValueCondition::~ValueCondition()
{
}

bool ValueCondition::isValid()
{
  // TODO implement
  return true;
}

json ValueCondition::serialize() const
{
  return nullptr;
}

std::string ValueCondition::GetOperatorName(unsigned int operatorType)
{
  switch (operatorType) {
    case ValueCondition::IsEqual():
      return "IsEqual";
    case ValueCondition::IsDifferent():
      return "IsDifferent";
    case ValueCondition::IsGreater():
      return "IsGreater";
    case ValueCondition::IsLesser():
      return "IsLesser";
    default:
      return "";
  }
}

} // end of namespace BABYLON
