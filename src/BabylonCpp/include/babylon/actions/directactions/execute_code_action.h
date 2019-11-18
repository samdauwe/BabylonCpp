#ifndef BABYLON_ACTIONS_DIRECTACTIONS_EXECUTE_CODE_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_EXECUTE_CODE_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to run code (external event) once
 * triggered.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT ExecuteCodeAction : public Action {

public:
  /**
   * @brief Instantiate the action.
   * @param triggerOptions defines the trigger options
   * @param func defines the callback function to run
   * @param condition defines the trigger related conditions
   */
  ExecuteCodeAction(unsigned int triggerOptions,
                    const std::function<void(const std::optional<IActionEvent>& evt)>& func,
                    Condition* condition = nullptr);
  ~ExecuteCodeAction() override; // = default

  /**
   * @brief Execute the action and run the attached code.
   */
  void execute(const std::optional<IActionEvent>& evt) override;

public:
  /**
   * The callback function to run.
   */
  std::function<void(const std::optional<IActionEvent>& evt)> func;

}; // end of class ExecuteCodeAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_EXECUTE_CODE_ACTION_H
