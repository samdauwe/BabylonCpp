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
   * @param options The options used to create the system
   * @param scene The scene the particle system belongs to
   * @param isAnimationSheetEnabled Must be true if using a spritesheet to
   * animate the particles texture
   */
  GPUParticleSystem(const string_t& name, size_t capacity = 5000,
                    nullable_t<int> randomTextureSize = nullopt_t,
                    Scene* scene                      = nullptr,
                    bool isAnimationSheetEnabled      = false);
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
   * @brief Starts the particle system and begins to emit
   * @param delay defines the delay in milliseconds before starting the system
   * (0 by default)
   */
  void start(size_t delay = 0) override;

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

  /**
   * @brief Gets the current list of color gradients.
   * You must use addColorGradient and removeColorGradient to udpate this list
   * @returns the list of color gradients
   */
  vector_t<ColorGradient>& getColorGradients() override;

  /**
   * @brief Gets the current list of size gradients.
   * You must use addSizeGradient and removeSizeGradient to udpate this list
   * @returns the list of size gradients
   */
  vector_t<FactorGradient>& getSizeGradients() override;

  /**
   * @brief Gets the current list of angular speed gradients.
   * You must use addAngularSpeedGradient and removeAngularSpeedGradient to
   * udpate this list
   * @returns the list of angular speed gradients
   */
  vector_t<FactorGradient>& getAngularSpeedGradients() override;

  /**
   * @brief Gets the current list of velocity gradients.
   * You must use addVelocityGradient and removeVelocityGradient to udpate this
   * list
   * @returns the list of angular speed gradients
   */
  vector_t<FactorGradient>& getVelocityGradients() override;

  /**
   * @brief Adds a new color gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param color defines the color to affect to the specified gradient
   * @param color2 defines an additional color used to define a range ([color,
   * color2]) with main color to pick the final color from
   * @returns the current particle system
   */
  GPUParticleSystem& addColorGradient(float gradient, const Color4& color1,
                                      const nullable_t<Color4>& color2
                                      = nullopt_t) override;

  /**
   * @brief Remove a specific color gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  GPUParticleSystem& removeColorGradient(float gradient) override;

  /**
   * @brief Adds a new size gradient
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the size factor to affect to the specified gradient
   * @returns the current particle system
   */
  GPUParticleSystem& addSizeGradient(float gradient, float factor,
                                     const nullable_t<float>& factor2
                                     = nullopt_t) override;

  /**
   * @brief Remove a specific size gradient
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  GPUParticleSystem& removeSizeGradient(float gradient) override;

  /**
   * @brief Adds a new angular speed gradient
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the size factor to affect to the specified gradient
   * @returns the current particle system
   */
  GPUParticleSystem& addAngularSpeedGradient(float gradient, float factor,
                                             const nullable_t<float>& factor2
                                             = nullopt_t) override;

  /**
   * @brief Remove a specific angular speed gradient
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  GPUParticleSystem& removeAngularSpeedGradient(float gradient) override;

  /**
   * @brief Adds a new velocity gradient
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the size factor to affect to the specified gradient
   * @returns the current particle system
   */
  GPUParticleSystem& addVelocityGradient(float gradient, float factor,
                                         const nullable_t<float>& factor2
                                         = nullopt_t) override;

  /**
   * @brief Remove a specific velocity gradient
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  GPUParticleSystem& removeVelocityGradient(float gradient) override;

  /**
   * @brief Hidden
   */
  void _recreateUpdateEffect();

  /**
   * @brief Hidden
   */
  void _recreateRenderEffect();

  /**
   * @brief Animates the particle system for the current frame by emitting new
   * particles and or animating the living ones.
   * @param preWarm defines if we are in the pre-warmimg phase
   */
  void animate(bool preWarm = false) override;

  /**
   * @brief Renders the particle system in its current state
   * @param preWarm defines if the system should only update the particles but
   * not render them
   * @returns the current number of particles
   */
  size_t render(bool preWarm = false) override;

  /**
   * @brief Rebuilds the particle system
   */
  void rebuild() override;

  /**
   * @brief Disposes the particle system and free the associated resources.
   * @param disposeTexture defines if the particule texture must be disposed as
   * well (true by default)
   */
  void dispose(bool disposeTexture             = true,
               bool disposeMaterialAndTextures = false) override;

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
  static IParticleSystem* Parse(const Json::value& parsedParticleSystem,
                                Scene* scene, const string_t& rootUrl);

protected:
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

  /**
   * @brief Gets whether an animation sprite sheet is enabled or not on the
   * particle system.
   */
  bool get_isAnimationSheetEnabled() const;

  /**
   * @brief Gets a boolean indicating if the particles must be rendered as
   * billboard or aligned with the direction
   */
  bool get_isBillboardBased() const override;

  /**
   * @brief Sets a boolean indicating if the particles must be rendered as
   * billboard or aligned with the direction
   */
  void set_isBillboardBased(bool value) override;

private:
  template <typename T>
  GPUParticleSystem& _removeGradient(float gradient, vector_t<T>& gradients,
                                     RawTexture* texture);
  void _addFactorGradient(vector_t<FactorGradient>& factorGradients,
                          float gradient, float factor);
  unique_ptr_t<GL::IGLVertexArrayObject> _createUpdateVAO(Buffer* source);
  unique_ptr_t<GL::IGLVertexArrayObject> _createRenderVAO(Buffer* source,
                                                          Buffer* spriteSource);
  void _initialize(bool force = false);
  RawTexture* _getRawTextureByName(const string_t& textureName);
  void _setRawTextureByName(const string_t& textureName,
                            unique_ptr_t<RawTexture>&& rawTexture);
  template <typename T>
  void _createFactorGradientTexture(const vector_t<T>& factorGradients,
                                    const string_t& textureName);
  void _createSizeGradientTexture();
  void _createAngularSpeedGradientTexture();
  void _createVelocityGradientTexture();
  void _createColorGradientTexture();
  void _releaseBuffers();
  void _releaseVAOs();

public:
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

  /**
   * Gets whether an animation sprite sheet is enabled or not on the particle
   * system
   */
  ReadOnlyProperty<GPUParticleSystem, bool> isAnimationSheetEnabled;

private:
  size_t _capacity;
  size_t _activeCount;
  size_t _currentActiveCount;
  size_t _accumulatedCount;
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

  RawTexturePtr _randomTexture;
  RawTexturePtr _randomTexture2;

  int _attributesStrideSize;
  EffectCreationOptions* _updateEffectOptions;

  int _randomTextureSize;
  int _actualFrame;

  Vector3 _zeroVector3;
  unsigned int _rawTextureWidth;
  bool _preWarmDone;
  bool _isAnimationSheetEnabled;
  bool _isBillboardBased;

  vector_t<ColorGradient> _colorGradients;
  RawTexturePtr _colorGradientsTexture;

  vector_t<FactorGradient> _angularSpeedGradients;
  RawTexturePtr _angularSpeedGradientsTexture;

  vector_t<FactorGradient> _sizeGradients;
  RawTexturePtr _sizeGradientsTexture;

  vector_t<FactorGradient> _velocityGradients;
  RawTexturePtr _velocityGradientsTexture;

}; // end of class GPUParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
