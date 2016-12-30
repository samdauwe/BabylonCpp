#include <babylon/actions/action.h>

#include <babylon/actions/action_event.h>
#include <babylon/actions/action_manager.h>
#include <babylon/actions/condition.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/core/json.h>
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

std::string Action::getTriggerParameter()
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

IAnimatable* Action::_getEffectiveTarget(IAnimatable* target,
                                         const std::string& propertyPath)
{
  return _actionManager->_getEffectiveTarget(target, propertyPath);
}

Json::object Action::serialize(Json::object& /*parent*/) const
{
  return Json::object();
}

Json::object Action::_serialize(const Json::object& serializedAction,
                                Json::object& /*parent*/) const
{
  auto serializationObject = Json::object(
    {Json::Pair("type", 1.0), Json::Pair("children", Json::array()),
     Json::Pair("name", serializedAction.at("name")),
     Json::Pair("name", serializedAction.at("properties"))});

  // Serialize child
  if (_child) {
    _child->serialize(serializationObject);
  }

  // Check if "this" has a condition
  /*if (_condition) {
    auto serializedCondition = _condition->serialize();
    serializedCondition.children.push(serializationObject);

    if (parent) {
      parent.children.push(serializedCondition);
    }
    return serializedCondition;
  }

  if (parent) {
    parent.children.push(serializationObject);
  }*/
  return serializationObject;
}

std::string Action::_SerializeValueAsString(const AnimationValue& value)
{
  switch (value.dataType) {
    case Animation::ANIMATIONTYPE_INT:
      return std::to_string(value.intData);
    case Animation::ANIMATIONTYPE_FLOAT:
      return std::to_string(value.floatData);
    case Animation::ANIMATIONTYPE_BOOL:
      return value.boolData ? "true" : "false";
    case Animation::ANIMATIONTYPE_STRING:
      return value.stringData;
    case Animation::ANIMATIONTYPE_COLOR3: {
      const Color3& _value = value.color3Data;
      return String::concat(_value.r, ", ", _value.g, ", ", _value.b);
    }
    case Animation::ANIMATIONTYPE_COLOR4: {
      const Color4& _value = value.color4Data;
      return String::concat(_value.r, ", ", _value.g, ", ", _value.b, ", ",
                            _value.a);
    }
    case Animation::ANIMATIONTYPE_VECTOR2: {
      const Vector2& _value = value.vector2Data;
      return String::concat(_value.x, ", ", _value.y);
    }
    case Animation::ANIMATIONTYPE_VECTOR3: {
      const Vector3& _value = value.vector3Data;
      return String::concat(_value.x, ", ", _value.y, ", ", _value.z);
    }
    default:
      return "";
  }
}

Json::object Action::_GetTargetProperty(IAnimatable* target)
{
  const std::string targetType
    = (target->animatableType() == IAnimatable::Type::MESH) ?
        "MeshProperties" :
        (target->animatableType() == IAnimatable::Type::LIGHT) ?
        "LightProperties" :
        (target->animatableType() == IAnimatable::Type::CAMERA) ?
        "CameraProperties" :
        "SceneProperties";
  const std::string value
    = (target->animatableType() == IAnimatable::Type::SCENE) ?
        "Scene" :
        dynamic_cast<Node*>(target)->name;
  return Json::object({Json::Pair("name", "target"),
                       Json::Pair("targetType", targetType),
                       Json::Pair("value", value)});
}

} // end of namespace BABYLON
