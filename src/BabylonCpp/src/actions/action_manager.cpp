#include <babylon/actions/action_manager.h>

#include <babylon/actions/action.h>
#include <babylon/actions/action_event.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

array_t<unsigned int, 17> ActionManager::Triggers{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
size_t ActionManager::DragMovementThreshold = 10;  // in pixels
size_t ActionManager::LongPressDelay        = 500; // in milliseconds

template <typename... Ts>
ActionManager* ActionManager::New(Ts&&... args)
{
  auto actionManager = ::std::make_unique<Action>(std::forward<Ts>(args)...);
  actionManager->_scene->_actionManagers.emplace_back(actionManager);
  return actionManager.get();
}

ActionManager::ActionManager(Scene* scene)
    : hoverCursor{""}, _scene{scene ? scene : Engine::LastCreatedScene()}
{
}

ActionManager::~ActionManager()
{
}

void ActionManager::dispose()
{
  for (auto& action : actions) {
    if ((action->trigger < ActionManager::Triggers.size())
        && (ActionManager::Triggers[action->trigger] > 0)) {
      --ActionManager::Triggers[action->trigger];
    }
  }

  _scene->_actionManagers.erase(
    ::std::remove_if(_scene->_actionManagers.begin(),
                     _scene->_actionManagers.end(),
                     [this](const unique_ptr_t<ActionManager>& actionManager) {
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
  return ::std::find_if(actions.begin(), actions.end(),
                        [&triggers](Action* action) {
                          return ::std::find(triggers.begin(), triggers.end(),
                                             action->trigger)
                                 != triggers.end();
                        })
         != actions.end();
}

bool ActionManager::hasSpecificTrigger(
  unsigned int trigger,
  const ::std::function<bool(const string_t& parameter)>& parameterPredicate)
  const
{
  return ::std::find_if(
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

  return ::std::find_if(
           actions.begin(), actions.end(),
           [](Action* action) {
             return action->trigger >= ActionManager::OnPickTrigger()
                    && action->trigger <= ActionManager::OnPointerOutTrigger();
           })
         != actions.end();
}

bool ActionManager::hasPickTriggers() const
{
  return ::std::find_if(
           actions.begin(), actions.end(),
           [](Action* action) {
             return action->trigger >= ActionManager::OnPickTrigger()
                    && action->trigger <= ActionManager::OnPickUpTrigger();
           })
         != actions.end();
}

bool ActionManager::HasTriggers()
{
  return ::std::accumulate(ActionManager::Triggers.begin(),
                           ActionManager::Triggers.end(), 0)
         != 0;
}

bool ActionManager::HasPickTriggers()
{
  const auto start = ActionManager::OnPickTrigger();
  const auto end   = ActionManager::OnPickUpTrigger() + 1;

  return ::std::accumulate(ActionManager::Triggers.begin() + start,
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
                 != ::std::to_string(static_cast<char>(sourceEvent.keyCode))) {
          auto lowerCase = String::toLowerCase(parameter);
          if (lowerCase.empty()) {
            continue;
          }

          if (lowerCase
              != ::std::to_string(static_cast<char>(sourceEvent.keyCode))) {
            auto unicode = sourceEvent.charCode ? sourceEvent.charCode :
                                                  sourceEvent.keyCode;
            auto actualkey = String::toLowerCase(
              ::std::to_string(static_cast<char>(unicode)));
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

IAnimatable*
ActionManager::_getEffectiveTarget(IAnimatable* target,
                                   const string_t& propertyPath) const
{
  vector_t<string_t> properties = String::split(propertyPath, '.');

  for (unsigned int index = 0; index < properties.size() - 1; ++index) {
    // target = (*target)[properties[index]];
  }

  return target;
}

string_t ActionManager::_getProperty(const string_t& propertyPath) const
{
  vector_t<string_t> properties = String::split(propertyPath, '.');

  return properties.back();
}

Json::object ActionManager::serialize(const string_t& /*name*/) const
{
  return Json::object();
}

void ActionManager::Parse(const vector_t<Json::value>& /*parsedActions*/,
                          AbstractMesh* /*object*/, Scene* /*scene*/)
{
  // TODO FIXME
}

string_t ActionManager::GetTriggerName(unsigned int trigger)
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
