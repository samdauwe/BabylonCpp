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

public:
  /** Statics **/
  static constexpr unsigned int NothingTrigger             = 0;
  static constexpr unsigned int OnPickTrigger              = 1;
  static constexpr unsigned int OnLeftPickTrigger          = 2;
  static constexpr unsigned int OnRightPickTrigger         = 3;
  static constexpr unsigned int OnCenterPickTrigger        = 4;
  static constexpr unsigned int OnPickDownTrigger          = 5;
  static constexpr unsigned int OnPickUpTrigger            = 6;
  static constexpr unsigned int OnLongPressTrigger         = 7;
  static constexpr unsigned int OnPointerOverTrigger       = 8;
  static constexpr unsigned int OnPointerOutTrigger        = 9;
  static constexpr unsigned int OnEveryFrameTrigger        = 10;
  static constexpr unsigned int OnIntersectionEnterTrigger = 11;
  static constexpr unsigned int OnIntersectionExitTrigger  = 12;
  static constexpr unsigned int OnKeyDownTrigger           = 13;
  static constexpr unsigned int OnKeyUpTrigger             = 14;
  static constexpr unsigned int OnPickOutTrigger           = 15;

  static size_t DragMovementThreshold; // in pixels
  static size_t LongPressDelay;        // in milliseconds

  template <typename... Ts>
  static ActionManager* New(Ts&&... args);
  virtual ~ActionManager();

  /** Methods **/
  void dispose(bool doNotRecurse = false) override;
  Scene* getScene() const;

  /**
   * Does this action manager handles actions of any of the given triggers
   * @param {number[]} triggers - the triggers to be tested
   * @return {boolean} whether one (or more) of the triggers is handeled
   */
  bool hasSpecificTriggers(const Uint32Array& triggers) const;

  /**
   * Does this action manager handles actions of a given trigger
   * @param {number} trigger - the trigger to be tested
   * @return {boolean} whether the trigger is handeled
   */
  bool hasSpecificTrigger(unsigned int trigger) const;

  /**
   * Does this action manager has pointer triggers
   * @return {boolean} whether or not it has pointer triggers
   */
  bool hasPointerTriggers() const;

  /**
   * Does this action manager has pick triggers
   * @return {boolean} whether or not it has pick triggers
   */
  bool hasPickTriggers() const;

  /**
   * Registers an action to this action manager
   * @param {BABYLON.Action} action - the action to be registered
   * @return {BABYLON.Action} the action amended (prepared) after registration
   */
  Action* registerAction(Action* action);

  /**
   * Process a specific trigger
   * @param {number} trigger - the trigger to process
   * @param evt {BABYLON.ActionEvent} the event details to be processed
   */
  void processTrigger(unsigned int trigger, const ActionEvent& evt) const;
  void processTrigger(unsigned int trigger) const;

  IAnimatable* _getEffectiveTarget(IAnimatable* target,
                                   const std::string& propertyPath) const;
  std::string _getProperty(const std::string& propertyPath) const;
  Json::object serialize(const std::string& name) const;

  // Statics
  static void Parse(const std::vector<Json::value>& parsedActions,
                    AbstractMesh* object, Scene* scene);
  static std::string GetTriggerName(unsigned int trigger);

protected:
  ActionManager(Scene* scene);

private:
  std::vector<Action*> actions;
  std::string hoverCursor;
  Scene* _scene;

}; // end of class ActionManager

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTION_MANAGER_H
