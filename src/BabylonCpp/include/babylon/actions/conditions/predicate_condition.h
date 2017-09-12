#ifndef BABYLON_ACTIONS_CONDITIONS_PREDICATE_CONDITION_H
#define BABYLON_ACTIONS_CONDITIONS_PREDICATE_CONDITION_H

#include <babylon/actions/condition.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PredicateCondition : public Condition {

public:
  PredicateCondition(ActionManager* actionManager,
                     const ::std::function<bool()>& predicate);
  ~PredicateCondition();

  /** Methods **/
  bool isValid() override;

private:
  /** Members **/
  ::std::function<bool()> _predicate;

}; // end of class PredicateCondition

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_CONDITIONS_PREDICATE_CONDITION_H
