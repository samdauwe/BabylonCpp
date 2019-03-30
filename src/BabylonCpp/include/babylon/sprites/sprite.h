#ifndef BABYLON_SPRITES_SPRITE_H
#define BABYLON_SPRITES_SPRITE_H

#include <babylon/babylon_api.h>

#include <babylon/animations/animation.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class ActionManager;
struct ISpriteManager;
class Sprite;
using ISpriteManagerPtr = std::shared_ptr<ISpriteManager>;
using SpritePtr         = std::shared_ptr<Sprite>;

/**
 * @brief Class used to represent a sprite.
 * @see http://doc.babylonjs.com/babylon101/sprites
 */
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

  /**
   * @brief Hidden
   */
  void addToSpriteManager(const SpritePtr& newSprite);

  /**
   * @brief Starts an animation.
   * @param from defines the initial key
   * @param to defines the end key
   * @param loop defines if the animation must loop
   * @param delay defines the start delay (in ms)
   * @param onAnimationEnd defines a callback to call when animation ends
   */
  void playAnimation(int from, int to, bool loop, float delay,
                     const std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Stops current animation (if any).
   */
  void stopAnimation();

  /**
   * @brief Hidden
   */
  void _animate(float deltaTime);

  /**
   * @brief Release associated resources.
   */
  void dispose();

protected:
  /**
   * @brief Creates a new Sprite.
   * @param name defines the name
   * @param manager defines the manager
   */
  Sprite(const std::string& name, const ISpriteManagerPtr& manager);

private:
  /**
   * @brief Gets the sprite size.
   */
  float get_size() const;

  /**
   * @brief Sets the sprite size.
   */
  void set_size(float value);

public:
  /**
   * Defines the name
   */
  std::string name;

  /**
   * Gets or sets the current world position
   */
  Vector3 position;

  /**
   * Gets or sets the main color
   */
  std::optional<Color4> color;

  /**
   * Gets or sets the width
   */
  float width;

  /**
   * Gets or sets the height
   */
  float height;

  /**
   * Gets or sets rotation angle
   */
  float angle;

  /**
   * Gets or sets the cell index in the sprite sheet
   */
  int cellIndex;

  /**
   * Gets or sets a boolean indicating if UV coordinates should be inverted in U
   * axis
   */
  int invertU;

  /**
   * Gets or sets a boolean indicating if UV coordinates should be inverted in B
   * axis
   */
  int invertV;

  /**
   * Gets or sets a boolean indicating that this sprite should be disposed after
   * animation ends
   */
  bool disposeWhenFinishedAnimating;

  /**
   * Gets the list of attached animations
   */
  std::vector<Animation*> animations;

  /**
   * Gets or sets a boolean indicating if the sprite can be picked
   */
  bool isPickable;

  /**
   * Gets or sets the associated action manager
   */
  ActionManager* actionManager;

  /**
   * Gets or sets a boolean indicating if the sprite is visible (renderable).
   * Default is true
   */
  bool isVisible;

  /**
   * Gets or sets the sprite size
   */
  Property<Sprite, float> size;

private:
  bool _animationStarted;
  bool _loopAnimation;
  int _fromIndex;
  int _toIndex;
  float _delay;
  int _direction;
  ISpriteManagerPtr _manager;
  float _time;
  std::function<void()> _onAnimationEnd;

}; // end of class Sprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_H
