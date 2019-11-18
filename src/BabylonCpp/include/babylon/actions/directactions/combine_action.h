#ifndef BABYLON_ACTIONS_DIRECTACTIONS_COMBINE_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_COMBINE_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to trigger several actions once
 * triggered.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT CombineAction : public Action {

public:
  /**
   * @brief Instantiate the action.
   * @param triggerOptions defines the trigger options
   * @param children defines the list of aggregated animations to run
   * @param condition defines the trigger related conditions
   */
  CombineAction(unsigned int triggerOptions, const std::vector<Action*>& children,
                Condition* condition = nullptr);
  ~CombineAction() override; // = default

  /**
   * @brief Hidden
   */
  void _prepare() override;

  /**
   * @brief Execute the action and executes all the aggregated actions.
   */
  void execute(const std::optional<IActionEvent>& evt) override;

  /**
   * @brief Serializes the actions and its related information.
   * @param parent defines the object to serialize in
   * @returns the serialized object
   */
  json serialize(json& parent) const override;

public:
  /**
   * The list of aggregated animations to run.
   */
  std::vector<Action*> children;

}; // end of class CombineAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_COMBINE_ACTION_H
