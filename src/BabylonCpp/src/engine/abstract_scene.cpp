#include <babylon/engine/abstract_scene.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json.h>
#include <babylon/engine/asset_container.h>
#include <babylon/engine/scene_component_constants.h>
#include <babylon/layer/effect_layer.h>
#include <babylon/layer/glow_layer.h>
#include <babylon/layer/highlight_layer.h>
#include <babylon/lensflare/lens_flare_system.h>
#include <babylon/particles/gpu_particle_system.h>
#include <babylon/particles/particle_system.h>

namespace BABYLON {

unordered_map_t<string_t, BabylonFileParser> AbstractScene::_BabylonFileParsers;
unordered_map_t<string_t, IndividualBabylonFileParser>
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
    [](const Json::value& parsedParticleSystem, Scene* scene,
       const string_t& rootUrl) -> any {
      if (parsedParticleSystem.contains("activeParticleCount")) {
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
    [](const Json::value& parsedData, Scene* scene, AssetContainer& container,
       const string_t& rootUrl) {
      if (parsedData.contains("effectLayers")) {
        for (const auto& effectLayer :
             Json::GetArray(parsedData, "effectLayers")) {
          auto parsedEffectLayer
            = EffectLayer::Parse(effectLayer, scene, rootUrl);
          container.effectLayers.emplace_back(parsedEffectLayer);
        }
      }
    });
  // Lens flare system parser
  AbstractScene::AddParser(
    SceneComponentConstants::NAME_LENSFLARESYSTEM,
    [](const Json::value& parsedData, Scene* scene, AssetContainer& container,
       const string_t& rootUrl) {
      // Lens flares
      if (parsedData.contains("lensFlareSystems")) {
        for (const auto& parsedLensFlareSystem :
             Json::GetArray(parsedData, "lensFlareSystems")) {
          auto lf
            = LensFlareSystem::Parse(parsedLensFlareSystem, scene, rootUrl);
          container.lensFlareSystems.emplace_back(lf);
        }
      }
    });
  // Particle system parser
  AbstractScene::AddParser(
    SceneComponentConstants::NAME_PARTICLESYSTEM,
    [](const Json::value& parsedData, Scene* scene, AssetContainer& container,
       const string_t& rootUrl) {
      auto individualParser = AbstractScene::GetIndividualParser(
        SceneComponentConstants::NAME_PARTICLESYSTEM);
      if (!individualParser) {
        return;
      }

      // Particles Systems
      if (parsedData.contains("particleSystems")) {
        for (const auto& parsedParticleSystem :
             Json::GetArray(parsedData, "particleSystems")) {
          auto particleSystem
            = individualParser.value()(parsedParticleSystem, scene, rootUrl)
                ._<ParticleSystem*>();
          container.particleSystems.emplace_back(particleSystem);
        }
      }
    });
}

void AbstractScene::AddParser(const string_t& name,
                              const BabylonFileParser& parser)
{
  _BabylonFileParsers[name] = parser;
}

nullable_t<BabylonFileParser> AbstractScene::GetParser(const string_t& name)
{
  if (stl_util::contains(_BabylonFileParsers, name)) {
    return _BabylonFileParsers[name];
  }

  return nullopt_t;
}

void AbstractScene::AddIndividualParser(
  const string_t& name, const IndividualBabylonFileParser& parser)
{
  _IndividualBabylonFileParsers[name] = parser;
}

nullable_t<IndividualBabylonFileParser>
AbstractScene::GetIndividualParser(const string_t& name)
{
  if (stl_util::contains(_IndividualBabylonFileParsers, name)) {
    return _IndividualBabylonFileParsers[name];
  }

  return nullopt_t;
}

void AbstractScene::Parse(Json::value& jsonData, Scene* scene,
                          AssetContainer& container, const string_t& rootUrl)
{
  for (const auto& _BabylonFileParserItem : _BabylonFileParsers) {
    _BabylonFileParserItem.second(jsonData, scene, container, rootUrl);
  }
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

GlowLayerPtr AbstractScene::getGlowLayerByName(const string_t& name)
{
  auto it = std::find_if(effectLayers.begin(), effectLayers.end(),
                           [&name](const EffectLayerPtr& effectLayer) {
                             return effectLayer->name == name
                                    && effectLayer->getEffectName()
                                         == GlowLayer::EffectName;
                           });

  return (it == effectLayers.end()) ?
           nullptr :
           std::static_pointer_cast<GlowLayer>(*it);
}

HighlightLayerPtr AbstractScene::getHighlightLayerByName(const string_t& name)
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

LensFlareSystemPtr AbstractScene::getLensFlareSystemByName(const string_t& name)
{
  auto it = std::find_if(lensFlareSystems.begin(), lensFlareSystems.end(),
                           [&name](const LensFlareSystemPtr& lensFlareSystem) {
                             return lensFlareSystem->name == name;
                           });

  return (it == lensFlareSystems.end()) ? nullptr : (*it);
}

LensFlareSystemPtr AbstractScene::getLensFlareSystemByID(const string_t& id)
{
  auto it = std::find_if(lensFlareSystems.begin(), lensFlareSystems.end(),
                           [&id](const LensFlareSystemPtr& lensFlareSystem) {
                             return lensFlareSystem->id == id;
                           });

  return (it == lensFlareSystems.end()) ? nullptr : (*it);
}

} // end of namespace BABYLON
