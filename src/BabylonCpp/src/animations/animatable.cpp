#include <babylon/animations/animatable.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/animation_value.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

Animatable::Animatable(Scene* scene, IAnimatable* iTarget, int iFromFrame,
                       int iToFrame, bool iLoopAnimation, float iSpeedRatio,
                       const std::function<void()>& iOnAnimationEnd,
                       const std::vector<Animation*>& animations)
    : target{iTarget}
    , animationStarted{false}
    , fromFrame{iFromFrame}
    , toFrame{iToFrame}
    , loopAnimation{iLoopAnimation}
    , speedRatio{iSpeedRatio}
    , onAnimationEnd{iOnAnimationEnd}
    , _localDelayOffset{-1}
    , _pausedDelay{-1}
    , _paused{false}
    , _scene{scene}
{
  if (!animations.empty()) {
    appendAnimations(target, animations);
  }

  scene->_activeAnimatables.emplace_back(this);
}

Animatable::~Animatable()
{
}

// Methods
std::vector<Animation*>& Animatable::getAnimations()
{
  return _animations;
}

void Animatable::appendAnimations(IAnimatable* iTarget,
                                  const std::vector<Animation*>& animations)
{
  for (auto& animation : animations) {
    animation->_target = iTarget;
    _animations.emplace_back(animation);
  }
}

Animation*
Animatable::getAnimationByTargetProperty(const std::string& property) const
{
  auto it = std::find_if(_animations.begin(), _animations.end(),
                         [&property](Animation* animation) {
                           return animation->targetProperty == property;
                         });

  return (it == _animations.end()) ? nullptr : *it;
}

void Animatable::reset()
{
  for (auto& animation : _animations) {
    animation->reset();
  }

  _localDelayOffset = std::chrono::milliseconds(-1);
  _pausedDelay      = std::chrono::milliseconds(-1);
}

void Animatable::enableBlending(float blendingSpeed)
{
  for (auto& animation : _animations) {
    animation->enableBlending = true;
    animation->blendingSpeed  = blendingSpeed;
  }
}

void Animatable::disableBlending()
{
  for (auto& animation : _animations) {
    animation->enableBlending = false;
  }
}

void Animatable::goToFrame(int frame)
{
  if (!_animations.empty() && _animations[0]) {
    auto fps          = _animations[0]->framePerSecond;
    auto currentFrame = _animations[0]->currentFrame;
    auto adjustTime   = frame - currentFrame;
    auto delay
      = static_cast<float>(adjustTime) * 1000.f / static_cast<float>(fps);
    _localDelayOffset -= std::chrono::milliseconds(static_cast<long>(delay));
  }

  for (auto& animation : _animations) {
    animation->goToFrame(frame);
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

void Animatable::stop(const std::string& animationName)
{
  if (!animationName.empty()) {
    auto idx = stl_util::index_of(_scene->_activeAnimatables, this);
    if (idx > -1) {
      for (size_t index = _animations.size() - 1; index-- > 0;) {
        if (_animations[index]->name != animationName) {
          continue;
        }
        _animations[index]->reset();
        stl_util::splice(_animations, static_cast<int>(index), 1);
      }
      if (_animations.empty()) {
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
      for (auto& animation : _animations) {
        animation->reset();
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
    if (_pausedDelay == std::chrono::milliseconds(-1)) {
      _pausedDelay = delay;
    }
    return true;
  }

  if (_localDelayOffset == std::chrono::milliseconds(-1)) {
    _localDelayOffset = delay;
  }
  else if (_pausedDelay != std::chrono::milliseconds(-1)) {
    _localDelayOffset += delay - _pausedDelay;
    _pausedDelay = std::chrono::milliseconds(-1);
  }

  // Animating
  bool running = false;

  for (auto& animation : _animations) {
    bool isRunning = animation->animate(delay - _localDelayOffset, fromFrame,
                                        toFrame, loopAnimation, speedRatio);
    running = running || isRunning;
  }

  animationStarted = running;

  if (!running) {
    // Remove from active animatables
    _scene->_activeAnimatables.erase(
      std::remove_if(
        _scene->_activeAnimatables.begin(), _scene->_activeAnimatables.end(),
        [this](const Animatable* animatable) { return animatable == this; }),
      _scene->_activeAnimatables.end());
  }

  if (!running && onAnimationEnd) {
    onAnimationEnd();
    onAnimationEnd = nullptr;
  }

  return running;
}

} // end of namespace BABYLON
