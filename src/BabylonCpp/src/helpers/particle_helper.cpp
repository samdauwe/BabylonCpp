#include <babylon/helpers/particle_helper.h>

#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/helpers/iparticleSystem_data.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/particles/gpu_particle_system.h>
#include <babylon/particles/particle_system.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

constexpr const char* ParticleHelper::_baseAssetsUrl;
Scene* ParticleHelper::_scene;
AbstractMesh* ParticleHelper::_emitter;

ParticleHelper* ParticleHelper::CreateAsync(const string_t& type,
                                            AbstractMesh* emitter, Scene* scene,
                                            bool gpu)
{
  scene = scene ? scene : Engine::LastCreatedScene();

  if (scene) {
    _scene = scene;
  }
  else {
    BABYLON_LOG_ERROR("A particle system need a scene.");
    return nullptr;
  }

  if (gpu && !GPUParticleSystem::IsSupported()) {
    BABYLON_LOG_ERROR("Particle system with GPU is not supported.");
    return nullptr;
  }

  _emitter = emitter;

  const auto url = String::concat(_baseAssetsUrl, "/systems/", type, ".json");
  Tools::LoadFile(url, [](const string_t&) {}, nullptr, false);
  return nullptr;
}

ParticleSystem* ParticleHelper::_createSystem(IParticleSystemData& data)
{
  // Create a particle system
  auto system = new ParticleSystem(data.type, data.capacity, _scene);
  // Texture of each particle
  const auto url
    = String::concat(_baseAssetsUrl, "/textures/", data.textureFile);
  system->particleTexture = Texture::New(url, _scene);
  // Where the particles come from
  system->emitter = ToVariant<AbstractMesh*, Vector3>(
    _emitter); // the starting object, the emitter

  // Colors of all particles
  system->color1
    = Color4(data.color1.r, data.color1.g, data.color1.b, data.color1.a);
  system->color2
    = Color4(data.color2.r, data.color2.g, data.color2.b, data.color2.a);
  system->colorDead = Color4(data.colorDead.r, data.colorDead.g,
                             data.colorDead.b, data.colorDead.a);

  // Size of each particle (random between...
  system->minSize = data.minSize;
  system->maxSize = data.maxSize;

  // Life time of each particle (random between...
  system->minLifeTime = data.minLifeTime;
  system->maxLifeTime = data.maxLifeTime;

  // Emission rate
  system->emitRate = data.emitRate;

  // Blend mode : BLENDMODE_ONEONE, or BLENDMODE_STANDARD
  system->blendMode = data.blendMode;

  // Set the gravity of all particles
  system->gravity = Vector3(data.gravity.x, data.gravity.y, data.gravity.z);

  // Angular speed, in radians
  system->minAngularSpeed = data.minAngularSpeed;
  system->maxAngularSpeed = data.maxAngularSpeed;

  // Speed
  system->minEmitPower = data.minEmitPower;
  system->maxEmitPower = data.maxEmitPower;
  system->updateSpeed  = data.updateSpeed;

  if (data.emitterType == "box") {
    if (!data.direction1 || !data.direction2) {
      throw ::std::runtime_error(
        "Directions are missing in this particle system.");
    }

    if (!data.minEmitBox || !data.maxEmitBox) {
      throw ::std::runtime_error("EmitBox is missing in this particle system.");
    }

    system->createBoxEmitter(
      Vector3((*data.direction1).x, (*data.direction1).y, (*data.direction1).z),
      Vector3((*data.direction2).x, (*data.direction2).y, (*data.direction2).z),
      Vector3((*data.minEmitBox).x, (*data.minEmitBox).y, (*data.minEmitBox).z),
      Vector3((*data.maxEmitBox).x, (*data.maxEmitBox).y,
              (*data.maxEmitBox).z));
  }
  else if (data.emitterType == "sphere") {
    system->createSphereEmitter(*data.radius);
  }
  else if (data.emitterType == "directed_sphere") {
    if (!data.direction1 || !data.direction2) {
      throw ::std::runtime_error(
        "Directions are missing in this particle system.");
    }

    system->createDirectedSphereEmitter(
      data.radius,
      Vector3((*data.direction1).x, (*data.direction1).y, (*data.direction1).z),
      Vector3((*data.direction2).x, (*data.direction2).y,
              (*data.direction2).z));
  }
  else if (data.emitterType == "cone") {
    system->createConeEmitter(*data.radius, *data.angle);
  }

  return system;
}

} // end of namespace BABYLON
