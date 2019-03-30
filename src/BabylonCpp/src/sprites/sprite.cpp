#include <babylon/sprites/sprite.h>

#include <cmath>

#include <babylon/materials/textures/texture.h>
#include <babylon/math/color4.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

Sprite::Sprite(const std::string& iName, const ISpriteManagerPtr& manager)
    : name{iName}
    , position{Vector3::Zero()}
    , color{Color4(1.f, 1.f, 1.f, 1.f)}
    , width{1.f}
    , height{1.f}
    , angle{0.f}
    , cellIndex{0}
    , invertU{0}
    , invertV{0}
    , isPickable{false}
    , actionManager{nullptr}
    , isVisible{true}
    , size{this, &Sprite::get_size, &Sprite::set_size}
    , _animationStarted{false}
    , _loopAnimation{false}
    , _fromIndex{0}
    , _toIndex{0}
    , _delay{0}
    , _direction{1}
    , _manager{manager}
    , _time{0}
    , _onAnimationEnd{nullptr}
{
}

Sprite::~Sprite()
{
}

void Sprite::addToSpriteManager(const SpritePtr& newSprite)
{
  _manager->sprites.emplace_back(newSprite);
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

void Sprite::playAnimation(int from, int to, bool loop, float delay,
                           const std::function<void()>& onAnimationEnd)
{
  _fromIndex        = from;
  _toIndex          = to;
  _loopAnimation    = loop;
  _delay            = delay;
  _animationStarted = true;

  _direction = from < to ? 1 : -1;

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
    if (cellIndex > _toIndex) {
      if (_loopAnimation) {
        cellIndex = _fromIndex;
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
  _manager->sprites.erase(std::remove_if(_manager->sprites.begin(),
                                         _manager->sprites.end(),
                                         [this](const SpritePtr& sprite) {
                                           return sprite.get() == this;
                                         }),
                          _manager->sprites.end());
}

} // end of namespace BABYLON
