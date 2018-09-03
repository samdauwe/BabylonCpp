#ifndef BABYLON_ACTIONS_DIRECTACTIONS_STOP_SOUND_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_STOP_SOUND_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT StopSoundAction : public Action {

public:
  StopSoundAction(unsigned int triggerOptions, Sound* sound,
                  Condition* condition = nullptr);
  ~StopSoundAction() override;

  /**
   * @brief Hidden
   */
  void _prepare() override;

  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

private:
  Sound* _sound;

}; // end of class StopSoundAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_STOP_SOUND_ACTION_H
