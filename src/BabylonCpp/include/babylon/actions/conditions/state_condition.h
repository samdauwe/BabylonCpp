#ifndef BABYLON_ACTIONS_CONDITIONS_STATE_CONDITION_H
#define BABYLON_ACTIONS_CONDITIONS_STATE_CONDITION_H

#include <babylon/actions/condition.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT StateCondition : public Condition {

public:
  StateCondition(ActionManager* actionManager, IAnimatable* target,
                 AnimationValue* value);
  virtual ~StateCondition();

  /** Methods **/
  bool isValid() override;
  Json::object serialize() const override;

public:
  AnimationValue* value;

private:
  /** Members **/
  IAnimatable* _target;

}; // end of class StateCondition

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_CONDITIONS_STATE_CONDITION_H
