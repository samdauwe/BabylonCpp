#ifndef BABYLON_PARTICLES_PARTICLE_H
#define BABYLON_PARTICLES_PARTICLE_H

#include <babylon/babylon_global.h>

#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Particle {

public:
  Particle();
  Particle(const Particle& other);            // Copy constructor
  Particle(Particle&& other);                 // Move constructor
  Particle& operator=(const Particle& other); // Copy assignment operator
  Particle& operator=(Particle&& other);      // Move assignment operator
  ~Particle();                                // Destructor
  Particle* clone() const;

  void copyTo(Particle& other);

public:
  Vector3 position;
  Vector3 direction;
  Color4 color;
  Color4 colorStep;
  float lifeTime;
  float age;
  float size;
  float angle;
  float angularSpeed;

}; // end of class Particle

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_H