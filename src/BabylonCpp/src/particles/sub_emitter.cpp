#include <babylon/particles/sub_emitter.h>

#include <nlohmann/json.hpp>

#include <babylon/meshes/abstract_mesh.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

SubEmitter::SubEmitter(const ParticleSystemPtr& iParticleSystem)
    : particleSystem{iParticleSystem}
    , type{SubEmitterType::END}
    , inheritDirection{false}
    , inheritedVelocityAmount{0.f}
{
  // Create mesh as emitter to support rotation
  if (!std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)) {
    particleSystem->emitter = AbstractMesh::New("SubemitterSystemEmitter",
                                                particleSystem->getScene());
  }

  // Automatically dispose of subemitter when system is disposed
  particleSystem->onDisposeObservable.add(
    [this](ParticleSystem* /*ps*/, EventState& /*es*/) {
      if (std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)) {
        std::get<AbstractMeshPtr>(particleSystem->emitter)->dispose();
      }
    });
}

SubEmitter::~SubEmitter() = default;

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
