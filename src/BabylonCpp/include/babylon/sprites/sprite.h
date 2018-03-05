#ifndef BABYLON_SPRITES_SPRITE_H
#define BABYLON_SPRITES_SPRITE_H

#include <babylon/babylon_global.h>

#include <babylon/animations/animation.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Sprite : public IDisposable {

public:
  template <typename... Ts>
  static Sprite* New(Ts&&... args)
  {
    auto sprite = new Sprite(std::forward<Ts>(args)...);
    sprite->addToSpriteManager(static_cast<unique_ptr_t<Sprite>>(sprite));

    return sprite;
  }
  virtual ~Sprite();

  void addToSpriteManager(unique_ptr_t<Sprite>&& newSprite);

  void playAnimation(int from, int to, bool loop, float delay,
                     const ::std::function<void()>& onAnimationEnd);
  void stopAnimation();
  void _animate(float deltaTime);
  void dispose(bool doNotRecurse = false) override;

protected:
  Sprite(const string_t& name, SpriteManager* manager);

private:
  int get_size() const;
  void set_size(int value);

public:
  string_t name;
  Vector3 position;
  unique_ptr_t<Color4> color;
  int width;
  int height;
  float angle;
  int cellIndex;
  int invertU;
  int invertV;
  bool disposeWhenFinishedAnimating;
  vector_t<Animation*> animations;
  bool isPickable;
  ActionManager* actionManager;

  Property<Sprite, int> size;

private:
  bool _animationStarted;
  bool _loopAnimation;
  int _fromIndex;
  int _toIndex;
  float _delay;
  int _direction;
  SpriteManager* _manager;
  float _time;
  ::std::function<void()> _onAnimationEnd;

}; // end of class Sprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_H
