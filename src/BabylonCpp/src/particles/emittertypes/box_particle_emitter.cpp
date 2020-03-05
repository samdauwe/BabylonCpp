#include <babylon/particles/emittertypes/box_particle_emitter.h>

#include <babylon/core/json_util.h>
#include <babylon/materials/effect.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/vector3.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

BoxParticleEmitter::BoxParticleEmitter()
    : direction1{Vector3{0.f, 1.f, 0.f}}
    , direction2{Vector3{0.f, 1.f, 0.f}}
    , minEmitBox{Vector3{-0.5f, -0.5f, -0.5f}}
    , maxEmitBox{Vector3{0.5f, 0.5f, 0.5f}}
{
}

BoxParticleEmitter::~BoxParticleEmitter() = default;

void BoxParticleEmitter::startDirectionFunction(const Matrix& worldMatrix,
                                                Vector3& directionToUpdate, Particle* /*particle*/,
                                                bool isLocal)
{
  const auto randX = Scalar::RandomRange(direction1.x, direction2.x);
  const auto randY = Scalar::RandomRange(direction1.y, direction2.y);
  const auto randZ = Scalar::RandomRange(direction1.z, direction2.z);

  if (isLocal) {
    directionToUpdate.x = randX;
    directionToUpdate.y = randY;
    directionToUpdate.z = randZ;
    return;
  }

  Vector3::TransformNormalFromFloatsToRef(randX, randY, randZ, worldMatrix, directionToUpdate);
}

void BoxParticleEmitter::startPositionFunction(const Matrix& worldMatrix, Vector3& positionToUpdate,
                                               Particle* /*particle*/, bool isLocal)
{
  const auto randX = Scalar::RandomRange(minEmitBox.x, maxEmitBox.x);
  const auto randY = Scalar::RandomRange(minEmitBox.y, maxEmitBox.y);
  const auto randZ = Scalar::RandomRange(minEmitBox.z, maxEmitBox.z);

  if (isLocal) {
    positionToUpdate.x = randX;
    positionToUpdate.y = randY;
    positionToUpdate.z = randZ;
    return;
  }

  Vector3::TransformCoordinatesFromFloatsToRef(randX, randY, randZ, worldMatrix, positionToUpdate);
}

std::unique_ptr<IParticleEmitterType> BoxParticleEmitter::clone() const
{
  auto newOne = std::make_unique<BoxParticleEmitter>();

  return newOne;
}

void BoxParticleEmitter::applyToShader(Effect* effect)
{
  effect->setVector3("direction1", direction1);
  effect->setVector3("direction2", direction2);
  effect->setVector3("minEmitBox", minEmitBox);
  effect->setVector3("maxEmitBox", maxEmitBox);
}

std::string BoxParticleEmitter::getEffectDefines() const
{
  return "#define BOXEMITTER";
}

std::string BoxParticleEmitter::getClassName() const
{
  return "BoxParticleEmitter";
}

json BoxParticleEmitter::serialize() const
{
  return nullptr;
}

void BoxParticleEmitter::parse(const json& serializationObject, Scene* /*scene*/)
{
  if (json_util::has_key(serializationObject, "direction1")) {
    Vector3::FromArrayToRef(json_util::get_array<float>(serializationObject, "direction1"), 0,
                            direction1);
  }
  if (json_util::has_key(serializationObject, "direction2")) {
    Vector3::FromArrayToRef(json_util::get_array<float>(serializationObject, "direction2"), 0,
                            direction2);
  }
  if (json_util::has_key(serializationObject, "minEmitBox")) {
    Vector3::FromArrayToRef(json_util::get_array<float>(serializationObject, "minEmitBox"), 0,
                            minEmitBox);
  }
  if (json_util::has_key(serializationObject, "maxEmitBox")) {
    Vector3::FromArrayToRef(json_util::get_array<float>(serializationObject, "maxEmitBox"), 0,
                            maxEmitBox);
  }
}

} // end of namespace BABYLON
