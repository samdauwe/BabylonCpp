#ifndef BABYLON_ACTIONS_ABSTRACT_ACTION_MANAGER_H
#define BABYLON_ACTIONS_ABSTRACT_ACTION_MANAGER_H

#include <array>
#include <functional>
#include <memory>
#include <optional>

#include <nlohmann/json_fwd.hpp>

#include <babylon/actions/iaction_event.h>
#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/interfaces/idisposable.h>

using json = nlohmann::json;

namespace BABYLON {

struct IActionEvent;
FWD_CLASS_SPTR(IAction)

/**
 * @brief Abstract class used to decouple action Manager from scene and meshes.
 * Do not instantiate.
 * @see https://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT AbstractActionManager : IDisposable {

public:
  AbstractActionManager();
  ~AbstractActionManager() override; // = default

  /**
   * @brief Releases all associated resources
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override = 0;

  /**
   * @brief Process a specific trigger.
   * @param trigger defines the trigger to process
   * @param evt defines the event details to be processed
   */
  virtual void processTrigger(unsigned int trigger,
                              const std::optional<IActionEvent>& evt = std::nullopt)
    = 0;

  /**
   * @brief Does this action manager handles actions of any of the given triggers.
   * @param triggers defines the triggers to be tested
   * @return a boolean indicating whether one (or more) of the triggers is handled
   */
  [[nodiscard]] virtual bool hasSpecificTriggers(const Uint32Array& triggers) const = 0;

  /**
   * @brief Does this action manager handles actions of any of the given triggers. This function
   * takes two arguments for speed.
   * @param triggerA defines the trigger to be tested
   * @param triggerB defines the trigger to be tested
   * @return a boolean indicating whether one (or more) of the triggers is handled
   */
  [[nodiscard]] virtual bool hasSpecificTriggers2(unsigned int triggerA,
                                                  unsigned int triggerB) const = 0;

  /**
   * @brief Does this action manager handles actions of a given trigger.
   * @param trigger defines the trigger to be tested
   * @param parameterPredicate defines an optional predicate to filter triggers by parameter
   * @return whether the trigger is handled
   */
  virtual bool
  hasSpecificTrigger(unsigned int trigger,
                     const std::function<bool(const std::string& parameter)>& parameterPredicate
                     = nullptr) const = 0;

  /**
   * @brief Serialize this manager to a JSON object.
   * @param name defines the property name to store this manager
   * @returns a JSON representation of this manager
   */
  [[nodiscard]] virtual json serialize(const std::string& name) const = 0;

  /**
   * @brief Registers an action to this action manager.
   * @param action defines the action to be registered
   * @return the action amended (prepared) after registration
   */
  virtual IActionPtr registerAction(const IActionPtr& action) = 0;

  /**
   * @brief Unregisters an action to this action manager.
   * @param action defines the action to be unregistered
   * @return a boolean indicating whether the action has been unregistered
   */
  virtual bool unregisterAction(const IActionPtr& action) = 0;

  /**
   * @brief Does exist one action manager with at least one trigger.
   **/
  static bool HasTriggers();

  /**
   * @brief Does exist one action manager with at least one pick trigger.
   **/
  static bool HasPickTriggers();

  /**
   * @brief Does exist one action manager that handles actions of a given trigger
   * @param trigger defines the trigger to be tested
   * @return a boolean indicating whether the trigger is handled by at least one action manager
   **/
  static bool HasSpecificTrigger(unsigned int trigger);

protected:
  /**
   * @brief Does this action manager has pointer triggers.
   */
  [[nodiscard]] virtual bool get_hasPointerTriggers() const = 0;

  /**
   * @brief Does this action manager has pick triggers.
   */
  [[nodiscard]] virtual bool get_hasPickTriggers() const = 0;

public:
  /** Gets the list of active triggers */
  static std::array<unsigned int, 17> Triggers;

  /** Gets the cursor to use when hovering items */
  std::string hoverCursor;

  /** Gets the list of actions */
  std::vector<IActionPtr> actions;

  /**
   * Gets or sets a boolean indicating that the manager is recursive meaning that it can trigger
   * action from children
   */
  bool isRecursive;

  /**
   * Does this action manager has pointer triggers
   */
  ReadOnlyProperty<AbstractActionManager, bool> hasPointerTriggers;

  /**
   * Does this action manager has pick triggers
   */
  ReadOnlyProperty<AbstractActionManager, bool> hasPickTriggers;

}; // end of class AbstractActionManager

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ABSTRACT_ACTION_MANAGER_H
