#ifndef BABYLON_PARTICLES_EMITTER_TYPES_HEMISPHERIC_PARTICLE_EMITTER_H
#define BABYLON_PARTICLES_EMITTER_TYPES_HEMISPHERIC_PARTICLE_EMITTER_H

#include <babylon/babylon_api.h>
#include <babylon/particles/emittertypes/iparticle_emitter_type.h>

namespace BABYLON {

/**
 * @brief Particle emitter emitting particles from the inside of a hemisphere.
 * It emits the particles alongside the hemisphere radius. The emission direction might be
 * randomized.
 */
class BABYLON_SHARED_EXPORT HemisphericParticleEmitter : public IParticleEmitterType {

public:
  /**
   * @brief Creates a new instance HemisphericParticleEmitter
   * @param radius the radius of the emission hemisphere (1 by default)
   * @param radiusRange the range of the emission hemisphere [0-1] 0 Surface only, 1 Entire Radius
   * (1 by default)
   * @param directionRandomizer defines how much to randomize the particle direction [0-1]
   */
  HemisphericParticleEmitter(float radius = 1.f, float radiusRange = 1.f,
                             float directionRandomizer = 0.f);
  ~HemisphericParticleEmitter() override; // = default

  /**
   * @brief Called by the particle System when the direction is computed for the created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   * @param isLocal defines if the direction should be set in local space
   */
  void startDirectionFunction(const Matrix& worldMatrix, Vector3& directionToUpdate,
                              Particle* particle, bool isLocal) override;

  /**
   * @brief Called by the particle System when the position is computed for the created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param positionToUpdate is the position vector to update with the result
   * @param particle is the particle we are computed the position for
   * @param isLocal defines if the position should be set in local space
   */
  void startPositionFunction(const Matrix& worldMatrix, Vector3& positionToUpdate,
                             Particle* particle, bool isLocal) override;

  /**
   * @brief Clones the current emitter and returns a copy of it.
   * @returns the new emitter
   */
  [[nodiscard]] std::unique_ptr<IParticleEmitterType> clone() const override;

  /**
   * @brief Called by the GPUParticleSystem to setup the update shader.
   * @param effect defines the update shader
   */
  void applyToShader(Effect* effect) override;

  /**
   * @brief Returns a string to use to update the GPU particles update shader.
   * @returns a string containing the defines string
   */
  [[nodiscard]] std::string getEffectDefines() const override;

  /**
   * @brief Returns the string "HemisphericParticleEmitter".
   * @returns a string containing the class name
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  [[nodiscard]] json serialize() const override;

  /**
   * @brief Parse properties from a JSON object.
   * @param serializationObject defines the JSON object
   * @param scene defines the hosting scene
   */
  void parse(const json& serializationObject, Scene* scene) override;

public:
  /**
   * The radius of the emission hemisphere.
   */
  float radius;

  /**
   * The range of emission [0-1] 0 Surface only, 1 Entire Radius.
   */
  float radiusRange;

  /**
   * How much to randomize the particle direction [0-1].
   */
  float directionRandomizer;

}; // end of class HemisphericParticleEmitter

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_EMITTER_TYPES_HEMISPHERIC_PARTICLE_EMITTER_H
