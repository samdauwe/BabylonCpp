#ifndef BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_IPARTICLE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/core/variant.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Interface representing a particle system in Babylon.
 * This groups the common functionalities that needs to be implemented in order
 * to create a particle system. A particle system represents a way to manage
 * particles from their emission to their animation and rendering.
 */
struct BABYLON_SHARED_EXPORT IParticleSystem : public IDisposable {

  virtual ~IParticleSystem();

  /**
   * The id of the Particle system.
   */
  string_t id;

  /**
   * The name of the Particle system.
   */
  string_t name;

  /**
   * The emitter represents the Mesh or position we are attaching the particle
   * system to.
   */
  Variant<AbstractMesh*, Vector3> emitter;

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
  Texture* particleTexture = nullptr;

  /**
   * Blend mode use to render the particle, it can be either
   * ParticleSystem.BLENDMODE_ONEONE or ParticleSystem.BLENDMODE_STANDARD.
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
   * The particle emitter type defines the emitter used by the particle system.
   * It can be for example box, sphere, or cone...
   */
  unique_ptr_t<IParticleEmitterType> particleEmitterType;

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
   */
  virtual void animate() = 0;

  /**
   * @brief Renders the particle system in its current state.
   * @returns the current number of particles.
   */
  virtual size_t render() = 0;

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
  virtual IParticleSystem* clone(const string_t& name, Mesh* newEmitter) = 0;

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
   */
  virtual void start() = 0;

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

}; // end of struct IParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
