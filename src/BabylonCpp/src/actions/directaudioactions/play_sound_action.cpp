#include <babylon/actions/directaudioactions/play_sound_action.h>

#include <babylon/audio/sound.h>
#include <nlohmann/json.hpp>

namespace BABYLON {

PlaySoundAction::PlaySoundAction(unsigned int iTriggerOptions, Sound* sound, Condition* condition)
    : Action(iTriggerOptions, condition), _sound{sound}
{
}

PlaySoundAction::~PlaySoundAction() = default;

void PlaySoundAction::_prepare()
{
}

void PlaySoundAction::execute(const std::optional<IActionEvent>& /*evt*/)
{
  if (_sound != nullptr) {
    _sound->play();
  }
}

json PlaySoundAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
