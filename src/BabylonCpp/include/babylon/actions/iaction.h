#ifndef BABYLON_ACTIONS_IACTION_H
#define BABYLON_ACTIONS_IACTION_H

#include <map>
#include <string>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>

using json = nlohmann::json;

namespace BABYLON {

class ActionEvent;
class ActionManager;
struct IActionEvent;

/**
 * @brief Interface used to define Action.
 */
struct BABYLON_SHARED_EXPORT IAction {
  virtual ~IAction() = default;
  /**
   * Trigger for the action
   */
  unsigned int trigger;

  /**
   * Options of the trigger
   */
  TriggerOptions triggerOptions;

  /**
   * @brief Gets the trigger parameters
   * @returns the trigger parameters
   */
  [[nodiscard]] virtual std::string getTriggerParameter() const = 0;

  /**
   * @brief Internal only - executes current action event.
   * Hidden
   */
  virtual void _executeCurrent(const std::optional<IActionEvent>& evt) = 0;

  /**
   * @brief Serialize placeholder for child classes.
   * @param parent of child
   * @returns the serialized object
   */
  virtual json serialize(json& parent) const = 0;

  /**
   * Internal only
   * @hidden
   */
  virtual void _prepare() = 0;

  /**
   * Internal only - manager for action
   * @hidden
   */
  ActionManager* _actionManager;

  /**
   * @brief Adds action to chain of actions, may be a DoNothingAction
   * @param action defines the next action to execute
   * @returns The action passed in
   * @see https://www.babylonjs-playground.com/#1T30HR#0
   */
  virtual IAction* then(IAction* action) = 0;

}; // end of struct IAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_IACTION_H
