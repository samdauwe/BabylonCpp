#ifndef BABYLON_PARTICLES_PARTICLE_H
#define BABYLON_PARTICLES_PARTICLE_H

#include <babylon/babylon_api.h>
#include <babylon/maths/color4.h>
#include <babylon/maths/vector2.h>
#include <babylon/maths/vector3.h>
#include <babylon/maths/vector4.h>
#include <babylon/misc/color_gradient.h>
#include <babylon/misc/factor_gradient.h>

namespace BABYLON {

class ParticleSystem;
class SubEmitter;
using SubEmitterPtr = std::shared_ptr<SubEmitter>;

/**
 * @brief A particle represents one of the element emitted by a particle system.
 * This is mainly define by its coordinates, direction, velocity and age.
 */
class BABYLON_SHARED_EXPORT Particle {

private:
  static size_t _Count;

public:
  /**
   * @brief Creates a new instance Particle.
   * @param particleSystem the particle system the particle belongs to
   */
  Particle(ParticleSystem* particleSystem);
  Particle(const Particle& other);            // Copy constructor
  Particle(Particle&& other);                 // Move constructor
  Particle& operator=(const Particle& other); // Copy assignment operator
  Particle& operator=(Particle&& other);      // Move assignment operator
  ~Particle();                                // = default
  [[nodiscard]] Particle* clone() const;

  /**
   * @brief Defines how the sprite cell index is updated for the particle.
   */
  void updateCellIndex();

  /**
   * @brief Hidden
   */
  void _inheritParticleInfoToSubEmitter(const SubEmitterPtr& subEmitter);

  /**
   * @brief Hidden
   */
  void _inheritParticleInfoToSubEmitters();

  /**
   * @brief Hidden
   */
  void _reset();

  /**
   * @brief Copy the properties of particle to another one.
   * @param other the particle to copy the information to.
   */
  void copyTo(Particle& other);

private:
  void updateCellInfoFromSystem();

public:
  /**
   * Unique ID of the particle
   */
  size_t id;

  /**
   * The world position of the particle in the scene.
   */
  Vector3 position;

  /**
   * The world direction of the particle in the scene.
   */
  Vector3 direction;

  /**
   * The color of the particle.
   */
  Color4 color;

  /**
   * The color change of the particle per step.
   */
  Color4 colorStep;

  /**
   * Defines how long will the life of the particle be.
   */
  float lifeTime;

  /**
   * The current age of the particle.
   */
  float age;

  /**
   * The current size of the particle.
   */
  float size;

  /**
   * The current scale of the particle.
   */
  Vector2 scale;

  /**
   * The current angle of the particle.
   */
  float angle;

  /**
   * Defines how fast is the angle changing.
   */
  float angularSpeed;

  /**
   * Defines the cell index used by the particle to be rendered from a sprite.
   */
  unsigned int cellIndex;

  /**
   * The information required to support color remapping
   */
  Vector4 remapData;

  /** @hidden */
  std::optional<float> _randomCellOffset;

  /** Hidden */
  std::optional<Vector3> _initialDirection;

  /** Hidden */
  std::vector<SubEmitterPtr> _attachedSubEmitters;

  /** Hidden */
  unsigned int _initialStartSpriteCellID;
  /** Hidden */
  unsigned int _initialEndSpriteCellID;

  /** Hidden */
  std::optional<ColorGradient> _currentColorGradient;
  /** Hidden */
  Color4 _currentColor1;
  /** Hidden */
  Color4 _currentColor2;

  /** Hidden */
  std::optional<FactorGradient> _currentSizeGradient;
  /** Hidden */
  float _currentSize1;
  /** Hidden */
  float _currentSize2;

  /** Hidden */
  std::optional<FactorGradient> _currentAngularSpeedGradient;
  /** Hidden */
  float _currentAngularSpeed1;
  /** Hidden */
  float _currentAngularSpeed2;

  /** Hidden */
  std::optional<FactorGradient> _currentVelocityGradient;
  /** Hidden */
  float _currentVelocity1;
  /** Hidden */
  float _currentVelocity2;

  /** Hidden */
  std::optional<FactorGradient> _currentLimitVelocityGradient;
  /** Hidden */
  float _currentLimitVelocity1;
  /** Hidden */
  float _currentLimitVelocity2;

  /** Hidden */
  std::optional<FactorGradient> _currentDragGradient;
  /** Hidden */
  float _currentDrag1;
  /** Hidden */
  float _currentDrag2;

  /** Hidden */
  std::optional<Vector3> _randomNoiseCoordinates1;
  /** Hidden */
  Vector3 _randomNoiseCoordinates2;

  /**
   * The particle system the particle belongs to.
   */
  ParticleSystem* particleSystem;

private:
  unsigned int _currentFrameCounter;

}; // end of class Particle

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_H
