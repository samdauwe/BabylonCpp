#ifndef BABYLON_ACTIONS_CONDITION_H
#define BABYLON_ACTIONS_CONDITION_H

#include <map>
#include <memory>
#include <string>

#include <babylon/babylon_api.h>

namespace picojson {
class value;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class ActionManager;
class IAnimatable;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

namespace Json {
typedef picojson::value value;
typedef picojson::object object;
} // namespace Json

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
  std::string _getProperty(const std::string& propertyPath);

  /**
   * @brief Internal only.
   */
  IAnimatablePtr _getEffectiveTarget(const IAnimatablePtr& target,
                                     const std::string& propertyPath);

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
