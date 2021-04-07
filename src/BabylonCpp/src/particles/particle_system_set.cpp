#include <babylon/particles/particle_system_set.h>

#include <nlohmann/json.hpp>

#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/mesh.h>
#include <babylon/particles/iparticle_system.h>

namespace BABYLON {

ParticleSystemSet::ParticleSystemSet()
    : emitterNode{this, &ParticleSystemSet::get_emitterNode, &ParticleSystemSet::set_emitterNode}
    , _emitterNode{std::nullopt}
    , _emitterNodeIsOwned{true}
{
}

ParticleSystemSet::~ParticleSystemSet() = default;

ParticleSystemSet::EmitterNodeType& ParticleSystemSet::get_emitterNode()
{
  return _emitterNode;
}

void ParticleSystemSet::set_emitterNode(const EmitterNodeType& value)
{
  if (_emitterNodeIsOwned && _emitterNode) {
    if (value && std::holds_alternative<AbstractMeshPtr>(*value)
        && std::get<AbstractMeshPtr>(*value)) {
      std::get<AbstractMeshPtr>(*value)->dispose();
    }
    _emitterNodeIsOwned = false;
  }

  if (value) {
    for (const auto& system : systems) {
      if (std::holds_alternative<AbstractMeshPtr>(*value)) {
        system->emitter = std::get<AbstractMeshPtr>(*value);
      }
      if (std::holds_alternative<Vector3>(*value)) {
        system->emitter = std::get<Vector3>(*value);
      }
    }
  }

  _emitterNode = value;
}

void ParticleSystemSet::setEmitterAsSphere(const EmitterCreationOptions& options,
                                           unsigned int renderingGroupId, Scene* scene)
{
  if (_emitterNodeIsOwned && _emitterNode) {
    if (_emitterNode && std::holds_alternative<AbstractMeshPtr>(*_emitterNode)
        && std::get<AbstractMeshPtr>(*_emitterNode)) {
      std::get<AbstractMeshPtr>(*_emitterNode)->dispose();
    }
  }

  _emitterNodeIsOwned = true;

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
    if (_emitterNode && std::holds_alternative<AbstractMeshPtr>(*_emitterNode)
        && std::get<AbstractMeshPtr>(*_emitterNode)) {
      std::get<AbstractMeshPtr>(*_emitterNode)->dispose();
    }
    _emitterNode = std::nullopt;
  }
}

json ParticleSystemSet::serialize(bool /*serializeTexture*/) const
{
  return nullptr;
}

std::unique_ptr<ParticleSystemSet> ParticleSystemSet::Parse(const json& /*data*/, Scene* /*scene*/,
                                                            bool /*gpu*/)
{
  return nullptr;
}

} // end of namespace BABYLON
