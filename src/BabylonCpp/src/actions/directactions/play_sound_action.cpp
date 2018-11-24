#include <babylon/actions/directactions/play_sound_action.h>

#include <babylon/audio/sound.h>
#include <nlohmann/json.hpp>

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

json PlaySoundAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
