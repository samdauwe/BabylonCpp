#ifndef BABYLON_PARTICLES_EMITTER_TYPES_CONE_PARTICLE_EMITTER_H
#define BABYLON_PARTICLES_EMITTER_TYPES_CONE_PARTICLE_EMITTER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_constants.h>
#include <babylon/particles/emittertypes/iparticle_emitter_type.h>

namespace BABYLON {

/**
 * @brief Particle emitter emitting particles from the inside of a cone.
 * It emits the particles alongside the cone volume from the base to the particle.
 * The emission direction might be randomized.
 */
class BABYLON_SHARED_EXPORT ConeParticleEmitter : public IParticleEmitterType {

public:
  /**
   * @brief Creates a new instance ConeParticleEmitter.
   * @param radius the radius of the emission cone (1 by default)
   * @param angle the cone base angle (PI by default)
   * @param directionRandomizer defines how much to randomize the particle direction [0-1] (default
   * is 0)
   */
  ConeParticleEmitter(float radius = 1.f, float angle = Math::PI, float directionRandomizer = 0.f);
  ~ConeParticleEmitter() override; // = default

  /**
   * @brief Called by the particle System when the direction is computed for the created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param directionToUpdate is the direction vector to update with the result
   * @param particle is the particle we are computed the direction for
   * @param isLocal defines if the direction should be set in local space
   */
  void startDirectionFunction(const Matrix& worldMatrix, Vector3& directionToUpdate,
                              Particle* particle, bool isLocal) override;

  /**
   * @brief Called by the particle System when the position is computed for the created particle.
   * @param worldMatrix is the world matrix of the particle system
   * @param positionToUpdate is the position vector to update with the result
   * @param particle is the particle we are computed the position for
   * @param isLocal defines if the position should be set in local space
   */
  void startPositionFunction(const Matrix& worldMatrix, Vector3& positionToUpdate,
                             Particle* particle, bool isLocal) override;

  /**
   * @brief Clones the current emitter and returns a copy of it.
   * @returns the new emitter
   */
  [[nodiscard]] std::unique_ptr<IParticleEmitterType> clone() const override;

  /**
   * @brief Called by the GPUParticleSystem to setup the update shader
   * @param effect defines the update shader
   */
  void applyToShader(Effect* effect) override;

  /**
   * @brief Returns a string to use to update the GPU particles update shader.
   * @returns a string containng the defines string
   */
  [[nodiscard]] std::string getEffectDefines() const override;

  /**
   * @brief Returns the string "ConeParticleEmitter".
   * @returns a string containing the class name
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Serializes the particle system to a JSON object.
   * @returns the JSON object
   */
  [[nodiscard]] json serialize() const override;

  /**
   * @brief Parse properties from a JSON object.
   * @param serializationObject defines the JSON object
   * @param scene defines the hosting scene
   */
  void parse(const json& serializationObject, Scene* scene) override;

protected:
  /**
   * @brief Gets the radius of the emission cone.
   */
  [[nodiscard]] float get_radius() const;

  /**
   * @brief Sets the radius of the emission cone.
   */
  void set_radius(float value);

  /**
   * @brief Gets the angle of the emission cone.
   */
  [[nodiscard]] float get_angle() const;

  /**
   * @brief Sets the angle of the emission cone.
   */
  void set_angle(float value);

private:
  void _buildHeight();

public:
  /**
   * Gets or sets a value indicating where on the radius the start position should be picked (1 =
   * everywhere, 0 = only surface)
   */
  float radiusRange;

  /**
   *
   * Gets or sets a value indicating where on the height the start position should be picked (1 =
   * everywhere, 0 = only surface)
   */
  float heightRange;

  /**
   * Gets or sets a value indicating if all the particles should be emitted from the spawn point
   * only (the base of the cone)
   */
  bool emitFromSpawnPointOnly;

  /**
   * The radius of the emission cone
   */
  Property<ConeParticleEmitter, float> radius;

  /**
   * The angle of the emission cone
   */
  Property<ConeParticleEmitter, float> angle;
  /**
   * The cone base angle
   */
  float directionRandomizer;

private:
  float _radius;
  float _angle;
  float _height;

}; // end of class ConeParticleEmitter

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_EMITTER_TYPES_CONE_PARTICLE_EMITTER_H
