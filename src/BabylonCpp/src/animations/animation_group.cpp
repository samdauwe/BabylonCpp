#include <babylon/animations/animation_group.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/animations/targeted_animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

AnimationGroup::AnimationGroup(const std::string& iName, Scene* scene)
    : name{iName}
    , from{this, &AnimationGroup::get_from}
    , to{this, &AnimationGroup::get_to}
    , isStarted{this, &AnimationGroup::get_isStarted}
    , speedRatio{this, &AnimationGroup::get_speedRatio,
                 &AnimationGroup::set_speedRatio}
    , targetedAnimations{this, &AnimationGroup::get_targetedAnimations}
    , animatables{this, &AnimationGroup::get_animatables}
    , _scene{scene ? scene : Engine::LastCreatedScene()}
    , _from{std::numeric_limits<float>::max()}
    , _to{std::numeric_limits<float>::lowest()}
    , _isStarted{false}
    , _speedRatio{1.f}
{
}

AnimationGroup::~AnimationGroup()
{
}

void AnimationGroup::addToScene(
  std::unique_ptr<AnimationGroup>&& newAnimationGroup)
{
  _scene->animationGroups.emplace_back(std::move(newAnimationGroup));
}

float AnimationGroup::get_from() const
{
  return _from;
}

float AnimationGroup::get_to() const
{
  return _to;
}

bool AnimationGroup::get_isStarted() const
{
  return _isStarted;
}

float AnimationGroup::get_speedRatio() const
{
  return _speedRatio;
}

void AnimationGroup::set_speedRatio(float value)
{
  if (stl_util::almost_equal(_speedRatio, value)) {
    return;
  }

  _speedRatio = value;

  for (auto& animatable : _animatables) {
    animatable->speedRatio = _speedRatio;
  }
}

std::vector<std::unique_ptr<TargetedAnimation>>&
AnimationGroup::get_targetedAnimations()
{
  return _targetedAnimations;
}

std::vector<AnimatablePtr>& AnimationGroup::get_animatables()
{
  return _animatables;
}

TargetedAnimation
AnimationGroup::addTargetedAnimation(const AnimationPtr& animation,
                                     const IAnimatablePtr& target)
{
  TargetedAnimation targetedAnimation{
    animation, // animation,
    target     // target
  };

  const auto& keys = animation->getKeys();
  if (_from > keys.front().frame) {
    _from = keys.front().frame;
  }

  if (_to < keys.back().frame) {
    _to = keys.back().frame;
  }

  _targetedAnimations.emplace_back(
    std::make_unique<TargetedAnimation>(targetedAnimation));

  return targetedAnimation;
}

AnimationGroup& AnimationGroup::normalize(const std::optional<int>& iBeginFrame,
                                          const std::optional<int>& iEndFrame)
{
  auto beginFrame = iBeginFrame ? *iBeginFrame : _from;
  auto endFrame   = iEndFrame ? *iEndFrame : _to;

  for (auto& targetedAnimation : _targetedAnimations) {
    auto& keys           = targetedAnimation->animation->getKeys();
    const auto& startKey = keys.front();
    const auto& endKey   = keys.back();

    if (startKey.frame > beginFrame) {
      IAnimationKey newKey(beginFrame, startKey.value);
      newKey.inTangent     = startKey.inTangent;
      newKey.outTangent    = startKey.outTangent;
      newKey.interpolation = startKey.interpolation;
      keys.insert(keys.begin(), newKey);
    }

    if (endKey.frame < endFrame) {
      IAnimationKey newKey(endFrame, endKey.value);
      newKey.inTangent     = endKey.inTangent;
      newKey.outTangent    = endKey.outTangent;
      newKey.interpolation = endKey.interpolation;
      keys.emplace_back(newKey);
    }
  }

  _from = beginFrame;
  _to   = endFrame;

  return *this;
}

AnimationGroup& AnimationGroup::start(bool loop, float speedRatio,
                                      std::optional<float> from,
                                      std::optional<float> to)
{
  if (_isStarted || _targetedAnimations.empty()) {
    return *this;
  }

  for (auto& targetedAnimation : _targetedAnimations) {
    auto animatable = _scene->beginDirectAnimation(
      targetedAnimation->target, {targetedAnimation->animation},
      from.has_value() ? *from : _from, to.has_value() ? *to : _to, loop,
      speedRatio);
    animatable->onAnimationEnd = [&]() {
      onAnimationEndObservable.notifyObservers(targetedAnimation.get());
      _checkAnimationGroupEnded(animatable);
    };
    _animatables.emplace_back(animatable);
  }

  _speedRatio = speedRatio;

  _isStarted = true;

  return *this;
}

AnimationGroup& AnimationGroup::pause()
{
  if (!_isStarted) {
    return *this;
  }

  for (auto& animatable : _animatables) {
    animatable->pause();
  }

  onAnimationGroupPauseObservable.notifyObservers(this);

  return *this;
}

AnimationGroup& AnimationGroup::play(bool loop)
{
  // only if all animatables are ready and exist
  if (isStarted() && _animatables.size() == _targetedAnimations.size()) {
    for (auto& animatable : _animatables) {
      animatable->loopAnimation = loop;
    }
    restart();
  }
  else {
    stop();
    start(loop, _speedRatio);
  }

  return *this;
}

AnimationGroup& AnimationGroup::reset()
{
  if (!_isStarted) {
    return *this;
  }

  for (auto& animatable : _animatables) {
    animatable->reset();
  }

  _isStarted = false;

  return *this;
}

AnimationGroup& AnimationGroup::restart()
{
  if (!_isStarted) {
    return *this;
  }

  for (auto& animatable : _animatables) {
    animatable->restart();
  }

  _isStarted = false;

  return *this;
}

AnimationGroup& AnimationGroup::stop()
{
  if (!_isStarted) {
    return *this;
  }

  for (auto& animatable : _animatables) {
    animatable->stop();
  }

  _isStarted = false;

  return *this;
}

AnimationGroup& AnimationGroup::setWeightForAllAnimatables(float weight)
{
  for (auto& animatable : _animatables) {
    animatable->weight = weight;
  }

  return *this;
}

AnimationGroup& AnimationGroup::syncAllAnimationsWith(Animatable* root)
{
  for (auto& animatable : _animatables) {
    animatable->syncWith(root);
  }

  return *this;
}

AnimationGroup& AnimationGroup::goToFrame(int frame)
{
  if (!_isStarted) {
    return *this;
  }

  for (auto& animatable : _animatables) {
    animatable->goToFrame(frame);
  }

  return *this;
}

void AnimationGroup::dispose(bool /*doNotRecurse*/,
                             bool /*disposeMaterialAndTextures*/)
{
  _targetedAnimations.clear();
  _animatables.clear();

  _scene->animationGroups.erase(
    std::remove_if(_scene->animationGroups.begin(),
                   _scene->animationGroups.end(),
                   [this](const AnimationGroupPtr& animationGroup) {
                     return animationGroup.get() == this;
                   }),
    _scene->animationGroups.end());
}

void AnimationGroup::_checkAnimationGroupEnded(const AnimatablePtr& animatable)
{
  // animatable should be taken out of the array
  _animatables.erase(
    std::remove(_animatables.begin(), _animatables.end(), animatable),
    _animatables.end());

  // all animatables were removed? animation group ended!
  if (_animatables.empty()) {
    _isStarted = false;
    onAnimationGroupEndObservable.notifyObservers(this);
  }
}

} // end of namespace BABYLON
