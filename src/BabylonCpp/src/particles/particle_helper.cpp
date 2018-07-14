#include <babylon/particles/particle_helper.h>

#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/particles/gpu_particle_system.h>
#include <babylon/particles/particle_system_set.h>

namespace BABYLON {

constexpr const char* ParticleHelper::BaseAssetsUrl;

ParticleSystemSet* ParticleHelper::CreateAsync(const string_t& /*type*/,
                                               Scene* scene, bool gpu)
{
  if (!scene) {
    scene = Engine::LastCreatedScene();
  }

  if (gpu && !GPUParticleSystem::IsSupported()) {
    BABYLON_LOG_ERROR("Particle system with GPU is not supported.");
    return nullptr;
  }

  // const auto url = String::concat(ParticleHelper::BaseAssetsUrl, "/systems/",
  // type, ".json");
  // Tools::LoadFile(url, [](const string_t&, const string_t&) {}, nullptr,
  // false);
  return nullptr;
}

unique_ptr_t<ParticleSystemSet>
ParticleHelper::_createSystem(const vector_t<IParticleSystem*>& systems)
{
  auto set = ::std::make_unique<ParticleSystemSet>();

  for (auto& system : systems) {
    set->systems.emplace_back(system);
  }

  return set;
}

} // end of namespace BABYLON
