#include <babylon/materials/textures/loaders/tga_texture_loader.h>

#include <babylon/core/string.h>
//#include <babylon/engines/engine.h>
#include <babylon/materials/textures/internal_texture.h>
//#include <babylon/misc/environment_texture_info.h>
#include <babylon/misc/tga.h>

namespace BABYLON {

_TGATextureLoader::_TGATextureLoader() = default;

_TGATextureLoader::~_TGATextureLoader() = default;

bool _TGATextureLoader::supportCascades() const
{
  return false;
}

bool _TGATextureLoader::canLoad(const std::string& extension,
                                const std::string& /*textureFormatInUse*/,
                                const InternalTexturePtr& /*fallback*/,
                                bool /*isBase64*/, bool /*isBuffer*/)
{
  return String::startsWith(extension, ".tga");
}

std::string
_TGATextureLoader::transformUrl(const std::string& rootUrl,
                                const std::string& /*textureFormatInUse*/)
{
  return rootUrl;
}

std::string _TGATextureLoader::getFallbackTextureUrl(
  const std::string& /*rootUrl*/, const std::string& /*textureFormatInUse*/)
{
  return "";
}

void _TGATextureLoader::loadCubeData(
  const std::variant<std::string, ArrayBuffer>& /*iData*/,
  const InternalTexturePtr& /*texture*/, bool /*createPolynomials*/,
  const std::function<void(const CubeTextureData& data)>& /*onLoad*/,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& /*onError*/)
{
  throw std::runtime_error(".env not supported in Cube.");
}

void _TGATextureLoader::loadCubeData(
  const std::vector<std::variant<std::string, ArrayBuffer>>& /*data*/,
  const InternalTexturePtr& /*texture*/, bool /*createPolynomials*/,
  const std::function<void(const CubeTextureData& data)>& /*onLoad*/,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& /*onError*/)
{
  throw std::runtime_error(".env not supported in Cube.");
}

void _TGATextureLoader::loadData(
  const ArrayBuffer& data, const InternalTexturePtr& texture,
  const std::function<void(int width, int height, bool loadMipmap,
                           bool isCompressed, const std::function<void()>& done,
                           bool loadFailed)>& callback)
{
  const auto& uintData = data;

  auto header = TGATools::GetTGAHeader(uintData);
  callback(
    header.width, header.height, texture->generateMipMaps, false,
    [&texture, &uintData]() -> void {
      TGATools::UploadContent(texture, uintData);
    },
    false);
}

} // end of namespace BABYLON
