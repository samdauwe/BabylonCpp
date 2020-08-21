#ifndef BABYLON_PARTICLES_BASE_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_BASE_PARTICLE_SYSTEM_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/maths/vector3.h>
#include <babylon/particles/iparticle_system.h>

namespace BABYLON {

struct ImageProcessingConfigurationDefines;
class ImageProcessingConfiguration;
class RawTexture;
class Scene;
class ThinEngine;
using ImageProcessingConfigurationPtr        = std::shared_ptr<ImageProcessingConfiguration>;
using ImageProcessingConfigurationDefinesPtr = std::shared_ptr<ImageProcessingConfigurationDefines>;
using RawTexturePtr                          = std::shared_ptr<RawTexture>;

/**
 * @brief This represents the base class for particle system in Babylon.
 * Particles are often small sprites used to simulate hard-to-reproduce phenomena like fire, smoke,
 * water, or abstract visual effects like magic glitter and faery dust. Particles can take different
 * shapes while emitted like box, sphere, cone or you can write your custom function.
 * @example https://doc.babylonjs.com/babylon101/particles
 */
class BABYLON_SHARED_EXPORT BaseParticleSystem : public IParticleSystem {

public:
  /** Statics **/
  /**
   * Source color is added to the destination color without alpha affecting the
   * result
   */
  static constexpr unsigned int BLENDMODE_ONEONE = 0;
  /**
   * Blend current color and particle color using particle’s alpha
   */
  static constexpr unsigned int BLENDMODE_STANDARD = 1;

  /**
   * Add current color and particle color multiplied by particle’s alpha
   */
  static constexpr unsigned int BLENDMODE_ADD = 2;
  /**
   * Multiply current color with particle color
   */
  static constexpr unsigned int BLENDMODE_MULTIPLY = 3;

  /**
   * Multiply current color with particle color then add current color and
   * particle color multiplied by particle’s alpha
   */
  static constexpr unsigned int BLENDMODE_MULTIPLYADD = 4;

public:
  /**
   * @brief Instantiates a particle system.
   * Particles are often small sprites used to simulate hard-to-reproduce
   * phenomena like fire, smoke, water, or abstract visual effects like magic
   * glitter and faery dust.
   * @param name The name of the particle system
   */
  BaseParticleSystem(const std::string& name);
  ~BaseParticleSystem() override; // = default

  /**
   * @brief Returns whether or not the particle system has an emitter.
   * @return Whether or not the particle system has an emitter
   */
  [[nodiscard]] bool hasEmitter() const override;

  /**
   * @brief Get hosting scene.
   * @returns the scene
   */
  [[nodiscard]] Scene* getScene() const override;

  /**
   * @brief Gets the current list of drag gradients.
   * You must use addDragGradient and removeDragGradient to udpate this list
   * @returns the list of drag gradients
   */
  std::vector<FactorGradient>& getDragGradients() override;

  /**
   * @brief Gets the current list of limit velocity gradients.
   * You must use addLimitVelocityGradient and removeLimitVelocityGradient to
   * udpate this list
   * @returns the list of limit velocity gradients
   */
  std::vector<FactorGradient>& getLimitVelocityGradients() override;

  /**
   * @brief Gets the current list of color gradients.
   * You must use addColorGradient and removeColorGradient to udpate this list
   * @returns the list of color gradients
   */
  std::vector<ColorGradient>& getColorGradients() override;

  /**
   * @brief Gets the current list of size gradients.
   * You must use addSizeGradient and removeSizeGradient to udpate this list
   * @returns the list of size gradients
   */
  std::vector<FactorGradient>& getSizeGradients() override;

  /**
   * @brief Gets the current list of color remap gradients.
   * You must use addColorRemapGradient and removeColorRemapGradient to udpate
   * this list
   * @returns the list of color remap gradients
   */
  std::vector<FactorGradient>& getColorRemapGradients() override;

  /**
   * @brief Gets the current list of alpha remap gradients.
   * You must use addAlphaRemapGradient and removeAlphaRemapGradient to udpate
   * this list
   * @returns the list of alpha remap gradients
   */
  std::vector<FactorGradient>& getAlphaRemapGradients() override;

  /**
   * @brief Gets the current list of life time gradients.
   * You must use addLifeTimeGradient and removeLifeTimeGradient to udpate this
   * list
   * @returns the list of life time gradients
   */
  std::vector<FactorGradient>& getLifeTimeGradients() override;

  /**
   * @brief Gets the current list of angular speed gradients.
   * You must use addAngularSpeedGradient and removeAngularSpeedGradient to
   * udpate this list
   * @returns the list of angular speed gradients
   */
  std::vector<FactorGradient>& getAngularSpeedGradients() override;

  /**
   * @brief Gets the current list of velocity gradients.
   * You must use addVelocityGradient and removeVelocityGradient to udpate this
   * list
   * @returns the list of velocity gradients
   */
  std::vector<FactorGradient>& getVelocityGradients() override;

  /**
   * @brief Gets the current list of start size gradients.
   * You must use addStartSizeGradient and removeStartSizeGradient to udpate
   * this list
   * @returns the list of start size gradients
   */
  std::vector<FactorGradient>& getStartSizeGradients() override;

  /**
   * @brief Gets the current list of emit rate gradients.
   * You must use addEmitRateGradient and removeEmitRateGradient to udpate this
   * list
   * @returns the list of emit rate gradients
   */
  std::vector<FactorGradient>& getEmitRateGradients() override;

  /**
   * @brief Creates a Point Emitter for the particle system (emits directly from
   * the emitter position).
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @returns the emitter
   */
  PointParticleEmitterPtr createPointEmitter(const Vector3& direction1,
                                             const Vector3& direction2) override;

  /**
   * @brief Creates a Hemisphere Emitter for the particle system (emits along
   * the hemisphere radius).
   * @param radius The radius of the hemisphere to emit from
   * @param radiusRange The range of the hemisphere to emit from [0-1] 0 Surface
   * Only, 1 Entire Radius
   * @returns the emitter
   */
  HemisphericParticleEmitterPtr createHemisphericEmitter(float radius      = 1.f,
                                                         float radiusRange = 1.f) override;

  /**
   * @brief Creates a Sphere Emitter for the particle system (emits along the
   * sphere radius).
   * @param radius The radius of the sphere to emit from
   * @param radiusRange The range of the sphere to emit from [0-1] 0 Surface
   * Only, 1 Entire Radius
   * @returns the emitter
   */
  SphereParticleEmitterPtr createSphereEmitter(float radius      = 1.f,
                                               float radiusRange = 1.f) override;

  /**
   * @brief Creates a Directed Sphere Emitter for the particle system (emits
   * between direction1 and direction2).
   * @param radius The radius of the sphere to emit from
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the sphere
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the sphere
   * @returns the emitter
   */
  SphereDirectedParticleEmitterPtr
  createDirectedSphereEmitter(float radius              = 1.f,
                              const Vector3& direction1 = Vector3(0.f, 1.f, 0.f),
                              const Vector3& direction2 = Vector3(0.f, 1.f, 0.f)) override;

  /**
   * @brief Creates a Cylinder Emitter for the particle system (emits from the
   * cylinder to the particle position).
   * @param radius The radius of the emission cylinder
   * @param height The height of the emission cylinder
   * @param radiusRange The range of emission [0-1] 0 Surface only, 1 Entire
   * Radius
   * @param directionRandomizer How much to randomize the particle direction
   * [0-1]
   * @returns the emitter
   */
  CylinderParticleEmitterPtr createCylinderEmitter(float radius = 1.f, float height = 1.f,
                                                   float radiusRange         = 1.f,
                                                   float directionRandomizer = 0.f) override;

  /**
   * @brief Creates a Cone Emitter for the particle system (emits from the cone
   * to the particle position).
   * @param radius The radius of the cone to emit from
   * @param angle The base angle of the cone
   * @returns the emitter
   */
  ConeParticleEmitterPtr createConeEmitter(float radius = 1.f, float angle = Math::PI_4) override;

  /**
   * @brief Creates a Directed Cylinder Emitter for the particle system (emits
   * between direction1 and direction2).
   * @param radius The radius of the cylinder to emit from
   * @param height The height of the emission cylinder
   * @param radiusRange the range of the emission cylinder [0-1] 0 Surface only,
   * 1 Entire Radius (1 by default)
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the cylinder
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the cylinder
   * @returns the emitter
   */
  CylinderDirectedParticleEmitterPtr
  createDirectedCylinderEmitter(float radius = 1.f, float height = 1.f, float radiusRange = 1.f,
                                const Vector3& direction1 = Vector3(0.f, 1.f, 0.f),
                                const Vector3& direction2 = Vector3(0.f, 1.f, 0.f)) override;

  /**
   * @brief Creates a Box Emitter for the particle system. (emits between
   * direction1 and direction2 from withing the box defined by minEmitBox
   * and maxEmitBox)
   * @param direction1 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @param direction2 Particles are emitted between the direction1 and
   * direction2 from within the box
   * @param minEmitBox Particles are emitted from the box between minEmitBox
   * and maxEmitBox
   * @param maxEmitBox  Particles are emitted from the box between
   * minEmitBox and maxEmitBox
   * @returns the emitter
   */
  BoxParticleEmitterPtr createBoxEmitter(const Vector3& direction1, const Vector3& direction2,
                                         const Vector3& minEmitBox,
                                         const Vector3& maxEmitBox) override;

protected:
  /**
   * @brief Gets a texture used to add random noise to particle positions.
   */
  ProceduralTexturePtr& get_noiseTexture() override;

  /**
   * @brief Sets a texture used to add random noise to particle positions.
   */
  void set_noiseTexture(const ProceduralTexturePtr& value) override;

  /**
   * @brief Gets whether an animation sprite sheet is enabled or not on the
   * particle system.
   */
  [[nodiscard]] bool get_isAnimationSheetEnabled() const override;

  /**
   * @brief Sets whether an animation sprite sheet is enabled or not on the
   * particle system.
   */
  void set_isAnimationSheetEnabled(bool value) override;

  /**
   * @brief Hidden
   */
  [[nodiscard]] bool _hasTargetStopDurationDependantGradient() const;

  /**
   * @brief Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors. This only works when particleEmitterTyps
   * is a BoxParticleEmitter.
   */
  Vector3& get_direction1();
  void set_direction1(const Vector3& value);

  /**
   * @brief Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors. This only works when particleEmitterTyps
   * is a BoxParticleEmitter.
   */
  Vector3& get_direction2();
  void set_direction2(const Vector3& value);

  /**
   * @brief Minimum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so. This only works when
   * particleEmitterTyps is a BoxParticleEmitter.
   */
  Vector3& get_minEmitBox();
  void set_minEmitBox(const Vector3& value);

  /**
   * @brief Maximum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than one
   * point, then you can tell it to do so. This only works when
   * particleEmitterTyps is a BoxParticleEmitter.
   */
  Vector3& get_maxEmitBox();
  void set_maxEmitBox(const Vector3& value);

  /**
   * @brief Gets or sets a boolean indicating if the particles must be rendered
   * as billboard or aligned with the direction.
   */
  [[nodiscard]] bool get_isBillboardBased() const override;

  /**
   * @brief Sets a boolean indicating if the particles must be rendered as
   * billboard or aligned with the direction.
   */
  void set_isBillboardBased(bool value) override;

  /**
   * @brief Gets the image processing configuration used either in this
   * material.
   */
  ImageProcessingConfigurationPtr& get_imageProcessingConfiguration();

  /**
   * @brief Sets the Default image processing configuration used either in the
   * this material.
   *
   * If sets to null, the scene one is in use.
   */
  void set_imageProcessingConfiguration(const ImageProcessingConfigurationPtr& value);

  /**
   * @brief Attaches a new image processing configuration to the Standard
   * Material.
   * @param configuration
   */
  void _attachImageProcessingConfiguration(const ImageProcessingConfigurationPtr& configuration);

  /**
   * Hidden
   */
  virtual void _reset();

  /**
   * Hidden
   */
  virtual BaseParticleSystem& _removeGradientAndTexture(float gradient,
                                                        std::vector<ColorGradient>& gradients,
                                                        const RawTexturePtr& texture);

  /**
   * Hidden
   */
  virtual BaseParticleSystem& _removeGradientAndTexture(float gradient,
                                                        std::vector<Color3Gradient>& gradients,
                                                        const RawTexturePtr& texture);

  /**
   * Hidden
   */
  virtual BaseParticleSystem& _removeGradientAndTexture(float gradient,
                                                        std::vector<FactorGradient>& gradients,
                                                        const RawTexturePtr& texture);

public:
  /**
   * Gets or sets the unique id of the particle system
   */
  size_t uniqueId;

  /**
   * If you want to launch only a few particles at once, that can be done, as well.
   */
  int manualEmitCount;

  /**
   * By default particle system starts as soon as they are created. This prevents the automatic
   * start to happen and let you decide when to start emitting particles.
   */
  bool preventAutoStart;

  /**
   * Callback triggered when the particle animation is ending.
   */
  std::function<void()> onAnimationEnd;

  /**
   * Forces the particle to write their depth information to the depth buffer.
   * This can help preventing other draw calls to override the particles.
   */
  bool forceDepthWrite;

  /**
   * Gets or sets a world offset applied to all particles
   */
  Vector3 worldOffset;

  /**
   * An optional mask to filter some colors out of the texture, or filter a part of the alpha
   * channel
   */
  Color4 textureMask;

  /**
   * Random direction of each particle after it has been emitted, between direction1 and direction2
   * vectors. This only works when particleEmitterTyps is a BoxParticleEmitter.
   */
  Property<BaseParticleSystem, Vector3> direction1;

  /**
   * Random direction of each particle after it has been emitted, between direction1 and direction2
   * vectors. This only works when particleEmitterTyps is a BoxParticleEmitter.
   */
  Property<BaseParticleSystem, Vector3> direction2;

  /**
   * Minimum box point around our emitter. Our emitter is the center of particles source, but if you
   * want your particles to emit from more than one point, then you can tell it to do so. This only
   * works when particleEmitterTyps is a BoxParticleEmitter.
   */
  Property<BaseParticleSystem, Vector3> minEmitBox;

  /**
   * Maximum box point around our emitter. Our emitter is the center of particles source, but if you
   * want your particles to emit from more than one point, then you can tell it to do so. This only
   * works when particleEmitterTyps is a BoxParticleEmitter.
   */
  Property<BaseParticleSystem, Vector3> maxEmitBox;

  /**
   * The image processing configuration used either in this material.
   */
  Property<BaseParticleSystem, Vector3> imageProcessingConfiguration;

  /**
   * Hidden
   */
  bool _isSubEmitter;

protected:
  /** Hidden */
  bool _isAnimationSheetEnabled;

  /** Hidden */
  bool _isBillboardBased;

  /**
   * The scene the particle system belongs to.
   */
  Scene* _scene;

  /**
   * The engine the particle system belongs to.
   */
  ThinEngine* _engine;

  /**
   * Local cache of defines for image processing.
   */
  ImageProcessingConfigurationDefinesPtr _imageProcessingConfigurationDefines;

  /**
   * Default configuration related to image processing available in the standard Material.
   */
  ImageProcessingConfigurationPtr _imageProcessingConfiguration;

  std::vector<ColorGradient> _colorGradients;
  std::vector<FactorGradient> _sizeGradients;
  std::vector<FactorGradient> _lifeTimeGradients;
  std::vector<FactorGradient> _angularSpeedGradients;
  std::vector<FactorGradient> _velocityGradients;
  std::vector<FactorGradient> _limitVelocityGradients;
  std::vector<FactorGradient> _dragGradients;
  std::vector<FactorGradient> _emitRateGradients;
  std::vector<FactorGradient> _startSizeGradients;
  std::vector<Color3Gradient> _rampGradients;
  std::vector<FactorGradient> _colorRemapGradients;
  std::vector<FactorGradient> _alphaRemapGradients;

private:
  ProceduralTexturePtr _noiseTexture;
  Vector3 _zeroVector3;

}; // end of struct BaseParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_BASE_PARTICLE_SYSTEM_H
