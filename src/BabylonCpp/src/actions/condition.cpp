#include <babylon/actions/condition.h>

#include <nlohmann/json.hpp>

#include <babylon/actions/action_manager.h>

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
    : _actionManager{std::move(other._actionManager)}
    , _evaluationId{std::move(other._evaluationId)}
    , _currentResult{std::move(other._currentResult)}
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
    _actionManager = std::move(other._actionManager);
    _evaluationId  = std::move(other._evaluationId);
    _currentResult = std::move(other._currentResult);
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

std::string Condition::_getProperty(const std::string& propertyPath)
{
  return _actionManager->_getProperty(propertyPath);
}

IAnimatablePtr Condition::_getEffectiveTarget(const IAnimatablePtr& target,
                                              const std::string& propertyPath)
{
  return _actionManager->_getEffectiveTarget(target, propertyPath);
}

json Condition::serialize() const
{
  return nullptr;
}

json Condition::_serialize(const json& /*serializedCondition*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
