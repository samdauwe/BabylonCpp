#include <babylon/sprites/thin_sprite.h>

#include <cmath>

namespace BABYLON {

ThinSprite::ThinSprite()
    : width{1.f}
    , height{1.f}
    , angle{0.f}
    , invertU{false}
    , invertV{false}
    , isVisible{true}
    , animationStarted{this, &ThinSprite::get_animationStarted}
    , fromIndex{this, &ThinSprite::get_fromIndex}
    , toIndex{this, &ThinSprite::get_toIndex}
    , loopAnimation{this, &ThinSprite::get_loopAnimation}
    , delay{this, &ThinSprite::get_delay}
    , _loopAnimation{false}
    , _fromIndex{0}
    , _toIndex{0}
    , _delay{0}
    , _animationStarted{false}
    , _direction{1}
    , _time{0}
    , _onBaseAnimationEnd{nullptr}
{
  position = Vector3{1.f, 1.f, 1.f};
  color    = Color4{1.f, 1.f, 1.f, 1.f};
}

ThinSprite::~ThinSprite() = default;

bool ThinSprite::get_animationStarted() const
{
  return _animationStarted;
}

int ThinSprite::get_fromIndex() const
{
  return _fromIndex;
}

int ThinSprite::get_toIndex() const
{
  return _toIndex;
}

bool ThinSprite::get_loopAnimation() const
{
  return _loopAnimation;
}

float ThinSprite::get_delay() const
{
  return std::max(_delay, 1);
}

void ThinSprite::playAnimation(int from, int to, bool loop, float delay,
                               const std::function<void()>& onAnimationEnd)
{
  _fromIndex          = from;
  _toIndex            = to;
  _loopAnimation      = loop;
  _delay              = delay;
  _animationStarted   = true;
  _onBaseAnimationEnd = onAnimationEnd;

  if (from < to) {
    _direction = 1;
  }
  else {
    _direction = -1;
    _toIndex   = from;
    _fromIndex = to;
  }

  cellIndex = from;
  _time     = 0.f;
}

void ThinSprite::stopAnimation()
{
  _animationStarted = false;
}

void ThinSprite::_animate(float deltaTime)
{
  if (!_animationStarted) {
    return;
  }

  _time += deltaTime;
  if (_time > _delay) {
    _time = std::fmod(_time, _delay);
    cellIndex += _direction;
    if ((_direction > 0 && cellIndex > _toIndex) || (_direction < 0 && cellIndex < _fromIndex)) {
      if (_loopAnimation) {
        cellIndex = _direction > 0 ? _fromIndex : _toIndex;
      }
      else {
        cellIndex         = _toIndex;
        _animationStarted = false;
        if (_onBaseAnimationEnd) {
          _onBaseAnimationEnd();
        }
      }
    }
  }
}

} // end of namespace BABYLON
