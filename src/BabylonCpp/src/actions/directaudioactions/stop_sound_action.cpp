#include <babylon/actions/directaudioactions/stop_sound_action.h>

#include <babylon/audio/sound.h>
#include <nlohmann/json.hpp>

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

json StopSoundAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
