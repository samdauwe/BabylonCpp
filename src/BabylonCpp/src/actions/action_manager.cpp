#include <babylon/actions/action_manager.h>

#include <babylon/actions/action.h>
#include <babylon/actions/action_event.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

size_t ActionManager::DragMovementThreshold = 10;  // in pixels
size_t ActionManager::LongPressDelay        = 500; // in milliseconds

template <typename... Ts>
ActionManager* ActionManager::New(Ts&&... args)
{
  auto actionManager = std_util::make_unique<Action>(std::forward<Ts>(args)...);
  actionManager->_scene->_actionManagers.emplace_back(actionManager);
  return actionManager.get();
}

ActionManager::ActionManager(Scene* scene) : hoverCursor{""}, _scene{scene}
{
}

ActionManager::~ActionManager()
{
}

void ActionManager::dispose(bool /*doNotRecurse*/)
{
  _scene->_actionManagers.erase(
    std::remove_if(_scene->_actionManagers.begin(),
                   _scene->_actionManagers.end(),
                   [this](const std::unique_ptr<ActionManager>& actionManager) {
                     return actionManager.get() == this;
                   }),
    _scene->_actionManagers.end());
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

bool ActionManager::hasSpecificTrigger(unsigned int trigger) const
{
  return std::find_if(
           actions.begin(), actions.end(),
           [&trigger](Action* action) { return action->trigger == trigger; })
         != actions.end();
}

bool ActionManager::hasPointerTriggers() const
{

  return std::find_if(actions.begin(), actions.end(),
                      [](Action* action) {
                        return action->trigger >= ActionManager::OnPickTrigger
                               && action->trigger
                                    <= ActionManager::OnPointerOutTrigger;
                      })
         != actions.end();
}

bool ActionManager::hasPickTriggers() const
{
  return std::find_if(actions.begin(), actions.end(),
                      [](Action* action) {
                        return action->trigger >= ActionManager::OnPickTrigger
                               && action->trigger
                                    <= ActionManager::OnPickUpTrigger;
                      })
         != actions.end();
}

Action* ActionManager::registerAction(Action* action)
{
  if (action->trigger == ActionManager::OnEveryFrameTrigger) {
    if (getScene()->actionManager != this) {
      BABYLON_LOG_WARN(
        "ActionManager",
        "OnEveryFrameTrigger can only be used with scene.actionManager");
      return nullptr;
    }
  }

  actions.emplace_back(action);

  action->_actionManager = this;
  action->_prepare();

  return action;
}

void ActionManager::processTrigger(unsigned int trigger,
                                   const ActionEvent& evt) const
{
  for (const auto& action : actions) {
    if (action->trigger == trigger) {
      if (trigger == ActionManager::OnKeyUpTrigger
          || trigger == ActionManager::OnKeyDownTrigger) {
        const std::string parameter = action->getTriggerParameter();
        if (!parameter.empty()) {
          // Actual key
          const char* unicode = evt.sourceEvent.charCode ?
                                  evt.sourceEvent.charCode :
                                  evt.sourceEvent.keyCode;
          const std::string actualkey(unicode);
          if (String::toLowerCase(actualkey)
              != String::toLowerCase(parameter)) {
            continue;
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

IAnimatable*
ActionManager::_getEffectiveTarget(IAnimatable* target,
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
    case ActionManager::NothingTrigger:
      return "NothingTrigger";
    case ActionManager::OnPickTrigger:
      return "OnPickTrigger";
    case ActionManager::OnLeftPickTrigger:
      return "OnLeftPickTrigger";
    case ActionManager::OnRightPickTrigger:
      return "OnRightPickTrigger";
    case ActionManager::OnCenterPickTrigger:
      return "OnCenterPickTrigger";
    case ActionManager::OnPickDownTrigger:
      return "OnPickDownTrigger";
    case ActionManager::OnPickUpTrigger:
      return "OnPickUpTrigger";
    case ActionManager::OnLongPressTrigger:
      return "OnLongPressTrigger";
    case ActionManager::OnPointerOverTrigger:
      return "OnPointerOverTrigger";
    case ActionManager::OnPointerOutTrigger:
      return "OnPointerOutTrigger";
    case ActionManager::OnEveryFrameTrigger:
      return "OnEveryFrameTrigger";
    case ActionManager::OnIntersectionEnterTrigger:
      return "OnIntersectionEnterTrigger";
    case ActionManager::OnIntersectionExitTrigger:
      return "OnIntersectionExitTrigger";
    case ActionManager::OnKeyDownTrigger:
      return "OnKeyDownTrigger";
    case ActionManager::OnKeyUpTrigger:
      return "OnKeyUpTrigger";
    case ActionManager::OnPickOutTrigger:
      return "OnPickOutTrigger";
    default:
      return "";
  }
}

} // end of namespace BABYLON
