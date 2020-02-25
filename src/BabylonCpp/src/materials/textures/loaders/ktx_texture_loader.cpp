#include <babylon/materials/textures/loaders/ktx_texture_loader.h>

#include <babylon/engines/engine.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/misc/khronos_texture_container.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

_KTXTextureLoader::_KTXTextureLoader() = default;

_KTXTextureLoader::~_KTXTextureLoader() = default;

bool _KTXTextureLoader::supportCascades() const
{
  return false;
}

bool _KTXTextureLoader::canLoad(const std::string& /*extension*/,
                                const std::string& textureFormatInUse,
                                const InternalTexturePtr& fallback, bool isBase64, bool isBuffer)
{
  return !textureFormatInUse.empty() && !isBase64 && !fallback && !isBuffer;
}

std::string _KTXTextureLoader::transformUrl(const std::string& rootUrl,
                                            const std::string& textureFormatInUse)
{
  auto lastDot = StringTools::lastIndexOf(rootUrl, ".");
  if (lastDot != -1 && rootUrl.substr(static_cast<size_t>(lastDot + 1)) == "ktx") {
    // Already transformed
    return rootUrl;
  }
  return (lastDot > -1 ? rootUrl.substr(0, static_cast<size_t>(lastDot)) : rootUrl)
         + textureFormatInUse;
}

std::string _KTXTextureLoader::getFallbackTextureUrl(const std::string& rootUrl,
                                                     const std::string& textureFormatInUse)
{
  // remove the format appended to the rootUrl in the original createCubeTexture call.
  const std::regex regex(textureFormatInUse, std::regex::optimize);
  return StringTools::regexReplace(rootUrl, regex, [](const std::smatch& /*m*/) { return ""; });
}

void _KTXTextureLoader::loadCubeData(
  const std::variant<std::string, ArrayBufferView>& iData, const InternalTexturePtr& texture,
  bool /*createPolynomials*/,
  const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& /*onError*/)
{
  if (!std::holds_alternative<ArrayBufferView>(iData)) {
    return;
  }
  auto data = std::get<ArrayBufferView>(iData);

  // Need to invert vScale as invertY via UNPACK_FLIP_Y_WEBGL is not supported by compressed texture
  texture->_invertVScale = !texture->invertY;
  auto engine            = texture->getEngine();
  KhronosTextureContainer ktx(data, 6);

  auto loadMipmap = ktx.numberOfMipmapLevels > 1 && texture->generateMipMaps;

  engine->_unpackFlipY(true);

  ktx.uploadLevels(texture, texture->generateMipMaps);

  texture->width  = static_cast<int>(ktx.pixelWidth);
  texture->height = static_cast<int>(ktx.pixelHeight);

  engine->_setCubeMapTextureParams(loadMipmap);
  texture->isReady = true;
  texture->onLoadedObservable.notifyObservers(texture.get());
  texture->onLoadedObservable.clear();

  if (onLoad) {
    onLoad(std::nullopt);
  }
}

void _KTXTextureLoader::loadCubeData(
  const std::vector<std::variant<std::string, ArrayBufferView>>& /*data*/,
  const InternalTexturePtr& /*texture*/, bool /*createPolynomials*/,
  const std::function<void(const std::optional<CubeTextureData>& data)>& /*onLoad*/,
  const std::function<void(const std::string& message, const std::string& exception)>& /*onError*/)
{
}

void _KTXTextureLoader::loadData(
  const ArrayBufferView& data, const InternalTexturePtr& texture,
  const std::function<void(int width, int height, bool loadMipmap, bool isCompressed,
                           const std::function<void()>& done, bool loadFailed)>& callback)
{
  // Need to invert vScale as invertY via UNPACK_FLIP_Y_WEBGL is not supported by compressed texture
  texture->_invertVScale = !texture->invertY;
  KhronosTextureContainer ktx(data, 1);

  callback(
    static_cast<int>(ktx.pixelWidth), static_cast<int>(ktx.pixelHeight), texture->generateMipMaps,
    true, [&ktx, &texture]() -> void { ktx.uploadLevels(texture, texture->generateMipMaps); },
    ktx.isInvalid);
}

} // end of namespace BABYLON
