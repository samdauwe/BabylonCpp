#include <babylon/animations/animatable.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/animation_value.h>
#include <babylon/animations/runtime_animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

Animatable::Animatable(Scene* scene, const IAnimatablePtr& iTarget,
                       float iFromFrame, float iToFrame, bool iLoopAnimation,
                       float iSpeedRatio,
                       const std::function<void()>& iOnAnimationEnd,
                       const std::vector<AnimationPtr>& animations)
    : target{iTarget}
    , disposeOnEnd{true}
    , animationStarted{false}
    , fromFrame{iFromFrame}
    , toFrame{iToFrame}
    , loopAnimation{iLoopAnimation}
    , onAnimationEnd{iOnAnimationEnd}
    , syncRoot{this, &Animatable::get_syncRoot}
    , masterFrame{this, &Animatable::get_masterFrame}
    , weight{this, &Animatable::get_weight, &Animatable::set_weight}
    , speedRatio{this, &Animatable::get_speedRatio, &Animatable::set_speedRatio}
    , _localDelayOffset{std::nullopt}
    , _pausedDelay{std::nullopt}
    , _paused{false}
    , _scene{scene}
    , _weight{-1.f}
    , _syncRoot{nullptr}
{
  if (!animations.empty()) {
    appendAnimations(target, animations);
  }

  _speedRatio = iSpeedRatio;
}

Animatable::~Animatable()
{
}

void Animatable::addToScene(const AnimatablePtr& newAnimatable)
{
  _scene->_activeAnimatables.emplace_back(newAnimatable);
}

Animatable*& Animatable::get_syncRoot()
{
  return _syncRoot;
}

float Animatable::get_masterFrame() const
{
  if (_runtimeAnimations.empty()) {
    return 0.f;
  }

  return _runtimeAnimations[0]->currentFrame;
}

float Animatable::get_weight() const
{
  return _weight;
}

void Animatable::set_weight(float value)
{
  if (stl_util::almost_equal(value, -1.f)) { // -1 is ok and means no weight
    _weight = -1.f;
    return;
  }

  // Else weight must be in [0, 1] range
  _weight = std::min(std::max(value, 0.f), 1.f);
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
Animatable& Animatable::syncWith(Animatable* root)
{
  _syncRoot = root;

  if (root) {
#if 0
    // Make sure this animatable will animate after the root
    auto index
      = stl_util::index_of(_scene->_activeAnimatables, shared_from_this());
    if (index > -1) {
      stl_util::splice(_scene->_activeAnimatables, index, 1);
      _scene->_activeAnimatables.emplace_back(shared_from_this());
    }
#endif
  }

  return *this;
}

std::vector<RuntimeAnimationPtr>& Animatable::getAnimations()
{
  return _runtimeAnimations;
}

void Animatable::appendAnimations(const IAnimatablePtr& iTarget,
                                  const std::vector<AnimationPtr>& animations)
{
  for (auto& animation : animations) {
    _runtimeAnimations.emplace_back(
      RuntimeAnimation::New(iTarget, animation, _scene, this));
  }
}

AnimationPtr
Animatable::getAnimationByTargetProperty(const std::string& property) const
{
  auto it = std::find_if(
    _runtimeAnimations.begin(), _runtimeAnimations.end(),
    [&property](const RuntimeAnimationPtr& runtimeAnimation) {
      return runtimeAnimation->animation()->targetProperty == property;
    });

  return (it == _runtimeAnimations.end()) ? nullptr : (*it)->animation();
}

RuntimeAnimationPtr Animatable::getRuntimeAnimationByTargetProperty(
  const std::string& property) const
{
  auto it = std::find_if(
    _runtimeAnimations.begin(), _runtimeAnimations.end(),
    [&property](const RuntimeAnimationPtr& runtimeAnimation) {
      return runtimeAnimation->animation()->targetProperty == property;
    });

  return (it == _runtimeAnimations.end()) ? nullptr : *it;
}

void Animatable::reset()
{
  for (auto& runtimeAnimation : _runtimeAnimations) {
    runtimeAnimation->reset(true);
  }

  _localDelayOffset = std::nullopt;
  _pausedDelay      = std::nullopt;
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

void Animatable::goToFrame(float frame)
{
  if (!_runtimeAnimations.empty() && _runtimeAnimations[0]) {
    auto fps          = _runtimeAnimations[0]->animation()->framePerSecond;
    auto currentFrame = _runtimeAnimations[0]->currentFrame();
    auto adjustTime   = frame - currentFrame;
    auto delay        = static_cast<float>(adjustTime) * 1000.f
                 / (static_cast<float>(fps) * speedRatio);
    if (_localDelayOffset == std::nullopt) {
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

void Animatable::_raiseOnAnimationEnd()
{
  if (onAnimationEnd) {
    onAnimationEnd();
  }

  onAnimationEndObservable.notifyObservers(this);
}

void Animatable::stop(
  const std::string& animationName,
  const std::function<bool(IAnimatable* target)>& targetMask)
{
  if (!animationName.empty() || targetMask) {
    auto idx = stl_util::index_of_ptr(_scene->_activeAnimatables, this);
    if (idx > -1) {
      for (size_t index = _runtimeAnimations.size(); index-- > 0;) {
        const auto& runtimeAnimation = _runtimeAnimations[index];
        if (runtimeAnimation->animation()->name != animationName) {
          continue;
        }
        if (targetMask && !targetMask(runtimeAnimation->target().get())) {
          continue;
        }

        runtimeAnimation->dispose();
        stl_util::splice(_runtimeAnimations, static_cast<int>(index), 1);
      }
      if (_runtimeAnimations.empty()) {
        stl_util::splice(_scene->_activeAnimatables, idx, 1);
        _raiseOnAnimationEnd();
      }
    }
  }
  else {
    auto index = stl_util::index_of_ptr(_scene->_activeAnimatables, this);
    if (index > -1) {
      stl_util::splice(_scene->_activeAnimatables, index, 1);
      for (auto& runtimeAnimation : _runtimeAnimations) {
        runtimeAnimation->dispose();
      }
      _raiseOnAnimationEnd();
    }
  }
}

bool Animatable::_animate(const millisecond_t& delay)
{
  if (_paused) {
    animationStarted = false;
    if (_pausedDelay == std::nullopt) {
      _pausedDelay = delay;
    }
    return true;
  }

  if (_localDelayOffset == std::nullopt) {
    _localDelayOffset = delay;
    _pausedDelay      = std::nullopt;
  }
  else if (_pausedDelay != std::nullopt) {
    _localDelayOffset = (*_localDelayOffset) + delay - (*_pausedDelay);
    _pausedDelay      = std::nullopt;
  }

  if (_weight == 0.f) { // We consider that an animation with a weight === 0 is
                        // "actively" paused
    return true;
  }

  // Animating
  bool running = false;

  for (auto& animation : _runtimeAnimations) {
    bool isRunning = animation->animate(
      delay - (*_localDelayOffset), static_cast<float>(fromFrame),
      static_cast<float>(toFrame), loopAnimation, speedRatio(), _weight);
    running = running || isRunning;
  }

  animationStarted = running;

  if (!running) {
    if (disposeOnEnd) {
      // Remove from active animatables
      _scene->_activeAnimatables.erase(
        std::remove_if(_scene->_activeAnimatables.begin(),
                       _scene->_activeAnimatables.end(),
                       [this](const AnimatablePtr& animatable) {
                         return animatable.get() == this;
                       }),
        _scene->_activeAnimatables.end());

      // Dispose all runtime animations
      for (auto& runtimeAnimation : _runtimeAnimations) {
        runtimeAnimation->dispose();
      }
    }

    _raiseOnAnimationEnd();

    if (disposeOnEnd) {
      onAnimationEnd = nullptr;
      onAnimationEndObservable.clear();
    }
  }

  return running;
}

} // end of namespace BABYLON
