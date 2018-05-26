#ifndef BABYLON_HELPERS_PARTICLE_HELPER_H
#define BABYLON_HELPERS_PARTICLE_HELPER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief This class is made for on one-liner static method to help creating
 * particle systems.
 */
class BABYLON_SHARED_EXPORT ParticleHelper {

private:
  /**
   * Base Assets URL.
   */
  static constexpr const char* _baseAssetsUrl
    = "https://assets.babylonjs.com/particles";

  static Scene* _scene;

  static AbstractMesh* _emitter;

public:
  /**
   * @brief This is the main static method (one-liner) of this helper to create
   * different particle systems.
   * @param type This string represents the type to the particle system to
   * create
   * @param emitter The object where the particle system will start to emit
   * from.
   * @param scene The scene where the particle system should live.
   * @param gpu If the system will use gpu.
   * @returns the ParticleSystem created.
   */
  static ParticleHelper* CreateAsync(const string_t& type,
                                     AbstractMesh* emitter,
                                     Scene* scene = nullptr, bool gpu = false);

private:
  static ParticleSystem* _createSystem(IParticleSystemData& data);

}; // end of class ParticleHelper

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_PARTICLE_HELPER_H
