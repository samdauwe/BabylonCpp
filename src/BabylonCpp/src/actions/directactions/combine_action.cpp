#include <babylon/actions/directactions/combine_action.h>

#include <babylon/core/json.h>

namespace BABYLON {

CombineAction::CombineAction(unsigned int triggerOptions,
                             const std::vector<Action*>& iChildren,
                             Condition* condition)
    : Action(triggerOptions, condition), children{iChildren}
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

Json::object CombineAction::serialize(Json::object& parent) const
{
  Json::array combine;
  Json::object tmp;
  for (auto& child : children) {
    combine.emplace_back(child->serialize(tmp));
  }

  return Action::_serialize(
    Json::object({Json::Pair("name", "CombineAction"),
                  Json::Pair("properties", Json::array()),
                  Json::Pair("combine", combine)}),
    parent);
}

} // end of namespace BABYLON
