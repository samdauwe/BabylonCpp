#include <babylon/particles/iparticle_system.h>

#include <babylon/particles/emittertypes/iparticle_emitter_type.h>

namespace BABYLON {

IParticleSystem::IParticleSystem()
    : isBillboardBased{this, &IParticleSystem::get_isBillboardBased,
                       &IParticleSystem::set_isBillboardBased}
    , isAnimationSheetEnabled{this, &IParticleSystem::get_isAnimationSheetEnabled,
                              &IParticleSystem::set_isAnimationSheetEnabled}
    , noiseTexture{this, &IParticleSystem::get_noiseTexture, &IParticleSystem::set_noiseTexture}
    , isLocal{this, &IParticleSystem::get_isLocal, &IParticleSystem::set_isLocal}
    , onBeforeDrawParticlesObservable{this, &IParticleSystem::get_onBeforeDrawParticlesObservable}
    , vertexShaderName{this, &IParticleSystem::get_vertexShaderName}
    , useRampGradients{this, &IParticleSystem::get_useRampGradients,
                       &IParticleSystem::set_useRampGradients}
{
}

IParticleSystem::~IParticleSystem() = default;

bool IParticleSystem::hasEmitter() const
{
  return std::holds_alternative<AbstractMeshPtr>(emitter)
         || std::holds_alternative<Vector3>(emitter);
}

bool IParticleSystem::get_isLocal() const
{
  return _isLocal;
}

void IParticleSystem::set_isLocal(bool value)
{
  _isLocal = value;
}

} // end of namespace BABYLON
