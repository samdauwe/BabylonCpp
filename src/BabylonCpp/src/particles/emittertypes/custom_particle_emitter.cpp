#include <babylon/particles/emittertypes/custom_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/maths/vector3.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

CustomParticleEmitter::CustomParticleEmitter()
    : particlePositionGenerator{nullptr}, particleDestinationGenerator{nullptr}
{
}

CustomParticleEmitter::~CustomParticleEmitter()
{
}

void CustomParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                   Vector3& directionToUpdate, Particle* particle)
{
  auto& tmpVector = TmpVectors::Vector3Array[0];

  if (particleDestinationGenerator) {
    particleDestinationGenerator(-1, particle, tmpVector);

    // Get direction
    auto& diffVector = TmpVectors::Vector3Array[1];
    tmpVector.subtractToRef(particle->position, diffVector);

    diffVector.scaleToRef(1.f / particle->lifeTime, tmpVector);
  }
  else {
    tmpVector.set(0.f, 0.f, 0.f);
  }

  Vector3::TransformNormalToRef(tmpVector, worldMatrix, directionToUpdate);
}

void CustomParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                  Vector3& positionToUpdate, Particle* particle)
{
  auto& tmpVector = TmpVectors::Vector3Array[0];

  if (particlePositionGenerator) {
    particlePositionGenerator(-1, particle, tmpVector);
  }
  else {
    tmpVector.set(0.f, 0.f, 0.f);
  }

  Vector3::TransformCoordinatesToRef(tmpVector, worldMatrix, positionToUpdate);
}

std::unique_ptr<IParticleEmitterType> CustomParticleEmitter::clone() const
{
  auto newOne = std::make_unique<CustomParticleEmitter>();

  return newOne;
}

void CustomParticleEmitter::applyToShader(Effect* /*effect*/)
{
}

std::string CustomParticleEmitter::getEffectDefines() const
{
  return "#define CUSTOMEMITTER";
}

std::string CustomParticleEmitter::getClassName() const
{
  return "CustomParticleEmitter";
}

json CustomParticleEmitter::serialize() const
{
  return nullptr;
}

void CustomParticleEmitter::parse(const json& /*serializationObject*/, Scene* /*scene*/)
{
}

} // end of namespace BABYLON
