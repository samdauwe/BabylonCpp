#include <babylon/materials/textures/raw_texture.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

RawTexture::RawTexture(const Uint8Array& data, int width, int height,
                       unsigned int iFormat, Scene* scene, bool generateMipMaps,
                       bool invertY, unsigned int samplingMode,
                       unsigned int iType)
    : Texture{nullptr, scene, !generateMipMaps, invertY}
    , format{iFormat}
    , wrapU{TextureConstants::CLAMP_ADDRESSMODE}
    , wrapV{TextureConstants::CLAMP_ADDRESSMODE}
    , type{iType}
{
  _engine  = scene->getEngine();
  _texture = scene->getEngine()->createRawTexture(data, width, height, format,
                                                  generateMipMaps, invertY,
                                                  samplingMode, nullptr, iType);

  _texture = scene->getEngine()->createRawTexture(
    data, width, height, iFormat, generateMipMaps, invertY, samplingMode);
}

RawTexture::~RawTexture()
{
}

void RawTexture::update(const Uint8Array& data)
{
  _engine->updateRawTexture(_texture, data, format, _invertY);
}

unique_ptr_t<RawTexture> RawTexture::CreateLuminanceTexture(
  const Uint8Array& data, int width, int height, Scene* scene,
  bool generateMipMaps, bool invertY, unsigned int samplingMode)
{
  return ::std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_LUMINANCE, scene,
    generateMipMaps, invertY, samplingMode);
}

unique_ptr_t<RawTexture> RawTexture::CreateLuminanceAlphaTexture(
  const Uint8Array& data, int width, int height, Scene* scene,
  bool generateMipMaps, bool invertY, unsigned int samplingMode)
{
  return ::std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_LUMINANCE_ALPHA, scene,
    generateMipMaps, invertY, samplingMode);
}

unique_ptr_t<RawTexture>
RawTexture::CreateAlphaTexture(const Uint8Array& data, int width, int height,
                               Scene* scene, bool generateMipMaps, bool invertY,
                               unsigned int samplingMode)
{
  return ::std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_ALPHA, scene,
    generateMipMaps, invertY, samplingMode);
}

unique_ptr_t<RawTexture>
RawTexture::CreateRGBTexture(const Uint8Array& data, int width, int height,
                             Scene* scene, bool generateMipMaps, bool invertY,
                             unsigned int samplingMode, unsigned int type)
{
  return ::std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_RGB, scene,
    generateMipMaps, invertY, samplingMode, type);
}

unique_ptr_t<RawTexture>
RawTexture::CreateRGBATexture(const Uint8Array& data, int width, int height,
                              Scene* scene, bool generateMipMaps, bool invertY,
                              unsigned int samplingMode, unsigned int type)
{
  return ::std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_RGBA, scene,
    generateMipMaps, invertY, samplingMode, type);
}

} // end of namespace BABYLON
