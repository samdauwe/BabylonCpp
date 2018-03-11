#ifndef BABYLON_ACTIONS_ACTION_MANAGER_H
#define BABYLON_ACTIONS_ACTION_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

/**
 * @brief Action Manager manages all events to be triggered on a given mesh or
 * the global scene.
 * A single scene can have many Action Managers to handle predefined actions on
 * specific meshes.
 */
class BABYLON_SHARED_EXPORT ActionManager : public IDisposable {

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
  static constexpr unsigned int NothingTrigger()
  {
    return ActionManager::_NothingTrigger;
  }

  static constexpr unsigned int OnPickTrigger()
  {
    return ActionManager::_OnPickTrigger;
  }

  static constexpr unsigned int OnLeftPickTrigger()
  {
    return ActionManager::_OnLeftPickTrigger;
  }

  static constexpr unsigned int OnRightPickTrigger()
  {
    return ActionManager::_OnRightPickTrigger;
  }

  static constexpr unsigned int OnCenterPickTrigger()
  {
    return ActionManager::_OnCenterPickTrigger;
  }

  static constexpr unsigned int OnPickDownTrigger()
  {
    return ActionManager::_OnPickDownTrigger;
  }

  static constexpr unsigned int OnDoublePickTrigger()
  {
    return ActionManager::_OnDoublePickTrigger;
  }

  static constexpr unsigned int OnPickUpTrigger()
  {
    return ActionManager::_OnPickUpTrigger;
  }

  static constexpr unsigned int OnLongPressTrigger()
  {
    return ActionManager::_OnLongPressTrigger;
  }

  static constexpr unsigned int OnPointerOverTrigger()
  {
    return ActionManager::_OnPointerOverTrigger;
  }

  static constexpr unsigned int OnPointerOutTrigger()
  {
    return ActionManager::_OnPointerOutTrigger;
  }

  static constexpr unsigned int OnEveryFrameTrigger()
  {
    return ActionManager::_OnEveryFrameTrigger;
  }

  static constexpr unsigned int OnIntersectionEnterTrigger()
  {
    return ActionManager::_OnIntersectionEnterTrigger;
  }

  static constexpr unsigned int OnIntersectionExitTrigger()
  {
    return ActionManager::_OnIntersectionExitTrigger;
  }

  static constexpr unsigned int OnKeyDownTrigger()
  {
    return ActionManager::_OnKeyDownTrigger;
  }

  static constexpr unsigned int OnKeyUpTrigger()
  {
    return ActionManager::_OnKeyUpTrigger;
  }

  // This trigger will only be raised if you also declared a OnPickDown
  static constexpr unsigned int OnPickOutTrigger()
  {
    return ActionManager::_OnPickOutTrigger;
  }

  static array_t<unsigned int, 17> Triggers;
  static size_t DragMovementThreshold; // in pixels
  static size_t LongPressDelay;        // in milliseconds

  template <typename... Ts>
  static ActionManager* New(Ts&&... args);
  virtual ~ActionManager();

  /** Methods **/
  void dispose(bool doNotRecurse = false) override;
  Scene* getScene() const;

  /**
   * @brief Does this action manager handles actions of any of the given
   * triggers
   * @param {number[]} triggers - the triggers to be tested
   * @return {boolean} whether one (or more) of the triggers is handeled
   */
  bool hasSpecificTriggers(const Uint32Array& triggers) const;

  /**
   * @brief Does this action manager handles actions of a given trigger
   * @param {number} trigger - the trigger to be tested
   * @return {boolean} whether the trigger is handeled
   */
  bool hasSpecificTrigger(unsigned int trigger) const;

  /**
   * @brief Does this action manager has pointer triggers
   * @return {boolean} whether or not it has pointer triggers
   */
  bool hasPointerTriggers() const;

  /**
   * @brief Does this action manager has pick triggers
   * @return {boolean} whether or not it has pick triggers
   */
  bool hasPickTriggers() const;

  /**
   * @brief Registers an action to this action manager
   * @param {BABYLON.Action} action - the action to be registered
   * @return {BABYLON.Action} the action amended (prepared) after registration
   */
  Action* registerAction(Action* action);

  /**
   * @brief Unregisters an action to this action manager
   * @param action The action to be unregistered
   * @return whether the action has been unregistered
   */
  bool unregisterAction(Action* action);

  /**
   * @brief Process a specific trigger
   * @param {number} trigger - the trigger to process
   * @param evt {BABYLON.ActionEvent} the event details to be processed
   */
  void processTrigger(unsigned int trigger, const ActionEvent& evt) const;
  void processTrigger(unsigned int trigger) const;

  IAnimatable* _getEffectiveTarget(IAnimatable* target,
                                   const string_t& propertyPath) const;
  string_t _getProperty(const string_t& propertyPath) const;
  Json::object serialize(const string_t& name) const;

  // Statics

  /**
   * @brief Does exist one action manager with at least one trigger
   * @return {boolean} whether or not it exists one action manager with one
   * trigger
   **/
  static bool HasTriggers();

  /**
   * @brief Does exist one action manager with at least one pick trigger
   * @return {boolean} whether or not it exists one action manager with one pick
   * trigger
   **/
  static bool HasPickTriggers();

  /**
   * @brief Does exist one action manager that handles actions of a given
   * trigger
   * @param {number} trigger - the trigger to be tested
   * @return {boolean} whether the trigger is handeled by at least one action
   * manager
   **/
  static bool HasSpecificTrigger(unsigned int trigger);

  static void Parse(const vector_t<Json::value>& parsedActions,
                    AbstractMesh* object, Scene* scene);
  static string_t GetTriggerName(unsigned int trigger);

protected:
  ActionManager(Scene* scene);

private:
  vector_t<Action*> actions;
  string_t hoverCursor;
  Scene* _scene;

}; // end of class ActionManager

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTION_MANAGER_H
