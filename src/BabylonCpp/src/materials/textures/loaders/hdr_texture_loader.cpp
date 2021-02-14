#include <babylon/materials/textures/loaders/hdr_texture_loader.h>

#include <babylon/engines/constants.h>
#include <babylon/engines/thin_engine.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/misc/highdynamicrange/hdr_tools.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

_HDRTextureLoader::_HDRTextureLoader() = default;

_HDRTextureLoader::~_HDRTextureLoader() = default;

bool _HDRTextureLoader::supportCascades() const
{
  return false;
}

bool _HDRTextureLoader::canLoad(const std::string& extension, const std::string& /*mimeType*/)
{
  return StringTools::endsWith(extension, ".hdr");
}

void _HDRTextureLoader::loadCubeData(
  const std::variant<std::string, ArrayBufferView>& /*iData*/,
  const InternalTexturePtr& /*texture*/, bool /*createPolynomials*/,
  const std::function<void(const std::optional<CubeTextureData>& data)>& /*onLoad*/,
  const std::function<void(const std::string& message, const std::string& exception)>& /*onError*/)
{
  throw std::runtime_error(".env not supported in Cube.");
}

void _HDRTextureLoader::loadCubeData(
  const std::vector<std::variant<std::string, ArrayBufferView>>& /*data*/,
  const InternalTexturePtr& /*texture*/, bool /*createPolynomials*/,
  const std::function<void(const std::optional<CubeTextureData>& data)>& /*onLoad*/,
  const std::function<void(const std::string& message, const std::string& exception)>& /*onError*/)
{
  throw std::runtime_error(".env not supported in Cube.");
}

void _HDRTextureLoader::loadData(
  const ArrayBufferView& data, const InternalTexturePtr& texture,
  const std::function<void(int width, int height, bool loadMipmap, bool isCompressed,
                           const std::function<void()>& done, bool loadFailed)>& callback)
{
  const auto uint8array = data.uint8Array();
  const auto hdrInfo    = HDRTools::RGBE_ReadHeader(uint8array);
  auto pixelsDataRGB32  = HDRTools::RGBE_ReadPixels(uint8array, hdrInfo);

  const auto pixels     = hdrInfo.width * hdrInfo.height;
  auto pixelsDataRGBA32 = Float32Array(pixels * 4);
  for (size_t i = 0; i < pixels; i += 1) {
    pixelsDataRGBA32[i * 4]     = pixelsDataRGB32[i * 3];
    pixelsDataRGBA32[i * 4 + 1] = pixelsDataRGB32[i * 3 + 1];
    pixelsDataRGBA32[i * 4 + 2] = pixelsDataRGB32[i * 3 + 2];
    pixelsDataRGBA32[i * 4 + 3] = 1;
  }

  callback(
    static_cast<int>(hdrInfo.width), static_cast<int>(hdrInfo.height), texture->generateMipMaps,
    false,
    [texture, pixelsDataRGBA32]() -> void {
      const auto engine    = texture->getEngine();
      texture->type        = Constants::TEXTURETYPE_FLOAT;
      texture->format      = Constants::TEXTUREFORMAT_RGBA;
      texture->_gammaSpace = false;
      engine->_uploadDataToTextureDirectly(texture, pixelsDataRGBA32);
    },
    false);
}

} // end of namespace BABYLON
