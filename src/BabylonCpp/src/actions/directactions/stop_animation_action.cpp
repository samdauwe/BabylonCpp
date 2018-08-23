#include <babylon/actions/directactions/stop_animation_action.h>

#include <babylon/actions/action_manager.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/core/json.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

StopAnimationAction::StopAnimationAction(unsigned int triggerOptions,
                                         const IAnimatablePtr& target,
                                         Condition* condition)
    : Action(triggerOptions, condition), _target{target}
{
}

StopAnimationAction::~StopAnimationAction()
{
}

void StopAnimationAction::_prepare()
{
}

void StopAnimationAction::execute(const ActionEvent& /*evt*/)
{
  // auto scene = _actionManager->getScene();
  // scene->stopAnimation(_target);
}

Json::object StopAnimationAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object({Json::Pair("name", "StopAnimationAction"),
                  Json::Pair("properties",
                             Json::array({{
                               Json::value(Action::_GetTargetProperty(_target)),
                             }}))}),
    parent);
}

} // end of namespace BABYLON
