#include <babylon/actions/condition.h>

#include <babylon/actions/action_manager.h>
#include <babylon/core/json.h>

namespace BABYLON {

Condition::Condition(ActionManager* actionManager)
    : _actionManager{actionManager}, _currentResult{false}
{
}

Condition::Condition(const Condition& other)
    : _actionManager{other._actionManager}
    , _evaluationId{other._evaluationId}
    , _currentResult{other._currentResult}
{
}

Condition::Condition(Condition&& other)
    : _actionManager{::std::move(other._actionManager)}
    , _evaluationId{::std::move(other._evaluationId)}
    , _currentResult{::std::move(other._currentResult)}
{
}

Condition& Condition::operator=(const Condition& other)
{
  if (&other != this) {
    _actionManager = other._actionManager;
    _evaluationId  = other._evaluationId;
    _currentResult = other._currentResult;
  }

  return *this;
}

Condition& Condition::operator=(Condition&& other)
{
  if (&other != this) {
    _actionManager = ::std::move(other._actionManager);
    _evaluationId  = ::std::move(other._evaluationId);
    _currentResult = ::std::move(other._currentResult);
  }

  return *this;
}

Condition::~Condition()
{
}

bool Condition::isValid()
{
  return true;
}

string_t Condition::_getProperty(const string_t& propertyPath)
{
  return _actionManager->_getProperty(propertyPath);
}

IAnimatablePtr Condition::_getEffectiveTarget(const IAnimatablePtr& target,
                                              const string_t& propertyPath)
{
  return _actionManager->_getEffectiveTarget(target, propertyPath);
}

Json::object Condition::serialize() const
{
  return Json::object();
}

Json::object
Condition::_serialize(const Json::object& serializedCondition) const
{
  return Json::object(
    {Json::Pair("type", 2.0), // Condition
     Json::Pair("children", Json::array()),
     Json::Pair("name", serializedCondition.at("name")),
     Json::Pair("properties", serializedCondition.at("properties"))});
}

} // end of namespace BABYLON
