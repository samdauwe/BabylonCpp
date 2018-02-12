#ifndef BABYLON_PARTICLES_IPARTICLE_EMITTER_TYPE_H
#define BABYLON_PARTICLES_IPARTICLE_EMITTER_TYPE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Particle emitter represents a volume emitting particles.
 * This is the responsibility of the implementation to define the volume shape
 * like cone/sphere/box.
 */
struct BABYLON_SHARED_EXPORT IParticleEmitterType {

  /**
   * @brief Called by the particle System when the direction is computed for the
   * created particle.
   * @param emitPower is the power of the particle (speed)
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   */
  virtual void
  startDirectionFunction(float emitPower, const Matrix& worldMatrix,
                         Vector3& directionToUpdate, Particle* particle)
    = 0;

  /**
   * @brief Called by the particle System when the position is computed for the
   * created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param positionToUpdate is the position vector to update with the result
   * @param particle is the particle we are computed the position for
   */
  virtual void startPositionFunction(const Matrix& worldMatrix,
                                     Vector3& positionToUpdate,
                                     Particle* particle)
    = 0;

  /**
   * @brief Clones the current emitter and returns a copy of it
   * @returns the new emitter
   */
  virtual unique_ptr_t<IParticleEmitterType> clone() const = 0;

}; // end of struct IParticleEmitterType

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_IPARTICLE_EMITTER_TYPE_H
