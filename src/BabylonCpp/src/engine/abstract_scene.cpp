#include <babylon/engine/abstract_scene.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

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
                          const string_t& rootUrl)
{
  for (const auto& _BabylonFileParserItem : _BabylonFileParsers) {
    _BabylonFileParserItem.second(jsonData, scene, rootUrl);
  }
}

} // end of namespace BABYLON
