#ifndef BABYLON_PARTICLES_EMITTER_TYPES_SPHERE_DIRECTED_PARTICLE_EMITTER_H
#define BABYLON_PARTICLES_EMITTER_TYPES_SPHERE_DIRECTED_PARTICLE_EMITTER_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/emittertypes/sphere_particle_emitter.h>

namespace BABYLON {

/**
 * @brief Particle emitter emitting particles from the inside of a sphere.
 * It emits the particles randomly between two vectors.
 */
class BABYLON_SHARED_EXPORT SphereDirectedParticleEmitter
    : public SphereParticleEmitter {

public:
  /**
   * @brief Creates a new instance of SphereDirectedParticleEmitter.
   * @param radius the radius of the emission sphere (1 by default)
   * @param direction1 the min limit of the emission direction (up vector by
   * default)
   * @param direction2 the max limit of the emission direction (up vector by
   * default)
   */
  SphereDirectedParticleEmitter(float radius = 1.f,
                                const Vector3& direction1
                                = Vector3{0.f, 1.f, 0.f},
                                const Vector3& direction2
                                = Vector3{0.f, 1.f, 0.f});
  ~SphereDirectedParticleEmitter() override;

  /**
   * @brief Called by the particle System when the direction is computed for the
   * created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   */
  void startDirectionFunction(const Matrix& worldMatrix,
                              Vector3& directionToUpdate,
                              Particle* particle) override;

  /**
   * @brief Clones the current emitter and returns a copy of it
   * @returns the new emitter
   */
  unique_ptr_t<IParticleEmitterType> clone() const override;

  /**
   * @brief Called by the GPUParticleSystem to setup the update shader
   * @param effect defines the update shader
   */
  void applyToShader(Effect* effect) override;

  /**
   * @brief Returns a string to use to update the GPU particles update shader
   * @returns a string containng the defines string
   */
  const char* getEffectDefines() const override;

  /**
   * @brief Returns the string "SphereDirectedParticleEmitter"
   * @returns a string containing the class name
   */
  const char* getClassName() const override;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  Json::object serialize() const override;

  /**
   * @brief Parse properties from a JSON object
   * @param serializationObject defines the JSON object
   */
  void parse(const Json::value& serializationObject) override;

public:
  /**
   * The min limit of the emission direction.
   */
  Vector3 direction1;
  /**
   * The max limit of the emission direction.
   */
  Vector3 direction2;

}; // end of class SphereDirectedParticleEmitter

} // end of namespace BABYLON

#endif // end of
       // BABYLON_PARTICLES_EMITTER_TYPES_SPHERE_DIRECTED_PARTICLE_EMITTER_H
