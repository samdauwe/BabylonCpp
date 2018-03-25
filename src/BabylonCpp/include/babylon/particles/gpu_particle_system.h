#ifndef BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/particles/iparticle_system.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief This represents a GPU particle system in Babylon
 * This is the fastest particle system in Babylon as it uses the GPU to update
 * the individual particle data
 * @see https://www.babylonjs-playground.com/#PU4WYI#4
 */
class BABYLON_SHARED_EXPORT GPUParticleSystem : public IParticleSystem,
                                                public IAnimatable {

public:
  /**
   * @brief Gets a boolean indicating if the GPU particles can be rendered on
   * current browser
   */
  static bool IsSupported();

public:
  /**
   * @brief Instantiates a GPU particle system.
   * Particles are often small sprites used to simulate hard-to-reproduce
   * phenomena like fire, smoke, water, or abstract visual effects like magic
   * glitter and faery dust.
   * @param name The name of the particle system
   * @param capacity The max number of particles alive at the same time
   * @param scene The scene the particle system belongs to
   */
  GPUParticleSystem(const string_t& name, size_t capacity = 5000,
                    Nullable<int> randomTextureSize = nullptr,
                    Scene* scene                    = nullptr);
  virtual ~GPUParticleSystem() override;

  /**
   * @brief Gets the maximum number of particles active at the same time.
   * @returns The max number of active particles.
   */
  size_t getCapacity() const override;

  /**
   * @brief Is this system ready to be used/rendered.
   * @return true if the system is ready
   */
  bool isReady() override;

  /**
   * @brief Gets Wether the system has been started.
   * @returns True if it has been started, otherwise false.
   */
  bool isStarted() const override;

  /**
   * @brief Starts the particle system and begins to emit.
   */
  void start() override;

  /**
   * @brief Stops the particle system.
   */
  void stop() override;

  /**
   * @brief Remove all active particles
   */
  void reset() override;

  /**
   * @brief Returns the string "GPUParticleSystem".
   * @returns a string containing the class name
   */
  const char* getClassName() const;

  void _recreateUpdateEffect();

  void _recreateRenderEffect();

  /**
   * @brief Animates the particle system for the current frame by emitting new
   * particles and or animating the living ones.
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
   * @param disposeTexture defines if the particule texture must be disposed as
   * well (true by default)
   */
  void dispose(bool disposeTexture = true) override;

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

  /**
   * @brief Parses a JSON object to create a GPU particle system.
   * @param parsedParticleSystem The JSON object to parse
   * @param scene The scene to create the particle system in
   * @param rootUrl The root url to use to load external dependencies like
   * texture
   * @returns the parsed GPU particle system
   */
  static unique_ptr_t<GPUParticleSystem>
  Parse(const Json::value& parsedParticleSystem, Scene* scene,
        const string_t& rootUrl);

private:
  Vector3& get_direction1();
  void set_direction1(const Vector3& value);
  Vector3& get_direction2();
  void set_direction2(const Vector3& value);
  Vector3& get_minEmitBox();
  void set_minEmitBox(const Vector3& value);
  Vector3& get_maxEmitBox();
  void set_maxEmitBox(const Vector3& value);
  size_t get_activeParticleCount() const;
  void set_activeParticleCount(size_t value);

  unique_ptr_t<GL::IGLVertexArrayObject> _createUpdateVAO(Buffer* source);
  unique_ptr_t<GL::IGLVertexArrayObject> _createRenderVAO(Buffer* source,
                                                          Buffer* spriteSource);
  void _initialize(bool force = false);
  void _releaseBuffers();
  void _releaseVAOs();

public:
  /**
   * List of animations used by the particle system.
   */
  vector_t<Animation*> animations;

  /**
   * An event triggered when the system is disposed.
   */
  Observable<GPUParticleSystem> onDisposeObservable;

  /**
   * Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors. This only works when particleEmitterTyps
   * is a BoxParticleEmitter
   */
  Property<GPUParticleSystem, Vector3> direction1;

  /**
   * Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors. This only works when particleEmitterTyps
   * is a BoxParticleEmitter
   */
  Property<GPUParticleSystem, Vector3> direction2;

  /**
   * Minimum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so. This only works when
   * particleEmitterTyps is a BoxParticleEmitter
   */
  Property<GPUParticleSystem, Vector3> minEmitBox;

  /**
   * Maximum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so. This only works when
   * particleEmitterTyps is a BoxParticleEmitter
   */
  Property<GPUParticleSystem, Vector3> maxEmitBox;

  /**
   * Forces the particle to write their depth information to the depth buffer.
   * This can help preventing other draw calls to override the particles.
   */
  bool forceDepthWrite;

  /**
   * Gets or set the number of active particles
   */
  Property<GPUParticleSystem, size_t> activeParticleCount;

private:
  size_t _capacity;
  size_t _activeCount;
  size_t _currentActiveCount;
  unique_ptr_t<Effect> _renderEffect;
  unique_ptr_t<Effect> _updateEffect;

  unique_ptr_t<Buffer> _buffer0;
  unique_ptr_t<Buffer> _buffer1;
  unique_ptr_t<Buffer> _spriteBuffer;
  vector_t<unique_ptr_t<GL::IGLVertexArrayObject>> _updateVAO;
  vector_t<unique_ptr_t<GL::IGLVertexArrayObject>> _renderVAO;

  size_t _targetIndex;
  Buffer* _sourceBuffer;
  Buffer* _targetBuffer;

  Scene* _scene;
  Engine* _engine;

  int _currentRenderId;
  bool _started;
  bool _stopped;

  float _timeDelta;

  unique_ptr_t<RawTexture> _randomTexture;

  int _attributesStrideSize;
  EffectCreationOptions* _updateEffectOptions;

  int _randomTextureSize;
  int _actualFrame;

  Vector3 _zeroVector3;

}; // end of class GPUParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
