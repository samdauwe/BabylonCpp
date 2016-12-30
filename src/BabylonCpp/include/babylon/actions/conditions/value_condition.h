#ifndef BABYLON_ACTIONS_CONDITIONS_VALUE_CONDITION_H
#define BABYLON_ACTIONS_CONDITIONS_VALUE_CONDITION_H

#include <babylon/actions/condition.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ValueCondition : public Condition {

public:
  /** Statics **/
  static constexpr unsigned int IsEqual     = 0;
  static constexpr unsigned int IsDifferent = 1;
  static constexpr unsigned int IsGreater   = 2;
  static constexpr unsigned int IsLesser    = 3;

  /** Members **/
  ValueCondition(ActionManager* actionManager, IAnimatable* target,
                 const std::string& propertyPath, AnimationValue* value,
                 unsigned int operatorType = ValueCondition::IsEqual);
  virtual ~ValueCondition();

  /** Methods **/
  bool isValid() override;
  Json::object serialize() const override;

  /** Statics **/
  static std::string GetOperatorName(unsigned int operatorType);

private:
  std::string _propertyPath;
  unsigned int _operatorType;
  IAnimatable* _target;
  IAnimatable* _effectiveTarget;
  std::string _property;
  AnimationValue* _value;

}; // end of class ValueCondition

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_CONDITIONS_VALUE_CONDITION_H
