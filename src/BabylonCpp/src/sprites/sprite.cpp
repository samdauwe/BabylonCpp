#include <babylon/sprites/sprite.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/color4.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

Sprite::Sprite(const std::string& iName, const ISpriteManagerPtr& manager)
    : name{iName}
    , color{Color4(1.f, 1.f, 1.f, 1.f)}
    , width{1.f}
    , height{1.f}
    , angle{0.f}
    , cellIndex{0}
    , invertU{false}
    , invertV{false}
    , isPickable{false}
    , useAlphaForPicking{false}
    , actionManager{nullptr}
    , isVisible{true}
    , size{this, &Sprite::get_size, &Sprite::set_size}
    , animationStarted{this, &Sprite::get_animationStarted}
    , manager{this, &Sprite::get_manager}
    , fromIndex{this, &Sprite::get_fromIndex, &Sprite::set_fromIndex}
    , toIndex{this, &Sprite::get_toIndex, &Sprite::set_toIndex}
    , loopAnimation{this, &Sprite::get_loopAnimation, &Sprite::set_loopAnimation}
    , delay{this, &Sprite::get_delay, &Sprite::set_delay}
    , _animationStarted{false}
    , _loopAnimation{false}
    , _fromIndex{0}
    , _toIndex{0}
    , _delay{0}
    , _direction{1}
    , _time{0}
    , _onAnimationEnd{nullptr}
{
  _manager = manager;

  uniqueId = _manager->scene()->getUniqueId();

  position = Vector3::Zero();
}

Sprite::~Sprite() = default;

Type Sprite::type() const
{
  return Type::SPRITE;
}

void Sprite::addToSpriteManager(const SpritePtr& newSprite)
{
  _manager->sprites.emplace_back(newSprite);
}

std::string Sprite::getClassName() const
{
  return "Sprite";
}

float Sprite::get_size() const
{
  return width;
}

void Sprite::set_size(float value)
{
  width  = value;
  height = value;
}

bool Sprite::get_animationStarted() const
{
  return _animationStarted;
}

ISpriteManagerPtr& Sprite::get_manager()
{
  return _manager;
}

int Sprite::get_fromIndex() const
{
  return _fromIndex;
}

void Sprite::set_fromIndex(int value)
{
  playAnimation(value, _toIndex, _loopAnimation, _delay, _onAnimationEnd);
}

int Sprite::get_toIndex() const
{
  return _toIndex;
}

void Sprite::set_toIndex(int value)
{
  playAnimation(_fromIndex, value, _loopAnimation, _delay, _onAnimationEnd);
}

bool Sprite::get_loopAnimation() const
{
  return _loopAnimation;
}

void Sprite::set_loopAnimation(bool value)
{
  playAnimation(_fromIndex, _toIndex, value, _delay, _onAnimationEnd);
}

float Sprite::get_delay() const
{
  return std::max(_delay, 1.f);
}

void Sprite::set_delay(float value)
{
  playAnimation(_fromIndex, _toIndex, _loopAnimation, value, _onAnimationEnd);
}

void Sprite::playAnimation(int from, int to, bool loop, float delay,
                           const std::function<void()>& onAnimationEnd)
{
  _fromIndex        = from;
  _toIndex          = to;
  _loopAnimation    = loop;
  _delay            = delay;
  _animationStarted = true;

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

  _onAnimationEnd = onAnimationEnd;
}

void Sprite::stopAnimation()
{
  _animationStarted = false;
}

void Sprite::_animate(float deltaTime)
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
        if (_onAnimationEnd) {
          _onAnimationEnd();
        }
        if (disposeWhenFinishedAnimating) {
          dispose();
        }
      }
    }
  }
}

void Sprite::dispose()
{
  // Remove from scene
  stl_util::remove_vector_elements_equal_sharedptr(_manager->sprites, this);

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

json Sprite::serialize() const
{
  return nullptr;
}

SpritePtr Sprite::Parse(const json& /*parsedSprite*/, const ISpriteManagerPtr& /*manager*/)
{
  return nullptr;
}

} // end of namespace BABYLON
