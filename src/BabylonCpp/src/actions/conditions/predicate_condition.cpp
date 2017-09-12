#include <babylon/actions/conditions/predicate_condition.h>

namespace BABYLON {

PredicateCondition::PredicateCondition(ActionManager* actionManager,
                                       const ::std::function<bool()>& predicate)
    : Condition{actionManager}, _predicate{predicate}
{
}

PredicateCondition::~PredicateCondition()
{
}

bool PredicateCondition::isValid()
{
  return _predicate();
}

} // end of namespace BABYLON
