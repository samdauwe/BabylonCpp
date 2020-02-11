#ifndef BABYLON_PARTICLES_EMITTER_TYPES_CUSTOM_PARTICLE_EMITTER_H
#define BABYLON_PARTICLES_EMITTER_TYPES_CUSTOM_PARTICLE_EMITTER_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/particles/emittertypes/iparticle_emitter_Type.h>

namespace BABYLON {

/**
 * @brief Particle emitter emitting particles from a custom list of positions.
 */
class BABYLON_SHARED_EXPORT CustomParticleEmitter : public IParticleEmitterType {

public:
  /**
   * @brief Creates a new instance CustomParticleEmitter.
   */
  CustomParticleEmitter();
  ~CustomParticleEmitter() override; // = default

  /**
   * @brief Called by the particle System when the direction is computed for the created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   */
  void startDirectionFunction(const Matrix& worldMatrix, Vector3& directionToUpdate,
                              Particle* particle) override;

  /**
   * @brief Called by the particle System when the position is computed for the created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param positionToUpdate is the position vector to update with the result
   * @param particle is the particle we are computed the position for
   */
  void startPositionFunction(const Matrix& worldMatrix, Vector3& positionToUpdate,
                             Particle* particle) override;

  /**
   * @brief Clones the current emitter and returns a copy of it.
   * @returns the new emitter
   */
  std::unique_ptr<IParticleEmitterType> clone() const override;

  /**
   * @brief Called by the GPUParticleSystem to setup the update shader
   * @param effect defines the update shader
   */
  void applyToShader(Effect* effect) override;

  /**
   * @brief Returns a string to use to update the GPU particles update shader.
   * @returns a string containng the defines string
   */
  std::string getEffectDefines() const override;

  /**
   * @brief Returns the string "ConeParticleEmitter".
   * @returns a string containing the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  json serialize() const override;

  /**
   * @brief Parse properties from a JSON object.
   * @param serializationObject defines the JSON object
   */
  void parse(const json& serializationObject) override;

public:
  /**
   * Gets or sets the position generator that will create the inital position of each particle.
   * Index will be provided when used with GPU particle. Particle will be provided when used with
   * CPU particles
   */
  std::function<void(int index, Particle* particle, Vector3& outPosition)>
    particlePositionGenerator;

  /**
   * Gets or sets the destination generator that will create the final destination of each particle.
   *  * Index will be provided when used with GPU particle. Particle will be provided when used with
   * CPU particles
   */
  std::function<void(int index, Particle* particle, Vector3& outDestination)>
    particleDestinationGenerator;

}; // end of class CustomParticleEmitter

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_EMITTER_TYPES_CUSTOM_PARTICLE_EMITTER_H
