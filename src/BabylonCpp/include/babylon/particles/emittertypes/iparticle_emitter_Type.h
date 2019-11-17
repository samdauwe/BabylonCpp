#ifndef BABYLON_PARTICLES_EMITTER_TYPES_IPARTICLE_EMITTER_TYPE_H
#define BABYLON_PARTICLES_EMITTER_TYPES_IPARTICLE_EMITTER_TYPE_H

#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vector>

#include <babylon/babylon_api.h>

using json = nlohmann::json;

namespace BABYLON {

class Effect;
class Matrix;
class Particle;
class Vector3;

/**
 * @brief Particle emitter represents a volume emitting particles.
 * This is the responsibility of the implementation to define the volume shape
 * like cone/sphere/box.
 */
struct BABYLON_SHARED_EXPORT IParticleEmitterType {
  virtual ~IParticleEmitterType() = default;
  /**
   * @brief Called by the particle System when the direction is computed for the
   * created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   */
  virtual void startDirectionFunction(const Matrix& worldMatrix, Vector3& directionToUpdate,
                                      Particle* particle)
    = 0;

  /**
   * @brief Called by the particle System when the position is computed for the
   * created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param positionToUpdate is the position vector to update with the result
   * @param particle is the particle we are computed the position for
   */
  virtual void startPositionFunction(const Matrix& worldMatrix, Vector3& positionToUpdate,
                                     Particle* particle)
    = 0;

  /**
   * @brief Clones the current emitter and returns a copy of it.
   * @returns the new emitter
   */
  [[nodiscard]] virtual std::unique_ptr<IParticleEmitterType> clone() const = 0;

  /**
   * @brief Called by the GPUParticleSystem to setup the update shader.
   * @param effect defines the update shader
   */
  virtual void applyToShader(Effect* effect) = 0;

  /**
   * @brief Returns a string to use to update the GPU particles update shader.
   * @returns a string containng the defines string
   */
  [[nodiscard]] virtual std::string getEffectDefines() const = 0;

  /**
   * @brief Returns the string "SphereDirectedParticleEmitter".
   * @returns a string containing the class name
   */
  [[nodiscard]] virtual std::string getClassName() const = 0;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  [[nodiscard]] virtual json serialize() const = 0;

  /**
   * @brief Parse properties from a JSON object.
   * @param serializationObject defines the JSON object
   */
  virtual void parse(const json& serializationObject) = 0;

}; // end of struct IParticleEmitterType

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_EMITTER_TYPES_IPARTICLE_EMITTER_TYPE_H
