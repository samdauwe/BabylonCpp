#include <babylon/actions/directactions/play_animation_action.h>

#include <babylon/actions/action_manager.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/core/json.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

PlayAnimationAction::PlayAnimationAction(unsigned int triggerOptions,
                                         IAnimatable* target, float iFrom,
                                         float iTo, bool iLoop,
                                         Condition* condition)
    : Action(triggerOptions, condition)
    , from{iFrom}
    , to{iTo}
    , loop{iLoop}
    , _target{target}
{
}

PlayAnimationAction::~PlayAnimationAction()
{
}

void PlayAnimationAction::_prepare()
{
}

void PlayAnimationAction::execute(const ActionEvent& /*evt*/)
{
  auto scene = _actionManager->getScene();
  scene->beginAnimation(_target, from, to, loop);
}

Json::object PlayAnimationAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object(
      {Json::Pair("name", "PlayAnimationAction"),
       Json::Pair(
         "properties",
         Json::array({{
           Json::value(Action::_GetTargetProperty(_target)),
           // Json::NameValuePair("from", from), Json::NameValuePair("to", to),
           Json::NameValuePair(
             "value", Action::_SerializeValueAsString(AnimationValue(loop))),
         }}))}),
    parent);
}

} // end of namespace BABYLON
