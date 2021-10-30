#include <babylon/particles/emittertypes/point_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/materials/uniform_buffer_effect_common_accessor.h>
#include <babylon/maths/scalar.h>

namespace BABYLON {

PointParticleEmitter::PointParticleEmitter()
    : direction1{Vector3(0.f, 1.f, 0.f)}, direction2{Vector3(0.f, 1.f, 0.f)}
{
}

PointParticleEmitter::~PointParticleEmitter() = default;

void PointParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                  Vector3& directionToUpdate,
                                                  Particle* /*particle*/, bool isLocal,
                                                  const Matrix& /*inverseWorldMatrix*/)
{
  auto randX = Scalar::RandomRange(direction1.x, direction2.x);
  auto randY = Scalar::RandomRange(direction1.y, direction2.y);
  auto randZ = Scalar::RandomRange(direction1.z, direction2.z);

  if (isLocal) {
    directionToUpdate.copyFromFloats(randX, randY, randZ);
    return;
  }

  Vector3::TransformNormalFromFloatsToRef(randX, randY, randZ, worldMatrix, directionToUpdate);
}

void PointParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                                 Vector3& positionToUpdate, Particle* /*particle*/,
                                                 bool isLocal)
{
  if (isLocal) {
    positionToUpdate.copyFromFloats(0.f, 0.f, 0.f);
    return;
  }
  Vector3::TransformCoordinatesFromFloatsToRef(0.f, 0.f, 0.f, worldMatrix, positionToUpdate);
}

std::unique_ptr<IParticleEmitterType> PointParticleEmitter::clone() const
{

  auto newOne = std::make_unique<PointParticleEmitter>();

  return newOne;
}

void PointParticleEmitter::applyToShader(UniformBufferEffectCommonAccessor* uboOrEffect)
{
  uboOrEffect->setVector3("direction1", direction1);
  uboOrEffect->setVector3("direction2", direction2);
}

void PointParticleEmitter::buildUniformLayout(UniformBuffer* ubo)
{
  ubo->addUniform("direction1", 3);
  ubo->addUniform("direction2", 3);
}

std::string PointParticleEmitter::getEffectDefines() const
{
  return "#define POINTEMITTER";
}

std::string PointParticleEmitter::getClassName() const
{
  return "PointParticleEmitter";
}

json PointParticleEmitter::serialize() const
{
  return nullptr;
}

void PointParticleEmitter::parse(const json& serializationObject, Scene* /*scene*/)
{
  if (json_util::has_key(serializationObject, "direction1")) {
    Vector3::FromArrayToRef(json_util::get_array<float>(serializationObject, "direction1"), 0,
                            direction1);
  }
  if (json_util::has_key(serializationObject, "direction2")) {
    Vector3::FromArrayToRef(json_util::get_array<float>(serializationObject, "direction2"), 0,
                            direction2);
  }
}

} // end of namespace BABYLON
