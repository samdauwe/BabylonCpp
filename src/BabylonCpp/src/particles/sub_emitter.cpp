#include <babylon/particles/sub_emitter.h>

#include <nlohmann/json.hpp>

#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
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
    particleSystem->emitter
      = AbstractMesh::New("SubemitterSystemEmitter", particleSystem->getScene());
  }

  // Automatically dispose of subemitter when system is disposed
  particleSystem->onDisposeObservable.add([this](IParticleSystem* /*ps*/, EventState& /*es*/) {
    if (std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)) {
      std::get<AbstractMeshPtr>(particleSystem->emitter)->dispose();
    }
  });
}

SubEmitter::~SubEmitter() = default;

SubEmitterPtr SubEmitter::clone() const
{
  // Clone particle system
  std::variant<AbstractMeshPtr, Mesh*, Vector3> emitter;
  if (std::holds_alternative<Vector3>(particleSystem->emitter)) {
    emitter = std::get<Vector3>(particleSystem->emitter);
  }
  else if (std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)
           || std::holds_alternative<Mesh*>(particleSystem->emitter)) {
    const auto meshEmitter
      = Mesh::New("", std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter) ?
                        std::get<AbstractMeshPtr>(particleSystem->emitter)->getScene() :
                        std::get<Mesh*>(particleSystem->emitter)->getScene());
    meshEmitter->isVisible = false;
    emitter                = meshEmitter;
  }
  auto clone = std::make_shared<SubEmitter>(
    std::static_pointer_cast<ParticleSystem>(particleSystem->clone("", emitter)));

  // Clone properties
  clone->particleSystem->name += "Clone";
  clone->type                    = type;
  clone->inheritDirection        = inheritDirection;
  clone->inheritedVelocityAmount = inheritedVelocityAmount;

  clone->particleSystem->_disposeEmitterOnDispose = true;
  clone->particleSystem->disposeOnStop            = true;
  return clone;
}

json SubEmitter::serialize() const
{
  return nullptr;
}

SubEmitterPtr SubEmitter::Parse(const json& /*parsedLight*/,
                                const std::variant<Scene*, ThinEngine*>& /*sceneOrEngine*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

void SubEmitter::dispose()
{
  particleSystem->dispose();
}

} // end of namespace BABYLON
