#ifndef BABYLON_PARTICLES_PARTICLE_HELPER_H
#define BABYLON_PARTICLES_PARTICLE_HELPER_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/core/variant.h>

namespace BABYLON {

class AbstractMesh;
struct IParticleSystem;
class ParticleSystem;
class ParticleSystemSet;
class Scene;
class Vector3;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

/**
 * @brief This class is made for on one-liner static method to help creating
 * particle systems.
 */
struct BABYLON_SHARED_EXPORT ParticleHelper {

  /**
   * Gets or sets base Assets URL
   */
  static constexpr const char* BaseAssetsUrl
    = "https://assets.babylonjs.com/particles";

  /**
   * @brief Create a default particle system that you can tweak.
   * @param emitter defines the emitter to use
   * @param capacity defines the system capacity (default is 500 particles)
   * @param scene defines the hosting scene
   * @returns the new Particle system
   */
  static ParticleSystem*
  CreateDefault(const Variant<AbstractMeshPtr, Vector3>& emitter,
                size_t capacity = 500, Scene* scene = nullptr);

  /**
   * @brief This is the main static method (one-liner) of this helper to create
   * different particle systems.
   * @param type This string represents the type to the particle system to
   * create
   * @param scene The scene where the particle system should live
   * @param gpu If the system will use gpu
   * @returns the ParticleSystemSet created
   */
  static ParticleSystemSet* CreateAsync(const std::string& type,
                                        Scene* scene = nullptr,
                                        bool gpu     = false);

  /**
   * @brief Static function used to export a particle system to a
   * ParticleSystemSet variable. Please note that the emitter shape is not
   * exported
   * @param system defines the particle systems to export
   */
  static std::unique_ptr<ParticleSystemSet>
  ExportSet(const std::vector<IParticleSystem*>& systems);

}; // end of class ParticleHelper

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_HELPER_H
