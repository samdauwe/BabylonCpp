#ifndef BABYLON_PARTICLES_PARTICLE_HELPER_H
#define BABYLON_PARTICLES_PARTICLE_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

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
   * @brief This is the main static method (one-liner) of this helper to create
   * different particle systems.
   * @param type This string represents the type to the particle system to
   * create
   * @param scene The scene where the particle system should live
   * @param gpu If the system will use gpu
   * @returns the ParticleSystemSet created
   */
  static ParticleSystemSet*
  CreateAsync(const string_t& type, Scene* scene = nullptr, bool gpu = false);

  /**
   * @brief Static function used to export a particle system to a
   * ParticleSystemSet variable. Please note that the emitter shape is not
   * exported
   * @param system defines the particle systems to export
   */
  static unique_ptr_t<ParticleSystemSet>
  _createSystem(const vector_t<IParticleSystem*>& systems);

}; // end of class ParticleHelper

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_HELPER_H
