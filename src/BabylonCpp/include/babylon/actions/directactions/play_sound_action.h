#ifndef BABYLON_ACTIONS_DIRECTACTIONS_PLAY_SOUND_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_PLAY_SOUND_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class Sound;

class BABYLON_SHARED_EXPORT PlaySoundAction : public Action {

public:
  PlaySoundAction(unsigned int triggerOptions, Sound* sound,
                  Condition* condition = nullptr);
  ~PlaySoundAction() override;

  /**
   * @brief Hidden
   */
  void _prepare() override;

  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

private:
  Sound* _sound;

}; // end of class PlaySoundAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_PLAY_SOUND_ACTION_H
