#ifndef BABYLON_ACTIONS_INTERPOLATEVALUEACTION_H
#define BABYLON_ACTIONS_INTERPOLATEVALUEACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_global.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT InterpolateValueAction : public Action {

public:
  InterpolateValueAction(unsigned int triggerOptions,
                         const IAnimatablePtr& target,
                         const string_t& propertyPath, AnimationValue* value,
                         int duration = 1000, Condition* condition = nullptr,
                         bool stopOtherAnimations = false,
                         const ::std::function<void()>& onInterpolationDone
                         = nullptr);
  ~InterpolateValueAction() override;

  /**
   * @brief Hidden
   */
  void _prepare() override;

  void execute(const ActionEvent& evt) override;
  Json::object serialize(Json::object& parent) const override;

public:
  string_t propertyPath;
  AnimationValue* value;
  int duration;
  bool stopOtherAnimations;
  ::std::function<void()> onInterpolationDone;

  Observable<InterpolateValueAction> onInterpolationDoneObservable;

private:
  IAnimatablePtr _target;
  IAnimatablePtr _effectiveTarget;
  string_t _property;

}; // end of class InterpolateValueAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_INTERPOLATEVALUEACTION_H
