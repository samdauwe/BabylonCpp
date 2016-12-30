#include <babylon/actions/directactions/play_sound_action.h>

#include <babylon/audio/sound.h>
#include <babylon/core/json.h>

namespace BABYLON {

PlaySoundAction::PlaySoundAction(unsigned int triggerOptions, Sound* sound,
                                 Condition* condition)
    : Action(triggerOptions, condition), _sound{sound}
{
}

PlaySoundAction::~PlaySoundAction()
{
}

void PlaySoundAction::_prepare()
{
}

void PlaySoundAction::execute(const ActionEvent& /*evt*/)
{
  if (_sound != nullptr) {
    _sound->play();
  }
}

Json::object PlaySoundAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object({Json::Pair("name", "PlaySoundAction"),
                  Json::Pair("properties", Json::array({{Json::NameValuePair(
                                             "sound", _sound->name)}}))}),
    parent);
}

} // end of namespace BABYLON
