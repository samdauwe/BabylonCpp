#include <babylon/particles/emittertypes/hemispheric_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/vector3.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

HemisphericParticleEmitter::HemisphericParticleEmitter(
  float iRadius, float iRadiusRange, float iDirectionRandomizer)
    : radius{iRadius}
    , radiusRange{iRadiusRange}
    , directionRandomizer{iDirectionRandomizer}
{
}

HemisphericParticleEmitter::~HemisphericParticleEmitter() = default;

void HemisphericParticleEmitter::startDirectionFunction(
  const Matrix& worldMatrix, Vector3& directionToUpdate, Particle* particle)
{
  auto direction
    = particle->position.subtract(worldMatrix.getTranslation()).normalize();
  auto randX = Scalar::RandomRange(0.f, directionRandomizer);
  auto randY = Scalar::RandomRange(0.f, directionRandomizer);
  auto randZ = Scalar::RandomRange(0.f, directionRandomizer);
  direction.x += randX;
  direction.y += randY;
  direction.z += randZ;
  direction.normalize();

  Vector3::TransformNormalFromFloatsToRef(direction.x, direction.y, direction.z,
                                          worldMatrix, directionToUpdate);
}

void HemisphericParticleEmitter::startPositionFunction(
  const Matrix& worldMatrix, Vector3& positionToUpdate, Particle* /*particle*/)
{
  auto randRadius = radius - Scalar::RandomRange(0.f, radius * radiusRange);
  auto v          = Scalar::RandomRange(0.f, 1.f);
  auto phi        = Scalar::RandomRange(0.f, Math::PI2);
  auto theta      = std::acos(2.f * v - 1.f);
  auto randX      = randRadius * std::cos(phi) * std::sin(theta);
  auto randY      = randRadius * std::cos(theta);
  auto randZ      = randRadius * std::sin(phi) * std::sin(theta);
  Vector3::TransformCoordinatesFromFloatsToRef(randX, std::abs(randY), randZ,
                                               worldMatrix, positionToUpdate);
}

std::unique_ptr<IParticleEmitterType> HemisphericParticleEmitter::clone() const
{
  auto newOne = std::make_unique<HemisphericParticleEmitter>();

  return newOne;
}

void HemisphericParticleEmitter::applyToShader(Effect* effect)
{
  effect->setFloat("radius", radius);
  effect->setFloat("radiusRange", radiusRange);
  effect->setFloat("directionRandomizer", directionRandomizer);
}

std::string HemisphericParticleEmitter::getEffectDefines() const
{
  return "#define HEMISPHERICEMITTER";
}

std::string HemisphericParticleEmitter::getClassName() const
{
  return "HemisphericParticleEmitter";
}

json HemisphericParticleEmitter::serialize() const
{
  return nullptr;
}

void HemisphericParticleEmitter::parse(const json& serializationObject)
{
  if (json_util::has_key(serializationObject, "radius")) {
    radius = json_util::get_number<float>(serializationObject, "radius", 1.f);
  }
  if (json_util::has_key(serializationObject, "radiusRange")) {
    radiusRange
      = json_util::get_number<float>(serializationObject, "radiusRange", 1.f);
  }
  if (json_util::has_key(serializationObject, "directionRandomizer")) {
    directionRandomizer = json_util::get_number<float>(
      serializationObject, "directionRandomizer", 0.f);
  }
}

} // end of namespace BABYLON
