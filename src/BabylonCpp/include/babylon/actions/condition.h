#ifndef BABYLON_ACTIONS_CONDITION_H
#define BABYLON_ACTIONS_CONDITION_H

#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>

#include <babylon/babylon_api.h>

using json = nlohmann::json;

namespace BABYLON {

class ActionManager;
class IAnimatable;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

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
  virtual ~Condition(); // = default

  /**
   * @brief Check if the current condition is valid.
   * @returns a boolean
   */
  virtual bool isValid();

  /**
   * @brief Internal only.
   */
  std::string _getProperty(const std::string& propertyPath);

  /**
   * @brief Internal only.
   */
  IAnimatablePtr _getEffectiveTarget(const IAnimatablePtr& target, const std::string& propertyPath);

  /**
   * @brief Serialize placeholder for child classes.
   * @returns the serialized object
   */
  [[nodiscard]] virtual json serialize() const;

protected:
  /**
   * @brief Internal only.
   */
  [[nodiscard]] json _serialize(const json& serializedCondition) const;

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
