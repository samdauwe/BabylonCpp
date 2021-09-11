#include <babylon/actions/directactions/combine_action.h>

#include <nlohmann/json.hpp>

namespace BABYLON {

CombineAction::CombineAction(unsigned int iTriggerOptions, const std::vector<Action*>& iChildren,
                             Condition* condition, bool iEnableChildrenConditions)
    : Action(iTriggerOptions, condition)
    , children{iChildren}
    , enableChildrenConditions{iEnableChildrenConditions}
{
}

CombineAction::~CombineAction() = default;

void CombineAction::_prepare()
{
  for (auto& child : children) {
    child->_actionManager = _actionManager;
    child->_prepare();
  }
}

void CombineAction::execute(const IActionEventPtr& evt)
{
  for (const auto& action : children) {
    if (!enableChildrenConditions || action->_evaluateConditionForCurrentFrame()) {
      action->execute(evt);
    }
  }
}

json CombineAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
