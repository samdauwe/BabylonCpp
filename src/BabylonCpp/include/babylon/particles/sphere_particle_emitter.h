#ifndef BABYLON_PARTICLES_SPHERE_PARTICLE_EMITTER_H
#define BABYLON_PARTICLES_SPHERE_PARTICLE_EMITTER_H

#include <babylon/babylon_global.h>
#include <babylon/particles/iparticle_emitter_Type.h>

namespace BABYLON {

/**
 * @brief Particle emitter emitting particles from the inside of a sphere.
 * It emits the particles alongside the sphere radius. The emission direction
 * might be randomized.
 */
class BABYLON_SHARED_EXPORT SphereParticleEmitter
    : public IParticleEmitterType {

public:
  /**
   * @brief Creates a new instance of @see SphereParticleEmitter
   * @param radius the radius of the emission sphere
   * @param directionRandomizer defines how much to randomize the particle
   * direction [0-1]
   */
  SphereParticleEmitter(float radius, float directionRandomizer = 0.f);
  virtual ~SphereParticleEmitter();

  /**
   * @brief Called by the particle System when the direction is computed for the
   * created particle.
   * @param emitPower is the power of the particle (speed)
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   */
  virtual void startDirectionFunction(float emitPower,
                                      const Matrix& worldMatrix,
                                      Vector3& directionToUpdate,
                                      Particle* particle) override;

  /**
   * @brief Called by the particle System when the position is computed for the
   * created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param positionToUpdate is the position vector to update with the result
   * @param particle is the particle we are computed the position for
   */
  void startPositionFunction(const Matrix& worldMatrix,
                             Vector3& positionToUpdate,
                             Particle* particle) override;

  /**
   * @brief Clones the current emitter and returns a copy of it
   * @returns the new emitter
   */
  virtual unique_ptr_t<IParticleEmitterType> clone() const override;

public:
  /**
   * The radius of the emission sphere.
   */
  float radius;
  /**
   * How much to randomize the particle direction [0-1].
   */
  float directionRandomizer;

}; // end of class SphereParticleEmitter

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_SPHERE_PARTICLE_EMITTER_H
