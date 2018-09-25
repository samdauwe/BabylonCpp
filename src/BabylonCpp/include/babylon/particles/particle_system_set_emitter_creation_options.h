#ifndef BABYLON_PARTICLES_PARTICLE_SYSTEM_SET_EMITTER_CREATION_OPTIONS_H
#define BABYLON_PARTICLES_PARTICLE_SYSTEM_SET_EMITTER_CREATION_OPTIONS_H

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT EmitterCreationOptions {
  float diameter;
  unsigned int segments;
  Color3 color;
}; // end of struct EmitterCreationOptions

/**
 * @brief Internal class used to store shapes for emitters.
 */
struct BABYLON_SHARED_EXPORT ParticleSystemSetEmitterCreationOptions {
  std::string kind;
  EmitterCreationOptions options;
  unsigned int renderingGroupId;
}; // end of struct ParticleSystemSetEmitterCreationOptions

} // end of namespace BABYLON

#endif // end of
       // BABYLON_PARTICLES_PARTICLE_SYSTEM_SET_EMITTER_CREATION_OPTIONS_H
