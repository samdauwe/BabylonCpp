#include <babylon/actions/directactions/stop_animation_action.h>

#include <nlohmann/json.hpp>

#include <babylon/actions/action_manager.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/engines/scene.h>

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

json StopAnimationAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
