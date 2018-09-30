#ifndef BABYLON_SPRITES_SPRITE_H
#define BABYLON_SPRITES_SPRITE_H

#include <babylon/babylon_api.h>

#include <babylon/animations/animation.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class ActionManager;
class Sprite;
class SpriteManager;
using SpritePtr        = std::shared_ptr<Sprite>;
using SpriteManagerPtr = std::shared_ptr<SpriteManager>;

class BABYLON_SHARED_EXPORT Sprite {

public:
  template <typename... Ts>
  static SpritePtr New(Ts&&... args)
  {
    auto sprite
      = std::shared_ptr<Sprite>(new Sprite(std::forward<Ts>(args)...));
    sprite->addToSpriteManager(sprite);

    return sprite;
  }
  virtual ~Sprite();

  void addToSpriteManager(const SpritePtr& newSprite);

  void playAnimation(int from, int to, bool loop, float delay,
                     const std::function<void()>& onAnimationEnd = nullptr);
  void stopAnimation();

  /**
   * @brief Hidden
   */
  void _animate(float deltaTime);
  void dispose();

protected:
  Sprite(const std::string& name, const SpriteManagerPtr& manager);

private:
  float get_size() const;
  void set_size(float value);

public:
  std::string name;
  Vector3 position;
  std::unique_ptr<Color4> color;
  float width;
  float height;
  float angle;
  int cellIndex;
  int invertU;
  int invertV;
  bool disposeWhenFinishedAnimating;
  std::vector<Animation*> animations;
  bool isPickable;
  ActionManager* actionManager;

  /**
   * Gets or sets a boolean indicating if the sprite is visible (renderable).
   * Default is true
   */
  bool isVisible;

  Property<Sprite, float> size;

private:
  bool _animationStarted;
  bool _loopAnimation;
  int _fromIndex;
  int _toIndex;
  float _delay;
  int _direction;
  SpriteManagerPtr _manager;
  float _time;
  std::function<void()> _onAnimationEnd;

}; // end of class Sprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_H
