#ifndef BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_IPARTICLE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/core/variant.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Interface representing a particle system in Babylon.
 * This groups the common functionalities that needs to be implemented in order
 * to create a particle system. A particle system represents a way to manage
 * particles (@see Particle) from their emission to their animation and
 * rendering.
 */
struct BABYLON_SHARED_EXPORT IParticleSystem : public IDisposable {
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
   * @brief Gets if the particle system has an emitter.
   * @returns true if the particle system has an emitter, otherwise false.
   */
  bool hasEmitter();
  /**
   * Gets if the particle system has been started.
   * @return true if the system has been started, otherwise false.
   */
  virtual bool isStarted() const = 0;
  /**
   * Animates the particle system for this frame.
   */
  virtual void animate() = 0;
  /**
   * @brief Renders the particle system in its current state.
   * @returns the current number of particles.
   */
  virtual size_t render() = 0;
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
   * @brief Rebuild the particle system
   */
  virtual void rebuild() = 0;
}; // end of struct IParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
