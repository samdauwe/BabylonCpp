#include <babylon/actions/action.h>

#include <nlohmann/json.hpp>

#include <babylon/actions/action_event.h>
#include <babylon/actions/action_manager.h>
#include <babylon/actions/condition.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/core/string.h>
#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

Action::Action(unsigned int triggerOptions, Condition* condition)
    : trigger{triggerOptions}, _nextActiveAction{this}, _condition{condition}
{
}

Action::Action(const TriggerOptions& triggerOptions, Condition* condition)
    : trigger{triggerOptions.trigger}
    , _nextActiveAction{this}
    , _condition{condition}
    , _triggerParameter{triggerOptions.parameter}
{
}

Action::~Action()
{
}

/** Methods **/
void Action::_prepare()
{
}

std::string Action::getTriggerParameter() const
{
  return _triggerParameter;
}

void Action::_executeCurrent(const ActionEvent& evt)
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

void Action::execute(const ActionEvent& /*evt*/)
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

Action* Action::then(Action* action)
{
  _child = action;

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

json Action::_serialize(const json& /*serializedAction*/,
                        json& /*parent*/) const
{
  return nullptr;
}

std::string Action::_SerializeValueAsString(const AnimationValue& value)
{
  switch (value.dataType) {
    case Animation::ANIMATIONTYPE_INT():
      return std::to_string(value.intData);
    case Animation::ANIMATIONTYPE_FLOAT():
      return std::to_string(value.floatData);
    case Animation::ANIMATIONTYPE_BOOL():
      return value.boolData ? "true" : "false";
    case Animation::ANIMATIONTYPE_STRING():
      return value.stringData;
    case Animation::ANIMATIONTYPE_COLOR3(): {
      const Color3& _value = value.color3Data;
      return String::concat(_value.r, ", ", _value.g, ", ", _value.b);
    }
    case Animation::ANIMATIONTYPE_COLOR4(): {
      const Color4& _value = value.color4Data;
      return String::concat(_value.r, ", ", _value.g, ", ", _value.b, ", ",
                            _value.a);
    }
    case Animation::ANIMATIONTYPE_VECTOR2(): {
      const Vector2& _value = value.vector2Data;
      return String::concat(_value.x, ", ", _value.y);
    }
    case Animation::ANIMATIONTYPE_VECTOR3(): {
      const Vector3& _value = value.vector3Data;
      return String::concat(_value.x, ", ", _value.y, ", ", _value.z);
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
