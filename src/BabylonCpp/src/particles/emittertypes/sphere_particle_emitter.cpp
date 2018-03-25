#include <babylon/particles/emittertypes/sphere_particle_emitter.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>
#include <babylon/math/matrix.h>
#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

SphereParticleEmitter::SphereParticleEmitter(float iRadius,
                                             float iDirectionRandomizer)
    : radius{iRadius}, directionRandomizer{iDirectionRandomizer}
{
}

SphereParticleEmitter::~SphereParticleEmitter()
{
}

void SphereParticleEmitter::startDirectionFunction(float emitPower,
                                                   const Matrix& worldMatrix,
                                                   Vector3& directionToUpdate,
                                                   Particle* particle)
{
  auto direction
    = particle->position.subtract(worldMatrix.getTranslation()).normalize();
  const auto randX = Scalar::RandomRange(0, directionRandomizer);
  const auto randY = Scalar::RandomRange(0, directionRandomizer);
  const auto randZ = Scalar::RandomRange(0, directionRandomizer);
  direction.x += randX;
  direction.y += randY;
  direction.z += randZ;
  direction.normalize();

  Vector3::TransformNormalFromFloatsToRef(
    direction.x * emitPower, direction.y * emitPower, direction.z * emitPower,
    worldMatrix, directionToUpdate);
}

void SphereParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                  Vector3& positionToUpdate,
                                                  Particle* /*particle*/)
{
  const auto phi   = Scalar::RandomRange(0.f, Math::PI2);
  const auto theta = Scalar::RandomRange(0, Math::PI);
  const auto randX = radius * ::std::cos(phi) * ::std::sin(theta);
  const auto randY = radius * ::std::cos(theta);
  const auto randZ = radius * ::std::sin(phi) * ::std::sin(theta);
  Vector3::TransformCoordinatesFromFloatsToRef(randX, randY, randZ, worldMatrix,
                                               positionToUpdate);
}

unique_ptr_t<IParticleEmitterType> SphereParticleEmitter::clone() const
{
  auto newOne
    = ::std::make_unique<SphereParticleEmitter>(radius, directionRandomizer);

  return newOne;
}

void SphereParticleEmitter::applyToShader(Effect* effect)
{
  effect->setFloat("radius", radius);
  effect->setFloat("directionRandomizer", directionRandomizer);
}

const char* SphereParticleEmitter::getEffectDefines() const
{
  return "#define SPHEREEMITTER";
}

const char* SphereParticleEmitter::getClassName() const
{
  return "SphereParticleEmitter";
}

Json::object SphereParticleEmitter::serialize() const
{
  return Json::object();
}

void SphereParticleEmitter::parse(const Json::value& serializationObject)
{
  if (serializationObject.contains("radius")) {
    radius = Json::GetNumber<float>(serializationObject, "radius", 1.f);
  }
  if (serializationObject.contains("directionRandomizer")) {
    directionRandomizer
      = Json::GetNumber<float>(serializationObject, "directionRandomizer", 0.f);
  }
}

} // end of namespace BABYLON
