#include <babylon/particles/emittertypes/sphere_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/materials/uniform_buffer_effect_common_accessor.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/vector3.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

SphereParticleEmitter::SphereParticleEmitter(float iRadius, float iRadiusRange,
                                             float iDirectionRandomizer)
    : radius{iRadius}, radiusRange{iRadiusRange}, directionRandomizer{iDirectionRandomizer}
{
}

SphereParticleEmitter::~SphereParticleEmitter() = default;

void SphereParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                   Vector3& directionToUpdate, Particle* particle,
                                                   bool isLocal)
{
  auto direction   = particle->position.subtract(worldMatrix.getTranslation()).normalize();
  const auto randX = Scalar::RandomRange(0, directionRandomizer);
  const auto randY = Scalar::RandomRange(0, directionRandomizer);
  const auto randZ = Scalar::RandomRange(0, directionRandomizer);
  direction.x += randX;
  direction.y += randY;
  direction.z += randZ;
  direction.normalize();

  if (isLocal) {
    directionToUpdate.copyFrom(direction);
    return;
  }

  Vector3::TransformNormalFromFloatsToRef(direction.x, direction.y, direction.z, worldMatrix,
                                          directionToUpdate);
}

void SphereParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                  Vector3& positionToUpdate, Particle* /*particle*/,
                                                  bool isLocal)
{
  const auto randRadius = radius - Scalar::RandomRange(0, radius * radiusRange);
  const auto v          = Scalar::RandomRange(0.f, 1.f);
  const auto phi        = Scalar::RandomRange(0.f, Math::PI2);
  const auto theta      = std::acos(2.f * v - 1.f);

  const auto randX = randRadius * std::cos(phi) * std::sin(theta);
  const auto randY = randRadius * std::cos(theta);
  const auto randZ = randRadius * std::sin(phi) * std::sin(theta);

  if (isLocal) {
    positionToUpdate.copyFromFloats(randX, randY, randZ);
    return;
  }

  Vector3::TransformCoordinatesFromFloatsToRef(randX, randY, randZ, worldMatrix, positionToUpdate);
}

std::unique_ptr<IParticleEmitterType> SphereParticleEmitter::clone() const
{
  auto newOne = std::make_unique<SphereParticleEmitter>(radius, directionRandomizer);

  return newOne;
}

void SphereParticleEmitter::applyToShader(UniformBufferEffectCommonAccessor* uboOrEffect)
{
  uboOrEffect->setFloat("radius", radius);
  uboOrEffect->setFloat("radiusRange", radiusRange);
  uboOrEffect->setFloat("directionRandomizer", directionRandomizer);
}

void SphereParticleEmitter::buildUniformLayout(UniformBuffer* ubo)
{
  ubo->addUniform("radius", 1);
  ubo->addUniform("radiusRange", 1);
  ubo->addUniform("directionRandomizer", 1);
}

std::string SphereParticleEmitter::getEffectDefines() const
{
  return "#define SPHEREEMITTER";
}

std::string SphereParticleEmitter::getClassName() const
{
  return "SphereParticleEmitter";
}

json SphereParticleEmitter::serialize() const
{
  return nullptr;
}

void SphereParticleEmitter::parse(const json& serializationObject, Scene* /*scene*/)
{
  if (json_util::has_key(serializationObject, "radius")) {
    radius = json_util::get_number<float>(serializationObject, "radius", 1.f);
  }
  if (json_util::has_key(serializationObject, "radiusRange")) {
    radiusRange = json_util::get_number<float>(serializationObject, "radiusRange", 1.f);
  }
  if (json_util::has_key(serializationObject, "directionRandomizer")) {
    directionRandomizer
      = json_util::get_number<float>(serializationObject, "directionRandomizer", 0.f);
  }
}

} // end of namespace BABYLON
