#ifndef BABYLON_PARTICLES_EMITTER_TYPES_IPARTICLE_EMITTER_TYPE_H
#define BABYLON_PARTICLES_EMITTER_TYPES_IPARTICLE_EMITTER_TYPE_H

#include <map>
#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class Effect;
class Matrix;
class Particle;
class Vector3;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

/**
 * @brief Particle emitter represents a volume emitting particles.
 * This is the responsibility of the implementation to define the volume shape
 * like cone/sphere/box.
 */
struct BABYLON_SHARED_EXPORT IParticleEmitterType {

  /**
   * @brief Called by the particle System when the direction is computed for the
   * created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   */
  virtual void startDirectionFunction(const Matrix& worldMatrix,
                                      Vector3& directionToUpdate,
                                      Particle* particle)
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
   * @brief Clones the current emitter and returns a copy of it.
   * @returns the new emitter
   */
  virtual std::unique_ptr<IParticleEmitterType> clone() const = 0;

  /**
   * @brief Called by the {BABYLON.GPUParticleSystem} to setup the update shader
   * @param effect defines the update shader
   */
  virtual void applyToShader(Effect* effect) = 0;

  /**
   * @brief Returns a string to use to update the GPU particles update shader
   * @returns a string containng the defines string
   */
  virtual const char* getEffectDefines() const = 0;

  /**
   * @brief Returns the string "SphereDirectedParticleEmitter"
   * @returns a string containing the class name
   */
  virtual const char* getClassName() const = 0;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  virtual Json::object serialize() const = 0;

  /**
   * @brief Parse properties from a JSON object
   * @param serializationObject defines the JSON object
   */
  virtual void parse(const Json::value& serializationObject) = 0;

}; // end of struct IParticleEmitterType

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_EMITTER_TYPES_IPARTICLE_EMITTER_TYPE_H
