#ifndef BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_IPARTICLE_SYSTEM_H

#include <map>
#include <nlohmann/json_fwd.hpp>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/misc/color_gradient.h>
#include <babylon/misc/factor_gradient.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class Animation;
class BaseTexture;
class BoxParticleEmitter;
struct Color3Gradient;
class ConeParticleEmitter;
class CylinderDirectedParticleEmitter;
class CylinderParticleEmitter;
class HemisphericParticleEmitter;
struct IParticleEmitterType;
class Mesh;
class PointParticleEmitter;
class ProceduralTexture;
class SphereDirectedParticleEmitter;
class SphereParticleEmitter;
class Scene;
class Texture;
using AbstractMeshPtr        = std::shared_ptr<AbstractMesh>;
using AnimationPtr           = std::shared_ptr<Animation>;
using BaseTexturePtr         = std::shared_ptr<BaseTexture>;
using BoxParticleEmitterPtr  = std::shared_ptr<BoxParticleEmitter>;
using ConeParticleEmitterPtr = std::shared_ptr<ConeParticleEmitter>;
using CylinderDirectedParticleEmitterPtr
  = std::shared_ptr<CylinderDirectedParticleEmitter>;
using CylinderParticleEmitterPtr = std::shared_ptr<CylinderParticleEmitter>;
using HemisphericParticleEmitterPtr
  = std::shared_ptr<HemisphericParticleEmitter>;
using PointParticleEmitterPtr = std::shared_ptr<PointParticleEmitter>;
using ProceduralTexturePtr    = std::shared_ptr<ProceduralTexture>;
using SphereDirectedParticleEmitterPtr
  = std::shared_ptr<SphereDirectedParticleEmitter>;
using SphereParticleEmitterPtr = std::shared_ptr<SphereParticleEmitter>;
using TexturePtr               = std::shared_ptr<Texture>;

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
  std::variant<AbstractMeshPtr, Vector3> emitter;

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
   * Defines the delay in milliseconds before starting the system (0 by default)
   */
  int startDelay = 0;

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
   * This allows the system to random pick the start cell ID between
   * startSpriteCellID and endSpriteCellID
   */
  bool spriteRandomStartCell;

  /**
   * @brief Gets whether an animation sprite sheet is enabled or not on the
   * particle system.
   */
  virtual bool get_isAnimationSheetEnabled() const = 0;

  /**
   * @brief Sets whether an animation sprite sheet is enabled or not on the
   * particle system.
   */
  virtual void set_isAnimationSheetEnabled(bool value) = 0;

  /**
   * Gets or sets a boolean indicating if a spritesheet is used to animate the
   * particles texture
   */
  Property<IParticleSystem, bool> isAnimationSheetEnabled;

  /**
   * Gets or sets a Vector2 used to move the pivot (by default (0,0))
   */
  Vector2 translationPivot;

  /**
   *
   * Gets or sets a texture used to add random noise to particle positions
   */
  ProceduralTexturePtr noiseTexture = nullptr;

  /**
   * Gets or sets the strength to apply to the noise value (default is (10, 10,
   * 10))
   */
  Vector3 noiseStrength;

  /**
   * Gets or sets the billboard mode to use when isBillboardBased = true.
   * Value can be: ParticleSystem.BILLBOARDMODE_ALL,
   * ParticleSystem.BILLBOARDMODE_Y, ParticleSystem.BILLBOARDMODE_STRETCHED
   */
  unsigned int billboardMode;

  /**
   * Gets or sets a value indicating the damping to apply if the limit velocity
   * factor is reached
   */
  float limitVelocityDamping;

  /**
   * Gets or sets a boolean indicating that hosted animations (in the
   * system.animations array) must be started when system.start() is called
   */
  bool beginAnimationOnStart;

  /**
   * Gets or sets the frame to start the animation from when
   * beginAnimationOnStart is true
   */
  int beginAnimationFrom;

  /**
   * Gets or sets the frame to end the animation on when beginAnimationOnStart
   * is true
   */
  int beginAnimationTo;

  /**
   * Gets or sets a boolean indicating if animations must loop when
   * beginAnimationOnStart is true
   */
  bool beginAnimationLoop;

  /**
   * Specifies whether the particle system will be disposed once it reaches the
   * end of the animation.
   */
  bool disposeOnStop;

  /**
   * @brief Returns whether or not the particle system has an emitter.
   * @return Whether or not the particle system has an emitter
   */
  virtual bool hasEmitter() const = 0;

  /**
   * @brief Gets the maximum number of particles active at the same time.
   * @returns The max number of active particles.
   */
  virtual size_t getCapacity() const = 0;

  /**
   * @brief Gets if the system has been started. (Note: this will still be true
   * after stop is called).
   * @returns True if it has been started, otherwise false.
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
   * @param preWarm defines if the system should only update the particles but
   * not render them
   * @returns the current number of particles
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
  virtual json serialize() const = 0;

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
   * @brief Adds a new color gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param color1 defines the color to affect to the specified gradient
   * @param color2 defines an additional color used to define a range ([color,
   * color2]) with main color to pick the final color from
   * @returns the current particle system
   */
  virtual IParticleSystem&
  addColorGradient(float gradient, const Color4& color1,
                   const std::optional<Color4>& color2 = std::nullopt)
    = 0;

  /**
   * @brief Remove a specific color gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeColorGradient(float gradient) = 0;

  /**
   * @brief Adds a new size gradient.
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
   * @brief Remove a specific size gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeSizeGradient(float gradient) = 0;

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
   * @brief Adds a new velocity gradient.
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
   * @brief Adds a new limit velocity gradient.
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
   * @brief Remove a specific limit velocity gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeLimitVelocityGradient(float gradient) = 0;

  /**
   * @brief Adds a new drag gradient.
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
   * @brief Remove a specific drag gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeDragGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of drag gradients.
   * You must use addDragGradient and removeDragGradient to udpate this list
   * @returns the list of drag gradients
   */
  virtual std::vector<FactorGradient>& getDragGradients() = 0;

  /**
   * @brief Adds a new emit rate gradient (please note that this will only work
   * if you set the targetStopDuration property).
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the emit rate to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  virtual IParticleSystem&
  addEmitRateGradient(float gradient, float factor,
                      const std::optional<float>& factor2 = std::nullopt)
    = 0;

  /**
   * @brief Remove a specific emit rate gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeEmitRateGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of emit rate gradients.
   * You must use addEmitRateGradient and removeEmitRateGradient to udpate this
   * list
   * @returns the list of emit rate gradients
   */
  virtual std::vector<FactorGradient>& getEmitRateGradients() = 0;

  /**
   * @brief Adds a new start size gradient (please note that this will only work
   * if you set the targetStopDuration property).
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the start size to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  virtual IParticleSystem&
  addStartSizeGradient(float gradient, float factor,
                       const std::optional<float>& factor2 = std::nullopt)
    = 0;

  /**
   * @brief Remove a specific start size gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeStartSizeGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of start size gradients.
   * You must use addStartSizeGradient and removeStartSizeGradient to udpate
   * this list
   * @returns the list of start size gradients
   */
  virtual std::vector<FactorGradient>& getStartSizeGradients() = 0;

  /**
   * @brief Adds a new life time gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the life time factor to affect to the specified
   * gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  virtual IParticleSystem&
  addLifeTimeGradient(float gradient, float factor,
                      const std::optional<float>& factor2 = std::nullopt)
    = 0;

  /**
   * @brief Remove a specific life time gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeLifeTimeGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of life time gradients.
   * You must use addLifeTimeGradient and removeLifeTimeGradient to udpate this
   * list
   * @returns the list of life time gradients
   */
  virtual std::vector<FactorGradient>& getLifeTimeGradients() = 0;

  /**
   * @brief Gets the current list of color gradients.
   * You must use addColorGradient and removeColorGradient to udpate this list
   * @returns the list of color gradients
   */
  virtual std::vector<ColorGradient>& getColorGradients() = 0;

  /**
   * @brief Adds a new ramp gradient used to remap particle colors.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param color defines the color to affect to the specified gradient
   * @returns the current particle system
   */
  virtual IParticleSystem& addRampGradient(float gradient, const Color3& color)
    = 0;

  /**
   * @brief Removes a ramp gradient used to remap particle colors.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeRampGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of ramp gradients.
   * You must use addRampGradient and removeRampGradient to udpate this list
   * @returns the list of ramp gradients
   */
  virtual std::vector<Color3Gradient>& getRampGradients() = 0;

  /**
   * @brief Not supported by GPUParticleSystem.
   * Gets a boolean indicating that ramp gradients must be used
   * @see http://doc.babylonjs.com/babylon101/particles#ramp-gradients
   */
  virtual bool get_useRampGradients() const = 0;

  /**
   * Not supported by GPUParticleSystem
   * Sets a boolean indicating that ramp gradients must be used
   * @see http://doc.babylonjs.com/babylon101/particles#ramp-gradients
   */
  virtual void set_useRampGradients(bool value) = 0;

  /**
   * @brief Gets or sets a boolean indicating that ramp gradients must be used.
   * @see http://doc.babylonjs.com/babylon101/particles#ramp-gradients
   */
  Property<IParticleSystem, bool> useRampGradients;

  /**
   * @brief Adds a new color remap gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param min defines the color remap minimal range
   * @param max defines the color remap maximal range
   * @returns the current particle system
   */
  virtual IParticleSystem& addColorRemapGradient(float gradient, float min,
                                                 float max)
    = 0;

  /**
   * @brief Removes a color remap gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeColorRemapGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of color remap gradients.
   * You must use addColorRemapGradient and removeColorRemapGradient to udpate
   * this list
   * @returns the list of color remap gradients
   */
  virtual std::vector<FactorGradient>& getColorRemapGradients() = 0;

  /**
   * @brief Adds a new alpha remap gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param min defines the alpha remap minimal range
   * @param max defines the alpha remap maximal range
   * @returns the current particle system
   */
  virtual IParticleSystem& addAlphaRemapGradient(float gradient, float min,
                                                 float max)
    = 0;

  /**
   * @brief Removes a alpha remap gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  virtual IParticleSystem& removeAlphaRemapGradient(float gradient) = 0;

  /**
   * @brief Gets the current list of alpha remap gradients.
   * You must use addAlphaRemapGradient and removeAlphaRemapGradient to udpate
   * this list
   * @returns the list of alpha remap gradients
   */
  virtual std::vector<FactorGradient>& getAlphaRemapGradients() = 0;

  /**
   * @brief Creates a Point Emitter for the particle system (emits directly from
   * the emitter position).
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @returns the emitter
   */
  virtual PointParticleEmitterPtr createPointEmitter(const Vector3& direction1,
                                                     const Vector3& direction2)
    = 0;

  /**
   * @brief Creates a Hemisphere Emitter for the particle system (emits along
   * the hemisphere radius).
   * @param radius The radius of the hemisphere to emit from
   * @param radiusRange The range of the hemisphere to emit from [0-1] 0 Surface
   * Only, 1 Entire Radius
   * @returns the emitter
   */
  virtual HemisphericParticleEmitterPtr
  createHemisphericEmitter(float radius = 1.f, float radiusRange = 1.f)
    = 0;

  /**
   * @brief Creates a Sphere Emitter for the particle system (emits along the
   * sphere radius).
   * @param radius The radius of the sphere to emit from
   * @param radiusRange The range of the sphere to emit from [0-1] 0 Surface
   * Only, 1 Entire Radius
   * @returns the emitter
   */
  virtual SphereParticleEmitterPtr createSphereEmitter(float radius      = 1.f,
                                                       float radiusRange = 1.f)
    = 0;

  /**
   * @brief Creates a Directed Sphere Emitter for the particle system (emits
   * between direction1 and direction2).
   * @param radius The radius of the sphere to emit from
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the sphere
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the sphere
   * @returns the emitter
   */
  virtual SphereDirectedParticleEmitterPtr createDirectedSphereEmitter(
    float radius = 1.f, const Vector3& direction1 = Vector3(0.f, 1.f, 0.f),
    const Vector3& direction2 = Vector3(0.f, 1.f, 0.f))
    = 0;

  /**
   * @brief Creates a Cylinder Emitter for the particle system (emits from the
   * cylinder to the particle position).
   * @param radius The radius of the emission cylinder
   * @param height The height of the emission cylinder
   * @param radiusRange The range of emission [0-1] 0 Surface only, 1 Entire
   * Radius
   * @param directionRandomizer How much to randomize the particle direction
   * [0-1]
   * @returns the emitter
   */
  virtual CylinderParticleEmitterPtr
  createCylinderEmitter(float radius = 1.f, float height = 1.f,
                        float radiusRange         = 1.f,
                        float directionRandomizer = 0.f)
    = 0;

  /**
   * @brief Creates a Directed Cylinder Emitter for the particle system (emits
   * between direction1 and direction2).
   * @param radius The radius of the cylinder to emit from
   * @param height The height of the emission cylinder
   * @param radiusRange the range of the emission cylinder [0-1] 0 Surface only,
   * 1 Entire Radius (1 by default)
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the cylinder
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the cylinder
   * @returns the emitter
   */
  virtual CylinderDirectedParticleEmitterPtr createDirectedCylinderEmitter(
    float radius = 1.f, float height = 1.f, float radiusRange = 1.f,
    const Vector3& direction1 = Vector3(0.f, 1.f, 0.f),
    const Vector3& direction2 = Vector3(0.f, 1.f, 0.f))
    = 0;

  /**
   * @brief Creates a Cone Emitter for the particle system (emits from the cone
   * to the particle position).
   * @param radius The radius of the cone to emit from
   * @param angle The base angle of the cone
   * @returns the emitter
   */
  virtual ConeParticleEmitterPtr createConeEmitter(float radius = 1.f,
                                                   float angle  = Math::PI_4)
    = 0;

  /**
   * @brief Creates a Box Emitter for the particle system. (emits between
   * direction1 and direction2 from withing the box defined by minEmitBox and
   * maxEmitBox).
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @param minEmitBox Particles are emitted from the box between minEmitBox and
   * maxEmitBox
   * @param maxEmitBox  Particles are emitted from the box between minEmitBox
   * and maxEmitBox
   * @returns the emitter
   */
  virtual BoxParticleEmitterPtr
  createBoxEmitter(const Vector3& direction1, const Vector3& direction2,
                   const Vector3& minEmitBox, const Vector3& maxEmitBox)
    = 0;
  /**
   * @brief Get hosting scene.
   * @returns the scene
   */
  virtual Scene* getScene() const = 0;

}; // end of struct IParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
