#include <babylon/materials/textures/raw_texture.h>

#include <babylon/core/array_buffer_view.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/internal_texture.h>

namespace BABYLON {

RawTexture::RawTexture(const ArrayBufferView& data, int width, int height, unsigned int iFormat,
                       const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine,
                       bool generateMipMaps, bool iInvertY, unsigned int iSamplingMode,
                       unsigned int iType)
    : Texture{"", sceneOrEngine, !generateMipMaps, iInvertY}, format{iFormat}, type{iType}
{
  if (!_engine) {
    return;
  }

  _texture = _engine->createRawTexture(data.uint8Array(), width, height, format, generateMipMaps,
                                       iInvertY, iSamplingMode, "", iType);

  wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  wrapV = TextureConstants::CLAMP_ADDRESSMODE;
}

RawTexture::~RawTexture() = default;

void RawTexture::update(const ArrayBufferView& data)
{
  if (_texture) {
    _getEngine()->updateRawTexture(_texture, data.uint8Array(), _texture->format, _texture->invertY,
                                   "", _texture->type);
  }
}

std::unique_ptr<RawTexture> RawTexture::CreateLuminanceTexture(
  const ArrayBufferView& data, int width, int height,
  const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine, bool generateMipMaps,
  bool iInvertY, unsigned int iSamplingMode)
{
  return std::make_unique<RawTexture>(data, width, height, Constants::TEXTUREFORMAT_LUMINANCE,
                                      sceneOrEngine, generateMipMaps, iInvertY, iSamplingMode);
}

std::unique_ptr<RawTexture> RawTexture::CreateLuminanceAlphaTexture(
  const ArrayBufferView& data, int width, int height,
  const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine, bool generateMipMaps,
  bool iInvertY, unsigned int iSamplingMode)
{
  return std::make_unique<RawTexture>(data, width, height, Constants::TEXTUREFORMAT_LUMINANCE_ALPHA,
                                      sceneOrEngine, generateMipMaps, iInvertY, iSamplingMode);
}

std::unique_ptr<RawTexture> RawTexture::CreateAlphaTexture(
  const ArrayBufferView& data, int width, int height,
  const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine, bool generateMipMaps,
  bool iInvertY, unsigned int iSamplingMode)
{
  return std::make_unique<RawTexture>(data, width, height, Constants::TEXTUREFORMAT_ALPHA,
                                      sceneOrEngine, generateMipMaps, iInvertY, iSamplingMode);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRGBTexture(const ArrayBufferView& data, int width, int height,
                             const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine,
                             bool generateMipMaps, bool iInvertY, unsigned int iSamplingMode,
                             unsigned int type)
{
  return std::make_unique<RawTexture>(data, width, height, Constants::TEXTUREFORMAT_RGB,
                                      sceneOrEngine, generateMipMaps, iInvertY, iSamplingMode,
                                      type);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRGBATexture(const ArrayBufferView& data, int width, int height,
                              const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine,
                              bool generateMipMaps, bool iInvertY, unsigned int iSamplingMode,
                              unsigned int type)
{
  return std::make_unique<RawTexture>(data, width, height, Constants::TEXTUREFORMAT_RGBA,
                                      sceneOrEngine, generateMipMaps, iInvertY, iSamplingMode,
                                      type);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRTexture(const ArrayBufferView& data, int width, int height,
                           const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine,
                           bool generateMipMaps, bool iInvertY, unsigned int iSamplingMode,
                           unsigned int type)
{
  return std::make_unique<RawTexture>(data, width, height, Constants::TEXTUREFORMAT_R,
                                      sceneOrEngine, generateMipMaps, iInvertY, iSamplingMode,
                                      type);
}

} // end of namespace BABYLON
