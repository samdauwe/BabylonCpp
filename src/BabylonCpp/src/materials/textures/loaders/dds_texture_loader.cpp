#include <babylon/materials/textures/loaders/dds_texture_loader.h>

#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/textures/internal_texture.h>
#include <babylon/tools/dds.h>

namespace BABYLON {

DDSTextureLoader::DDSTextureLoader()
{
}

DDSTextureLoader::~DDSTextureLoader()
{
}

bool DDSTextureLoader::supportCascades() const
{
  return true;
}

bool DDSTextureLoader::canLoad(const std::string& extension,
                               const std::string& /*textureFormatInUse*/,
                               const InternalTexturePtr& /*fallback*/,
                               bool /*isBase64*/, bool /*isBuffer*/)
{
  return String::endsWith(extension, ".dds");
}

std::string
DDSTextureLoader::transformUrl(const std::string& rootUrl,
                               const std::string& /*textureFormatInUse*/)
{
  return rootUrl;
}

std::string DDSTextureLoader::getFallbackTextureUrl(
  const std::string& /*rootUrl*/, const std::string& /*textureFormatInUse*/)
{
  return "";
}

void DDSTextureLoader::loadCubeData(
  const std::variant<std::string, ArrayBuffer>& img,
  const InternalTexturePtr& texture, bool createPolynomials,
  const std::function<void(const CubeTextureData& data)>& onLoad,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& /*onError*/)
{
  auto engine = texture->getEngine();

  auto data = img;
  auto info = DDSTools::GetDDSInfo(data);

  texture->width  = info.width;
  texture->height = info.height;

  if (createPolynomials) {
    info.sphericalPolynomial = std::make_shared<SphericalPolynomial>();
  }

  auto loadMipmap = (info.isRGB || info.isLuminance || info.mipmapCount > 1)
                    && texture->generateMipMaps;
  engine->_unpackFlipY(info.isCompressed);

  DDSTools::UploadDDSLevels(engine, texture, data, info, loadMipmap, 6);

  if (!info.isFourCC && info.mipmapCount == 1) {
    engine->generateMipMapsForCubemap(texture);
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

void DDSTextureLoader::loadCubeData(
  const std::vector<std::variant<std::string, ArrayBuffer>>& imgs,
  const InternalTexturePtr& texture, bool createPolynomials,
  const std::function<void(const CubeTextureData& data)>& onLoad,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& /*onError*/)
{
  auto engine = texture->getEngine();
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

    loadMipmap = (info.isRGB || info.isLuminance || info.mipmapCount > 1)
                 && texture->generateMipMaps;
    engine->_unpackFlipY(info.isCompressed);

    DDSTools::UploadDDSLevels(engine, texture, data, info, loadMipmap, 6);

    if (!info.isFourCC && info.mipmapCount == 1) {
      engine->generateMipMapsForCubemap(texture);
    }
  }

  engine->_setCubeMapTextureParams(loadMipmap);
  texture->isReady = true;

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

void DDSTextureLoader::loadData(
  const ArrayBuffer& data, const InternalTexturePtr& texture,
  const std::function<void(int width, int height, bool loadMipmap,
                           bool isCompressed,
                           const std::function<void()>& done)>& callback)
{
  auto info = DDSTools::GetDDSInfo(data);

  auto loadMipmap = (info.isRGB || info.isLuminance || info.mipmapCount > 1)
                    && texture->generateMipMaps
                    && ((info.width >> (info.mipmapCount - 1)) == 1);
  callback(info.width, info.height, loadMipmap, info.isFourCC,
           [&texture, &data, &info, &loadMipmap]() {
             DDSTools::UploadDDSLevels(texture->getEngine(), texture, data,
                                       info, loadMipmap, 1);
           });
}

} // end of namespace BABYLON
