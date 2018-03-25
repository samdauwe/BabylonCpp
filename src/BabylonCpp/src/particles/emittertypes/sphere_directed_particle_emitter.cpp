#include <babylon/particles/emittertypes/sphere_directed_particle_emitter.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>
#include <babylon/math/scalar.h>

namespace BABYLON {

SphereDirectedParticleEmitter::SphereDirectedParticleEmitter(
  float radius, const Vector3& iDirection1, const Vector3& iDirection2)
    : SphereParticleEmitter{radius}
    , direction1{iDirection1}
    , direction2{iDirection2}
{
}

SphereDirectedParticleEmitter::~SphereDirectedParticleEmitter()
{
}

void SphereDirectedParticleEmitter::startDirectionFunction(
  float emitPower, const Matrix& worldMatrix, Vector3& directionToUpdate,
  Particle* /*particle*/)
{
  const auto randX = Scalar::RandomRange(direction1.x, direction2.x);
  const auto randY = Scalar::RandomRange(direction1.y, direction2.y);
  const auto randZ = Scalar::RandomRange(direction1.z, direction2.z);
  Vector3::TransformNormalFromFloatsToRef(randX * emitPower, randY * emitPower,
                                          randZ * emitPower, worldMatrix,
                                          directionToUpdate);
}

unique_ptr_t<IParticleEmitterType> SphereDirectedParticleEmitter::clone() const
{
  auto newOne = ::std::make_unique<SphereDirectedParticleEmitter>(
    radius, direction1, direction2);

  return newOne;
}

void SphereDirectedParticleEmitter::applyToShader(Effect* effect)
{
  effect->setFloat("radius", radius);
  effect->setVector3("direction1", direction1);
  effect->setVector3("direction2", direction2);
}

const char* SphereDirectedParticleEmitter::getEffectDefines() const
{
  return "#define SPHEREEMITTER\n#define DIRECTEDSPHEREEMITTER";
}

const char* SphereDirectedParticleEmitter::getClassName() const
{
  return "SphereDirectedParticleEmitter";
}

Json::object SphereDirectedParticleEmitter::serialize() const
{
  return Json::object();
}

void SphereDirectedParticleEmitter::parse(
  const Json::value& serializationObject)
{
  SphereParticleEmitter::parse(serializationObject);
  if (serializationObject.contains("direction1")) {
    direction1.copyFrom(Vector3::FromArray(
      Json::ToArray<float>(serializationObject, "direction1")));
  }
  if (serializationObject.contains("direction2")) {
    direction2.copyFrom(Vector3::FromArray(
      Json::ToArray<float>(serializationObject, "direction2")));
  }
}

} // end of namespace BABYLON
