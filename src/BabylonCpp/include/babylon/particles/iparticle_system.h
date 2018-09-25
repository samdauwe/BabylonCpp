#ifndef BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_IPARTICLE_SYSTEM_H

#include <map>

#include <babylon/babylon_api.h>
#include <babylon/core/variant.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/color_gradient.h>
#include <babylon/tools/factor_gradient.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class AbstractMesh;
class Animation;
class BaseTexture;
struct IParticleEmitterType;
class Mesh;
class Texture;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using AnimationPtr    = std::shared_ptr<Animation>;
using BaseTexturePtr  = std::shared_ptr<BaseTexture>;
using TexturePtr      = std::shared_ptr<Texture>;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

/**
 * @brief Interface representing a particle system in Babylon.js.
 * This groups the common functionalities that needs to be implemented in order
 * to create a particle system. A particle system represents a way to manage
 * particles from their emission to their animation and rendering.
 */
struct BABYLON_SHARED_EXPORT IParticleSystem : public IDisposable {

  IParticleSystem();
  virtual ~IParticleSystem();

  /**
   * List of animations used by the particle system.
   */
  std::vector<AnimationPtr> animations;

  /**
   * The id of the Particle system.
   */
  std::string id;

  /**
   * The name of the Particle system.
   */
  std::string name;

  /**
   * The emitter represents the Mesh or position we are attaching the particle
   * system to.
   */
  Variant<AbstractMeshPtr, Vector3> emitter;

  /**
   * @brief Gets or sets a boolean indicating if the particles must be rendered
   * as billboard or aligned with the direction.
   */
  virtual bool get_isBillboardBased() const = 0;

  /**
   * @brief Sets a boolean indicating if the particles must be rendered as
   * billboard or aligned with the direction.
   */
  virtual void set_isBillboardBased(bool value) = 0;

  /**
   * Gets or sets a boolean indicating if the particles must be rendered as
   * billboard or aligned with the direction
   */
  Property<IParticleSystem, bool> isBillboardBased;

  /**
   * The rendering group used by the Particle system to chose when to render.
   */
  unsigned int renderingGroupId = 0;

  /**
   * The layer mask we are rendering the particles through.
   */
  unsigned int layerMask = 0x0FFFFFFF;

  /**
   * The overall motion speed (0.01 is default update speed, faster updates =
   * faster animation)
   */
  float updateSpeed;

  /**
   * The amount of time the particle system is running (depends of the overall
   * update speed).
   */
  int targetStopDuration;

  /**
   * The texture used to render each particle. (this can be a spritesheet)
   */
  TexturePtr particleTexture = nullptr;

  /**
   * Blend mode use to render the particle, it can be either
   * ParticleSystem.BLENDMODE_ONEONE, ParticleSystem.BLENDMODE_STANDARD or
   * ParticleSystem.BLENDMODE_ADD.
   */
  unsigned int blendMode;

  /**
   * Minimum life time of emitting particles.
   */
  float minLifeTime;

  /**
   * Maximum life time of emitting particles.
   */
  float maxLifeTime;

  /**
   * Minimum Size of emitting particles.
   */
  float minSize;

  /**
   * Maximum Size of emitting particles.
   */
  float maxSize;

  /**
   * Minimum scale of emitting particles on X axis.
   */
  float minScaleX = 1.f;

  /**
   * Maximum scale of emitting particles on X axis.
   */
  float maxScaleX = 1.f;

  /**
   * Minimum scale of emitting particles on Y axis.
   */
  float minScaleY = 1.f;

  /**
   * Maximum scale of emitting particles on Y axis.
   */
  float maxScaleY = 1.f;

  /**
   * Random color of each particle after it has been emitted, between color1 and
   * color2 vectors.
   */
  Color4 color1;

  /**
   * Random color of each particle after it has been emitted, between color1 and
   * color2 vectors.
   */
  Color4 color2;

  /**
   * Color the particle will have at the end of its lifetime.
   */
  Color4 colorDead;

  /**
   * The maximum number of particles to emit per frame until we reach the
   * activeParticleCount value
   */
  int emitRate;

  /**
   * You can use gravity if you want to give an orientation to your particles.
   */
  Vector3 gravity;

  /**
   * Minimum power of emitting particles.
   */
  float minEmitPower;

  /**
   * Maximum power of emitting particles.
   */
  float maxEmitPower;

  /**
   * Minimum angular speed of emitting particles (Z-axis rotation for each
   * particle).
   */
  float minAngularSpeed;

  /**
   * Maximum angular speed of emitting particles (Z-axis rotation for each
   * particle).
   */
  float maxAngularSpeed;

  /**
   * Gets or sets the minimal initial rotation in radians.
   */
  float minInitialRotation = 0.f;

  /**
   * Gets or sets the maximal initial rotation in radians.
   */
  float maxInitialRotation = 0.f;

  /**
   * The particle emitter type defines the emitter used by the particle system.
   * It can be for example box, sphere, or cone...
   */
  std::shared_ptr<IParticleEmitterType> particleEmitterType;

  /**
   * Gets or sets a value indicating how many cycles (or frames) must be
   * executed before first rendering (this value has to be set before starting
   * the system). Default is 0
   */
  size_t preWarmCycles;

  /**
   * Gets or sets a value indicating the time step multiplier to use in pre-warm
   * mode (default is 1)
   */
  size_t preWarmStepOffset;

  /**
   * If using a spritesheet (isAnimationSheetEnabled) defines the speed of the
   * sprite loop (default is 1 meaning the animation will play once during the
   * entire particle lifetime)
   */
  float spriteCellChangeSpeed;

  /**
   * If using a spritesheet (isAnimationSheetEnabled) defines the first sprite
   * cell to display
   */
  unsigned int startSpriteCellID;

  /**
   * If using a spritesheet (isAnimationSheetEnabled) defines the last sprite
   * cell to display
   */
  unsigned int endSpriteCellID;

  /**
   * If using a spritesheet (isAnimationSheetEnabled), defines the sprite cell
   * width to use
   */
  unsigned int spriteCellWidth;

  /**
   * If using a spritesheet (isAnimationSheetEnabled), defines the sprite cell
   * height to use
   */
  unsigned int spriteCellHeight;

  /**
   * Gets or sets a Vector2 used to move the pivot (by default (0,0))
   */
  Vector2 translationPivot;

  /**
   *
   * Gets or sets a texture used to add random noise to particle positions
   */
  BaseTexturePtr noiseTexture = nullptr;

  /**
   * Gets or sets the strength to apply to the noise value (default is (10, 10,
   * 10))
   */
  Vector3 noiseStrength;

  /**
   * Gets or sets the billboard mode to use when isBillboardBased = true.
   * Only BABYLON.AbstractMesh.BILLBOARDMODE_ALL and
   * AbstractMesh.BILLBOARDMODE_Y are supported so far
   */
  unsigned int billboardMode;

  /**
   * Gets or sets a value indicating the damping to apply if the limit velocity
   * factor is reached
   */
  float limitVelocityDamping;

  /**
   * @brief Returns whether or not the particle system has an emitter.
   * @return Whether or not the particle system has an emitter
   */
  bool hasEmitter() const;

  /**
   * @brief Gets the maximum number of particles active at the same time.
   * @returns The max number of active particles.
   */
  virtual size_t getCapacity() const = 0;

  /**
   * @brief Gets if the particle system has been started.
   * @return true if the system has been started, otherwise false.
   */
  virtual bool isStarted() const = 0;

  /**
   * @brief Animates the particle system for this frame.
   * @param preWarmOnly will prevent the system from updating the vertex buffer
   * (default is false)
   */
  virtual void animate(bool preWarmOnly = false) = 0;

  /**
   * @brief Renders the particle system in its current state.
   * @returns the current number of particles.
   */
  virtual size_t render(bool preWarm = false) = 0;

  /**
   * @brief Dispose the particle system and frees its associated resources.
   * @param disposeTexture defines if the particule texture must be disposed as
   * well (true by default)
   */
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false)
    = 0;

  /**
   * @brief Clones the particle system.
   * @param name The name of the cloned object
   * @param newEmitter The new emitter to use
   * @returns the cloned particle system
   */
  virtual IParticleSystem* clone(const std::string& name, Mesh* newEmitter) = 0;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  virtual Json::object serialize() const = 0;

  /**
   * @brief Rebuild the particle system.
   */
  virtual void rebuild() = 0;

  /**
   * @brief Starts the particle system and begins to emit.
   * @param delay defines the delay in milliseconds before starting the system
   * (0 by default)
   */
  virtual void start(size_t delay = 0) = 0;

  /**
   * @brief Stops the particle system.
   */
  virtual void stop() = 0;

  /**
   * @brief Remove all active particles.
   */
  virtual void reset() = 0;

  /**
   * @brief Is this system ready to be used/rendered.
   * @return true if the system is ready
   */
  virtual bool isReady() = 0;

  /**
   * @brief Adds a new color gradient
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param color defines the color to affect to the specified gradient
   * @param color2 defines an additional color used to define a range ([color,
   * color2]) with main color to pick the final color from
   * @returns the current particle system
   */
  virtual IParticleSystem&
  addColorGradient(float gradient, const Color4& color1,
                   const std::optional<Color4>& color2 = std::nullopt)
    = 0;

  /**
   * @brief Remove a specific color gradient
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeColorGradient(float gradient) = 0;

  /**
   * @brief Adds a new size gradient
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the size factor to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  virtual IParticleSystem& addSizeGradient(float gradient, float factor,
                                           const std::optional<float>& factor2
                                           = std::nullopt)
    = 0;

  /**
   * @brief Remove a specific size gradient
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeSizeGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of color gradients.
   * You must use addColorGradient and removeColorGradient to udpate this list
   * @returns the list of color gradients
   */
  virtual std::vector<ColorGradient>& getColorGradients() = 0;

  /**
   * @brief Gets the current list of size gradients.
   * You must use addSizeGradient and removeSizeGradient to udpate this list
   * @returns the list of size gradients
   */
  virtual std::vector<FactorGradient>& getSizeGradients() = 0;

  /**
   * @brief Gets the current list of angular speed gradients.
   * You must use addAngularSpeedGradient and removeAngularSpeedGradient to
   * udpate this list
   * @returns the list of angular speed gradients
   */
  virtual std::vector<FactorGradient>& getAngularSpeedGradients() = 0;

  /**
   * @brief Adds a new angular speed gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the angular speed to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  virtual IParticleSystem&
  addAngularSpeedGradient(float gradient, float factor,
                          const std::optional<float>& factor2 = std::nullopt)
    = 0;

  /**
   * @brief Remove a specific angular speed gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeAngularSpeedGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of velocity gradients.
   * You must use addVelocityGradient and removeVelocityGradient to udpate this
   * list
   * @returns the list of velocity gradients
   */
  virtual std::vector<FactorGradient>& getVelocityGradients() = 0;

  /**
   * @brief Adds a new velocity gradient
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the velocity to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  virtual IParticleSystem&
  addVelocityGradient(float gradient, float factor,
                      const std::optional<float>& factor2 = std::nullopt)
    = 0;

  /**
   * @brief Remove a specific velocity gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeVelocityGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of limit velocity gradients.
   * You must use addLimitVelocityGradient and removeLimitVelocityGradient to
   * udpate this list
   * @returns the list of limit velocity gradients
   */
  virtual std::vector<FactorGradient>& getLimitVelocityGradients() = 0;

  /**
   * @brief Adds a new limit velocity gradient
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the limit velocity to affect to the specified
   * gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  virtual IParticleSystem&
  addLimitVelocityGradient(float gradient, float factor,
                           const std::optional<float>& factor2 = std::nullopt)
    = 0;

  /**
   * Remove a specific limit velocity gradient
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeLimitVelocityGradient(float gradient) = 0;

  /**
   * @brief Adds a new drag gradient
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the drag to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  virtual IParticleSystem& addDragGradient(float gradient, float factor,
                                           const std::optional<float>& factor2
                                           = std::nullopt)
    = 0;

  /**
   * @brief Remove a specific drag gradient
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeDragGradient(float gradient) = 0;

}; // end of struct IParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
