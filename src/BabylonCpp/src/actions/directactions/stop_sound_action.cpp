#include <babylon/actions/directactions/stop_sound_action.h>

#include <nlohmann/json.hpp>

#include <babylon/audio/sound.h>

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
