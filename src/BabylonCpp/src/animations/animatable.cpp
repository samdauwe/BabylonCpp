#include <babylon/animations/animatable.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/runtime_animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

Animatable::Animatable(Scene* scene, IAnimatable* iTarget, int iFromFrame,
                       int iToFrame, bool iLoopAnimation, float iSpeedRatio,
                       const ::std::function<void()>& iOnAnimationEnd,
                       const vector_t<Animation*>& animations)
    : target{iTarget}
    , animationStarted{false}
    , fromFrame{iFromFrame}
    , toFrame{iToFrame}
    , loopAnimation{iLoopAnimation}
    , onAnimationEnd{iOnAnimationEnd}
    , speedRatio{this, &Animatable::get_speedRatio, &Animatable::set_speedRatio}
    , _localDelayOffset{nullptr}
    , _pausedDelay{nullptr}
    , _paused{false}
    , _scene{scene}
    , _speedRatio{iSpeedRatio}
{
  if (!animations.empty()) {
    appendAnimations(target, animations);
  }

  scene->_activeAnimatables.emplace_back(this);
}

Animatable::~Animatable()
{
}

float Animatable::get_speedRatio() const
{
  return _speedRatio;
}

void Animatable::set_speedRatio(float value)
{
  for (auto& animation : _runtimeAnimations) {
    animation->_prepareForSpeedRatioChange(value);
  }
  _speedRatio = value;
}

// Methods
vector_t<RuntimeAnimation*>& Animatable::getAnimations()
{
  return _runtimeAnimations;
}

void Animatable::appendAnimations(IAnimatable* /*iTarget*/,
                                  const vector_t<Animation*>& animations)
{
  for (auto& animation : animations) {
    _runtimeAnimations.emplace_back(new RuntimeAnimation(target, animation));
  }
}

Animation*
Animatable::getAnimationByTargetProperty(const string_t& property) const
{
  auto it = ::std::find_if(
    _runtimeAnimations.begin(), _runtimeAnimations.end(),
    [&property](RuntimeAnimation* runtimeAnimation) {
      return runtimeAnimation->animation()->targetProperty == property;
    });

  return (it == _runtimeAnimations.end()) ? nullptr : (*it)->animation();
}

RuntimeAnimation*
Animatable::getRuntimeAnimationByTargetProperty(const string_t& property) const
{
  auto it = ::std::find_if(
    _runtimeAnimations.begin(), _runtimeAnimations.end(),
    [&property](RuntimeAnimation* runtimeAnimation) {
      return runtimeAnimation->animation()->targetProperty == property;
    });

  return (it == _runtimeAnimations.end()) ? nullptr : *it;
}

void Animatable::reset()
{
  for (auto& runtimeAnimation : _runtimeAnimations) {
    runtimeAnimation->reset();
  }

  // Reset to original value
  for (auto& animation : _runtimeAnimations) {
    animation->animate(millisecond_t(0), fromFrame, toFrame, false,
                       _speedRatio);
  }

  _localDelayOffset = nullptr;
  _pausedDelay      = nullptr;
}

void Animatable::enableBlending(float blendingSpeed)
{
  for (auto& runtimeAnimation : _runtimeAnimations) {
    runtimeAnimation->animation()->enableBlending = true;
    runtimeAnimation->animation()->blendingSpeed  = blendingSpeed;
  }
}

void Animatable::disableBlending()
{
  for (auto& runtimeAnimation : _runtimeAnimations) {
    runtimeAnimation->animation()->enableBlending = false;
  }
}

void Animatable::goToFrame(int frame)
{
  if (!_runtimeAnimations.empty() && _runtimeAnimations[0]) {
    auto fps          = _runtimeAnimations[0]->animation()->framePerSecond;
    auto currentFrame = _runtimeAnimations[0]->currentFrame;
    auto adjustTime   = frame - currentFrame;
    auto delay
      = static_cast<float>(adjustTime) * 1000.f / static_cast<float>(fps);
    if (_localDelayOffset == nullptr) {
      _localDelayOffset = millisecond_t(0);
    }
    _localDelayOffset = (*_localDelayOffset)
                        - std::chrono::milliseconds(static_cast<long>(delay));
  }

  for (auto& runtimeAnimations : _runtimeAnimations) {
    runtimeAnimations->goToFrame(frame);
  }
}

void Animatable::pause()
{
  if (_paused) {
    return;
  }
  _paused = true;
}

void Animatable::restart()
{
  _paused = false;
}

void Animatable::stop(const string_t& animationName)
{
  if (!animationName.empty()) {
    auto idx = stl_util::index_of(_scene->_activeAnimatables, this);
    if (idx > -1) {
      for (size_t index = _runtimeAnimations.size(); index-- > 0;) {
        if (_runtimeAnimations[index]->animation()->name != animationName) {
          continue;
        }
        _runtimeAnimations[index]->dispose();
        stl_util::splice(_runtimeAnimations, static_cast<int>(index), 1);
      }
      if (_runtimeAnimations.empty()) {
        stl_util::splice(_scene->_activeAnimatables, idx, 1);
        if (onAnimationEnd) {
          onAnimationEnd();
        }
      }
    }
  }
  else {
    auto index = stl_util::index_of(_scene->_activeAnimatables, this);
    if (index > -1) {
      stl_util::splice(_scene->_activeAnimatables, index, 1);
      for (auto& runtimeAnimation : _runtimeAnimations) {
        runtimeAnimation->dispose();
      }
      if (onAnimationEnd) {
        onAnimationEnd();
      }
    }
  }
}

bool Animatable::_animate(const millisecond_t& delay)
{
  if (_paused) {
    animationStarted = false;
    if (_pausedDelay == nullptr) {
      _pausedDelay = delay;
    }
    return true;
  }

  if (_localDelayOffset == nullptr) {
    _localDelayOffset = delay;
    _pausedDelay      = nullptr;
  }
  else if (_pausedDelay != nullptr) {
    _localDelayOffset = (*_localDelayOffset) + delay - (*_pausedDelay);
    _pausedDelay      = nullptr;
  }

  // Animating
  bool running = false;

  for (auto& animation : _runtimeAnimations) {
    bool isRunning = animation->animate(delay - (*_localDelayOffset), fromFrame,
                                        toFrame, loopAnimation, speedRatio());
    running        = running || isRunning;
  }

  animationStarted = running;

  if (!running) {
    // Remove from active animatables
    _scene->_activeAnimatables.erase(
      ::std::remove_if(
        _scene->_activeAnimatables.begin(), _scene->_activeAnimatables.end(),
        [this](const Animatable* animatable) { return animatable == this; }),
      _scene->_activeAnimatables.end());

    // Dispose all runtime animations
    for (auto& runtimeAnimation : _runtimeAnimations) {
      runtimeAnimation->dispose();
    }
  }

  if (!running && onAnimationEnd) {
    onAnimationEnd();
    onAnimationEnd = nullptr;
  }

  return running;
}

} // end of namespace BABYLON
