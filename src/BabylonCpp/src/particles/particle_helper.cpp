#include <babylon/particles/particle_helper.h>

#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/particles/gpu_particle_system.h>
#include <babylon/particles/particle_system.h>
#include <babylon/particles/particle_system_set.h>

namespace BABYLON {

const char* ParticleHelper::BaseAssetsUrl = ParticleSystemSet::BaseAssetsUrl;

IParticleSystem*
ParticleHelper::CreateDefault(const std::variant<AbstractMeshPtr, Vector3>& emitter,
                              size_t capacity, Scene* scene, bool useGPU)
{
  IParticleSystem* system = nullptr;
  if (useGPU) {
    system = new GPUParticleSystem("default system", capacity, std::nullopt, scene);
  }
  else {
    system = new ParticleSystem("default system", capacity, scene);
  }

  system->emitter = emitter;
  system->particleTexture
    = Texture::New("https://www.babylonjs.com/assets/Flare.png", system->getScene());
  system->createConeEmitter(0.1f, Math::PI / 4.f);

  // Particle color
  system->color1    = Color4(1.f, 1.f, 1.f, 1.f);
  system->color2    = Color4(1.f, 1.f, 1.f, 1.f);
  system->colorDead = Color4(1.f, 1.f, 1.f, 0.f);

  // Particle Size
  system->minSize = 0.1f;
  system->maxSize = 0.1f;

  // Emission speed
  system->minEmitPower = 2.f;
  system->maxEmitPower = 2.f;

  // Update speed
  system->updateSpeed = 1.f / 60.f;

  system->emitRate = 30;

  return system;
}

ParticleSystemSet* ParticleHelper::CreateAsync(const std::string& /*type*/, Scene* /*scene*/,
                                               bool /*gpu*/)
{
  return nullptr;
}

std::unique_ptr<ParticleSystemSet>
ParticleHelper::ExportSet(const std::vector<IParticleSystem*>& systems)
{
  auto set = std::make_unique<ParticleSystemSet>();

  for (auto& system : systems) {
    set->systems.emplace_back(system);
  }

  return set;
}

} // end of namespace BABYLON
