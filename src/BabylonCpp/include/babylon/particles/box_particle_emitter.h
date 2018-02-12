#ifndef BABYLON_PARTICLES_BOX_PARTICLE_EMITTER_H
#define BABYLON_PARTICLES_BOX_PARTICLE_EMITTER_H

#include <babylon/babylon_global.h>
#include <babylon/particles/iparticle_emitter_Type.h>

namespace BABYLON {

/**
 * @brief Particle emitter emitting particles from the inside of a box.
 * It emits the particles randomly between 2 given directions.
 */
class BABYLON_SHARED_EXPORT BoxParticleEmitter : public IParticleEmitterType {

public:
  /**
   * @brief Creates a new instance of @see BoxParticleEmitter
   * @param _particleSystem the particle system associated with the emitter
   */
  BoxParticleEmitter(ParticleSystem* particleSystem);
  virtual ~BoxParticleEmitter();

  /**
   * @brief Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors.
   */
  Vector3& direction1();

  /**
   * @brief Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors.
   */
  void setDirection1(const Vector3& value);

  /**
   * @brief Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors.
   */
  Vector3& direction2();

  /**
   * @brief Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors.
   */
  void setDirection2(const Vector3& value);

  /**
   * @brief Minimum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so.
   */
  Vector3& minEmitBox();

  /**
   * @brief Minimum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so.
   */
  void setMinEmitBox(const Vector3& value);

  /**
   * @brief Maximum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so.
   */
  Vector3& maxEmitBox();

  /**
   * @brief Maximum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so.
   */
  void setMaxEmitBox(const Vector3& value);

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

private:
  ParticleSystem* _particleSystem;

}; // end of class BoxParticleEmitter

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_BOX_PARTICLE_EMITTER_H
