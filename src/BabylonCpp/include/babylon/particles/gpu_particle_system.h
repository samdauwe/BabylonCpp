#ifndef BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/particles/iparticle_system.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief This represents a GPU particle system in Babylon.
 * This os the fastest particle system in Babylon as it uses the GPU to update the individual particle data.
 */
class BABYLON_SHARED_EXPORT GPUParticleSystem : public IParticleSystem {

public:
  /**
   * @brief Instantiates a GPU particle system.
   * Particles are often small sprites used to simulate hard-to-reproduce phenomena like fire, smoke, water, or abstract visual effects like magic glitter and faery dust.
   * @param name The name of the particle system
   * @param capacity The max number of particles alive at the same time
   * @param scene The scene the particle system belongs to
   */
  GPUParticleSystem(const string_t& name, size_t capacity, Scene* scene);
  virtual ~GPUParticleSystem();

  /**
   * @brief Gets Wether the system has been started.
   * @returns True if it has been started, otherwise false.
   */
  bool isStarted() const override;

  /**
   * @brief Starts the particle system and begins to emit.
   */
  void start();

  /**
   * @brief Stops the particle system.
   */
  void stop();

  /**
   * @brief Animates the particle system for the current frame by emitting new particles and or animating the living ones.
   */
  void animate() override;

  /**
   * @brief Renders the particle system in its current state.
   * @returns the current number of particles.
   */
  size_t render() override;

  /**
   * @brief Rebuilds the particle system
   */
  void rebuild() override;

  /**
   * @brief Disposes the particle system and free the associated resources.
   */
  void dispose(bool doNotRecurse = false) override;

  /**
   * @brief Clones the particle system.
   * @param name The name of the cloned object
   * @param newEmitter The new emitter to use
   * @returns the cloned particle system
   */
  IParticleSystem* clone(const string_t& name, Mesh* newEmitter) override;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  Json::object serialize() const override;

private:
  void _initialize();

public:
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
