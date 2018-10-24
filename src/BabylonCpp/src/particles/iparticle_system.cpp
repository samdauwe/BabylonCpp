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
  return std::holds_alternative<AbstractMeshPtr>(emitter)
         || std::holds_alternative<Vector3>(emitter);
}

} // end of namespace BABYLON
