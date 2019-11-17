#ifndef BABYLON_PARTICLES_EMITTER_TYPES_BOX_PARTICLE_EMITTER_H
#define BABYLON_PARTICLES_EMITTER_TYPES_BOX_PARTICLE_EMITTER_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/emittertypes/iparticle_emitter_Type.h>

namespace BABYLON {

/**
 * @brief Particle emitter emitting particles from the inside of a box.
 * It emits the particles randomly between 2 given directions.
 */
class BABYLON_SHARED_EXPORT BoxParticleEmitter : public IParticleEmitterType {

public:
  /**
   * @brief Creates a new instance BoxParticleEmitter.
   */
  BoxParticleEmitter();
  ~BoxParticleEmitter() override; // = default

  /**
   * @brief Called by the particle System when the direction is computed for the
   * created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   */
  void startDirectionFunction(const Matrix& worldMatrix, Vector3& directionToUpdate,
                              Particle* particle) override;

  /**
   * @brief Called by the particle System when the position is computed for the
   * created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param positionToUpdate is the position vector to update with the result
   * @param particle is the particle we are computed the position for
   */
  void startPositionFunction(const Matrix& worldMatrix, Vector3& positionToUpdate,
                             Particle* particle) override;

  /**
   * @brief Clones the current emitter and returns a copy of it
   * @returns the new emitter
   */
  std::unique_ptr<IParticleEmitterType> clone() const override;

  /**
   * @brief Called by the GPUParticleSystem to setup the update shader.
   * @param effect defines the update shader
   */
  void applyToShader(Effect* effect) override;

  /**
   * @brief Returns a string to use to update the GPU particles update shader
   * @returns a string containng the defines string
   */
  const std::string getEffectDefines() const override;

  /**
   * @brief Returns the string "BoxParticleEmitter"
   * @returns a string containing the class name
   */
  const std::string getClassName() const override;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  json serialize() const override;

  /**
   * @brief Parse properties from a JSON object
   * @param serializationObject defines the JSON object
   */
  void parse(const json& serializationObject) override;

public:
  /**
   * Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors.
   */
  Vector3 direction1;
  /**
   * Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors.
   */
  Vector3 direction2;

  /**
   * Minimum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so.
   */
  Vector3 minEmitBox;
  /**
   * Maximum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so.
   */
  Vector3 maxEmitBox;

}; // end of class BoxParticleEmitter

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_EMITTER_TYPES_BOX_PARTICLE_EMITTER_H
