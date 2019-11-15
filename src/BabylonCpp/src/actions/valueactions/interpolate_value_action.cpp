#include <babylon/actions/valueactions/interpolate_value_action.h>

#include <nlohmann/json.hpp>

#include <babylon/actions/action_manager.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/audio/sound.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

InterpolateValueAction::InterpolateValueAction(
  unsigned int iTriggerOptions, const IAnimatablePtr& target,
  const std::string& iPropertyPath, AnimationValue* iValue, int iDuration,
  Condition* condition, bool iStopOtherAnimations,
  const std::function<void()>& iOnInterpolationDone)
    : Action(iTriggerOptions, condition)
    , propertyPath{iPropertyPath}
    , value{iValue}
    , duration{iDuration}
    , stopOtherAnimations{iStopOtherAnimations}
    , onInterpolationDone{iOnInterpolationDone}
    , _target{target}
    , _effectiveTarget{target}
{
}

InterpolateValueAction::~InterpolateValueAction() = default;

void InterpolateValueAction::_prepare()
{
  _effectiveTarget = _getEffectiveTarget(_effectiveTarget, propertyPath);
  _property        = _getProperty(propertyPath);
}

void InterpolateValueAction::execute(const std::optional<IActionEvent>& /*evt*/)
{
  auto scene = _actionManager->getScene();
  std::vector<IAnimationKey> keys{
    IAnimationKey(0, _effectiveTarget->getProperty({_property})), //
    IAnimationKey(100, *value)};

  auto dataType = value->animationType();
  if (!dataType.has_value()) {
    BABYLON_LOG_WARN("InterpolateValueAction",
                     "InterpolateValueAction: Unsupported type")
  }

  auto animation
    = Animation::New("InterpolateValueAction", _property,
                     static_cast<size_t>(100 * (1000.f / duration)),
                     dataType.value(), Animation::ANIMATIONLOOPMODE_CONSTANT());

  animation->setKeys(keys);

  if (stopOtherAnimations) {
    scene->stopAnimation(_effectiveTarget);
  }

  const auto wrapper = [this]() {
    onInterpolationDoneObservable.notifyObservers(this);
    if (onInterpolationDone) {
      onInterpolationDone();
    }
  };

  scene->beginDirectAnimation(_effectiveTarget, {animation}, 0, 100, false, 1,
                              wrapper);
}

json InterpolateValueAction::serialize(json& /*parent*/) const
{
  return nullptr;
}

} // end of namespace BABYLON
