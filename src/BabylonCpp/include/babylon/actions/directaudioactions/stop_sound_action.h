#ifndef BABYLON_ACTIONS_DIRECT_AUDIO_ACTIONS_STOP_SOUND_ACTION_H
#define BABYLON_ACTIONS_DIRECT_AUDIO_ACTIONS_STOP_SOUND_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class Sound;

/**
 * @brief This defines an action helpful to stop a defined sound on a triggered
 * action.
 */
class BABYLON_SHARED_EXPORT StopSoundAction : public Action {

public:
  /**
   * @brief Instantiate the action.
   * @param triggerOptions defines the trigger options
   * @param sound defines the sound to stop
   * @param condition defines the trigger related conditions
   */
  StopSoundAction(unsigned int triggerOptions, Sound* sound,
                  Condition* condition = nullptr);
  ~StopSoundAction(); // = default

  /**
   * @brief Hidden
   */
  void _prepare() override;

  /**
   * @brief Execute the action and stop the sound.
   */
  void execute(const std::optional<IActionEvent>& evt) override;

  /**
   * @brief Serializes the actions and its related information.
   * @param parent defines the object to serialize in
   * @returns the serialized object
   */
  json serialize(json& parent) const override;

private:
  Sound* _sound;

}; // end of class StopSoundAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECT_AUDIO_ACTIONS_STOP_SOUND_ACTION_H
