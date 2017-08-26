#ifndef BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/particles/iparticle_system.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT GPUParticleSystem : public IParticleSystem {

public:
  GPUParticleSystem(const std::string& name, std::size_t capacity,
                    Scene* scene);
  virtual ~GPUParticleSystem();

  bool isStarted() const override;
  void animate() override;
  size_t render() override;
  void dispose(bool doNotRecurse = false) override;
  IParticleSystem* clone(const std::string& name, Mesh* newEmitter) override;
  Json::object serialize() const override;

public:
  /**
   * An event triggered when the system is disposed.
   */
  Observable<GPUParticleSystem> onDisposeObservable;

private:
  Scene* _scene;

}; // end of class GPUParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
