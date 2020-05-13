#ifndef BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/misc/observable.h>
#include <babylon/particles/base_particle_system.h>

namespace BABYLON {

namespace GL {
class IGLVertexArrayObject;
} // end of namespace GL

class Buffer;
class Effect;
class Engine;
class GPUParticleSystem;
struct IEffectCreationOptions;
class Mesh;
class RawTexture;
using EffectPtr                 = std::shared_ptr<Effect>;
using RawTexturePtr             = std::shared_ptr<RawTexture>;
using WebGLVertexArrayObjectPtr = std::shared_ptr<GL::IGLVertexArrayObject>;

/**
 * @brief This represents a GPU particle system in Babylon
 * This is the fastest particle system in Babylon as it uses the GPU to update the individual
 * particle data
 * @see https://www.babylonjs-playground.com/#PU4WYI#4
 */
class BABYLON_SHARED_EXPORT GPUParticleSystem : public BaseParticleSystem, public IAnimatable {

public:
  /**
   * @brief Gets a boolean indicating if the GPU particles can be rendered on current browser
   */
  static bool IsSupported();

public:
  /**
   * @brief Instantiates a GPU particle system.
   * Particles are often small sprites used to simulate hard-to-reproduce phenomena like fire,
   * smoke, water, or abstract visual effects like magic glitter and faery dust.
   * @param name The name of the particle system
   * @param options The options used to create the system
   * @param scene The scene the particle system belongs to
   * @param isAnimationSheetEnabled Must be true if using a spritesheet to
   * animate the particles texture
   */
  GPUParticleSystem(const std::string& name, size_t capacity = 5000,
                    std::optional<size_t> randomTextureSize = std::nullopt, Scene* scene = nullptr,
                    bool isAnimationSheetEnabled = false);
  ~GPUParticleSystem() override; // = default

  /**/
  size_t getActiveCount() const override
  {
    return 0;
  }
  void forceRefreshGradients() override
  {
  }
  bool isStopping() const override
  {
    return false;
  }
  /**/

  /**
   * @brief Returns the object type.
   * @return the object type
   */
  [[nodiscard]] Type type() const override;

  /**
   * @brief Gets the maximum number of particles active at the same time.
   * @returns The max number of active particles.
   */
  [[nodiscard]] size_t getCapacity() const override;

  /**
   * @brief Is this system ready to be used/rendered.
   * @return true if the system is ready
   */
  bool isReady() override;

  /**
   * @brief Gets if the system has been started. (Note: this will still be true after stop is
   * called).
   * @returns True if it has been started, otherwise false.
   */
  [[nodiscard]] bool isStarted() const override;

  /**
   * @brief Starts the particle system and begins to emit
   * @param delay defines the delay in milliseconds before starting the system (0 by default)
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
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Adds a new color gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param color1 defines the color to affect to the specified gradient
   * @param color2 defines an additional color used to define a range ([color, color2]) with main
   * color to pick the final color from
   * @returns the current particle system
   */
  GPUParticleSystem& addColorGradient(float gradient, const Color4& color1,
                                      const std::optional<Color4>& color2 = std::nullopt) override;

  /**
   * @brief Remove a specific color gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  GPUParticleSystem& removeColorGradient(float gradient) override;

  /**
   * @brief Adds a new size gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the size factor to affect to the specified gradient
   * @returns the current particle system
   */
  GPUParticleSystem& addSizeGradient(float gradient, float factor,
                                     const std::optional<float>& factor2 = std::nullopt) override;

  /**
   * @brief Remove a specific size gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  GPUParticleSystem& removeSizeGradient(float gradient) override;

  /**
   * @brief Adds a new angular speed gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the angular speed to affect to the specified gradient
   * @returns the current particle system
   */
  GPUParticleSystem& addAngularSpeedGradient(float gradient, float factor,
                                             const std::optional<float>& factor2
                                             = std::nullopt) override;

  /**
   * @brief Remove a specific angular speed gradient.
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
                                         const std::optional<float>& factor2
                                         = std::nullopt) override;

  /**
   * @brief Remove a specific velocity gradient
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  GPUParticleSystem& removeVelocityGradient(float gradient) override;

  /**
   * @brief Adds a new limit velocity gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the limit velocity value to affect to the specified
   * gradient
   * @returns the current particle system
   */
  IParticleSystem& addLimitVelocityGradient(float gradient, float factor,
                                            const std::optional<float>& factor2
                                            = std::nullopt) override;

  /**
   * @brief Remove a specific limit velocity gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeLimitVelocityGradient(float gradient) override;

  /**
   * @brief Adds a new drag gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the drag value to affect to the specified gradient
   * @returns the current particle system
   */
  IParticleSystem& addDragGradient(float gradient, float factor,
                                   const std::optional<float>& factor2 = std::nullopt) override;

  /**
   * @brief Remove a specific drag gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeDragGradient(float gradient) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the emit rate value to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range ([factor, factor2]) with
   * main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addEmitRateGradient(float gradient, float factor,
                                       const std::optional<float>& factor2 = std::nullopt) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeEmitRateGradient(float gradient) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the start size value to affect to the specified
   * gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addStartSizeGradient(float gradient, float factor,
                                        const std::optional<float>& factor2
                                        = std::nullopt) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeStartSizeGradient(float gradient) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param min defines the color remap minimal range
   * @param max defines the color remap maximal range
   * @returns the current particle system
   */
  IParticleSystem& addColorRemapGradient(float gradient, float min, float max) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeColorRemapGradient(float gradient = 0.f) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param min defines the alpha remap minimal range
   * @param max defines the alpha remap maximal range
   * @returns the current particle system
   */
  IParticleSystem& addAlphaRemapGradient(float gradient, float min, float max) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeAlphaRemapGradient(float gradient) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param color defines the color to affect to the specified gradient
   * @returns the current particle system
   */
  IParticleSystem& addRampGradient(float gradient, const Color3& color) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeRampGradient(float gradient = 0.f) override;

  /**
   * @brief Not supported by GPUParticleSystem
   * @returns the list of ramp gradients
   */
  std::vector<Color3Gradient>& getRampGradients() override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * Gets a boolean indicating that ramp gradients must be used
   * @see http://doc.babylonjs.com/babylon101/particles#ramp-gradients
   */
  [[nodiscard]] bool get_useRampGradients() const override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * Sets a boolean indicating that ramp gradients must be used
   * @see http://doc.babylonjs.com/babylon101/particles#ramp-gradients
   */
  void set_useRampGradients(bool value) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the life time factor to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range ([factor, factor2]) with
   * main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addLifeTimeGradient(float gradient, float factor,
                                       const std::optional<float>& factor2 = std::nullopt) override;

  /**
   * @brief Not supported by GPUParticleSystem.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeLifeTimeGradient(float gradient) override;

  /**
   * @brief Hidden
   */
  void _recreateUpdateEffect();

  /**
   * @brief Hidden
   */
  void _recreateRenderEffect();

  /**
   * @brief Animates the particle system for the current frame by emitting new particles and or
   * animating the living ones.
   * @param preWarm defines if we are in the pre-warmimg phase
   */
  void animate(bool preWarm = false) override;

  /**
   * @brief Renders the particle system in its current state
   * @param preWarm defines if the system should only update the particles but not render them
   * @returns the current number of particles
   */
  size_t render(bool preWarm = false) override;

  /**
   * @brief Rebuilds the particle system
   */
  void rebuild() override;

  /**
   * @brief Disposes the particle system and free the associated resources.
   * @param disposeTexture defines if the particule texture must be disposed as well (true by
   * default)
   */
  void dispose(bool disposeTexture = true, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Clones the particle system.
   * @param name The name of the cloned object
   * @param newEmitter The new emitter to use
   * @returns the cloned particle system
   */
  IParticleSystem* clone(const std::string& name, Mesh* newEmitter) override;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  [[nodiscard]] json serialize(bool serializeTexture) const override;

  /**
   * @brief Parses a JSON object to create a GPU particle system.
   * @param parsedParticleSystem The JSON object to parse
   * @param scene The scene to create the particle system in
   * @param rootUrl The root url to use to load external dependencies like texture
   * @param doNotStart Ignore the preventAutoStart attribute and does not start
   * @returns the parsed GPU particle system
   */
  static IParticleSystem* Parse(const json& parsedParticleSystem, Scene* scene,
                                const std::string& rootUrl, bool doNotStart = false);

protected:
  /**
   * @brief Gets the number of active particles.
   */
  [[nodiscard]] size_t get_activeParticleCount() const;

  /**
   * @brief Set the number of active particles.
   */
  void set_activeParticleCount(size_t value);

  /**
   * @brief Specifies if the particles are updated in emitter local space or world space.
   * This is always false for GPU particles
   */
  bool get_isLocal() const override;

  /**
   * @brief Specifies if the particles are updated in emitter local space or world space.
   * This is always false for GPU particles
   */
  void set_isLocal(bool value) override;

  /**
   * @brief Hidden
   */
  void _reset() override;

  /**
   * Hidden
   */
  BaseParticleSystem& _removeGradientAndTexture(float gradient,
                                                std::vector<ColorGradient>& gradients,
                                                const RawTexturePtr& texture) override;

  /**
   * Hidden
   */
  BaseParticleSystem& _removeGradientAndTexture(float gradient,
                                                std::vector<Color3Gradient>& gradients,
                                                const RawTexturePtr& texture) override;

  /**
   * Hidden
   */
  BaseParticleSystem& _removeGradientAndTexture(float gradient,
                                                std::vector<FactorGradient>& gradients,
                                                const RawTexturePtr& texture) override;

private:
  void _addFactorGradient(std::vector<FactorGradient>& factorGradients, float gradient,
                          float factor);
  WebGLVertexArrayObjectPtr _createUpdateVAO(Buffer* source);
  WebGLVertexArrayObjectPtr _createRenderVAO(Buffer* source, Buffer* spriteSource);
  void _initialize(bool force = false);
  RawTexture* _getRawTextureByName(const std::string& textureName);
  void _setRawTextureByName(const std::string& textureName,
                            std::unique_ptr<RawTexture>&& rawTexture);
  template <typename T>
  void _createFactorGradientTexture(const std::vector<T>& factorGradients,
                                    const std::string& textureName);
  void _createSizeGradientTexture();
  void _createAngularSpeedGradientTexture();
  void _createVelocityGradientTexture();
  void _createLimitVelocityGradientTexture();
  void _createDragGradientTexture();
  void _createColorGradientTexture();
  void _releaseBuffers();
  void _releaseVAOs();

public:
  /**
   * An event triggered when the system is disposed.
   */
  Observable<GPUParticleSystem> onDisposeObservable;

  /**
   * Gets or set the number of active particles
   */
  Property<GPUParticleSystem, size_t> activeParticleCount;

private:
  size_t _capacity;
  size_t _activeCount;
  size_t _currentActiveCount;
  size_t _accumulatedCount;
  EffectPtr _renderEffect;
  EffectPtr _updateEffect;

  std::unique_ptr<Buffer> _buffer0;
  std::unique_ptr<Buffer> _buffer1;
  std::unique_ptr<Buffer> _spriteBuffer;
  std::vector<WebGLVertexArrayObjectPtr> _updateVAO;
  std::vector<WebGLVertexArrayObjectPtr> _renderVAO;

  size_t _targetIndex;
  Buffer* _sourceBuffer;
  Buffer* _targetBuffer;

  Engine* _engine;

  int _currentRenderId;
  bool _started;
  bool _stopped;

  float _timeDelta;

  RawTexturePtr _randomTexture;
  RawTexturePtr _randomTexture2;

  int _attributesStrideSize;
  IEffectCreationOptions* _updateEffectOptions;

  size_t _randomTextureSize;
  int _actualFrame;

  Vector3 _zeroVector3;
  unsigned int _rawTextureWidth;
  bool _preWarmDone;

  RawTexturePtr _colorGradientsTexture;
  RawTexturePtr _angularSpeedGradientsTexture;
  RawTexturePtr _sizeGradientsTexture;
  RawTexturePtr _velocityGradientsTexture;
  RawTexturePtr _limitVelocityGradientsTexture;
  RawTexturePtr _dragGradientsTexture;

  std::vector<Color3Gradient> _emptyRampGradients;

}; // end of class GPUParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_GPU_PARTICLE_SYSTEM_H
