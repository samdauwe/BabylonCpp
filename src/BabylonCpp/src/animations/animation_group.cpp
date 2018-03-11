#include <babylon/animations/animation_group.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/animations/targeted_animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

AnimationGroup::AnimationGroup(const string_t& iName, Scene* scene)
    : name{iName}
    , isStarted{this, &AnimationGroup::get_isStarted}
    , speedRatio{this, &AnimationGroup::get_speedRatio,
                 &AnimationGroup::set_speedRatio}
    , targetedAnimations{this, &AnimationGroup::get_targetedAnimations}
    , _scene{scene ? scene : Engine::LastCreatedScene()}
    , _from{numeric_limits_t<int>::max()}
    , _to{numeric_limits_t<int>::lowest()}
    , _isStarted{false}
    , _speedRatio{1.f}
{
}

AnimationGroup::~AnimationGroup()
{
}

void AnimationGroup::addToScene(
  unique_ptr_t<AnimationGroup>&& newAnimationGroup)
{
  _scene->animationGroups.emplace_back(::std::move(newAnimationGroup));
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

vector_t<unique_ptr_t<TargetedAnimation>>&
AnimationGroup::get_targetedAnimations()
{
  return _targetedAnimations;
}

TargetedAnimation AnimationGroup::addTargetedAnimation(Animation* animation,
                                                       IAnimatable* target)
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
    ::std::make_unique<TargetedAnimation>(targetedAnimation));

  return targetedAnimation;
}

AnimationGroup& AnimationGroup::normalize(int beginFrame, int endFrame)
{
  beginFrame = ::std::max(beginFrame, _from);
  endFrame   = ::std::max(endFrame, _to);

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

  return *this;
}

AnimationGroup& AnimationGroup::start(bool loop, float speedRatio)
{
  if (_isStarted || _targetedAnimations.empty()) {
    return *this;
  }

  for (auto& targetedAnimation : _targetedAnimations) {
    _animatables.emplace_back(_scene->beginDirectAnimation(
      targetedAnimation->target, {targetedAnimation->animation}, _from, _to,
      loop, speedRatio, [this, &targetedAnimation]() {
        onAnimationEndObservable.notifyObservers(targetedAnimation.get());
      }));
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

  return *this;
}

AnimationGroup& AnimationGroup::play(bool loop)
{
  if (isStarted()) {
    for (auto& animatable : _animatables) {
      animatable->loopAnimation = loop;
    }
    restart();
  }
  else {
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

void AnimationGroup::dispose(bool /*doNotRecurse*/)
{
  _targetedAnimations.clear();
  _animatables.clear();

  _scene->animationGroups.erase(
    ::std::remove_if(
      _scene->animationGroups.begin(), _scene->animationGroups.end(),
      [this](const unique_ptr_t<AnimationGroup>& animationGroup) {
        return animationGroup.get() == this;
      }),
    _scene->animationGroups.end());
}

} // end of namespace BABYLON
