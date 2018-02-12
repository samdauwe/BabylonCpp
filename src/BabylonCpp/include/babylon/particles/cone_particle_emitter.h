#ifndef BABYLON_PARTICLES_CONE_PARTICLE_EMITTER_H
#define BABYLON_PARTICLES_CONE_PARTICLE_EMITTER_H

#include <babylon/babylon_global.h>
#include <babylon/particles/iparticle_emitter_Type.h>

namespace BABYLON {

/**
 * @brief Particle emitter emitting particles from the inside of a cone.
 * It emits the particles alongside the cone volume from the base to the
 * particle. The emission direction might be randomized.
 */
class BABYLON_SHARED_EXPORT ConeParticleEmitter : public IParticleEmitterType {

public:
  /**
   * @brief Creates a new instance of @see ConeParticleEmitter
   * @param radius the radius of the emission cone
   * @param angles the cone base angle
   * @param directionRandomizer defines how much to randomize the particle
   * direction [0-1]
   */
  ConeParticleEmitter(float radius, float angle,
                      float directionRandomizer = 0.f);
  virtual ~ConeParticleEmitter();

  /**
   * @brief Gets the radius of the emission cone.
   */
  float radius() const;

  /**
   * @brief Sets the radius of the emission cone.
   */
  void setRadius(float value);

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
  unique_ptr_t<IParticleEmitterType> clone() const override;

public:
  /**
   * The radius of the emission cone.
   */
  float angle;
  /**
   * The cone base angle.
   */
  float directionRandomizer;

private:
  float _radius;
  float _height;

}; // end of class ConeParticleEmitter

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_CONE_PARTICLE_EMITTER_H
