#ifndef BABYLON_HELPERS_IPARTICLE_SYSTEM_DATA_H
#define BABYLON_HELPERS_IPARTICLE_SYSTEM_DATA_H

#include <babylon/babylon_global.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector3.h>

#include <babylon/core/nullable.h>

namespace BABYLON {

/**
 * @brief Represents all the data needed to create a ParticleSystem.
 */
struct BABYLON_SHARED_EXPORT IParticleSystemData {
  /**
   * ParticleSystem type
   */
  string_t type;
  /**
   * Shape of the emitter
   */
  string_t emitterType;
  /**
   * Maximum number of particles in the system
   */
  size_t capacity;
  /**
   * Link for the texture file
   */
  string_t textureFile;
  /**
   * minEmitBox Vector3
   */
  Nullable<Vector3> minEmitBox = nullptr;
  /**
   * maxEmitBox Vector3
   */
  Nullable<Vector3> maxEmitBox = nullptr;
  /**
   * color1 Color4
   */
  Color4 color1;
  /**
   * color2 Color4
   */
  Color4 color2;
  /**
   * colorDead Color4
   */
  Color4 colorDead;
  /**
   * Minimum size of each particle
   */
  float minSize;
  /**
   * Maximum size of each particle
   */
  float maxSize;
  /**
   * Minimum lifetime for each particle
   */
  float minLifeTime;
  /**
   * Maximum lifetime for each particle
   */
  float maxLifeTime;
  /**
   * Emit rate
   */
  int emitRate;
  /**
   * Blend Mode
   */
  unsigned int blendMode;
  /**
   * gravity Vector3
   */
  Vector3 gravity;
  /**
   * direction1 Vector3
   */
  Nullable<Vector3> direction1 = nullptr;
  /**
   * direction2 Vector3
   */
  Nullable<Vector3> direction2 = nullptr;
  /**
   * Minimum Angular Speed
   */
  float minAngularSpeed;
  /**
   * Maximum Angular Speed
   */
  float maxAngularSpeed;
  /**
   * Minimum Emit Power
   */
  float minEmitPower;
  /**
   * Maximum Emit Power
   */
  float maxEmitPower;
  /**
   * Update Speed
   */
  float updateSpeed;
  /**
   * Radius
   */
  Nullable<float> radius = nullptr;
  /**
   * Angle
   */
  Nullable<float> angle = nullptr;
}; // end of struct IParticleSystemData

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_IPARTICLE_SYSTEM_DATA_H
