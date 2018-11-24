#include <babylon/particles/sub_emitter.h>

#include <nlohmann/json.hpp>

#include <babylon/particles/particle_system.h>

namespace BABYLON {

SubEmitter::SubEmitter(const ParticleSystemPtr& iParticleSystem)
    : particleSystem{iParticleSystem}
    , type{SubEmitterType::END}
    , inheritDirection{false}
    , inheritedVelocityAmount{0.f}
{
}

SubEmitter::~SubEmitter()
{
}

SubEmitterPtr SubEmitter::clone() const
{
  return nullptr;
}

json SubEmitter::serialize() const
{
  return nullptr;
}

SubEmitterPtr SubEmitter::Parse(const json& /*parsedLight*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

void SubEmitter::dispose()
{
  particleSystem->dispose();
}

} // end of namespace BABYLON
