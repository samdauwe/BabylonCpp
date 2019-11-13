#include <babylon/actions/condition.h>

#include <nlohmann/json.hpp>

#include <babylon/actions/action_manager.h>

namespace BABYLON {

Condition::Condition(ActionManager* actionManager)
    : _actionManager{actionManager}, _currentResult{false}
{
}

Condition::Condition(const Condition& other) = default;

Condition::Condition(Condition&& other) = default;

Condition& Condition::operator=(const Condition& other) = default;

Condition& Condition::operator=(Condition&& other) = default;

Condition::~Condition() = default;

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
