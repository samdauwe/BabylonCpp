#include <babylon/particles/particle_system_set.h>

#include <babylon/core/json.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/particles/iparticle_system.h>

namespace BABYLON {

ParticleSystemSet::ParticleSystemSet()
    : emitterMesh{this, &ParticleSystemSet::get_emitterMesh}
    , _emitterMesh{nullptr}
{
}

ParticleSystemSet::~ParticleSystemSet()
{
}

Mesh*& ParticleSystemSet::get_emitterMesh()
{
  return _emitterMesh;
}

void ParticleSystemSet::setEmitterAsSphere(
  const EmitterCreationOptions& options, unsigned int renderingGroupId,
  Scene* scene)
{
  if (_emitterMesh) {
    _emitterMesh->dispose();
  }

  _emitterCreationOptions = {
    "Sphere",        // kind
    options,         // options,
    renderingGroupId // renderingGroupId
  };

  SphereOptions sphereOptions(options.diameter);
  sphereOptions.segments = options.segments;

  _emitterMesh
    = MeshBuilder::CreateSphere("emitterSphere", sphereOptions, scene);
  _emitterMesh->renderingGroupId = renderingGroupId;

  auto material = StandardMaterial::New("emitterSphereMaterial", scene);
  material->emissiveColor = options.color;
  _emitterMesh->material  = material;

  for (auto& system : systems) {
    system->emitter.set<AbstractMesh*>(_emitterMesh);
  }
}

void ParticleSystemSet::start(AbstractMesh* emitter)
{
  for (auto& system : systems) {
    if (emitter) {
      system->emitter.set<AbstractMesh*>(emitter);
    }
    system->start();
  }
}

void ParticleSystemSet::dispose(bool doNotRecurse,
                                bool disposeMaterialAndTextures)
{
  for (auto& system : systems) {
    system->dispose(doNotRecurse, disposeMaterialAndTextures);
  }

  systems.clear();

  if (_emitterMesh) {
    _emitterMesh->dispose();
    _emitterMesh = nullptr;
  }
}

Json::object ParticleSystemSet::serialize() const
{
  return Json::object();
}

unique_ptr_t<ParticleSystemSet>
ParticleSystemSet::Parse(const Json::value& /*data*/, Scene* /*scene*/,
                         bool /*gpu*/)
{
  return nullptr;
}

} // end of namespace BABYLON
