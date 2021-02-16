#ifndef BABYLON_SPRITES_THIN_SPRITE_H
#define BABYLON_SPRITES_THIN_SPRITE_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/maths/color4.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

/**
 * @brief ThinSprite Class used to represent a thin sprite.
 * This is the base class for sprites but can also directly be used with ThinEngine
 * @see https://doc.babylonjs.com/babylon101/sprites
 */
class BABYLON_SHARED_EXPORT ThinSprite {

public:
  /**
   * @brief Creates a new Thin Sprite.
   */
  ThinSprite();
  virtual ~ThinSprite(); // = default

  /**
   * @brief Starts an animation.
   * @param from defines the initial key
   * @param to defines the end key
   * @param loop defines if the animation must loop
   * @param delay defines the start delay (in ms)
   * @param onAnimationEnd defines a callback for when the animation ends
   */
  void playAnimation(int from, int to, bool loop, float delay = 1.f,
                     const std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Stops current animation (if any).
   */
  void stopAnimation();

  /**
   * @brief Hidden
   */
  void _animate(float deltaTime);

protected:
  /**
   * @brief Returns a boolean indicating if the animation is started.
   */
  bool get_animationStarted() const;

  /**
   * @brief Gets the initial key for the animation (setting it will restart the animation).
   */
  virtual int get_fromIndex() const;

  /**
   * @brief Sets the initial key for the animation (setting it will restart the animation).
   */
  virtual void set_fromIndex(int value);

  /**
   * @brief Gets or sets the end key for the animation (setting it will restart the animation).
   */
  virtual int get_toIndex() const;

  /**
   * @brief Sets the end key for the animation (setting it will restart the animation).
   */
  virtual void set_toIndex(int value);

  /**
   * @brief Gets or sets a boolean indicating if the animation is looping (setting it will restart
   * the animation).
   */
  virtual bool get_loopAnimation() const;

  /**
   * @brief Sets a boolean indicating if the animation is looping (setting it will restart the
   * animation).
   */
  virtual void set_loopAnimation(bool value);

  /**
   * @brief Gets or sets the delay between cell changes (setting it will restart the animation).
   */
  virtual float get_delay() const;

  /**
   * @brief Sets the delay between cell changes (setting it will restart the animation).
   */
  virtual void set_delay(float value);

public:
  /** Gets or sets the cell index in the sprite sheet */
  int cellIndex;
  /** Gets or sets the cell reference in the sprite sheet, uses sprite's filename when added to
   * sprite sheet */
  std::string cellRef;
  /** Gets or sets the current world position */
  Vector3 position;
  /** Gets or sets the main color */
  Color4 color;
  /** Gets or sets the width */
  float width;
  /** Gets or sets the height */
  float height;
  /** Gets or sets rotation angle */
  float angle;
  /** Gets or sets a boolean indicating if UV coordinates should be inverted in U axis */
  bool invertU;
  /** Gets or sets a boolean indicating if UV coordinates should be inverted in B axis */
  bool invertV;
  /** Gets or sets a boolean indicating if the sprite is visible (renderable). Default is true */
  bool isVisible;

  /**
   * Returns a boolean indicating if the animation is started
   */
  ReadOnlyProperty<ThinSprite, bool> animationStarted;

  /**
   * Gets the initial key for the animation (setting it will restart the animation)
   */
  Property<ThinSprite, int> fromIndex;

  /**
   * Gets or sets the end key for the animation (setting it will restart the animation)
   */
  Property<ThinSprite, int> toIndex;

  /**
   * Gets or sets a boolean indicating if the animation is looping (setting it will restart the
   * animation)
   */
  Property<ThinSprite, bool> loopAnimation;

  /**
   * Gets or sets the delay between cell changes (setting it will restart the animation)
   */
  Property<ThinSprite, float> delay;

  /** @hidden */
  int _xOffset;
  /** @hidden */
  int _yOffset;
  /** @hidden */
  int _xSize;
  /** @hidden */
  int _ySize;

protected:
  bool _loopAnimation;
  int _fromIndex;
  int _toIndex;
  float _delay;

private:
  bool _animationStarted;
  int _direction;
  float _time;
  std::function<void()> _onBaseAnimationEnd;

}; // end of class ThinSprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_THIN_SPRITE_H
