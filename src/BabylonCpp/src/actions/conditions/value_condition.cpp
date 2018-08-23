#include <babylon/actions/conditions/value_condition.h>

#include <babylon/actions/action.h>
#include <babylon/core/json.h>

namespace BABYLON {

ValueCondition::ValueCondition(ActionManager* actionManager,
                               const IAnimatablePtr& target,
                               const string_t& propertyPath,
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

Json::object ValueCondition::serialize() const
{
  auto object = Json::object(
    {Json::Pair("name", "ValueCondition"),
     Json::Pair(
       "properties",
       Json::array(
         {{Json::value(Action::_GetTargetProperty(_target)),
           Json::NameValuePair("propertyPath", _propertyPath),
           Json::NameValuePair("value",
                               Action::_SerializeValueAsString(*_value)),
           Json::NameValuePair<string_t>(
             "operator", ValueCondition::GetOperatorName(_operatorType))}}))});
  return _serialize(object);
}

string_t ValueCondition::GetOperatorName(unsigned int operatorType)
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
