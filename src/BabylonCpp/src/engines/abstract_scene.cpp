#include <babylon/engines/abstract_scene.h>

#include <babylon/audio/audio_engine.h>
#include <babylon/audio/sound.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/engines/asset_container.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/node.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/layers/effect_layer.h>
#include <babylon/layers/glow_layer.h>
#include <babylon/layers/highlight_layer.h>
#include <babylon/lensflares/lens_flare_system.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/cascaded_shadow_generator.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/particles/gpu_particle_system.h>
#include <babylon/particles/particle_system.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/sub_surface_configuration.h>

namespace BABYLON {

std::unordered_map<std::string, BabylonFileParser> AbstractScene::_BabylonFileParsers;
std::unordered_map<std::string, IndividualBabylonFileParser>
  AbstractScene::_IndividualBabylonFileParsers;

AbstractScene::AbstractScene()
    : environmentTexture{this, &AbstractScene::get_environmentTexture,
                         &AbstractScene::set_environmentTexture}
    , subSurfaceConfiguration{this, &AbstractScene::get_subSurfaceConfiguration,
                              &AbstractScene::set_subSurfaceConfiguration}
    , _environmentTexture{nullptr}
    , _subSurfaceConfiguration{nullptr}
{
  _addIndividualParsers();
  _addParsers();
}

AbstractScene::~AbstractScene() = default;

void AbstractScene::_addIndividualParsers()
{
  // Particle system parser
  AbstractScene::AddIndividualParser(
    SceneComponentConstants::NAME_PARTICLESYSTEM,
    [](const json& parsedParticleSystem, Scene* scene, const std::string& rootUrl) -> any {
      if (json_util::has_valid_key_value(parsedParticleSystem, "activeParticleCount")) {
        auto ps = GPUParticleSystem::Parse(parsedParticleSystem, scene, rootUrl);
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
      if (json_util::has_valid_key_value(parsedData, "effectLayers")) {
        for (const auto& effectLayer : json_util::get_array<json>(parsedData, "effectLayers")) {
          auto parsedEffectLayer = EffectLayer::Parse(effectLayer, scene, rootUrl);
          container.effectLayers.emplace_back(parsedEffectLayer);
        }
      }
    });
  // Lens flare system parser
  AbstractScene::AddParser(SceneComponentConstants::NAME_LENSFLARESYSTEM,
                           [](const json& parsedData, Scene* scene, AssetContainer& container,
                              const std::string& rootUrl) {
                             // Lens flares
                             if (json_util::has_valid_key_value(parsedData, "lensFlareSystems")) {
                               for (const auto& parsedLensFlareSystem :
                                    json_util::get_array<json>(parsedData, "lensFlareSystems")) {
                                 auto lf
                                   = LensFlareSystem::Parse(parsedLensFlareSystem, scene, rootUrl);
                                 container.lensFlareSystems.emplace_back(lf);
                               }
                             }
                           });
  // Particle system parser
  AbstractScene::AddParser(SceneComponentConstants::NAME_PARTICLESYSTEM,
                           [](const json& parsedData, Scene* scene, AssetContainer& container,
                              const std::string& rootUrl) {
                             auto individualParser = AbstractScene::GetIndividualParser(
                               SceneComponentConstants::NAME_PARTICLESYSTEM);
                             if (!individualParser) {
                               return;
                             }

                             // Particles Systems
                             if (json_util::has_valid_key_value(parsedData, "particleSystems")) {
                               for (const auto& parsedParticleSystem :
                                    json_util::get_array<json>(parsedData, "particleSystems")) {
                                 auto particleSystem = reinterpret_cast<ParticleSystem*>(
                                   individualParser.value()(parsedParticleSystem, scene, rootUrl));
                                 container.particleSystems.emplace_back(particleSystem);
                               }
                             }
                           });
  // Shadows parser
  AbstractScene::AddParser(SceneComponentConstants::NAME_SHADOWGENERATOR,
                           [](const json& parsedData, Scene* scene, AssetContainer& /*container*/,
                              const std::string& /*rootUrl*/) {
                             // Shadows
                             if (json_util::has_valid_key_value(parsedData, "shadowGenerators")) {
                               for (const auto& parsedShadowGenerator :
                                    json_util::get_array<json>(parsedData, "shadowGenerators")) {
                                 if (json_util::get_string(parsedShadowGenerator, "className")
                                     == CascadedShadowGenerator::CLASSNAME) {
                                   CascadedShadowGenerator::Parse(parsedShadowGenerator, scene);
                                 }
                                 else {
                                   ShadowGenerator::Parse(parsedShadowGenerator, scene);
                                 }
                                 // SG would be available on their associated lights
                               }
                             }
                           });
  // Sound parsers
  AbstractScene::AddParser(
    SceneComponentConstants::NAME_AUDIO, [](const json& parsedData, Scene* scene,
                                            AssetContainer& container, const std::string& rootUrl) {
      std::unordered_map<std::string, SoundPtr> loadedSounds;
      SoundPtr loadedSound;
      if (json_util::has_valid_key_value(parsedData, "sounds")) {
        for (const auto& parsedSound : json_util::get_array<json>(parsedData, "sounds")) {
          auto parsedSoundName = json_util::get_string(parsedSound, "name");
          if (Engine::audioEngine->canUseWebAudio) {
            std::string parsedSoundUrl;
            if (!json_util::has_valid_key_value(parsedSound, "url")) {
              parsedSoundUrl = parsedSoundName;
            }
            else {
              parsedSoundUrl = json_util::get_string(parsedSound, "url");
            }
            if (!stl_util::contains(loadedSounds, parsedSoundUrl)) {
              loadedSound                  = Sound::Parse(parsedSound, scene, rootUrl);
              loadedSounds[parsedSoundUrl] = loadedSound;
              container.sounds.emplace_back(loadedSound);
            }
            else {
              container.sounds.emplace_back(
                Sound::Parse(parsedSound, scene, rootUrl, loadedSounds[parsedSoundUrl]));
            }
          }
          else {
            container.sounds.emplace_back(Sound::New(parsedSoundName, std::nullopt, scene));
          }
        }
      }

      loadedSounds.clear();
    });
  // Adds the parser to the scene parsers.
  AbstractScene::AddParser(
    SceneComponentConstants::NAME_SUBSURFACE,
    [](const json& parsedData, Scene* scene, AssetContainer& /*container*/,
       const std::string& /*rootUrl*/) {
      // Diffusion profiles
      if (json_util::has_valid_key_value(parsedData, "ssDiffusionProfileColors")) {
        scene->enableSubSurfaceForPrePass();
        if (scene->subSurfaceConfiguration()) {
          for (const auto& color :
               json_util::get_array<json>(parsedData, "ssDiffusionProfileColors")) {
            scene->subSurfaceConfiguration()->addDiffusionProfile(Color3(
              json_util::get_number<float>(color, "r"), json_util::get_number<float>(color, "g"),
              json_util::get_number<float>(color, "b")));
          }
        }
      }
    });
}

void AbstractScene::AddParser(const std::string& name, const BabylonFileParser& parser)
{
  _BabylonFileParsers[name] = parser;
}

std::optional<BabylonFileParser> AbstractScene::GetParser(const std::string& name)
{
  if (stl_util::contains(_BabylonFileParsers, name)) {
    return _BabylonFileParsers[name];
  }

  return std::nullopt;
}

void AbstractScene::AddIndividualParser(const std::string& name,
                                        const IndividualBabylonFileParser& parser)
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
                          const AssetContainerPtr& /*container*/, const std::string& /*rootUrl*/)
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
  auto it = std::find_if(
    effectLayers.begin(), effectLayers.end(), [&name](const EffectLayerPtr& effectLayer) {
      return effectLayer->name == name && effectLayer->getEffectName() == GlowLayer::EffectName;
    });

  return (it == effectLayers.end()) ? nullptr : std::static_pointer_cast<GlowLayer>(*it);
}

HighlightLayerPtr AbstractScene::getHighlightLayerByName(const std::string& name)
{
  auto it = std::find_if(effectLayers.begin(), effectLayers.end(),
                         [&name](const EffectLayerPtr& effectLayer) {
                           return effectLayer->name == name
                                  && effectLayer->getEffectName() == HighlightLayer::EffectName;
                         });

  return (it == effectLayers.end()) ? nullptr : std::static_pointer_cast<HighlightLayer>(*it);
}

int AbstractScene::removeLensFlareSystem(const LensFlareSystemPtr& toRemove)
{
  auto it   = std::find(lensFlareSystems.begin(), lensFlareSystems.end(), toRemove);
  int index = static_cast<int>(it - lensFlareSystems.begin());
  if (it != lensFlareSystems.end()) {
    lensFlareSystems.erase(it);
  }
  return index;
}

void AbstractScene::addLensFlareSystem(const LensFlareSystemPtr& newLensFlareSystem)
{
  lensFlareSystems.emplace_back(newLensFlareSystem);
}

LensFlareSystemPtr AbstractScene::getLensFlareSystemByName(const std::string& name)
{
  auto it = std::find_if(
    lensFlareSystems.begin(), lensFlareSystems.end(),
    [&name](const LensFlareSystemPtr& lensFlareSystem) { return lensFlareSystem->name == name; });

  return (it == lensFlareSystems.end()) ? nullptr : (*it);
}

LensFlareSystemPtr AbstractScene::getLensFlareSystemByID(const std::string& id)
{
  auto it = std::find_if(
    lensFlareSystems.begin(), lensFlareSystems.end(),
    [&id](const LensFlareSystemPtr& lensFlareSystem) { return lensFlareSystem->id == id; });

  return (it == lensFlareSystems.end()) ? nullptr : (*it);
}

int AbstractScene::removeReflectionProbe(const ReflectionProbePtr& toRemove)
{
  if (reflectionProbes.empty()) {
    return -1;
  }

  auto it    = std::find(reflectionProbes.begin(), reflectionProbes.end(), toRemove);
  auto index = static_cast<int>(it - reflectionProbes.begin());
  if (it != reflectionProbes.end()) {
    reflectionProbes.erase(it);
  }
  return index;
}

void AbstractScene::addReflectionProbe(const ReflectionProbePtr& newReflectionProbe)
{
  reflectionProbes.emplace_back(newReflectionProbe);
}

BaseTexturePtr& AbstractScene::get_environmentTexture()
{
  return _environmentTexture;
}

void AbstractScene::set_environmentTexture(const BaseTexturePtr& value)
{
  _environmentTexture = value;
}

SubSurfaceConfigurationPtr& AbstractScene::get_subSurfaceConfiguration()
{
  return _subSurfaceConfiguration;
}

void AbstractScene::set_subSurfaceConfiguration(const SubSurfaceConfigurationPtr& /*value*/)
{
}

std::vector<NodePtr> AbstractScene::getNodes() const
{
  std::vector<NodePtr> nodes;
  for (const auto& mesh : meshes) {
    nodes.emplace_back(std::static_pointer_cast<Node>(mesh));
  }
  for (const auto& light : lights) {
    nodes.emplace_back(std::static_pointer_cast<Node>(light));
  }
  for (const auto& camera : cameras) {
    nodes.emplace_back(std::static_pointer_cast<Node>(camera));
  }
  for (const auto& transformNode : transformNodes) { // dummies
    nodes.emplace_back(std::static_pointer_cast<Node>(transformNode));
  }
  for (const auto& skeleton : skeletons) {
    for (const auto& bone : skeleton->bones) {
      nodes.emplace_back(std::static_pointer_cast<Node>(bone));
    }
  }
  return nodes;
}

SubSurfaceConfigurationPtr AbstractScene::enableSubSurfaceForPrePass()
{
  return nullptr;
}

void AbstractScene::disableSubSurfaceForPrePass()
{
}

} // end of namespace BABYLON
