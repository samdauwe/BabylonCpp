#ifndef BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/particles/iparticle_system.h>
#include <babylon/tools/observable.h>

#include <babylon/core/nullable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT GPUParticleSystem : public IParticleSystem {

public:
  GPUParticleSystem(const string_t& name, size_t capacity, Scene* scene);
  virtual ~GPUParticleSystem();

  bool isStarted() const override;
  void start();
  void stop();
  void animate() override;
  size_t render() override;
  void rebuild() override;
  void dispose(bool doNotRecurse = false) override;
  IParticleSystem* clone(const string_t& name, Mesh* newEmitter) override;
  Json::object serialize() const override;

private:
  void _initialize();

public:
  // Members
  string_t id;
  Nullable<Variant<AbstractMesh*, Vector3>> emitter;
  unsigned int renderingGroupId;
  unsigned int layerMask;
  /**
   * An event triggered when the system is disposed.
   */
  Observable<GPUParticleSystem> onDisposeObservable;

private:
  size_t _capacity;
  unique_ptr_t<Effect> _renderEffect;
  unique_ptr_t<Effect> _updateEffect;

  unique_ptr_t<Buffer> _updateBuffer;
  unique_ptr_t<GL::IGLVertexArrayObject> _updateVAO;
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> _updateVertexBuffers;
  unique_ptr_t<Buffer> _renderBuffer;
  unique_ptr_t<GL::IGLVertexArrayObject> _renderVAO;
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> _renderVertexBuffers;

  GL::IGLVertexArrayObject* _sourceVAO;
  GL::IGLVertexArrayObject* _targetVAO;
  Buffer* _sourceBuffer;
  Buffer* _targetBuffer;

  Scene* _scene;
  Engine* _engine;

  int _currentRenderId;
  bool _started = true;

}; // end of class GPUParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
