#include <babylon/particles/emittertypes/cylinder_directed_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/scalar.h>
#include <babylon/particles/particle.h>

namespace BABYLON {

CylinderDirectedParticleEmitter::CylinderDirectedParticleEmitter(
  float iRadius, float iHeight, float iRadiusRange, const Vector3& iDirection1,
  const Vector3& iDirection2)
    : CylinderParticleEmitter{iRadius, iHeight, iRadiusRange}
    , direction1{iDirection1}
    , direction2{iDirection2}
{
}

CylinderDirectedParticleEmitter::~CylinderDirectedParticleEmitter() = default;

void CylinderDirectedParticleEmitter::startDirectionFunction(
  const Matrix& worldMatrix, Vector3& directionToUpdate, Particle* /*particle*/)
{
  const auto randX = Scalar::RandomRange(direction1.x, direction2.x);
  const auto randY = Scalar::RandomRange(direction1.y, direction2.y);
  const auto randZ = Scalar::RandomRange(direction1.z, direction2.z);
  Vector3::TransformNormalFromFloatsToRef(randX, randY, randZ, worldMatrix,
                                          directionToUpdate);
}

std::unique_ptr<IParticleEmitterType>
CylinderDirectedParticleEmitter::clone() const
{
  auto newOne = std::make_unique<CylinderDirectedParticleEmitter>(
    radius, height, radiusRange, direction1, direction2);

  return newOne;
}

void CylinderDirectedParticleEmitter::applyToShader(Effect* effect)
{
  effect->setFloat("radius", radius);
  effect->setFloat("height", height);
  effect->setFloat("radiusRange", radiusRange);
  effect->setVector3("direction1", direction1);
  effect->setVector3("direction2", direction2);
}

std::string CylinderDirectedParticleEmitter::getEffectDefines() const
{
  return "#define CYLINDEREMITTER\n#define DIRECTEDCYLINDEREMITTER";
}

std::string CylinderDirectedParticleEmitter::getClassName() const
{
  return "CylinderDirectedParticleEmitter";
}

json CylinderDirectedParticleEmitter::serialize() const
{
  return nullptr;
}

void CylinderDirectedParticleEmitter::parse(const json& serializationObject)
{
  CylinderParticleEmitter::parse(serializationObject);
  if (json_util::has_key(serializationObject, "direction1")) {
    direction1.copyFrom(Vector3::FromArray(
      json_util::get_array<float>(serializationObject, "direction1")));
  }
  if (json_util::has_key(serializationObject, "direction2")) {
    direction2.copyFrom(Vector3::FromArray(
      json_util::get_array<float>(serializationObject, "direction2")));
  }
}

} // end of namespace BABYLON
