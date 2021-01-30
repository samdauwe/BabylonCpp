#include <babylon/actions/action.h>

#include <nlohmann/json.hpp>

#include <babylon/actions/action_event.h>
#include <babylon/actions/action_manager.h>
#include <babylon/actions/condition.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/engines/node.h>
#include <babylon/engines/scene.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

Action::Action(unsigned int iTriggerOptions, Condition* condition)
    : _nextActiveAction{this}, _condition{condition}
{
  trigger = iTriggerOptions;
}

Action::Action(const TriggerOptions& iTriggerOptions, Condition* condition)
    : _nextActiveAction{this}, _condition{condition}, _triggerParameter{triggerOptions.parameter}
{
  triggerOptions = iTriggerOptions;
}

Action::~Action() = default;

void Action::_prepare()
{
}

std::string Action::getTriggerParameter() const
{
  return _triggerParameter;
}

void Action::setTriggerParameter(const std::string& value)
{
  _triggerParameter = value;
}

void Action::_executeCurrent(const std::optional<IActionEvent>& evt)
{
  if (_nextActiveAction->_condition) {
    auto condition       = _nextActiveAction->_condition;
    auto currentRenderId = _actionManager->getScene()->getRenderId();

    // We cache the current evaluation for the current frame
    if (condition->_evaluationId == currentRenderId) {
      if (!condition->_currentResult) {
        return;
      }
    }
    else {
      condition->_evaluationId = currentRenderId;

      if (!condition->isValid()) {
        condition->_currentResult = false;
        return;
      }

      condition->_currentResult = true;
    }
  }

  onBeforeExecuteObservable.notifyObservers(this);
  _nextActiveAction->execute(evt);

  skipToNextActiveAction();
}

void Action::execute(const std::optional<IActionEvent>& /*evt*/)
{
}

void Action::skipToNextActiveAction()
{
  if (_nextActiveAction->_child) {

    if (!_nextActiveAction->_child->_actionManager) {
      _nextActiveAction->_child->_actionManager = _actionManager;
    }

    _nextActiveAction = _nextActiveAction->_child;
  }
  else {
    _nextActiveAction = this;
  }
}

IAction* Action::then(IAction* action)
{
  _child = static_cast<Action*>(action);

  action->_actionManager = _actionManager;
  action->_prepare();

  return action;
}

std::string Action::_getProperty(const std::string& propertyPath)
{
  return _actionManager->_getProperty(propertyPath);
}

IAnimatablePtr Action::_getEffectiveTarget(const IAnimatablePtr& target,
                                           const std::string& propertyPath)
{
  return _actionManager->_getEffectiveTarget(target, propertyPath);
}

json Action::serialize(json& /*parent*/) const
{
  return json();
}

json Action::_serialize(const json& /*serializedAction*/, json& /*parent*/) const
{
  return nullptr;
}

std::string Action::_SerializeValueAsString(const AnimationValue& value)
{
  auto animationType = value.animationType();
  if (!animationType.has_value()) {
    return "";
  }

  switch (animationType.value()) {
    case Animation::ANIMATIONTYPE_INT:
      return std::to_string(value.get<int>());
    case Animation::ANIMATIONTYPE_FLOAT:
      return std::to_string(value.get<float>());
    case Animation::ANIMATIONTYPE_BOOL:
      return value.get<bool>() ? "true" : "false";
    case Animation::ANIMATIONTYPE_STRING:
      return value.get<std::string>();
    case Animation::ANIMATIONTYPE_COLOR3: {
      const auto& _value = value.get<Color3>();
      return StringTools::concat(_value.r, ", ", _value.g, ", ", _value.b);
    }
    case Animation::ANIMATIONTYPE_COLOR4: {
      const auto& _value = value.get<Color4>();
      return StringTools::concat(_value.r, ", ", _value.g, ", ", _value.b, ", ", _value.a);
    }
    case Animation::ANIMATIONTYPE_VECTOR2: {
      const auto& _value = value.get<Vector2>();
      return StringTools::concat(_value.x, ", ", _value.y);
    }
    case Animation::ANIMATIONTYPE_VECTOR3: {
      const auto& _value = value.get<Vector3>();
      return StringTools::concat(_value.x, ", ", _value.y, ", ", _value.z);
    }
    default:
      return "";
  }
}

json Action::_GetTargetProperty(const IAnimatablePtr& /*target*/)
{
  return nullptr;
}

} // end of namespace BABYLON
