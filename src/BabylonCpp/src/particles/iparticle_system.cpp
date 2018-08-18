#include <babylon/particles/iparticle_system.h>

#include <babylon/particles/emittertypes/iparticle_emitter_Type.h>

namespace BABYLON {

IParticleSystem::IParticleSystem()
    : isBillboardBased{this, &IParticleSystem::get_isBillboardBased,
                       &IParticleSystem::set_isBillboardBased}
{
}

IParticleSystem::~IParticleSystem()
{
}

bool IParticleSystem::hasEmitter() const
{
  return emitter.is<AbstractMesh*>() || emitter.is<Vector3>();
}

} // end of namespace BABYLON
