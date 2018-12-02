#ifndef BABYLON_ACTIONS_ACTION_MANAGER_H
#define BABYLON_ACTIONS_ACTION_MANAGER_H

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class Action;
class ActionEvent;
class ActionManager;
class IAnimatable;
class Scene;
using ActionManagerPtr = std::shared_ptr<ActionManager>;
using IAnimatablePtr   = std::shared_ptr<IAnimatable>;

/**
 * @brief Action Manager manages all events to be triggered on a given mesh or
 * the global scene. A single scene can have many Action Managers to handle
 * predefined actions on specific meshes.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT ActionManager {

public:
  friend class Scene;

public:
  /**
   * Nothing
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int NothingTrigger = 0;

  /**
   * On pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickTrigger = 1;

  /**
   * On left pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnLeftPickTrigger = 2;

  /**
   * On right pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnRightPickTrigger = 3;

  /**
   * On center pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnCenterPickTrigger = 4;

  /**
   * On pick down
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickDownTrigger = 5;

  /**
   * On double pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnDoublePickTrigger = 6;

  /**
   * On pick up
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickUpTrigger = 7;

  /**
   * On long press
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnLongPressTrigger = 8;

  /**
   * On pointer over
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPointerOverTrigger = 9;

  /**
   * On pointer out
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPointerOutTrigger = 10;

  /**
   * On every frame
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnEveryFrameTrigger = 11;

  /**
   * On intersection enter
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnIntersectionEnterTrigger = 12;

  /**
   * On intersection exit
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnIntersectionExitTrigger = 13;

  /**
   * On key down
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnKeyDownTrigger = 14;

  /**
   * On key up
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnKeyUpTrigger = 15;

  /**
   * On pick out.
   * This trigger will only be raised if you also declared a OnPickDown
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickOutTrigger = 16;

  /** Gets the list of active triggers */
  static std::array<unsigned int, 17> Triggers;
  static size_t DragMovementThreshold; // in pixels
  static size_t LongPressDelay;        // in milliseconds

  template <typename... Ts>
  static ActionManagerPtr New(Ts&&... args)
  {
    auto actionManagerRawPtr = new ActionManager(std::forward<Ts>(args)...);
    auto actionManager
      = static_cast<std::shared_ptr<ActionManager>>(actionManagerRawPtr);
    actionManager->addToScene(actionManager);
    return actionManager;
  }
  virtual ~ActionManager();

  /** Methods **/

  void addToScene(const std::shared_ptr<ActionManager>& newActionManager);

  /**
   * @brief Releases all associated resources.
   */
  void dispose();

  /**
   * @brief Gets hosting scene.
   * @returns the hosting scene
   */
  Scene* getScene() const;

  /**
   * @brief Does this action manager handles actions of any of the given
   * triggers.
   * @param triggers defines the triggers to be tested
   * @return a boolean indicating whether one (or more) of the triggers is
   * handled
   */
  bool hasSpecificTriggers(const Uint32Array& triggers) const;

  /**
   * @brief Does this action manager handles actions of any of the given
   * triggers. This function takes two arguments for speed.
   * @param triggerA defines the trigger to be tested
   * @param triggerB defines the trigger to be tested
   * @return a boolean indicating whether one (or more) of the triggers is
   * handled
   */
  bool hasSpecificTriggers2(unsigned int triggerA, unsigned int triggerB) const;

  /**
   * @brief Does this action manager handles actions of a given trigger.
   * @param trigger defines the trigger to be tested
   * @param parameterPredicate defines an optional predicate to filter triggers
   * by parameter
   * @return whether the trigger is handled
   */
  bool hasSpecificTrigger(
    unsigned int trigger,
    const std::function<bool(const std::string& parameter)>& parameterPredicate
    = nullptr) const;

  /**
   * @brief Does this action manager has pointer triggers.
   * @return {boolean} whether or not it has pointer triggers
   */
  bool hasPointerTriggers() const;

  /**
   * @brief Does this action manager has pick triggers.
   * @return {boolean} whether or not it has pick triggers
   */
  bool hasPickTriggers() const;

  /**
   * @brief Registers an action to this action manager.
   * @param action defines the action to be registered
   * @return the action amended (prepared) after registration
   */
  Action* registerAction(Action* action);

  /**
   * @brief Unregisters an action to this action manager.
   * @param action defines the action to be unregistered
   * @return a boolean indicating whether the action has been unregistered
   */
  bool unregisterAction(Action* action);

  /**
   * @brief Process a specific trigger.
   * @param trigger defines the trigger to process
   * @param evt defines the event details to be processed
   */
  void processTrigger(unsigned int trigger, const ActionEvent& evt) const;

  /**
   * @brief Process a specific trigger.
   * @param trigger defines the trigger to process
   */
  void processTrigger(unsigned int trigger) const;

  /** hidden */
  IAnimatablePtr _getEffectiveTarget(const IAnimatablePtr& target,
                                     const std::string& propertyPath) const;

  /** hidden */
  std::string _getProperty(const std::string& propertyPath) const;

  /**
   * @brief Serialize this manager to a JSON object.
   * @param name defines the property name to store this manager
   * @returns a JSON representation of this manager
   */
  json serialize(const std::string& name) const;

  // Statics

  /**
   * @brief Does exist one action manager with at least one trigger.
   * @return {boolean} whether or not it exists one action manager with one
   * trigger
   **/
  static bool HasTriggers();

  /**
   * @brief Does exist one action manager with at least one pick trigger.
   * @return {boolean} whether or not it exists one action manager with one pick
   * trigger
   **/
  static bool HasPickTriggers();

  /**
   * @brief Does exist one action manager that handles actions of a given
   *trigger.
   * @param trigger defines the trigger to be tested
   * @return a boolean indicating whether the trigger is handeled by at least
   *one action manager
   **/
  static bool HasSpecificTrigger(unsigned int trigger);

  /**
   * @brief Creates a new ActionManager from a JSON data.
   * @param parsedActions defines the JSON data to read from
   * @param object defines the hosting mesh
   * @param scene defines the hosting scene
   */
  static void Parse(const std::vector<json>& parsedActions,
                    AbstractMesh* object, Scene* scene);

  /**
   * @brief Get a trigger name by index.
   * @param trigger defines the trigger index
   * @returns a trigger name
   */
  static std::string GetTriggerName(unsigned int trigger);

protected:
  /**
   * @brief Creates a new action manager.
   * @param scene defines the hosting scene
   */
  ActionManager(Scene* scene);

public:
  /** Gets the cursor to use when hovering items */
  std::string hoverCursor;

private:
  /** Gets the list of actions */
  std::vector<Action*> actions;

  Scene* _scene;

}; // end of class ActionManager

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTION_MANAGER_H
