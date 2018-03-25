#include <babylon/particles/emittertypes/box_particle_emitter.h>

#include <babylon/core/json.h>
#include <babylon/materials/effect.h>
#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

BoxParticleEmitter::BoxParticleEmitter()
    : direction1{Vector3{0.f, 1.f, 0.f}}
    , direction2{Vector3{0.f, 1.f, 0.f}}
    , minEmitBox{Vector3{-0.5f, -0.5f, -0.5f}}
    , maxEmitBox{Vector3{0.5f, 0.5f, 0.5f}}
{
}

BoxParticleEmitter::~BoxParticleEmitter()
{
}

void BoxParticleEmitter::startDirectionFunction(float emitPower,
                                                const Matrix& worldMatrix,
                                                Vector3& directionToUpdate,
                                                Particle* /*particle*/)
{
  const auto randX = Scalar::RandomRange(direction1.x, direction2.x);
  const auto randY = Scalar::RandomRange(direction1.y, direction2.y);
  const auto randZ = Scalar::RandomRange(direction1.z, direction2.z);

  Vector3::TransformNormalFromFloatsToRef(randX * emitPower, randY * emitPower,
                                          randZ * emitPower, worldMatrix,
                                          directionToUpdate);
}

void BoxParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                               Vector3& positionToUpdate,
                                               Particle* /*particle*/)
{
  const auto randX = Scalar::RandomRange(minEmitBox.x, maxEmitBox.x);
  const auto randY = Scalar::RandomRange(minEmitBox.y, maxEmitBox.y);
  const auto randZ = Scalar::RandomRange(minEmitBox.z, maxEmitBox.z);

  Vector3::TransformCoordinatesFromFloatsToRef(randX, randY, randZ, worldMatrix,
                                               positionToUpdate);
}

unique_ptr_t<IParticleEmitterType> BoxParticleEmitter::clone() const
{
  auto newOne = ::std::make_unique<BoxParticleEmitter>();

  return newOne;
}

void BoxParticleEmitter::applyToShader(Effect* effect)
{
  effect->setVector3("direction1", direction1);
  effect->setVector3("direction2", direction2);
  effect->setVector3("minEmitBox", minEmitBox);
  effect->setVector3("maxEmitBox", maxEmitBox);
}

const char* BoxParticleEmitter::getEffectDefines() const
{
  return "#define BOXEMITTER";
}

const char* BoxParticleEmitter::getClassName() const
{
  return "BoxEmitter";
  ;
}

Json::object BoxParticleEmitter::serialize() const
{
  return Json::object();
}

void BoxParticleEmitter::parse(const Json::value& serializationObject)
{
  if (serializationObject.contains("direction1")) {
    direction1.copyFrom(Vector3::FromArray(
      Json::ToArray<float>(serializationObject, "direction1")));
  }
  if (serializationObject.contains("direction2")) {
    direction2.copyFrom(Vector3::FromArray(
      Json::ToArray<float>(serializationObject, "direction2")));
  }
  if (serializationObject.contains("minEmitBox")) {
    minEmitBox.copyFrom(Vector3::FromArray(
      Json::ToArray<float>(serializationObject, "minEmitBox")));
  }
  if (serializationObject.contains("maxEmitBox")) {
    maxEmitBox.copyFrom(Vector3::FromArray(
      Json::ToArray<float>(serializationObject, "maxEmitBox")));
  }
}

} // end of namespace BABYLON
