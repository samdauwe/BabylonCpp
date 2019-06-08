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

/**
 * @brief Interface used to define Action.
 */
struct BABYLON_SHARED_EXPORT IAction {

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
  virtual std::string getTriggerParameter() const = 0;

  /**
   * @brief Internal only - executes current action event.
   * Hidden
   */
  virtual void _executeCurrent(const ActionEvent& evt) = 0;

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

}; // end of struct IAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_IACTION_H
