#ifndef BABYLON_ACTIONS_DIRECTACTIONS_PLAY_ANIMATION_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_PLAY_ANIMATION_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PlayAnimationAction : public Action {

public:
  PlayAnimationAction(unsigned int triggerOptions, IAnimatable* target,
                      float from, float to, bool loop = false,
                      Condition* condition = nullptr);
  ~PlayAnimationAction();

  void _prepare() override;
  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

public:
  float from;
  float to;
  bool loop;

private:
  IAnimatable* _target;

}; // end of class PlayAnimationAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_PLAY_ANIMATION_ACTION_H
