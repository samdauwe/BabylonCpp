#include <babylon/materials/textures/loaders/ktx_texture_loader.h>

#include <babylon/core/logging.h>
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

bool _KTXTextureLoader::canLoad(const std::string& extension)
{
  // The ".ktx2" file extension is still up for debate:
  // https://github.com/KhronosGroup/KTX-Specification/issues/18
  return StringTools::endsWith(extension, ".ktx") || StringTools::endsWith(extension, ".ktx2");
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
  if (KhronosTextureContainer::IsValid(data)) {
    // Need to invert vScale as invertY via UNPACK_FLIP_Y_WEBGL is not supported by compressed
    // texture
    texture->_invertVScale = !texture->invertY;
    KhronosTextureContainer ktx(data, 1);

    callback(
      static_cast<int>(ktx.pixelWidth), static_cast<int>(ktx.pixelHeight), texture->generateMipMaps,
      true, [&ktx, &texture]() -> void { ktx.uploadLevels(texture, texture->generateMipMaps); },
      ktx.isInvalid);
  }
  else {
    BABYLON_LOG_ERROR("_KTXTextureLoader", "texture missing KTX identifier")
    callback(
      0, 0, false, false, []() -> void {}, true);
  }
}

} // end of namespace BABYLON
