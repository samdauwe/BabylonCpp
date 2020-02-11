#include <babylon/particles/emittertypes/sphere_directed_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/maths/scalar.h>

namespace BABYLON {

SphereDirectedParticleEmitter::SphereDirectedParticleEmitter(float radius,
                                                             const Vector3& iDirection1,
                                                             const Vector3& iDirection2)
    : SphereParticleEmitter{radius}, direction1{iDirection1}, direction2{iDirection2}
{
}

SphereDirectedParticleEmitter::~SphereDirectedParticleEmitter() = default;

void SphereDirectedParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                           Vector3& directionToUpdate,
                                                           Particle* /*particle*/)
{
  const auto randX = Scalar::RandomRange(direction1.x, direction2.x);
  const auto randY = Scalar::RandomRange(direction1.y, direction2.y);
  const auto randZ = Scalar::RandomRange(direction1.z, direction2.z);
  Vector3::TransformNormalFromFloatsToRef(randX, randY, randZ, worldMatrix, directionToUpdate);
}

std::unique_ptr<IParticleEmitterType> SphereDirectedParticleEmitter::clone() const
{
  auto newOne = std::make_unique<SphereDirectedParticleEmitter>(radius, direction1, direction2);

  return newOne;
}

void SphereDirectedParticleEmitter::applyToShader(Effect* effect)
{
  effect->setFloat("radius", radius);
  effect->setFloat("radiusRange", radiusRange);
  effect->setVector3("direction1", direction1);
  effect->setVector3("direction2", direction2);
}

std::string SphereDirectedParticleEmitter::getEffectDefines() const
{
  return "#define SPHEREEMITTER\n#define DIRECTEDSPHEREEMITTER";
}

std::string SphereDirectedParticleEmitter::getClassName() const
{
  return "SphereDirectedParticleEmitter";
}

json SphereDirectedParticleEmitter::serialize() const
{
  return nullptr;
}

void SphereDirectedParticleEmitter::parse(const json& serializationObject, Scene* scene)
{
  SphereParticleEmitter::parse(serializationObject, scene);
  if (json_util::has_key(serializationObject, "direction1")) {
    direction1.copyFrom(
      Vector3::FromArray(json_util::get_array<float>(serializationObject, "direction1")));
  }
  if (json_util::has_key(serializationObject, "direction2")) {
    direction2.copyFrom(
      Vector3::FromArray(json_util::get_array<float>(serializationObject, "direction2")));
  }
}

} // end of namespace BABYLON
