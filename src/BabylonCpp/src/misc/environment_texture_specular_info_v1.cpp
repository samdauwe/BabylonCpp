#include <babylon/misc/environment_texture_specular_info_v1.h>

#include <babylon/core/json_util.h>

namespace BABYLON {

EnvironmentTextureSpecularInfoV1::EnvironmentTextureSpecularInfoV1()
    : specularDataPosition{std::nullopt}, lodGenerationScale{std::nullopt}
{
}

EnvironmentTextureSpecularInfoV1::EnvironmentTextureSpecularInfoV1(
  const EnvironmentTextureSpecularInfoV1& other)
  = default;

EnvironmentTextureSpecularInfoV1::EnvironmentTextureSpecularInfoV1(
  EnvironmentTextureSpecularInfoV1&& other)
  = default;

EnvironmentTextureSpecularInfoV1&
EnvironmentTextureSpecularInfoV1::operator=(const EnvironmentTextureSpecularInfoV1& other)
  = default;

EnvironmentTextureSpecularInfoV1&
EnvironmentTextureSpecularInfoV1::operator=(EnvironmentTextureSpecularInfoV1&& other)
  = default;

EnvironmentTextureSpecularInfoV1::~EnvironmentTextureSpecularInfoV1() = default;

EnvironmentTextureSpecularInfoV1Ptr
EnvironmentTextureSpecularInfoV1::Parse(const json& parsedSpecular)
{
  EnvironmentTextureSpecularInfoV1 specular;

  // specularDataPosition
  if (json_util::has_key(parsedSpecular, "specularDataPosition")
      && parsedSpecular["specularDataPosition"].is_number()) {
    specular.specularDataPosition
      = json_util::get_number<size_t>(parsedSpecular, "specularDataPosition");
  }

  // mipmaps
  if (json_util::has_key(parsedSpecular, "mipmaps") && parsedSpecular["mipmaps"].is_array()) {
    auto mipmaps = parsedSpecular["mipmaps"].get<std::vector<json>>();
    for (const auto& mipmap : mipmaps) {
      specular.mipmaps.emplace_back(BufferImageData{
        json_util::get_number<size_t>(mipmap, "length"),   // length
        json_util::get_number<size_t>(mipmap, "position"), // position
      });
    }
  }

  // specularDataPosition
  if (json_util::has_key(parsedSpecular, "lodGenerationScale")
      && parsedSpecular["lodGenerationScale"].is_number()) {
    specular.lodGenerationScale
      = json_util::get_number<float>(parsedSpecular, "lodGenerationScale");
  }

  return std::make_shared<EnvironmentTextureSpecularInfoV1>(specular);
}

} // end of namespace BABYLON
