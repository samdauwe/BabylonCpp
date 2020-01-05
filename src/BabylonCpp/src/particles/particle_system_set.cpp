#include <babylon/particles/particle_system_set.h>

#include <nlohmann/json.hpp>

#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/mesh.h>
#include <babylon/particles/iparticle_system.h>

namespace BABYLON {

ParticleSystemSet::ParticleSystemSet()
    : emitterNode{this, &ParticleSystemSet::get_emitterNode}, _emitterNode{nullptr}
{
}

ParticleSystemSet::~ParticleSystemSet() = default;

TransformNodePtr& ParticleSystemSet::get_emitterNode()
{
  return _emitterNode;
}

void ParticleSystemSet::setEmitterAsSphere(const EmitterCreationOptions& options,
                                           unsigned int renderingGroupId, Scene* scene)
{
  if (_emitterNode) {
    _emitterNode->dispose();
  }

  _emitterCreationOptions = {
    "Sphere",        // kind
    options,         // options,
    renderingGroupId // renderingGroupId
  };

  SphereOptions sphereOptions;
  sphereOptions.diameter = options.diameter;
  sphereOptions.segments = options.segments;

  auto emitterMesh = SphereBuilder::CreateSphere("emitterSphere", sphereOptions, scene);
  emitterMesh->renderingGroupId = static_cast<int>(renderingGroupId);

  auto material           = StandardMaterial::New("emitterSphereMaterial", scene);
  material->emissiveColor = options.color;
  emitterMesh->material   = material;

  for (const auto& system : systems) {
    system->emitter = emitterMesh;
  }

  _emitterNode = emitterMesh;
}

void ParticleSystemSet::start(const AbstractMeshPtr& emitter)
{
  for (const auto& system : systems) {
    if (emitter) {
      system->emitter = emitter;
    }
    system->start();
  }
}

void ParticleSystemSet::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  for (const auto& system : systems) {
    system->dispose(doNotRecurse, disposeMaterialAndTextures);
  }

  systems.clear();

  if (_emitterNode) {
    _emitterNode->dispose();
    _emitterNode = nullptr;
  }
}

json ParticleSystemSet::serialize() const
{
  return nullptr;
}

std::unique_ptr<ParticleSystemSet> ParticleSystemSet::Parse(const json& /*data*/, Scene* /*scene*/,
                                                            bool /*gpu*/)
{
  return nullptr;
}

} // end of namespace BABYLON
