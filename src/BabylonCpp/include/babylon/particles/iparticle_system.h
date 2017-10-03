#ifndef BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_IPARTICLE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/core/variant.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IParticleSystem : public IDisposable {
  string_t id;
  string_t name;
  Variant<AbstractMesh*, Vector3> emitter;
  unsigned int renderingGroupId;
  unsigned int layerMask;
  bool hasEmitter();
  virtual bool isStarted() const = 0;
  virtual void animate()         = 0;
  virtual size_t render()        = 0;
  virtual IParticleSystem* clone(const string_t& name, Mesh* newEmitter) = 0;
  virtual Json::object serialize() const = 0;
  virtual void rebuild()                 = 0;
}; // end of struct IParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_IPARTICLE_SYSTEM_H
