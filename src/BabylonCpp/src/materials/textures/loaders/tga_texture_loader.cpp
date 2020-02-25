#include <babylon/materials/textures/loaders/tga_texture_loader.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/misc/string_tools.h>
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
                                const InternalTexturePtr& /*fallback*/, bool /*isBase64*/,
                                bool /*isBuffer*/)
{
  return StringTools::startsWith(extension, ".tga");
}

std::string _TGATextureLoader::transformUrl(const std::string& rootUrl,
                                            const std::string& /*textureFormatInUse*/)
{
  return rootUrl;
}

std::string _TGATextureLoader::getFallbackTextureUrl(const std::string& /*rootUrl*/,
                                                     const std::string& /*textureFormatInUse*/)
{
  return "";
}

void _TGATextureLoader::loadCubeData(
  const std::variant<std::string, ArrayBufferView>& /*iData*/,
  const InternalTexturePtr& /*texture*/, bool /*createPolynomials*/,
  const std::function<void(const std::optional<CubeTextureData>& data)>& /*onLoad*/,
  const std::function<void(const std::string& message, const std::string& exception)>& /*onError*/)
{
  throw std::runtime_error(".env not supported in Cube.");
}

void _TGATextureLoader::loadCubeData(
  const std::vector<std::variant<std::string, ArrayBufferView>>& /*data*/,
  const InternalTexturePtr& /*texture*/, bool /*createPolynomials*/,
  const std::function<void(const std::optional<CubeTextureData>& data)>& /*onLoad*/,
  const std::function<void(const std::string& message, const std::string& exception)>& /*onError*/)
{
  throw std::runtime_error(".env not supported in Cube.");
}

void _TGATextureLoader::loadData(
  const ArrayBufferView& data, const InternalTexturePtr& texture,
  const std::function<void(int width, int height, bool loadMipmap, bool isCompressed,
                           const std::function<void()>& done, bool loadFailed)>& callback)
{
  const auto bytes = stl_util::to_array<uint8_t>(data.buffer(), data.byteOffset, data.byteLength());

  auto header = TGATools::GetTGAHeader(bytes);
  callback(
    header.width, header.height, texture->generateMipMaps, false,
    [&texture, &bytes]() -> void { TGATools::UploadContent(texture, bytes); }, false);
}

} // end of namespace BABYLON
