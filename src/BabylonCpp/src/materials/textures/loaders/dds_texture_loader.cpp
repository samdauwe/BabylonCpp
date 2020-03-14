#include <babylon/materials/textures/loaders/dds_texture_loader.h>

#include <babylon/engines/engine.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/misc/dds.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

_DDSTextureLoader::_DDSTextureLoader() = default;

_DDSTextureLoader::~_DDSTextureLoader() = default;

bool _DDSTextureLoader::supportCascades() const
{
  return true;
}

bool _DDSTextureLoader::canLoad(const std::string& extension)
{
  return StringTools::endsWith(extension, ".dds");
}

void _DDSTextureLoader::loadCubeData(
  const std::variant<std::string, ArrayBufferView>& img, const InternalTexturePtr& texture,
  bool createPolynomials,
  const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& /*onError*/)
{
  auto engine = texture->getEngine();

  auto data = img;
  auto info = DDSTools::GetDDSInfo(data);

  texture->width  = info.width;
  texture->height = info.height;

  if (createPolynomials) {
    info.sphericalPolynomial = std::make_shared<SphericalPolynomial>();
  }

  auto loadMipmap
    = (info.isRGB || info.isLuminance || info.mipmapCount > 1) && texture->generateMipMaps;
  engine->_unpackFlipY(info.isCompressed);

  DDSTools::UploadDDSLevels(engine, texture, data, info, loadMipmap, 6);

  if (!info.isFourCC && info.mipmapCount == 1) {
    auto _engine = static_cast<Engine*>(engine);
    if (_engine) {
      _engine->generateMipMapsForCubemap(texture);
    }
  }

  engine->_setCubeMapTextureParams(loadMipmap);
  texture->isReady = true;

  if (onLoad) {
    onLoad(CubeTextureData{
      true,           // isDDS
      texture->width, // width
      info,           // info
      {img},          // data
      texture,        // texture
    });
  }
}

void _DDSTextureLoader::loadCubeData(
  const std::vector<std::variant<std::string, ArrayBufferView>>& imgs,
  const InternalTexturePtr& texture, bool createPolynomials,
  const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
  const std::function<void(const std::string& message, const std::string& exception)>& /*onError*/)
{
  auto engine = static_cast<Engine*>(texture->getEngine());
  if (!engine) {
    return;
  }
  DDSInfo info;
  bool loadMipmap = false;
  for (const auto& img : imgs) {
    auto data = img;
    info      = DDSTools::GetDDSInfo(data);

    texture->width  = info.width;
    texture->height = info.height;

    if (createPolynomials) {
      info.sphericalPolynomial = std::make_shared<SphericalPolynomial>();
    }

    loadMipmap
      = (info.isRGB || info.isLuminance || info.mipmapCount > 1) && texture->generateMipMaps;
    engine->_unpackFlipY(info.isCompressed);

    DDSTools::UploadDDSLevels(engine, texture, data, info, loadMipmap, 6);

    if (!info.isFourCC && info.mipmapCount == 1) {
      // Do not unbind as we still need to set the parameters.
      engine->generateMipMapsForCubemap(texture, false);
    }
  }

  engine->_setCubeMapTextureParams(loadMipmap);
  texture->isReady = true;
  texture->onLoadedObservable.notifyObservers(texture.get());
  texture->onLoadedObservable.clear();

  if (onLoad) {
    onLoad(CubeTextureData{
      true,           // isDDS
      texture->width, // width
      info,           // info
      imgs,           // data
      texture,        // texture
    });
  }
}

void _DDSTextureLoader::loadData(
  const ArrayBufferView& data, const InternalTexturePtr& texture,
  const std::function<void(int width, int height, bool loadMipmap, bool isCompressed,
                           const std::function<void()>& done, bool loadFailed)>& callback)
{
  auto info = DDSTools::GetDDSInfo(data);

  auto loadMipmap = (info.isRGB || info.isLuminance || info.mipmapCount > 1)
                    && texture->generateMipMaps && ((info.width >> (info.mipmapCount - 1)) == 1);
  callback(
    info.width, info.height, loadMipmap, info.isFourCC,
    [&texture, &data, &info, &loadMipmap]() {
      DDSTools::UploadDDSLevels(texture->getEngine(), texture, data, info, loadMipmap, 1);
    },
    false);
}

} // end of namespace BABYLON
