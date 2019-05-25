#ifndef BABYLON_PARTICLES_DEPTH_SORTED_PARTICLE_H
#define BABYLON_PARTICLES_DEPTH_SORTED_PARTICLE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Represents a Depth Sorted Particle in the solid particle system.
 */
struct BABYLON_SHARED_EXPORT DepthSortedParticle {
  /**
   * Index of the particle in the "indices" array
   */
  size_t ind = 0;
  /**
   * Index of the particle in the "indices" array
   */
  size_t indicesLength = 0;
  /**
   * Index of the particle in the "indices" array
   */
  float sqDistance = 0.f;
}; // end of struct DepthSortedParticle

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_DEPTH_SORTED_PARTICLE_H
