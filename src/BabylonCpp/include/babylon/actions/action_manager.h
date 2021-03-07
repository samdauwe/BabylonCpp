#ifndef BABYLON_ACTIONS_ACTION_MANAGER_H
#define BABYLON_ACTIONS_ACTION_MANAGER_H

#include <babylon/actions/abstract_action_manager.h>
#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>

namespace BABYLON {

class Action;
class ActionEvent;
class Scene;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(ActionManager)
FWD_CLASS_SPTR(IAnimatable)

/**
 * @brief Action Manager manages all events to be triggered on a given mesh or the global scene.
 * A single scene can have many Action Managers to handle predefined actions on specific meshes.
 * @see https://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT ActionManager : public AbstractActionManager {

public:
  friend class Scene;

public:
  /**
   * Nothing
   * @see @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int NothingTrigger = Constants::ACTION_NothingTrigger;

  /**
   * On pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickTrigger = Constants::ACTION_OnPickTrigger;

  /**
   * On left pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnLeftPickTrigger = Constants::ACTION_OnLeftPickTrigger;

  /**
   * On right pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnRightPickTrigger = Constants::ACTION_OnRightPickTrigger;

  /**
   * On center pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnCenterPickTrigger = Constants::ACTION_OnCenterPickTrigger;

  /**
   * On pick down
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickDownTrigger = Constants::ACTION_OnPickDownTrigger;

  /**
   * On double pick
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnDoublePickTrigger = Constants::ACTION_OnDoublePickTrigger;

  /**
   * On pick up
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickUpTrigger = Constants::ACTION_OnPickUpTrigger;
  /**
   * On pick out.
   * This trigger will only be raised if you also declared a OnPickDown
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPickOutTrigger = Constants::ACTION_OnPickOutTrigger;

  /**
   * On long press
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnLongPressTrigger = Constants::ACTION_OnLongPressTrigger;

  /**
   * On pointer over
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPointerOverTrigger = Constants::ACTION_OnPointerOverTrigger;

  /**
   * On pointer out
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnPointerOutTrigger = Constants::ACTION_OnPointerOutTrigger;

  /**
   * On every frame
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnEveryFrameTrigger = Constants::ACTION_OnEveryFrameTrigger;
  /**
   * On intersection enter
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnIntersectionEnterTrigger
    = Constants::ACTION_OnIntersectionEnterTrigger;

  /**
   * On intersection exit
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnIntersectionExitTrigger
    = Constants::ACTION_OnIntersectionExitTrigger;

  /**
   * On key down
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnKeyDownTrigger = Constants::ACTION_OnKeyDownTrigger;

  /**
   * On key up
   * @see https://doc.babylonjs.com/how_to/how_to_use_actions#triggers
   */
  static constexpr unsigned int OnKeyUpTrigger = 15;

  template <typename... Ts>
  static ActionManagerPtr New(Ts&&... args)
  {
    auto actionManagerRawPtr = new ActionManager(std::forward<Ts>(args)...);
    auto actionManager       = static_cast<std::shared_ptr<ActionManager>>(actionManagerRawPtr);
    actionManager->addToScene(actionManager);
    return actionManager;
  }
  ~ActionManager() override; // = default

  /** Methods **/

  void addToScene(const ActionManagerPtr& newActionManager);

  /**
   * @brief Releases all associated resources.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Gets hosting scene.
   * @returns the hosting scene
   */
  [[nodiscard]] Scene* getScene() const;

  /**
   * @brief Does this action manager handles actions of any of the given
   * triggers.
   * @param triggers defines the triggers to be tested
   * @return a boolean indicating whether one (or more) of the triggers is
   * handled
   */
  [[nodiscard]] bool hasSpecificTriggers(const Uint32Array& triggers) const override;

  /**
   * @brief Does this action manager handles actions of any of the given
   * triggers. This function takes two arguments for speed.
   * @param triggerA defines the trigger to be tested
   * @param triggerB defines the trigger to be tested
   * @return a boolean indicating whether one (or more) of the triggers is
   * handled
   */
  [[nodiscard]] bool hasSpecificTriggers2(unsigned int triggerA,
                                          unsigned int triggerB) const override;

  /**
   * @brief Does this action manager handles actions of a given trigger.
   * @param trigger defines the trigger to be tested
   * @param parameterPredicate defines an optional predicate to filter triggers
   * by parameter
   * @return whether the trigger is handled
   */
  bool
  hasSpecificTrigger(unsigned int trigger,
                     const std::function<bool(const std::string& parameter)>& parameterPredicate
                     = nullptr) const override;

  /**
   * @brief Registers an action to this action manager.
   * @param action defines the action to be registered
   * @return the action amended (prepared) after registration
   */
  IActionPtr registerAction(const IActionPtr& action) override;

  /**
   * @brief Unregisters an action to this action manager.
   * @param action defines the action to be unregistered
   * @return a boolean indicating whether the action has been unregistered
   */
  bool unregisterAction(const IActionPtr& action) override;

  /**
   * @brief Process a specific trigger.
   * @param trigger defines the trigger to process
   * @param evt defines the event details to be processed
   */
  void processTrigger(unsigned int trigger, const IActionEventPtr& evt = nullptr) override;

  /** hidden */
  [[nodiscard]] IAnimatablePtr _getEffectiveTarget(const IAnimatablePtr& target,
                                                   const std::string& propertyPath) const;

  /** hidden */
  [[nodiscard]] std::string _getProperty(const std::string& propertyPath) const;

  /**
   * @brief Serialize this manager to a JSON object.
   * @param name defines the property name to store this manager
   * @returns a JSON representation of this manager
   */
  [[nodiscard]] json serialize(const std::string& name) const override;

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
  static void Parse(const std::vector<json>& parsedActions, const AbstractMeshPtr& object,
                    Scene* scene);

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

  /**
   * @brief Does this action manager has pointer triggers.
   * @return {boolean} whether or not it has pointer triggers
   */
  [[nodiscard]] bool get_hasPointerTriggers() const override;

  /**
   * @brief Does this action manager has pick triggers.
   * @return {boolean} whether or not it has pick triggers
   */
  [[nodiscard]] bool get_hasPickTriggers() const override;

private:
  Scene* _scene;

}; // end of class ActionManager

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_ACTION_MANAGER_H
