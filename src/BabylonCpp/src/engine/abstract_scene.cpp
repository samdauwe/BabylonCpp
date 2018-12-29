#include <babylon/engine/abstract_scene.h>

#include <babylon/audio/audio_engine.h>
#include <babylon/audio/sound.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/engine/asset_container.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene_component_constants.h>
#include <babylon/layer/effect_layer.h>
#include <babylon/layer/glow_layer.h>
#include <babylon/layer/highlight_layer.h>
#include <babylon/lensflare/lens_flare_system.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/particles/gpu_particle_system.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

std::unordered_map<std::string, BabylonFileParser>
  AbstractScene::_BabylonFileParsers;
std::unordered_map<std::string, IndividualBabylonFileParser>
  AbstractScene::_IndividualBabylonFileParsers;

AbstractScene::AbstractScene()
{
  _addIndividualParsers();
  _addParsers();
}

AbstractScene::~AbstractScene()
{
}

void AbstractScene::_addIndividualParsers()
{
  // Particle system parser
  AbstractScene::AddIndividualParser(
    SceneComponentConstants::NAME_PARTICLESYSTEM,
    [](const json& parsedParticleSystem, Scene* scene,
       const std::string& rootUrl) -> any {
      if (json_util::has_key(parsedParticleSystem, "activeParticleCount")) {
        auto ps
          = GPUParticleSystem::Parse(parsedParticleSystem, scene, rootUrl);
        return ps;
      }
      else {
        auto ps = ParticleSystem::Parse(parsedParticleSystem, scene, rootUrl);
        return ps;
      }
    });
}

void AbstractScene::_addParsers()
{
  // Effect layer parser
  AbstractScene::AddParser(
    SceneComponentConstants::NAME_EFFECTLAYER,
    [](const json& parsedData, Scene* scene, AssetContainer& container,
       const std::string& rootUrl) {
      if (json_util::has_key(parsedData, "effectLayers")) {
        for (const auto& effectLayer :
             json_util::get_array<json>(parsedData, "effectLayers")) {
          auto parsedEffectLayer
            = EffectLayer::Parse(effectLayer, scene, rootUrl);
          container.effectLayers.emplace_back(parsedEffectLayer);
        }
      }
    });
  // Lens flare system parser
  AbstractScene::AddParser(
    SceneComponentConstants::NAME_LENSFLARESYSTEM,
    [](const json& parsedData, Scene* scene, AssetContainer& container,
       const std::string& rootUrl) {
      // Lens flares
      if (json_util::has_key(parsedData, "lensFlareSystems")) {
        for (const auto& parsedLensFlareSystem :
             json_util::get_array<json>(parsedData, "lensFlareSystems")) {
          auto lf
            = LensFlareSystem::Parse(parsedLensFlareSystem, scene, rootUrl);
          container.lensFlareSystems.emplace_back(lf);
        }
      }
    });
  // Particle system parser
  AbstractScene::AddParser(
    SceneComponentConstants::NAME_PARTICLESYSTEM,
    [](const json& parsedData, Scene* scene, AssetContainer& container,
       const std::string& rootUrl) {
      auto individualParser = AbstractScene::GetIndividualParser(
        SceneComponentConstants::NAME_PARTICLESYSTEM);
      if (!individualParser) {
        return;
      }

      // Particles Systems
      if (json_util::has_key(parsedData, "particleSystems")) {
        for (const auto& parsedParticleSystem :
             json_util::get_array<json>(parsedData, "particleSystems")) {
          auto particleSystem
            = individualParser.value()(parsedParticleSystem, scene, rootUrl)
                ._<ParticleSystem*>();
          container.particleSystems.emplace_back(particleSystem);
        }
      }
    });
  // Shadows parser
  AbstractScene::AddParser(
    SceneComponentConstants::NAME_SHADOWGENERATOR,
    [](const json& parsedData, Scene* scene, AssetContainer& /*container*/,
       const std::string& /*rootUrl*/) {
      // Shadows
      if (json_util::has_key(parsedData, "shadowGenerators")) {
        for (const auto& parsedShadowGenerator :
             json_util::get_array<json>(parsedData, "shadowGenerators")) {
          ShadowGenerator::Parse(parsedShadowGenerator, scene);
          // SG would be available on their associated lights
        }
      }
    });
  // Sound parsers
  AbstractScene::AddParser(
    SceneComponentConstants::NAME_AUDIO,
    [](const json& parsedData, Scene* scene, AssetContainer& container,
       const std::string& rootUrl) {
      std::unordered_map<std::string, SoundPtr> loadedSounds;
      SoundPtr loadedSound;
      if (json_util::has_key(parsedData, "sounds")) {
        for (const auto& parsedSound :
             json_util::get_array<json>(parsedData, "sounds")) {
          auto parsedSoundName = json_util::get_string(parsedSound, "name");
          if (Engine::AudioEngine()->canUseWebAudio) {
            std::string parsedSoundUrl;
            if (!json_util::has_key(parsedSound, "url")) {
              parsedSoundUrl = parsedSoundName;
            }
            else {
              parsedSoundUrl = json_util::get_string(parsedSound, "url");
            }
            if (!stl_util::contains(loadedSounds, parsedSoundUrl)) {
              loadedSound = Sound::Parse(parsedSound, scene, rootUrl);
              loadedSounds[parsedSoundUrl] = loadedSound;
              container.sounds.emplace_back(loadedSound);
            }
            else {
              container.sounds.emplace_back(Sound::Parse(
                parsedSound, scene, rootUrl, loadedSounds[parsedSoundUrl]));
            }
          }
          else {
            container.sounds.emplace_back(
              Sound::New(parsedSoundName, std::nullopt, scene));
          }
        }
      }

      loadedSounds.clear();
    });
}

void AbstractScene::AddParser(const std::string& name,
                              const BabylonFileParser& parser)
{
  _BabylonFileParsers[name] = parser;
}

std::optional<BabylonFileParser>
AbstractScene::GetParser(const std::string& name)
{
  if (stl_util::contains(_BabylonFileParsers, name)) {
    return _BabylonFileParsers[name];
  }

  return std::nullopt;
}

void AbstractScene::AddIndividualParser(
  const std::string& name, const IndividualBabylonFileParser& parser)
{
  _IndividualBabylonFileParsers[name] = parser;
}

std::optional<IndividualBabylonFileParser>
AbstractScene::GetIndividualParser(const std::string& name)
{
  if (stl_util::contains(_IndividualBabylonFileParsers, name)) {
    return _IndividualBabylonFileParsers[name];
  }

  return std::nullopt;
}

void AbstractScene::Parse(const json& /*jsonData*/, Scene* /*scene*/,
                          const AssetContainerPtr& /*container*/,
                          const std::string& /*rootUrl*/)
{
#if 0
  for (const auto& _BabylonFileParserItem : _BabylonFileParsers) {
    _BabylonFileParserItem.second(jsonData, scene, container, rootUrl);
  }
#endif
}

int AbstractScene::removeEffectLayer(const EffectLayerPtr& toRemove)
{
  auto it   = std::find(effectLayers.begin(), effectLayers.end(), toRemove);
  int index = static_cast<int>(it - effectLayers.begin());
  if (it != effectLayers.end()) {
    effectLayers.erase(it);
  }
  return index;
}

void AbstractScene::addEffectLayer(const EffectLayerPtr& newEffectLayer)
{
  effectLayers.emplace_back(newEffectLayer);
}

GlowLayerPtr AbstractScene::getGlowLayerByName(const std::string& name)
{
  auto it = std::find_if(effectLayers.begin(), effectLayers.end(),
                         [&name](const EffectLayerPtr& effectLayer) {
                           return effectLayer->name == name
                                  && effectLayer->getEffectName()
                                       == GlowLayer::EffectName;
                         });

  return (it == effectLayers.end()) ? nullptr :
                                      std::static_pointer_cast<GlowLayer>(*it);
}

HighlightLayerPtr
AbstractScene::getHighlightLayerByName(const std::string& name)
{
  auto it = std::find_if(effectLayers.begin(), effectLayers.end(),
                         [&name](const EffectLayerPtr& effectLayer) {
                           return effectLayer->name == name
                                  && effectLayer->getEffectName()
                                       == HighlightLayer::EffectName;
                         });

  return (it == effectLayers.end()) ?
           nullptr :
           std::static_pointer_cast<HighlightLayer>(*it);
}

int AbstractScene::removeLensFlareSystem(const LensFlareSystemPtr& toRemove)
{
  auto it
    = std::find(lensFlareSystems.begin(), lensFlareSystems.end(), toRemove);
  int index = static_cast<int>(it - lensFlareSystems.begin());
  if (it != lensFlareSystems.end()) {
    lensFlareSystems.erase(it);
  }
  return index;
}

void AbstractScene::addLensFlareSystem(
  const LensFlareSystemPtr& newLensFlareSystem)
{
  lensFlareSystems.emplace_back(newLensFlareSystem);
}

LensFlareSystemPtr
AbstractScene::getLensFlareSystemByName(const std::string& name)
{
  auto it = std::find_if(lensFlareSystems.begin(), lensFlareSystems.end(),
                         [&name](const LensFlareSystemPtr& lensFlareSystem) {
                           return lensFlareSystem->name == name;
                         });

  return (it == lensFlareSystems.end()) ? nullptr : (*it);
}

LensFlareSystemPtr AbstractScene::getLensFlareSystemByID(const std::string& id)
{
  auto it = std::find_if(lensFlareSystems.begin(), lensFlareSystems.end(),
                         [&id](const LensFlareSystemPtr& lensFlareSystem) {
                           return lensFlareSystem->id == id;
                         });

  return (it == lensFlareSystems.end()) ? nullptr : (*it);
}

} // end of namespace BABYLON
