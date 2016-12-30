#ifndef BABYLON_ACTIONS_CONDITION_H
#define BABYLON_ACTIONS_CONDITION_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Condition {

public:
  Condition(ActionManager* actionManager);
  Condition(const Condition& other);
  Condition(Condition&& other);
  Condition& operator=(const Condition& other);
  Condition& operator=(Condition&& other);
  virtual ~Condition();

  virtual bool isValid();
  std::string _getProperty(const std::string& propertyPath);
  IAnimatable* _getEffectiveTarget(IAnimatable* target,
                                   const std::string& propertyPath);
  virtual Json::object serialize() const;

protected:
  Json::object _serialize(const Json::object& serializedCondition) const;

public:
  ActionManager* _actionManager;
  int _evaluationId;
  bool _currentResult;

}; // end of class Condition

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_CONDITION_H
