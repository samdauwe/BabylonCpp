#include <babylon/actions/action_manager.h>

#include <babylon/actions/action.h>
#include <babylon/actions/action_event.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

std::array<unsigned int, 17> ActionManager::Triggers{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
size_t ActionManager::DragMovementThreshold = 10;  // in pixels
size_t ActionManager::LongPressDelay        = 500; // in milliseconds

ActionManager::ActionManager(Scene* scene)
    : hoverCursor{""}, _scene{scene ? scene : Engine::LastCreatedScene()}
{
}

ActionManager::~ActionManager()
{
}

void ActionManager::addToScene(
  const std::shared_ptr<ActionManager>& newActionManager)
{
  _scene->actionManagers.emplace_back(newActionManager);
}

void ActionManager::dispose()
{
  for (auto& action : actions) {
    if ((action->trigger < ActionManager::Triggers.size())
        && (ActionManager::Triggers[action->trigger] > 0)) {
      --ActionManager::Triggers[action->trigger];
    }
  }

  _scene->actionManagers.erase(
    std::remove_if(_scene->actionManagers.begin(), _scene->actionManagers.end(),
                   [this](const std::shared_ptr<ActionManager>& actionManager) {
                     return actionManager.get() == this;
                   }),
    _scene->actionManagers.end());
}

Scene* ActionManager::getScene() const
{
  return _scene;
}

bool ActionManager::hasSpecificTriggers(const Uint32Array& triggers) const
{
  return std::find_if(actions.begin(), actions.end(),
                      [&triggers](Action* action) {
                        return std::find(triggers.begin(), triggers.end(),
                                         action->trigger)
                               != triggers.end();
                      })
         != actions.end();
}

bool ActionManager::hasSpecificTriggers2(unsigned int triggerA,
                                         unsigned int triggerB) const
{
  for (const auto& action : actions) {
    if (triggerA == action->trigger || triggerB == action->trigger) {
      return true;
    }
  }

  return false;
}

bool ActionManager::hasSpecificTrigger(
  unsigned int trigger,
  const std::function<bool(const std::string& parameter)>& parameterPredicate)
  const
{
  return std::find_if(
           actions.begin(), actions.end(),
           [&trigger, &parameterPredicate](Action* action) {
             if (action->trigger == trigger) {
               if (parameterPredicate) {
                 if (parameterPredicate(action->getTriggerParameter())) {
                   return true;
                 }
               }
               else {
                 return true;
               }
             }
             return false;
           })
         != actions.end();
}

bool ActionManager::hasPointerTriggers() const
{

  return std::find_if(actions.begin(), actions.end(),
                      [](Action* action) {
                        return action->trigger >= ActionManager::OnPickTrigger()
                               && action->trigger
                                    <= ActionManager::OnPointerOutTrigger();
                      })
         != actions.end();
}

bool ActionManager::hasPickTriggers() const
{
  return std::find_if(actions.begin(), actions.end(),
                      [](Action* action) {
                        return action->trigger >= ActionManager::OnPickTrigger()
                               && action->trigger
                                    <= ActionManager::OnPickUpTrigger();
                      })
         != actions.end();
}

bool ActionManager::HasTriggers()
{
  return std::accumulate(ActionManager::Triggers.begin(),
                         ActionManager::Triggers.end(), 0)
         != 0;
}

bool ActionManager::HasPickTriggers()
{
  const auto start = ActionManager::OnPickTrigger();
  const auto end   = ActionManager::OnPickUpTrigger() + 1;

  return std::accumulate(ActionManager::Triggers.begin() + start,
                         ActionManager::Triggers.begin() + end, 0)
         != 0;
}

bool ActionManager::HasSpecificTrigger(unsigned int trigger)
{
  return (trigger < ActionManager::Triggers.size())
         && (ActionManager::Triggers[trigger] > 0);
}

Action* ActionManager::registerAction(Action* action)
{
  if (action->trigger == ActionManager::OnEveryFrameTrigger()) {
    if (getScene()->actionManager != this) {
      BABYLON_LOG_WARN(
        "ActionManager",
        "OnEveryFrameTrigger can only be used with scene.actionManager");
      return nullptr;
    }
  }

  actions.emplace_back(action);

  if (action->trigger < ActionManager::Triggers.size()) {
    ++ActionManager::Triggers[action->trigger];
  }

  action->_actionManager = this;
  action->_prepare();

  return action;
}

bool ActionManager::unregisterAction(Action* action)
{
  auto index = stl_util::index_of(actions, action);
  if (index == -1) {
    stl_util::remove(actions, static_cast<size_t>(index));
    ActionManager::Triggers[action->trigger] -= 1;
    if (ActionManager::Triggers[action->trigger] == 0) {
      // delete ActionManager::Triggers[action->trigger]
    }
    action->_actionManager = nullptr;
    return true;
  }
  return false;
}

void ActionManager::processTrigger(unsigned int trigger,
                                   const ActionEvent& evt) const
{
  for (const auto& action : actions) {
    if (action->trigger == trigger) {
      if (trigger == ActionManager::OnKeyUpTrigger()
          || trigger == ActionManager::OnKeyDownTrigger()) {
        const auto parameter   = action->getTriggerParameter();
        const auto sourceEvent = *evt.sourceEvent;
        if (!parameter.empty()
            && parameter
                 != std::to_string(static_cast<char>(sourceEvent.keyCode))) {
          auto lowerCase = String::toLowerCase(parameter);
          if (lowerCase.empty()) {
            continue;
          }

          if (lowerCase
              != std::to_string(static_cast<char>(sourceEvent.keyCode))) {
            auto unicode = sourceEvent.charCode ? sourceEvent.charCode :
                                                  sourceEvent.keyCode;
            auto actualkey
              = String::toLowerCase(std::to_string(static_cast<char>(unicode)));
            if (actualkey != lowerCase) {
              continue;
            }
          }
        }
      }
      action->_executeCurrent(evt);
    }
  }
}

void ActionManager::processTrigger(unsigned int /*trigger*/) const
{
}

IAnimatablePtr
ActionManager::_getEffectiveTarget(const IAnimatablePtr& target,
                                   const std::string& propertyPath) const
{
  std::vector<std::string> properties = String::split(propertyPath, '.');

  for (unsigned int index = 0; index < properties.size() - 1; ++index) {
    // target = (*target)[properties[index]];
  }

  return target;
}

std::string ActionManager::_getProperty(const std::string& propertyPath) const
{
  std::vector<std::string> properties = String::split(propertyPath, '.');

  return properties.back();
}

Json::object ActionManager::serialize(const std::string& /*name*/) const
{
  return Json::object();
}

void ActionManager::Parse(const std::vector<Json::value>& /*parsedActions*/,
                          AbstractMesh* /*object*/, Scene* /*scene*/)
{
  // TODO FIXME
}

std::string ActionManager::GetTriggerName(unsigned int trigger)
{
  switch (trigger) {
    case ActionManager::NothingTrigger():
      return "NothingTrigger";
    case ActionManager::OnPickTrigger():
      return "OnPickTrigger";
    case ActionManager::OnLeftPickTrigger():
      return "OnLeftPickTrigger";
    case ActionManager::OnRightPickTrigger():
      return "OnRightPickTrigger";
    case ActionManager::OnCenterPickTrigger():
      return "OnCenterPickTrigger";
    case ActionManager::OnPickDownTrigger():
      return "OnPickDownTrigger";
    case ActionManager::OnDoublePickTrigger():
      return "_OnDoublePickTrigger";
    case ActionManager::OnPickUpTrigger():
      return "OnPickUpTrigger";
    case ActionManager::OnLongPressTrigger():
      return "OnLongPressTrigger";
    case ActionManager::OnPointerOverTrigger():
      return "OnPointerOverTrigger";
    case ActionManager::OnPointerOutTrigger():
      return "OnPointerOutTrigger";
    case ActionManager::OnEveryFrameTrigger():
      return "OnEveryFrameTrigger";
    case ActionManager::OnIntersectionEnterTrigger():
      return "OnIntersectionEnterTrigger";
    case ActionManager::OnIntersectionExitTrigger():
      return "OnIntersectionExitTrigger";
    case ActionManager::OnKeyDownTrigger():
      return "OnKeyDownTrigger";
    case ActionManager::OnKeyUpTrigger():
      return "OnKeyUpTrigger";
    case ActionManager::OnPickOutTrigger():
      return "OnPickOutTrigger";
    default:
      return "";
  }
}

} // end of namespace BABYLON
