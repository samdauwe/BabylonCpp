#include <babylon/materials/textures/raw_texture.h>

#include <babylon/core/array_buffer_view.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

RawTexture::RawTexture(const ArrayBufferView& data, int width, int height,
                       unsigned int iFormat, Scene* scene, bool generateMipMaps,
                       bool iInvertY, unsigned int iSamplingMode,
                       unsigned int iType)
    : Texture{"", scene, !generateMipMaps, iInvertY}
    , format{iFormat}
    , type{iType}
{
  _engine  = scene->getEngine();
  _texture = scene->getEngine()->createRawTexture(
    data.uint8Array, width, height, format, generateMipMaps, iInvertY,
    iSamplingMode, "", iType);

  wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  wrapV = TextureConstants::CLAMP_ADDRESSMODE;
}

RawTexture::~RawTexture()
{
}

void RawTexture::update(const ArrayBufferView& data)
{
  if (_texture) {
    _engine->updateRawTexture(_texture, data.uint8Array, _texture->format,
                              _texture->invertY, "", _texture->type);
  }
}

std::unique_ptr<RawTexture> RawTexture::CreateLuminanceTexture(
  const ArrayBufferView& data, int width, int height, Scene* scene,
  bool generateMipMaps, bool iInvertY, unsigned int iSamplingMode)
{
  return std::make_unique<RawTexture>(data, width, height,
                                      Constants::TEXTUREFORMAT_LUMINANCE, scene,
                                      generateMipMaps, iInvertY, iSamplingMode);
}

std::unique_ptr<RawTexture> RawTexture::CreateLuminanceAlphaTexture(
  const ArrayBufferView& data, int width, int height, Scene* scene,
  bool generateMipMaps, bool iInvertY, unsigned int iSamplingMode)
{
  return std::make_unique<RawTexture>(
    data, width, height, Constants::TEXTUREFORMAT_LUMINANCE_ALPHA, scene,
    generateMipMaps, iInvertY, iSamplingMode);
}

std::unique_ptr<RawTexture>
RawTexture::CreateAlphaTexture(const ArrayBufferView& data, int width,
                               int height, Scene* scene, bool generateMipMaps,
                               bool iInvertY, unsigned int iSamplingMode)
{
  return std::make_unique<RawTexture>(data, width, height,
                                      Constants::TEXTUREFORMAT_ALPHA, scene,
                                      generateMipMaps, iInvertY, iSamplingMode);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRGBTexture(const ArrayBufferView& data, int width, int height,
                             Scene* scene, bool generateMipMaps, bool iInvertY,
                             unsigned int iSamplingMode, unsigned int type)
{
  return std::make_unique<RawTexture>(
    data, width, height, Constants::TEXTUREFORMAT_RGB, scene, generateMipMaps,
    iInvertY, iSamplingMode, type);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRGBATexture(const ArrayBufferView& data, int width,
                              int height, Scene* scene, bool generateMipMaps,
                              bool iInvertY, unsigned int iSamplingMode,
                              unsigned int type)
{
  return std::make_unique<RawTexture>(
    data, width, height, Constants::TEXTUREFORMAT_RGBA, scene, generateMipMaps,
    iInvertY, iSamplingMode, type);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRTexture(const ArrayBufferView& data, int width, int height,
                           Scene* scene, bool generateMipMaps, bool iInvertY,
                           unsigned int iSamplingMode, unsigned int type)
{
  return std::make_unique<RawTexture>(
    data, width, height, Constants::TEXTUREFORMAT_R, scene, generateMipMaps,
    iInvertY, iSamplingMode, type);
}

} // end of namespace BABYLON
