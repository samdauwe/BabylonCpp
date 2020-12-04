#include <babylon/sprites/sprite.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/color4.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

Sprite::Sprite(const std::string& iName, const ISpriteManagerPtr& manager)
    : ThinSprite{}
    , name{iName}
    , isPickable{false}
    , useAlphaForPicking{false}
    , actionManager{nullptr}
    , size{this, &Sprite::get_size, &Sprite::set_size}
    , manager{this, &Sprite::get_manager}
    , _onAnimationEnd{nullptr}
{
  color    = Color4(1.f, 1.f, 1.f, 1.f);
  position = Vector3::Zero();

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

void Sprite::playAnimation(int from, int to, bool loop, float iDelay,
                           const std::function<void()>& onAnimationEnd)
{
  _onAnimationEnd = onAnimationEnd;

  ThinSprite::playAnimation(from, to, loop, iDelay, [this]() -> void { _endAnimation(); });
}

void Sprite::_endAnimation()
{
  if (_onAnimationEnd) {
    _onAnimationEnd();
  }
  if (disposeWhenFinishedAnimating) {
    dispose();
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
