#include <babylon/actions/directactions/stop_sound_action.h>

#include <babylon/audio/sound.h>
#include <babylon/core/json.h>

namespace BABYLON {

StopSoundAction::StopSoundAction(unsigned int triggerOptions, Sound* sound,
                                 Condition* condition)
    : Action(triggerOptions, condition), _sound{sound}
{
}

StopSoundAction::~StopSoundAction()
{
}

void StopSoundAction::_prepare()
{
}

void StopSoundAction::execute(const ActionEvent& /*evt*/)
{
  if (_sound != nullptr) {
    _sound->stop();
  }
}

Json::object StopSoundAction::serialize(Json::object& parent) const
{
  return Action::_serialize(
    Json::object({Json::Pair("name", "StopSoundAction"),
                  Json::Pair("properties", Json::array({{Json::NameValuePair(
                                             "sound", _sound->name)}}))}),
    parent);
}

} // end of namespace BABYLON
