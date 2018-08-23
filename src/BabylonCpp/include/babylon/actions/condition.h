#ifndef BABYLON_ACTIONS_CONDITION_H
#define BABYLON_ACTIONS_CONDITION_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief A Condition applied to an Action.
 */
class BABYLON_SHARED_EXPORT Condition {

public:
  /**
   * @brief Creates a new Condition.
   * @param actionManager the manager of the action the condition is applied to
   */
  Condition(ActionManager* actionManager);
  Condition(const Condition& other);
  Condition(Condition&& other);
  Condition& operator=(const Condition& other);
  Condition& operator=(Condition&& other);
  virtual ~Condition();

  /**
   * @brief Check if the current condition is valid.
   * @returns a boolean
   */
  virtual bool isValid();

  /**
   * @brief Internal only.
   */
  string_t _getProperty(const string_t& propertyPath);

  /**
   * @brief Internal only.
   */
  IAnimatablePtr _getEffectiveTarget(const IAnimatablePtr& target,
                                     const string_t& propertyPath);

  /**
   * @brief Serialize placeholder for child classes.
   * @returns the serialized object
   */
  virtual Json::object serialize() const;

protected:
  /**
   * @brief Internal only.
   */
  Json::object _serialize(const Json::object& serializedCondition) const;

public:
  /**
   * Internal only - manager for action
   */
  ActionManager* _actionManager;

  /**
   * Internal only
   */
  int _evaluationId;

  /**
   * Internal only
   */
  bool _currentResult;

}; // end of class Condition

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_CONDITION_H
