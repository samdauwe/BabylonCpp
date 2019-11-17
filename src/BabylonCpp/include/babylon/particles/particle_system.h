#ifndef BABYLON_PARTICLES_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_PARTICLE_SYSTEM_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>
#include <babylon/particles/base_particle_system.h>
#include <babylon/particles/iparticle_system.h>
#include <unordered_map>

namespace BABYLON {

class Buffer;
class Effect;
class Mesh;
class Particle;
class Scene;
class VertexBuffer;
using EffectPtr       = std::shared_ptr<Effect>;
using VertexBufferPtr = std::shared_ptr<VertexBuffer>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

/**
 * @brief This represents a particle system in Babylon.
 * Particles are often small sprites used to simulate hard-to-reproduce
 * phenomena like fire, smoke, water, or abstract visual effects like magic
 * glitter and faery dust. Particles can take different shapes while emitted
 * like box, sphere, cone or you can write your custom function.
 * Example: https://doc.babylonjs.com/babylon101/particles
 */
class BABYLON_SHARED_EXPORT ParticleSystem : public BaseParticleSystem, public IAnimatable {

public:
  /**
   * Billboard mode will only apply to Y axis
   */
  static constexpr unsigned int BILLBOARDMODE_Y = Constants::PARTICLES_BILLBOARDMODE_Y;
  /**
   * Billboard mode will apply to all axes
   */
  static constexpr unsigned int BILLBOARDMODE_ALL = Constants::PARTICLES_BILLBOARDMODE_ALL;
  /**
   * Special billboard mode where the particle will be biilboard to the camera
   * but rotated to align with direction
   */
  static constexpr unsigned int BILLBOARDMODE_STRETCHED
    = Constants::PARTICLES_BILLBOARDMODE_STRETCHED;

public:
  /**
   * @brief Instantiates a particle system.
   * Particles are often small sprites used to simulate hard-to-reproduce
   * phenomena like fire, smoke, water, or abstract visual effects like magic
   * glitter and faery dust.
   * @param name The name of the particle system
   * @param capacity The max number of particles alive at the same time
   * @param scene The scene the particle system belongs to
   * @param customEffect a custom effect used to change the way particles are
   * rendered by default
   * @param isAnimationSheetEnabled Must be true if using a spritesheet to
   * animate the particles texture
   * @param epsilon Offset used to render the particles
   */
  ParticleSystem(const std::string& name, size_t capacity, Scene* scene,
                 const EffectPtr& customEffect = nullptr, bool isAnimationSheetEnabled = false,
                 float epsilon = 0.01f);
  ~ParticleSystem() override; // = default

  /**
   * @brief Returns the object type.
   * @return the object type
   */
  Type type() const override;

  /**
   * @brief Adds a new life time gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the life time factor to affect to the specified
   * gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addLifeTimeGradient(float gradient, float factor,
                                       const std::optional<float>& factor2 = std::nullopt) override;

  /**
   * @brief Remove a specific life time gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeLifeTimeGradient(float gradient) override;

  /**
   * @brief Adds a new size gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the size factor to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addSizeGradient(float gradient, float factor,
                                   const std::optional<float>& factor2 = std::nullopt) override;

  /**
   * @brief Remove a specific size gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeSizeGradient(float gradient) override;

  /**
   * @brief Adds a new color remap gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param min defines the color remap minimal range
   * @param max defines the color remap maximal range
   * @returns the current particle system
   */
  IParticleSystem& addColorRemapGradient(float gradient, float min, float max) override;

  /**
   * @brief Remove a specific color remap gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeColorRemapGradient(float gradient) override;

  /**
   * @brief Adds a new alpha remap gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param min defines the alpha remap minimal range
   * @param max defines the alpha remap maximal range
   * @returns the current particle system
   */
  IParticleSystem& addAlphaRemapGradient(float gradient, float min, float max) override;

  /**
   * @brief Remove a specific alpha remap gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeAlphaRemapGradient(float gradient) override;

  /**
   * @brief Adds a new angular speed gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the angular speed  to affect to the specified
   * gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addAngularSpeedGradient(float gradient, float factor,
                                           const std::optional<float>& factor2
                                           = std::nullopt) override;

  /**
   * @brief Remove a specific angular speed gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeAngularSpeedGradient(float gradient) override;

  /**
   * @brief Adds a new velocity gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the velocity to affect to the specified gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addVelocityGradient(float gradient, float factor,
                                       const std::optional<float>& factor2 = std::nullopt) override;

  /**
   * @brief Remove a specific velocity gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeVelocityGradient(float gradient) override;

  /**
   * @brief Adds a new limit velocity gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the limit velocity value to affect to the specified
   * gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
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
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
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
   * @brief Adds a new emit rate gradient (please note that this will only work
   * if you set the targetStopDuration property).
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param factor defines the emit rate value to affect to the specified
   * gradient
   * @param factor2 defines an additional factor used to define a range
   * ([factor, factor2]) with main value to pick the final value from
   * @returns the current particle system
   */
  IParticleSystem& addEmitRateGradient(float gradient, float factor,
                                       const std::optional<float>& factor2 = std::nullopt) override;

  /**
   * @brief  Remove a specific emit rate gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeEmitRateGradient(float gradient) override;

  /**
   * @brief Adds a new start size gradient (please note that this will only work
   * if you set the targetStopDuration property).
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
   * @brief Remove a specific start size gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeStartSizeGradient(float gradient) override;

  /**
   * @brief Gets the current list of ramp gradients.
   * You must use addRampGradient and removeRampGradient to udpate this list
   * @returns the list of ramp gradients
   */
  std::vector<Color3Gradient>& getRampGradients() override;

  /**
   * @brief Adds a new ramp gradient used to remap particle colors.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param color defines the color to affect to the specified gradient
   * @returns the current particle system
   */
  IParticleSystem& addRampGradient(float gradient, const Color3& color) override;

  /**
   * @brief Remove a specific ramp gradient.
   * @param gradient defines the gradient to remove
   * @returns the current particle system
   */
  IParticleSystem& removeRampGradient(float gradient) override;

  /**
   * @brief Adds a new color gradient.
   * @param gradient defines the gradient to use (between 0 and 1)
   * @param color1 defines the color to affect to the specified gradient
   * @param color2 defines an additional color used to define a range ([color,
   * color2]) with main color to pick the final color from
   * @returns this particle system
   */
  IParticleSystem& addColorGradient(float gradient, const Color4& color1,
                                    const std::optional<Color4>& color2 = std::nullopt) override;

  /**
   * @brief Remove a specific color gradient.
   * @param gradient defines the gradient to remove
   * @returns this particle system
   */
  IParticleSystem& removeColorGradient(float gradient) override;

  /**
   * @brief Gets the current list of active particles.
   */
  std::vector<Particle*>& particles();

  /**
   * @brief Returns the string "ParticleSystem".
   * @returns a string containing the class name
   */
  const char* getClassName() const;

  /**
   * @brief Gets the maximum number of particles active at the same time.
   * @returns The max number of active particles.
   */
  size_t getCapacity() const override;

  /**
   * @brief Gets whether there are still active particles in the system.
   * @returns True if it is alive, otherwise false.
   */
  bool isAlive() const;

  /**
   * @brief Gets if the system has been started. (Note: this will still be true
   * after stop is called).
   * @returns True if it has been started, otherwise false.
   */
  bool isStarted() const override;

  /**
   * @brief Starts the particle system and begins to emit.
   * @param delay defines the delay in milliseconds before starting the system
   * (this.startDelay by default)
   */
  void start(size_t delay = 0) override;

  /**
   * @brief Stops the particle system.
   */
  void stop() override;

  /**
   * @brief Stops the particle system.
   * @param stopSubEmitters if true it will stop the current system and all
   * created sub-Systems if false it will stop the current root system only,
   * this param is used by the root particle system only. the default value is
   * true.
   */
  void stop(bool stopSubEmitters);

  /**
   * @brief Remove all active particles
   */
  void reset() override;

  /**
   * @brief For internal use only.
   */
  void _appendParticleVertex(unsigned int index, Particle* particle, int offsetX, int offsetY);

  /**
   * @brief "Recycles" one of the particle by copying it back to the "stock" of
   * particles and removing it from the active list. Its lifetime will start
   * back at 0.
   */
  void recycleParticle(Particle* particle);

  /**
   * @brief Hidden
   */
  static std::vector<std::string> _GetAttributeNamesOrOptions(bool isAnimationSheetEnabled = false,
                                                              bool isBillboardBased        = false,
                                                              bool useRampGradients        = false);

  /**
   * @brief Hidden
   */
  static std::vector<std::string> _GetEffectCreationOptions(bool isAnimationSheetEnabled = false);

  /**
   * @brief For internal use only.
   */
  void _appendParticleVertexWithAnimation(unsigned int index, Particle* particle, int offsetX,
                                          int offsetY);

  /**
   * @brief Animates the particle system for the current frame by emitting new
   * particles and or animating the living ones.
   * @param preWarmOnly will prevent the system from updating the vertex buffer
   * (default is false)
   */
  void animate(bool preWarmOnly = false) override;

  /**
   * @brief Rebuilds the particle system.
   */
  void rebuild() override;

  /**
   * @brief Is this system ready to be used/rendered.
   * @return true if the system is ready
   */
  bool isReady() override;

  /**
   * @brief Renders the particle system in its current state.
   * @param preWarm defines if the system should only update the particles but
   * not render them
   * @returns the current number of particles
   */
  size_t render(bool preWarm = false) override;

  /**
   * @brief Disposes the particle system and free the associated resources.
   * @param disposeTexture defines if the particule texture must be disposed as
   * well (true by default)
   */
  void dispose(bool disposeTexture = true, bool disposeMaterialAndTextures = false) override;

  std::vector<AnimationPtr> getAnimations() override;

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
  json serialize() const override;

  /**
   * @brief Hidden
   */
  static void _Serialize(json& serializationObject, IParticleSystem* particleSystem);

  /**
   * @brief Hidden
   */
  static ParticleSystem* _Parse(const json& parsedParticleSystem, IParticleSystem* particleSystem,
                                Scene* scene, const std::string& url);

  /**
   * @brief Parses a JSON object to create a particle system.
   * @param parsedParticleSystem The JSON object to parse
   * @param scene The scene to create the particle system in
   * @param rootUrl The root url to use to load external dependencies like
   * texture
   * @returns the Parsed particle system
   */
  static ParticleSystem* Parse(const json& parsedParticleSystem, Scene* scene,
                               const std::string& url);

protected:
  /**
   * @brief Sets a callback that will be triggered when the system is disposed.
   */
  void set_onDispose(const std::function<void(ParticleSystem*, EventState&)>& callback);

  /** @brief Gets a boolean indicating that ramp gradients must be used.
   * @see http://doc.babylonjs.com/babylon101/particles#ramp-gradients
   */
  bool get_useRampGradients() const override;

  /** @brief Sets a boolean indicating that ramp gradients must be used.
   * @see http://doc.babylonjs.com/babylon101/particles#ramp-gradients
   */
  void set_useRampGradients(bool value) override;

  void _reset() override;

private:
  float _fetchR(float u, float v, float width, float height, const Uint8Array& pixels);
  void _addFactorGradient(std::vector<FactorGradient>& factorGradients, float gradient,
                          float factor, const std::optional<float>& factor2 = std::nullopt);
  void _removeFactorGradient(std::vector<FactorGradient>& factorGradients, float gradient);
  void _createRampGradientTexture();
  void _resetEffect();
  void _createVertexBuffers();
  void _createIndexBuffer();
  void _prepareSubEmitterInternalArray();
  // Start of sub system methods
  void _stopSubEmitters();
  Particle* _createParticle();
  void _removeFromRoot();
  void _emitFromParticle(Particle* particle);
  // End of sub system methods
  void _update(int newParticles);
  EffectPtr _getEffect(unsigned int blendMode);
  void _appendParticleVertices(unsigned int offset, Particle* particle);
  size_t _render(unsigned int blendMode);

public:
  /**
   * This can help using your own shader to render the particle system.
   * The according effect will be created
   */
  std::string customShader;

  /**
   * This function can be defined to provide custom update for active
   * particles. This function will be called instead of regular update (age,
   * position, color, etc.). Do not forget that this function will be called
   * on every frame so try to keep it simple and fast :)
   */
  std::function<void(std::vector<Particle*>& particles)> updateFunction;

  /**
   * This function can be defined to specify initial direction for every new
   * particle. It by default use the emitterType defined function
   */
  std::function<void(const Matrix& worldMatrix, Vector3& directionToUpdate, Particle* particle)>
    startDirectionFunction;

  /**
   * This function can be defined to specify initial position for every new
   * particle. It by default use the emitterType defined function
   */
  std::function<void(const Matrix& worldMatrix, Vector3& positionToUpdate, Particle* particle)>
    startPositionFunction;

  /**
   * Hidden
   */
  Vector3 _inheritedVelocityOffset;

  /**
   * An event triggered when the system is disposed
   */
  Observable<ParticleSystem> onDisposeObservable;

  /**
   * Sets a callback that will be triggered when the system is disposed
   */
  WriteOnlyProperty<ParticleSystem, std::function<void(ParticleSystem*, EventState&)>> onDispose;

  /** Hidden */
  std::optional<FactorGradient> _currentEmitRateGradient;
  /** Hidden */
  float _currentEmitRate1;
  /** Hidden */
  float _currentEmitRate2;

  /** Hidden */
  std::optional<FactorGradient> _currentStartSizeGradient;
  /** Hidden */
  float _currentStartSize1;
  /** Hidden */
  float _currentStartSize2;

  // Sub-emitters
  /**
   * The Sub-emitters templates that will be used to generate the sub particle
   * system to be associated with the system, this property is used by the root
   * particle system only. When a particle is spawned, an array will be chosen
   * at random and all the emitters in that array will be attached to the
   * particle.  (Default: [])
   */
  std::vector<ParticleSystem*> subEmitters;

  /**
   * Hidden
   * If the particle systems emitter should be disposed when the particle system
   * is disposed
   */
  bool _disposeEmitterOnDispose;

  /**
   * The current active Sub-systems, this property is used by the root
   * particle system only.
   */
  std::vector<ParticleSystem*> activeSubSystems;

private:
  Observer<ParticleSystem>::Ptr _onDisposeObserver;
  std::vector<Particle*> _particles;
  float _epsilon;
  size_t _capacity;
  std::vector<Particle*> _stockParticles;
  int _newPartsExcess;
  Float32Array _vertexData;
  std::unique_ptr<Buffer> _vertexBuffer;
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  std::unique_ptr<Buffer> _spriteBuffer;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  EffectPtr _effect;
  EffectPtr _customEffect;
  std::string _cachedDefines;

  Color4 _scaledColorStep;
  Color4 _colorDiff;
  Vector3 _scaledDirection;
  Vector3 _scaledGravity;
  int _currentRenderId;
  bool _alive;
  bool _useInstancing;

  bool _started;
  bool _stopped;
  int _actualFrame;
  int _scaledUpdateSpeed;
  unsigned int _vertexBufferSize;
  int _rawTextureWidth;
  RawTexturePtr _rampGradientsTexture;
  bool _useRampGradients;

  std::function<void(unsigned int offset, Particle* particle)> _appendParticleVertexes;

  std::vector<std::vector<ParticleSystem*>> _subEmitters;
  ParticleSystem* _rootParticleSystem;
  Vector3 _zeroVector3;

  Matrix _emitterWorldMatrix;

}; // end of class ParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_SYSTEM_H
