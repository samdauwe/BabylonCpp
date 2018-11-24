#include <babylon/particles/emittertypes/cylinder_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/math/matrix.h>
#include <babylon/math/scalar.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

CylinderParticleEmitter::CylinderParticleEmitter(float iRadius, float iHeight,
                                                 float iRadiusRange,
                                                 float iDirectionRandomizer)
    : radius{iRadius}
    , height{iHeight}
    , radiusRange{iRadiusRange}
    , directionRandomizer{iDirectionRandomizer}
{
}

CylinderParticleEmitter::~CylinderParticleEmitter()
{
}

void CylinderParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                     Vector3& directionToUpdate,
                                                     Particle* particle)
{
  auto direction
    = particle->position.subtract(worldMatrix.getTranslation()).normalize();
  auto randY = Scalar::RandomRange(-directionRandomizer / 2.f,
                                   directionRandomizer / 2.f);

  auto angle = std::atan2(direction.x, direction.z);
  angle += Scalar::RandomRange(-Math::PI_2, Math::PI_2) * directionRandomizer;

  direction.y
    = randY; // set direction y to rand y to mirror normal of cylinder surface
  direction.x = std::sin(angle);
  direction.z = std::cos(angle);
  direction.normalize();

  Vector3::TransformNormalFromFloatsToRef(direction.x, direction.y, direction.z,
                                          worldMatrix, directionToUpdate);
}

void CylinderParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                    Vector3& positionToUpdate,
                                                    Particle* /*particle*/)
{
  auto yPos  = Scalar::RandomRange(-height / 2.f, height / 2.f);
  auto angle = Scalar::RandomRange(0.f, Math::PI2);

  // Pick a properly distributed point within the circle
  // https://programming.guide/random-point-within-circle.html
  auto radiusDistribution
    = Scalar::RandomRange((1.f - radiusRange) * (1.f - radiusRange), 1.f);
  auto positionRadius = std::sqrt(radiusDistribution) * radius;
  auto xPos           = positionRadius * std::cos(angle);
  auto zPos           = positionRadius * std::sin(angle);

  Vector3::TransformCoordinatesFromFloatsToRef(xPos, yPos, zPos, worldMatrix,
                                               positionToUpdate);
}

std::unique_ptr<IParticleEmitterType> CylinderParticleEmitter::clone() const
{
  auto newOne
    = std::make_unique<CylinderParticleEmitter>(radius, directionRandomizer);

  return newOne;
}

void CylinderParticleEmitter::applyToShader(Effect* effect)
{
  effect->setFloat("radius", radius);
  effect->setFloat("height", height);
  effect->setFloat("radiusRange", radiusRange);
  effect->setFloat("directionRandomizer", directionRandomizer);
}

const char* CylinderParticleEmitter::getEffectDefines() const
{
  return "#define CYLINDEREMITTER";
}

const char* CylinderParticleEmitter::getClassName() const
{
  return "CylinderParticleEmitter";
}

json CylinderParticleEmitter::serialize() const
{
  return nullptr;
}

void CylinderParticleEmitter::parse(const json& serializationObject)
{
  if (json_util::has_key(serializationObject, "radius")) {
    radius = json_util::get_number<float>(serializationObject, "radius", 1.f);
  }
  if (json_util::has_key(serializationObject, "height")) {
    height = json_util::get_number<float>(serializationObject, "height", 1.f);
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
