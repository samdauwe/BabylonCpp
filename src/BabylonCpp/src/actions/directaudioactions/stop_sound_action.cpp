#include <babylon/actions/directaudioactions/stop_sound_action.h>

#include <babylon/audio/sound.h>
#include <nlohmann/json.hpp>

namespace BABYLON {

StopSoundAction::StopSoundAction(unsigned int iTriggerOptions, Sound* sound, Condition* condition)
    : Action(iTriggerOptions, condition), _sound{sound}
{
}

StopSoundAction::~StopSoundAction() = default;

void StopSoundAction::_prepare()
{
}

void StopSoundAction::execute(const IActionEventPtr& /*evt*/)
{
  if (_sound != nullptr) {
    _sound->stop();
  }
}

json StopSoundAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
