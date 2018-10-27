#include <babylon/materials/textures/raw_texture.h>

#include <babylon/core/array_buffer_view.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

RawTexture::RawTexture(const ArrayBufferView& data, int width, int height,
                       unsigned int iFormat, Scene* scene, bool generateMipMaps,
                       bool invertY, unsigned int samplingMode,
                       unsigned int iType)
    : Texture{nullptr, scene, !generateMipMaps, invertY}
    , format{iFormat}
    , type{iType}
{
  _engine  = scene->getEngine();
  _texture = scene->getEngine()->createRawTexture(
    data.uint8Array, width, height, format, generateMipMaps, invertY,
    samplingMode, nullptr, iType);

  _texture = scene->getEngine()->createRawTexture(
    data.uint8Array, width, height, iFormat, generateMipMaps, invertY,
    samplingMode);

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
  bool generateMipMaps, bool invertY, unsigned int samplingMode)
{
  return std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_LUMINANCE, scene,
    generateMipMaps, invertY, samplingMode);
}

std::unique_ptr<RawTexture> RawTexture::CreateLuminanceAlphaTexture(
  const ArrayBufferView& data, int width, int height, Scene* scene,
  bool generateMipMaps, bool invertY, unsigned int samplingMode)
{
  return std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_LUMINANCE_ALPHA, scene,
    generateMipMaps, invertY, samplingMode);
}

std::unique_ptr<RawTexture>
RawTexture::CreateAlphaTexture(const ArrayBufferView& data, int width,
                               int height, Scene* scene, bool generateMipMaps,
                               bool invertY, unsigned int samplingMode)
{
  return std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_ALPHA, scene,
    generateMipMaps, invertY, samplingMode);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRGBTexture(const ArrayBufferView& data, int width, int height,
                             Scene* scene, bool generateMipMaps, bool invertY,
                             unsigned int samplingMode, unsigned int type)
{
  return std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_RGB, scene,
    generateMipMaps, invertY, samplingMode, type);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRGBATexture(const ArrayBufferView& data, int width,
                              int height, Scene* scene, bool generateMipMaps,
                              bool invertY, unsigned int samplingMode,
                              unsigned int type)
{
  return std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_RGBA, scene,
    generateMipMaps, invertY, samplingMode, type);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRTexture(const ArrayBufferView& data, int width, int height,
                           Scene* scene, bool generateMipMaps, bool invertY,
                           unsigned int samplingMode, unsigned int type)
{
  return std::make_unique<RawTexture>(
    data, width, height, EngineConstants::TEXTUREFORMAT_R, scene,
    generateMipMaps, invertY, samplingMode, type);
}

} // end of namespace BABYLON
