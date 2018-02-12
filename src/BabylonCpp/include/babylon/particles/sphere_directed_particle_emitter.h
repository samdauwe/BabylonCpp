#ifndef BABYLON_PARTICLES_SPHERE_DIRECTED_PARTICLE_EMITTER_H
#define BABYLON_PARTICLES_SPHERE_DIRECTED_PARTICLE_EMITTER_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/sphere_particle_emitter.h>

namespace BABYLON {

/**
 * @brief Particle emitter emitting particles from the inside of a sphere.
 * It emits the particles randomly between two vectors.
 */
class BABYLON_SHARED_EXPORT SphereDirectedParticleEmitter
    : public SphereParticleEmitter {

public:
  /**
   * @brief Creates a new instance of @see SphereDirectedParticleEmitter
   * @param radius the radius of the emission sphere
   * @param direction1 the min limit of the emission direction
   * @param direction2 the max limit of the emission direction
   */
  SphereDirectedParticleEmitter(float radius, const Vector3& direction1,
                                const Vector3& direction2);
  ~SphereDirectedParticleEmitter() override;

  /**
   * @brief Called by the particle System when the direction is computed for the
   * created particle.
   * @param emitPower is the power of the particle (speed)
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   */
  void startDirectionFunction(float emitPower, const Matrix& worldMatrix,
                              Vector3& directionToUpdate,
                              Particle* particle) override;

  /**
   * @brief Clones the current emitter and returns a copy of it
   * @returns the new emitter
   */
  unique_ptr_t<IParticleEmitterType> clone() const override;

public:
  /**
   * The min limit of the emission direction.
   */
  Vector3 direction1;
  /**
   * The max limit of the emission direction.
   */
  Vector3 direction2;

}; // end of class SphereDirectedParticleEmitter

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_SPHERE_DIRECTED_PARTICLE_EMITTER_H
