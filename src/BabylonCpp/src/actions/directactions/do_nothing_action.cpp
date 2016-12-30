#include <babylon/actions/directactions/do_nothing_action.h>

#include <babylon/core/json.h>

namespace BABYLON {

DoNothingAction::DoNothingAction(unsigned int triggerOptions,
                                 Condition* condition)
    : Action(triggerOptions, condition)
{
}

DoNothingAction::~DoNothingAction()
{
}

void DoNothingAction::execute(const ActionEvent& /*evt*/)
{
}

Json::object DoNothingAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object({Json::Pair("name", "DoNothingAction"),
                  Json::Pair("properties", Json::array())}),
    parent);
}

} // end of namespace BABYLON
