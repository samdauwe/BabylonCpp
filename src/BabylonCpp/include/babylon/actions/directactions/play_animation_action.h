#ifndef BABYLON_ACTIONS_DIRECTACTIONS_PLAY_ANIMATION_ACTION_H
#define BABYLON_ACTIONS_DIRECTACTIONS_PLAY_ANIMATION_ACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PlayAnimationAction : public Action {

public:
  PlayAnimationAction(unsigned int triggerOptions, const IAnimatablePtr& target,
                      int from, int to, bool loop = false,
                      Condition* condition = nullptr);
  ~PlayAnimationAction() override;

  /**
   * @brief Hidden
   */
  void _prepare() override;

  void execute(const ActionEvent& evt) override;
  json serialize(json& parent) const override;

public:
  int from;
  int to;
  bool loop;

private:
  IAnimatablePtr _target;

}; // end of class PlayAnimationAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_DIRECTACTIONS_PLAY_ANIMATION_ACTION_H
