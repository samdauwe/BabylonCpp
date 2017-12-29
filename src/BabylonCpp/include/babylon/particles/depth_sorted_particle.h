#ifndef BABYLON_PARTICLES_DEPTH_SORTED_PARTICLE_H
#define BABYLON_PARTICLES_DEPTH_SORTED_PARTICLE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT DepthSortedParticle {
  // index of the particle in the "indices" array
  unsigned int ind = 0;
  // length of the particle shape in the "indices" array
  size_t indicesLength = 0;
  // squared distance from the particle to the camera end of class SolidParticle
  float sqDistance = 0.f;
}; // end of struct DepthSortedParticle

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_DEPTH_SORTED_PARTICLE_H
