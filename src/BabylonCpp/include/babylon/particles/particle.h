#ifndef BABYLON_PARTICLES_PARTICLE_H
#define BABYLON_PARTICLES_PARTICLE_H

#include <babylon/babylon_global.h>

#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief A particle represents one of the element emitted by a particle system.
 * This is mainly define by its coordinates, direction, velocity and age.
 */
class BABYLON_SHARED_EXPORT Particle {

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
  ~Particle();                                // Destructor
  Particle* clone() const;

  /**
   * @brief Copy the properties of particle to another one.
   * @param other the particle to copy the information to.
   */
  void copyTo(Particle& other);

private:
  void updateCellInfoFromSystem();
  void _updateCellIndexWithSpeedCalculated(float scaledUpdateSpeed);
  void _updateCellIndexWithCustomSpeed();

public:
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
   * Defines how the sprite cell index is updated for the particle. This is
   * defined as a callback.
   */
  ::std::function<void(float scaledUpdateSpeed)> updateCellIndex;

  /**
   * The particle system the particle belongs to.
   */
  ParticleSystem* particleSystem;

private:
  unsigned int _currentFrameCounter;

}; // end of class Particle

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_H
