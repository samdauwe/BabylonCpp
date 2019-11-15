#ifndef BABYLON_ACTIONS_INTERPOLATEVALUEACTION_H
#define BABYLON_ACTIONS_INTERPOLATEVALUEACTION_H

#include <babylon/actions/action.h>
#include <babylon/babylon_api.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

/**
 * @brief This defines an action responsible to change the value of a property
 * by interpolating between its current value and the newly set one once
 * triggered.
 * @see http://doc.babylonjs.com/how_to/how_to_use_actions
 */
class BABYLON_SHARED_EXPORT InterpolateValueAction : public Action {

public:
  /**
   * @brief Instantiate the action.
   * @param triggerOptions defines the trigger options
   * @param target defines the object containing the value to interpolate
   * @param propertyPath defines the path to the property in the target object
   * @param value defines the target value at the end of the interpolation
   * @param duration deines the time it will take for the property to
   * interpolate to the value.
   * @param condition defines the trigger related conditions
   * @param stopOtherAnimations defines if the other scene animations should be
   * stopped when the action has been triggered
   * @param onInterpolationDone defines a callback raised once the interpolation
   * animation has been done
   */
  InterpolateValueAction(unsigned int triggerOptions,
                         const IAnimatablePtr& target,
                         const std::string& propertyPath, AnimationValue* value,
                         int duration = 1000, Condition* condition = nullptr,
                         bool stopOtherAnimations = false,
                         const std::function<void()>& onInterpolationDone
                         = nullptr);
  ~InterpolateValueAction(); // = default

  /**
   * @brief Hidden
   */
  void _prepare() override;

  /**
   * @brief Execute the action starts the value interpolation.
   */
  void execute(const std::optional<IActionEvent>& evt) override;

  /**
   * @brief Serializes the actions and its related information.
   * @param parent defines the object to serialize in
   * @returns the serialized object
   */
  json serialize(json& parent) const override;

public:
  /**
   * Defines the path of the property where the value should be interpolated
   */
  std::string propertyPath;

  /**
   * Defines the target value at the end of the interpolation.
   */
  AnimationValue* value;

  /**
   * Defines the time it will take for the property to interpolate to the value.
   */
  int duration;

  /**
   * Defines if the other scene animations should be stopped when the action has
   * been triggered
   */
  bool stopOtherAnimations;

  /**
   * Defines a callback raised once the interpolation animation has been done.
   */
  std::function<void()> onInterpolationDone;

  /**
   * Observable triggered once the interpolation animation has been done.
   */
  Observable<InterpolateValueAction> onInterpolationDoneObservable;

private:
  IAnimatablePtr _target;
  IAnimatablePtr _effectiveTarget;
  std::string _property;

}; // end of class InterpolateValueAction

} // end of namespace BABYLON

#endif // end of BABYLON_ACTIONS_INTERPOLATEVALUEACTION_H
