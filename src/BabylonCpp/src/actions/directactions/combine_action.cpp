#include <babylon/actions/directactions/combine_action.h>

#include <nlohmann/json.hpp>

namespace BABYLON {

CombineAction::CombineAction(unsigned int iTriggerOptions,
                             const std::vector<Action*>& iChildren,
                             Condition* condition)
    : Action(iTriggerOptions, condition), children{iChildren}
{
}

CombineAction::~CombineAction()
{
}

void CombineAction::_prepare()
{
  for (auto& child : children) {
    child->_actionManager = _actionManager;
    child->_prepare();
  }
}

void CombineAction::execute(const ActionEvent& evt)
{
  for (auto& child : children) {
    child->execute(evt);
  }
}

json CombineAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
