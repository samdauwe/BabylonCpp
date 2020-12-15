#ifndef BABYLON_ACTIONS_CONDITIONS_PREDICATE_CONDITION_H
#define BABYLON_ACTIONS_CONDITIONS_PREDICATE_CONDITION_H

#include <functional>

#include <babylon/actions/condition.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Defines a predicate condition as an extension of Condition.
 */
class BABYLON_SHARED_EXPORT PredicateCondition : public Condition {

public:
  /**
   * @brief Creates a new PredicateCondition.
   * @param actionManager manager for the action the condition applies to
   * @param predicate defines the predicate function used to validate the condition
   */
  PredicateCondition(ActionManager* actionManager, const std::function<bool()>& predicate);
  ~PredicateCondition() override; // = default

  /** Methods **/

  /**
   * @returns the validity of the predicate condition.
   */
  bool isValid() override;

private:
  /** Members **/
  std::function<bool()> _predicate;

}; // end of class PredicateCondition

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_CONDITIONS_PREDICATE_CONDITION_H
