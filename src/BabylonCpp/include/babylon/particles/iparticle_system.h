#ifndef BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_IPARTICLE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/core/variant.h>

namespace BABYLON {

struct IParticleSystem {
  std::string id;
  std::string name;
  Variant<AbstractMesh*, Vector3> emitter;
  unsigned int renderingGroupId;
  unsigned int layerMask;
  virtual bool isStarted() const = 0;
  virtual void animate()         = 0;
  virtual size_t render()        = 0;
  virtual IParticleSystem* clone(const std::string& name, Mesh* newEmitter) = 0;
  virtual Json::object serialize() const = 0;
}; // end of struct IParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
