#include <babylon/particles/box_particle_emitter.h>

#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

BoxParticleEmitter::BoxParticleEmitter(ParticleSystem* particleSystem)
    : _particleSystem{particleSystem}
{
}

BoxParticleEmitter::~BoxParticleEmitter()
{
}

Vector3& BoxParticleEmitter::direction1()
{
  return _particleSystem->direction1;
}

void BoxParticleEmitter::setDirection1(const Vector3& value)
{
  _particleSystem->direction1 = value;
}

Vector3& BoxParticleEmitter::direction2()
{
  return _particleSystem->direction2;
}

void BoxParticleEmitter::setDirection2(const Vector3& value)
{
  _particleSystem->direction2 = value;
}

Vector3& BoxParticleEmitter::minEmitBox()
{
  return _particleSystem->minEmitBox;
}

void BoxParticleEmitter::setMinEmitBox(const Vector3& value)
{
  _particleSystem->minEmitBox = value;
}

Vector3& BoxParticleEmitter::maxEmitBox()
{
  return _particleSystem->maxEmitBox;
}

void BoxParticleEmitter::setMaxEmitBox(const Vector3& value)
{
  _particleSystem->maxEmitBox = value;
}

void BoxParticleEmitter::startDirectionFunction(float emitPower,
                                                const Matrix& worldMatrix,
                                                Vector3& directionToUpdate,
                                                Particle* /*particle*/)
{
  const auto randX = Scalar::RandomRange(direction1().x, direction2().x);
  const auto randY = Scalar::RandomRange(direction1().y, direction2().y);
  const auto randZ = Scalar::RandomRange(direction1().z, direction2().z);

  Vector3::TransformNormalFromFloatsToRef(randX * emitPower, randY * emitPower,
                                          randZ * emitPower, worldMatrix,
                                          directionToUpdate);
}

void BoxParticleEmitter::startPositionFunction(const Matrix& worldMatrix,
                                               Vector3& positionToUpdate,
                                               Particle* /*particle*/)
{
  const auto randX = Scalar::RandomRange(minEmitBox().x, maxEmitBox().x);
  const auto randY = Scalar::RandomRange(minEmitBox().y, maxEmitBox().y);
  const auto randZ = Scalar::RandomRange(minEmitBox().z, maxEmitBox().z);

  Vector3::TransformCoordinatesFromFloatsToRef(randX, randY, randZ, worldMatrix,
                                               positionToUpdate);
}

unique_ptr_t<IParticleEmitterType> BoxParticleEmitter::clone() const
{
  auto newOne = ::std::make_unique<BoxParticleEmitter>(_particleSystem);

  return newOne;
}

} // end of namespace BABYLON
