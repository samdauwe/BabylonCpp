#ifndef BABYLON_PARTICLES_SUB_EMITTER_H
#define BABYLON_PARTICLES_SUB_EMITTER_H

#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vector>

#include <babylon/babylon_api.h>

using json = nlohmann::json;

namespace BABYLON {

class ParticleSystem;
class Scene;
class SubEmitter;
using ParticleSystemPtr = std::shared_ptr<ParticleSystem>;
using SubEmitterPtr     = std::shared_ptr<SubEmitter>;

/**
 * @brief Type of sub emitter.
 */
enum class SubEmitterType {
  /**
   * Attached to the particle over it's lifetime
   */
  ATTACHED,
  /**
   * Created when the particle dies
   */
  END
}; // end of enum class SubEmitterType

/**
 * @brief Sub emitter class used to emit particles from an existing particle.
 */
class BABYLON_SHARED_EXPORT SubEmitter {

public:
  /**
   * @brief Creates a sub emitter.
   * @param particleSystem the particle system to be used by the sub emitter
   */
  SubEmitter(const ParticleSystemPtr& particleSystem);
  ~SubEmitter(); // = default

  /**
   * @brief Clones the sub emitter.
   * @returns the cloned sub emitter
   */
  [[nodiscard]] SubEmitterPtr clone() const;

  /**
   * @brief Serialize current object to a JSON object.
   * @returns the serialized object
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Creates a new SubEmitter from a serialized JSON version.
   * @param serializationObject defines the JSON object to read from
   * @param scene defines the hosting scene
   * @param rootUrl defines the rootUrl for data loading
   * @returns a new SubEmitter
   */
  static SubEmitterPtr Parse(const json& parsedLight, Scene* scene, const std::string& rootUrl);

  /**
   * @brief Release associated resources.
   */
  void dispose();

public:
  /**
   * The particle system to be used by the sub emitter
   */
  ParticleSystemPtr particleSystem;

  /**
   * Type of the submitter (Default: END)
   */
  SubEmitterType type;

  /**
   * If the particle should inherit the direction from the particle it's
   * attached to. (+Y will face the direction the particle is moving) (Default:
   * false) Note: This only is supported when using an emitter of type Mesh
   */
  bool inheritDirection;

  /**
   * How much of the attached particles speed should be added to the sub emitted
   * particle (default: 0)
   */
  float inheritedVelocityAmount;

}; // namespace BABYLON

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_SUB_EMITTER_H
