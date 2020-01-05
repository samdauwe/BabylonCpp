#ifndef BABYLON_PARTICLES_DEPTH_SORTED_PARTICLE_H
#define BABYLON_PARTICLES_DEPTH_SORTED_PARTICLE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Represents a Depth Sorted Particle in the solid particle system.
 * @hidden
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
  /**
   * Material index when used with MultiMaterials
   */
  size_t materialIndex = 0;

  /**
   * @brief Creates a new sorted particle.
   * @param materialIndex
   */
  DepthSortedParticle(size_t iInd, size_t indLength, size_t iMaterialIndex)
      : ind{iInd}, indicesLength{indLength}, materialIndex{iMaterialIndex}
  {
  }
  DepthSortedParticle(const DepthSortedParticle& other) = default;
  DepthSortedParticle(DepthSortedParticle&& other)      = default;
  DepthSortedParticle& operator=(const DepthSortedParticle& other) = default;
  DepthSortedParticle& operator=(DepthSortedParticle&& other) = default;
  ~DepthSortedParticle()                                      = default;

}; // end of struct DepthSortedParticle

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_DEPTH_SORTED_PARTICLE_H
