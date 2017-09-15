#include <babylon/particles/gpu_particle_system.h>

#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

GPUParticleSystem::GPUParticleSystem(const string_t& name,
                                     std::size_t /*capacity*/, Scene* scene)
    : _scene{scene ? scene : Engine::LastCreatedScene()}
{
  id               = name;
  renderingGroupId = 0;
  layerMask        = 0x0FFFFFFF;

  scene->particleSystems.emplace_back(this);
}

GPUParticleSystem::~GPUParticleSystem()
{
}

bool GPUParticleSystem::isStarted() const
{
  return false;
}

void GPUParticleSystem::animate()
{
}

size_t GPUParticleSystem::render()
{
  return 0;
}

void GPUParticleSystem::dispose(bool /*doNotRecurse*/)
{
  // Remove from scene
  _scene->particleSystems.erase(
    ::std::remove_if(
      _scene->particleSystems.begin(), _scene->particleSystems.end(),
      [this](const unique_ptr_t<IParticleSystem>& particleSystem) {
        return particleSystem.get() == this;
      }),
    _scene->particleSystems.end());

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

IParticleSystem* GPUParticleSystem::clone(const string_t& /*name*/,
                                          Mesh* /*newEmitter*/)
{
  return nullptr;
}

Json::object GPUParticleSystem::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
