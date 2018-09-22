#ifndef BABYLON_PARTICLES_BASE_PARTICLE_SYSTEM_H
#define BABYLON_PARTICLES_BASE_PARTICLE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/iparticle_system.h>

namespace BABYLON {

class BoxParticleEmitter;
class ConeParticleEmitter;
class CylinderParticleEmitter;
class HemisphericParticleEmitter;
struct ImageProcessingConfigurationDefines;
class PointParticleEmitter;
class SphereDirectedParticleEmitter;
class SphereParticleEmitter;
using BoxParticleEmitterPtr      = std::shared_ptr<BoxParticleEmitter>;
using ConeParticleEmitterPtr     = std::shared_ptr<ConeParticleEmitter>;
using CylinderParticleEmitterPtr = std::shared_ptr<CylinderParticleEmitter>;
using HemisphericParticleEmitterPtr
  = std::shared_ptr<HemisphericParticleEmitter>;
using ImageProcessingConfigurationPtr
  = std::shared_ptr<ImageProcessingConfiguration>;
using ImageProcessingConfigurationDefinesPtr
  = std::shared_ptr<ImageProcessingConfigurationDefines>;
using PointParticleEmitterPtr = std::shared_ptr<PointParticleEmitter>;
using SphereDirectedParticleEmitterPtr
  = std::shared_ptr<SphereDirectedParticleEmitter>;
using SphereParticleEmitterPtr = std::shared_ptr<SphereParticleEmitter>;

/**
 * @brief This represents the base class for particle system in Babylon.
 * Particles are often small sprites used to simulate hard-to-reproduce
 * phenomena like fire, smoke, water, or abstract visual effects like magic
 * glitter and faery dust. Particles can take different shapes while emitted
 * like box, sphere, cone or you can write your custom function.
 * Example https://doc.babylonjs.com/babylon101/particles
 */
class BABYLON_SHARED_EXPORT BaseParticleSystem : public IParticleSystem {

public:
  /** Statics **/
  /**
   * Source color is added to the destination color without alpha affecting the
   * result.
   */
  static constexpr int BLENDMODE_ONEONE = 0;
  /**
   * Blend current color and particle color using particle’s alpha.
   */
  static constexpr int BLENDMODE_STANDARD = 1;

  /**
   * Add current color and particle color multiplied by particle’s alpha.
   */
  static constexpr int BLENDMODE_ADD = 2;

public:
  /**
   * @brief Instantiates a particle system.
   * Particles are often small sprites used to simulate hard-to-reproduce
   * phenomena like fire, smoke, water, or abstract visual effects like magic
   * glitter and faery dust.
   * @param name The name of the particle system
   */
  BaseParticleSystem(const string_t& name);
  ~BaseParticleSystem();

  /**
   * @brief Gets the current list of drag gradients.
   * You must use addDragGradient and removeDragGradient to udpate this list
   * @returns the list of drag gradients
   */
  vector_t<FactorGradient>& getDragGradients();

  /**
   * @brief Gets the current list of limit velocity gradients.
   * You must use addLimitVelocityGradient and removeLimitVelocityGradient to
   * udpate this list
   * @returns the list of limit velocity gradients
   */
  vector_t<FactorGradient>& getLimitVelocityGradients();

  /**
   * @brief Gets the current list of color gradients.
   * You must use addColorGradient and removeColorGradient to udpate this list
   * @returns the list of color gradients
   */
  vector_t<ColorGradient>& getColorGradients();

  /**
   * @brief Gets the current list of size gradients.
   * You must use addSizeGradient and removeSizeGradient to udpate this list
   * @returns the list of size gradients
   */
  vector_t<FactorGradient>& getSizeGradients();

  /**
   * @brief Gets the current list of life time gradients.
   * You must use addLifeTimeGradient and removeLifeTimeGradient to udpate this
   * list
   * @returns the list of life time gradients
   */
  vector_t<FactorGradient>& getLifeTimeGradients();

  /**
   * @brief Gets the current list of angular speed gradients.
   * You must use addAngularSpeedGradient and removeAngularSpeedGradient to
   * udpate this list
   * @returns the list of angular speed gradients
   */
  vector_t<FactorGradient>& getAngularSpeedGradients();

  /**
   * @brief Gets the current list of velocity gradients.
   * You must use addVelocityGradient and removeVelocityGradient to udpate this
   * list
   * @returns the list of velocity gradients
   */
  vector_t<FactorGradient>& getVelocityGradients();

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
                                             const Vector3& direction2);

  /**
   * @brief Creates a Hemisphere Emitter for the particle system (emits along
   * the hemisphere radius).
   * @param radius The radius of the hemisphere to emit from
   * @param radiusRange The range of the hemisphere to emit from [0-1] 0 Surface
   * Only, 1 Entire Radius
   * @returns the emitter
   */
  HemisphericParticleEmitterPtr
  createHemisphericEmitter(float radius = 1.f, float radiusRange = 1.f);

  /**
   * @brief Creates a Sphere Emitter for the particle system (emits along the
   * sphere radius).
   * @param radius The radius of the sphere to emit from
   * @param radiusRange The range of the sphere to emit from [0-1] 0 Surface
   * Only, 1 Entire Radius
   * @returns the emitter
   */
  SphereParticleEmitterPtr createSphereEmitter(float radius      = 1.f,
                                               float radiusRange = 1.f);

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
  SphereDirectedParticleEmitterPtr createDirectedSphereEmitter(
    float radius = 1.f, const Vector3& direction1 = Vector3(0.f, 1.f, 0.f),
    const Vector3& direction2 = Vector3(0.f, 1.f, 0.f));

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
  CylinderParticleEmitterPtr
  createCylinderEmitter(float radius = 1.f, float height = 1.f,
                        float radiusRange         = 1.f,
                        float directionRandomizer = 0.f);

  /**
   * @brief Creates a Cone Emitter for the particle system (emits from the cone
   * to the particle position).
   * @param radius The radius of the cone to emit from
   * @param angle The base angle of the cone
   * @returns the emitter
   */
  ConeParticleEmitterPtr createConeEmitter(float radius = 1.f,
                                           float angle  = Math::PI_4);

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
  BoxParticleEmitterPtr createBoxEmitter(const Vector3& direction1,
                                         const Vector3& direction2,
                                         const Vector3& minEmitBox,
                                         const Vector3& maxEmitBox);

protected:
  /**
   * @brief Gets whether an animation sprite sheet is enabled or not on the
   * particle system.
   */
  bool get_isAnimationSheetEnabled() const;

  /**
   * @brief Sets whether an animation sprite sheet is enabled or not on the
   * particle system.
   */
  void set_isAnimationSheetEnabled(bool value);

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
  bool get_isBillboardBased() const;

  /**
   * @brief Sets a boolean indicating if the particles must be rendered as
   * billboard or aligned with the direction.
   */
  void set_isBillboardBased(bool value);

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
  void set_imageProcessingConfiguration(
    const ImageProcessingConfigurationPtr& value);

  /**
   * @brief Attaches a new image processing configuration to the Standard
   * Material.
   * @param configuration
   */
  void _attachImageProcessingConfiguration(
    const ImageProcessingConfigurationPtr& configuration);

  /**
   * Hidden
   */
  virtual void _reset();

public:
  /**
   * If you want to launch only a few particles at once, that can be done, as
   * well.
   */
  int manualEmitCount;

  /**
   * Specifies whether the particle system will be disposed once it reaches the
   * end of the animation.
   */
  bool disposeOnStop;

  /**
   * By default particle system starts as soon as they are created. This
   * prevents the automatic start to happen and let you decide when to start
   * emitting particles.
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
   * An optional mask to filter some colors out of the texture, or filter a part
   * of the alpha channel
   */
  Color4 textureMask;

  /**
   * Gets or sets whether an animation sprite sheet is enabled or not on the
   * particle system
   */
  Property<BaseParticleSystem, bool> isAnimationSheetEnabled;

  /**
   * Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors. This only works when
   * particleEmitterTyps is a BoxParticleEmitter.
   */
  Property<BaseParticleSystem, Vector3> direction1;

  /**
   * Random direction of each particle after it has been emitted, between
   * direction1 and direction2 vectors. This only works when
   * particleEmitterTyps is a BoxParticleEmitter.
   */
  Property<BaseParticleSystem, Vector3> direction2;

  /**
   * Minimum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than
   * one point, then you can tell it to do so. This only works when
   * particleEmitterTyps is a BoxParticleEmitter.
   */
  Property<BaseParticleSystem, Vector3> minEmitBox;

  /**
   * Maximum box point around our emitter. Our emitter is the center of
   * particles source, but if you want your particles to emit from more than
   * one point, then you can tell it to do so. This only works when
   * particleEmitterTyps is a BoxParticleEmitter.
   */
  Property<BaseParticleSystem, Vector3> maxEmitBox;

  /**
   * The image processing configuration used either in this material.
   */
  Property<BaseParticleSystem, Vector3> imageProcessingConfiguration;

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
   * Local cache of defines for image processing.
   */
  ImageProcessingConfigurationDefinesPtr _imageProcessingConfigurationDefines;

  /**
   * Default configuration related to image processing available in the standard
   * Material.
   */
  ImageProcessingConfigurationPtr _imageProcessingConfiguration;

  vector_t<ColorGradient> _colorGradients;
  vector_t<FactorGradient> _sizeGradients;
  vector_t<FactorGradient> _lifeTimeGradients;
  vector_t<FactorGradient> _angularSpeedGradients;
  vector_t<FactorGradient> _velocityGradients;
  vector_t<FactorGradient> _limitVelocityGradients;
  vector_t<FactorGradient> _dragGradients;

private:
  Vector3 _zeroVector3;

}; // end of struct BaseParticleSystem

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_BASE_PARTICLE_SYSTEM_H
