#ifndef BABYLON_ACTIONS_ACTION_MANAGER_H
#define BABYLON_ACTIONS_ACTION_MANAGER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Action Manager manages all events to be triggered on a given mesh or
 * the global scene. A single scene can have many Action Managers to handle
 * predefined actions on specific meshes.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT ActionManager {

public:
  friend class Scene;

private:
  /** Statics **/
  static constexpr unsigned int _NothingTrigger             = 0;
  static constexpr unsigned int _OnPickTrigger              = 1;
  static constexpr unsigned int _OnLeftPickTrigger          = 2;
  static constexpr unsigned int _OnRightPickTrigger         = 3;
  static constexpr unsigned int _OnCenterPickTrigger        = 4;
  static constexpr unsigned int _OnPickDownTrigger          = 5;
  static constexpr unsigned int _OnDoublePickTrigger        = 6;
  static constexpr unsigned int _OnPickUpTrigger            = 7;
  static constexpr unsigned int _OnLongPressTrigger         = 8;
  static constexpr unsigned int _OnPointerOverTrigger       = 9;
  static constexpr unsigned int _OnPointerOutTrigger        = 10;
  static constexpr unsigned int _OnEveryFrameTrigger        = 11;
  static constexpr unsigned int _OnIntersectionEnterTrigger = 12;
  static constexpr unsigned int _OnIntersectionExitTrigger  = 13;
  static constexpr unsigned int _OnKeyDownTrigger           = 14;
  static constexpr unsigned int _OnKeyUpTrigger             = 15;
  static constexpr unsigned int _OnPickOutTrigger           = 16;

public:
  /**
   * @brief Nothing
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int NothingTrigger()
  {
    return ActionManager::_NothingTrigger;
  }

  /**
   * @brief On pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickTrigger()
  {
    return ActionManager::_OnPickTrigger;
  }

  /**
   * @brief On left pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnLeftPickTrigger()
  {
    return ActionManager::_OnLeftPickTrigger;
  }

  /**
   * @brief On right pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnRightPickTrigger()
  {
    return ActionManager::_OnRightPickTrigger;
  }

  /**
   * @brief On center pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnCenterPickTrigger()
  {
    return ActionManager::_OnCenterPickTrigger;
  }

  /**
   * @brief On pick down
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickDownTrigger()
  {
    return ActionManager::_OnPickDownTrigger;
  }

  /**
   * @brief On double pick
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnDoublePickTrigger()
  {
    return ActionManager::_OnDoublePickTrigger;
  }

  /**
   * @brief On pick up
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickUpTrigger()
  {
    return ActionManager::_OnPickUpTrigger;
  }

  /**
   * @brief On long press
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnLongPressTrigger()
  {
    return ActionManager::_OnLongPressTrigger;
  }

  /**
   * @brief On pointer over
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPointerOverTrigger()
  {
    return ActionManager::_OnPointerOverTrigger;
  }

  /**
   * @brief On pointer out
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPointerOutTrigger()
  {
    return ActionManager::_OnPointerOutTrigger;
  }

  /**
   * @brief On every frame
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnEveryFrameTrigger()
  {
    return ActionManager::_OnEveryFrameTrigger;
  }

  /**
   * @brief On intersection enter
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnIntersectionEnterTrigger()
  {
    return ActionManager::_OnIntersectionEnterTrigger;
  }

  /**
   * @brief On intersection exit
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnIntersectionExitTrigger()
  {
    return ActionManager::_OnIntersectionExitTrigger;
  }

  /**
   * @brief On key down
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnKeyDownTrigger()
  {
    return ActionManager::_OnKeyDownTrigger;
  }

  /**
   * @brief On key up
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnKeyUpTrigger()
  {
    return ActionManager::_OnKeyUpTrigger;
  }

  /**
   * @brief On pick out.
   * This trigger will only be raised if you also declared a OnPickDown
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickOutTrigger()
  {
    return ActionManager::_OnPickOutTrigger;
  }

  static array_t<unsigned int, 17> Triggers;
  static size_t DragMovementThreshold; // in pixels
  static size_t LongPressDelay;        // in milliseconds

  template <typename... Ts>
  static shared_ptr_t<ActionManager> New(Ts&&... args)
  {
    auto actionManagerRawPtr = new ActionManager(std::forward<Ts>(args)...);
    auto actionManager
      = static_cast<shared_ptr_t<ActionManager>>(actionManagerRawPtr);
    actionManager->addToScene(actionManager);
    return actionManager;
  }
  virtual ~ActionManager();

  /** Methods **/

  void addToScene(const shared_ptr_t<ActionManager>& newActionManager);

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
   * @brief Does this action manager handles actions of a given trigger.
   * @param trigger defines the trigger to be tested
   * @param parameterPredicate defines an optional predicate to filter triggers
   * by parameter
   * @return whether the trigger is handled
   */
  bool hasSpecificTrigger(
    unsigned int trigger,
    const ::std::function<bool(const string_t& parameter)>& parameterPredicate
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
                                     const string_t& propertyPath) const;

  /** hidden */
  string_t _getProperty(const string_t& propertyPath) const;

  /**
   * @brief Serialize this manager to a JSON object.
   * @param name defines the property name to store this manager
   * @returns a JSON representation of this manager
   */
  Json::object serialize(const string_t& name) const;

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
  static void Parse(const vector_t<Json::value>& parsedActions,
                    AbstractMesh* object, Scene* scene);

  /**
   * @brief Get a trigger name by index.
   * @param trigger defines the trigger index
   * @returns a trigger name
   */
  static string_t GetTriggerName(unsigned int trigger);

protected:
  /**
   * @brief Creates a new action manager.
   * @param scene defines the hosting scene
   */
  ActionManager(Scene* scene);

public:
  /** Gets the cursor to use when hovering items */
  string_t hoverCursor;

private:
  /** Gets the list of actions */
  vector_t<Action*> actions;

  Scene* _scene;

}; // end of class ActionManager

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTION_MANAGER_H
