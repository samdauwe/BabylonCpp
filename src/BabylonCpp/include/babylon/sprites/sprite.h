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
    sprite->addToSpriteManager(static_cast<std::unique_ptr<Sprite>>(sprite));

    return sprite;
  }
  virtual ~Sprite();

  void addToSpriteManager(std::unique_ptr<Sprite>&& newSprite);

  int size() const;
  void setSize(int value);
  void playAnimation(int from, int to, bool loop, float delay,
                     const std::function<void()>& onAnimationEnd);
  void stopAnimation();
  void _animate(float deltaTime);
  void dispose(bool doNotRecurse = false) override;

protected:
  Sprite(const std::string& name, SpriteManager* manager);

public:
  std::string name;
  Vector3 position;
  std::unique_ptr<Color4> color;
  int width;
  int height;
  float angle;
  int cellIndex;
  int invertU;
  int invertV;
  bool disposeWhenFinishedAnimating;
  std::vector<Animation*> animations;
  bool isPickable;
  ActionManager* actionManager;

private:
  bool _animationStarted;
  bool _loopAnimation;
  int _fromIndex;
  int _toIndex;
  float _delay;
  int _direction;
  int _frameCount;
  SpriteManager* _manager;
  float _time;
  std::function<void()> _onAnimationEnd;

}; // end of class Sprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_H
