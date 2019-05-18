#include <babylon/materials/textures/loaders/env_texture_loader.h>

#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/tools/environment_texture_info.h>
#include <babylon/tools/environment_texture_tools.h>

namespace BABYLON {

ENVTextureLoader::ENVTextureLoader()
{
}

ENVTextureLoader::~ENVTextureLoader()
{
}

bool ENVTextureLoader::supportCascades() const
{
  return false;
}

bool ENVTextureLoader::canLoad(const std::string& extension,
                               const std::string& /*textureFormatInUse*/,
                               const InternalTexturePtr& /*fallback*/,
                               bool /*isBase64*/, bool /*isBuffer*/)
{
  return String::endsWith(extension, ".env");
}

std::string
ENVTextureLoader::transformUrl(const std::string& rootUrl,
                               const std::string& /*textureFormatInUse*/)
{
  return rootUrl;
}

std::string ENVTextureLoader::getFallbackTextureUrl(
  const std::string& /*rootUrl*/, const std::string& /*textureFormatInUse*/)
{
  return "";
}

void ENVTextureLoader::loadCubeData(
  const std::variant<std::string, ArrayBuffer>& iData,
  const InternalTexturePtr& texture, bool /*createPolynomials*/,
  const std::function<void(const CubeTextureData& data)>& onLoad,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError)
{
  if (!std::holds_alternative<ArrayBuffer>(iData)) {
    return;
  }

  auto data = std::get<ArrayBuffer>(iData);
  auto info = EnvironmentTextureTools::GetEnvInfo(data);
  if (info) {
    texture->width  = info->width;
    texture->height = info->width;

    EnvironmentTextureTools::UploadEnvSpherical(texture, *info);
    EnvironmentTextureTools::UploadEnvLevels(texture, data, *info);
    {
      texture->isReady = true;
      if (onLoad) {
        // onLoad();
      }
    };
  }
  else if (onError) {
    onError("Can not parse the environment file", "");
  }
}

void ENVTextureLoader::loadCubeData(
  const std::vector<std::variant<std::string, ArrayBuffer>>& /*data*/,
  const InternalTexturePtr& /*texture*/, bool /*createPolynomials*/,
  const std::function<void(const CubeTextureData& data)>& /*onLoad*/,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& /*onError*/)
{
}

void ENVTextureLoader::loadData(
  const ArrayBuffer& /*data*/, const InternalTexturePtr& /*texture*/,
  const std::function<void(int /*width*/, int /*height*/, bool /*loadMipmap*/,
                           bool /*isCompressed*/,
                           const std::function<void()>& done)>& /*callback*/)
{
  throw std::runtime_error(".env not supported in 2d.");
}

} // end of namespace BABYLON
