#include <babylon/particles/emittertypes/hemispheric_particle_emitter.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>
#include <babylon/math/matrix.h>
#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

HemisphericParticleEmitter::HemisphericParticleEmitter(
  float iRadius, float iRadiusRange, float iDirectionRandomizer)
    : radius{iRadius}
    , radiusRange{iRadiusRange}
    , directionRandomizer{iDirectionRandomizer}
{
}

HemisphericParticleEmitter::~HemisphericParticleEmitter()
{
}

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
  auto theta      = ::std::acos(2.f * v - 1.f);
  auto randX      = randRadius * ::std::cos(phi) * ::std::sin(theta);
  auto randY      = randRadius * ::std::cos(theta);
  auto randZ      = randRadius * ::std::sin(phi) * ::std::sin(theta);
  Vector3::TransformCoordinatesFromFloatsToRef(randX, ::std::abs(randY), randZ,
                                               worldMatrix, positionToUpdate);
}

unique_ptr_t<IParticleEmitterType> HemisphericParticleEmitter::clone() const
{
  auto newOne = ::std::make_unique<HemisphericParticleEmitter>();

  return newOne;
}

void HemisphericParticleEmitter::applyToShader(Effect* effect)
{
  effect->setFloat("radius", radius);
  effect->setFloat("radiusRange", radiusRange);
  effect->setFloat("directionRandomizer", directionRandomizer);
}

const char* HemisphericParticleEmitter::getEffectDefines() const
{
  return "#define HEMISPHERICEMITTER";
}

const char* HemisphericParticleEmitter::getClassName() const
{
  return "HemisphericParticleEmitter";
}

Json::object HemisphericParticleEmitter::serialize() const
{
  return Json::object();
}

void HemisphericParticleEmitter::parse(const Json::value& serializationObject)
{
  if (serializationObject.contains("radius")) {
    radius = Json::GetNumber<float>(serializationObject, "radius", 1.f);
  }
  if (serializationObject.contains("radiusRange")) {
    radiusRange
      = Json::GetNumber<float>(serializationObject, "radiusRange", 1.f);
  }
  if (serializationObject.contains("directionRandomizer")) {
    directionRandomizer
      = Json::GetNumber<float>(serializationObject, "directionRandomizer", 0.f);
  }
}

} // end of namespace BABYLON
