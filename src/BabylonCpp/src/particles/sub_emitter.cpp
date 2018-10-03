#include <babylon/particles/sub_emitter.h>

#include <babylon/core/json.h>
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

Json::object SubEmitter::serialize() const
{
  return Json::object();
}

SubEmitterPtr SubEmitter::Parse(const Json::value& /*parsedLight*/,
                                Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

void SubEmitter::dispose()
{
  particleSystem->dispose();
}

} // end of namespace BABYLON
