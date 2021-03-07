#include <babylon/actions/directactions/play_animation_action.h>

#include <nlohmann/json.hpp>

#include <babylon/actions/action_manager.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

PlayAnimationAction::PlayAnimationAction(unsigned int iTtriggerOptions,
                                         const IAnimatablePtr& target, float iFrom, float iTo,
                                         bool iLoop, Condition* condition)
    : Action(iTtriggerOptions, condition), from{iFrom}, to{iTo}, loop{iLoop}, _target{target}
{
}

PlayAnimationAction::~PlayAnimationAction() = default;

void PlayAnimationAction::_prepare()
{
}

void PlayAnimationAction::execute(const IActionEventPtr& /*evt*/)
{
  auto scene = _actionManager->getScene();
  scene->beginAnimation(_target, from, to, loop);
}

json PlayAnimationAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
