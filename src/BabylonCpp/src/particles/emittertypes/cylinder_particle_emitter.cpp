#include <babylon/particles/emittertypes/cylinder_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/materials/uniform_buffer_effect_common_accessor.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/scalar.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

CylinderParticleEmitter::CylinderParticleEmitter(float iRadius, float iHeight, float iRadiusRange,
                                                 float iDirectionRandomizer)
    : radius{iRadius}
    , height{iHeight}
    , radiusRange{iRadiusRange}
    , directionRandomizer{iDirectionRandomizer}
    , _tempVector{Vector3::Zero()}
{
}

CylinderParticleEmitter::~CylinderParticleEmitter() = default;

void CylinderParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                     Vector3& directionToUpdate, Particle* particle,
                                                     bool isLocal, const Matrix& inverseWorldMatrix)
{
  particle->position.subtractToRef(worldMatrix.getTranslation(), _tempVector);

  _tempVector.normalize();

  Vector3::TransformNormalToRef(_tempVector, inverseWorldMatrix, _tempVector);

  const auto randY = Scalar::RandomRange(-directionRandomizer / 2.f, directionRandomizer / 2.f);

  auto angle = std::atan2(_tempVector.x, _tempVector.z);
  angle += Scalar::RandomRange(-Math::PI_2, Math::PI_2) * directionRandomizer;

  _tempVector.y = randY; // set direction y to rand y to mirror normal of cylinder surface
  _tempVector.x = std::sin(angle);
  _tempVector.z = std::cos(angle);
  _tempVector.normalize();

  if (isLocal) {
    directionToUpdate.copyFrom(_tempVector);
    return;
  }

  Vector3::TransformNormalFromFloatsToRef(_tempVector.x, _tempVector.y, _tempVector.z, worldMatrix,
                                          directionToUpdate);
}

void CylinderParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                    Vector3& positionToUpdate,
                                                    Particle* /*particle*/, bool isLocal)
{
  auto yPos  = Scalar::RandomRange(-height / 2.f, height / 2.f);
  auto angle = Scalar::RandomRange(0.f, Math::PI2);

  // Pick a properly distributed point within the circle
  // https://programming.guide/random-point-within-circle.html
  auto radiusDistribution = Scalar::RandomRange((1.f - radiusRange) * (1.f - radiusRange), 1.f);
  auto positionRadius     = std::sqrt(radiusDistribution) * radius;
  auto xPos               = positionRadius * std::cos(angle);
  auto zPos               = positionRadius * std::sin(angle);

  if (isLocal) {
    positionToUpdate.copyFromFloats(xPos, yPos, zPos);
    return;
  }

  Vector3::TransformCoordinatesFromFloatsToRef(xPos, yPos, zPos, worldMatrix, positionToUpdate);
}

std::unique_ptr<IParticleEmitterType> CylinderParticleEmitter::clone() const
{
  auto newOne = std::make_unique<CylinderParticleEmitter>(radius, directionRandomizer);

  return newOne;
}

void CylinderParticleEmitter::applyToShader(UniformBufferEffectCommonAccessor* uboOrEffect)
{
  uboOrEffect->setFloat("radius", radius);
  uboOrEffect->setFloat("height", height);
  uboOrEffect->setFloat("radiusRange", radiusRange);
  uboOrEffect->setFloat("directionRandomizer", directionRandomizer);
}

void CylinderParticleEmitter::buildUniformLayout(UniformBuffer* ubo)
{
  ubo->addUniform("radius", 1);
  ubo->addUniform("height", 1);
  ubo->addUniform("radiusRange", 1);
  ubo->addUniform("directionRandomizer", 1);
}

std::string CylinderParticleEmitter::getEffectDefines() const
{
  return "#define CYLINDEREMITTER";
}

std::string CylinderParticleEmitter::getClassName() const
{
  return "CylinderParticleEmitter";
}

json CylinderParticleEmitter::serialize() const
{
  return nullptr;
}

void CylinderParticleEmitter::parse(const json& serializationObject, Scene* /*scene*/)
{
  if (json_util::has_key(serializationObject, "radius")) {
    radius = json_util::get_number<float>(serializationObject, "radius", 1.f);
  }
  if (json_util::has_key(serializationObject, "height")) {
    height = json_util::get_number<float>(serializationObject, "height", 1.f);
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
