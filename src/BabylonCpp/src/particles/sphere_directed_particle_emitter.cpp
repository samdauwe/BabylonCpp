#include <babylon/particles/sphere_directed_particle_emitter.h>

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

} // end of namespace BABYLON
