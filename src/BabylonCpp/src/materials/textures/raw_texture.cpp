#include <babylon/materials/textures/raw_texture.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

RawTexture::RawTexture(const Uint8Array& data, int width, int height,
                       int iFormat, Scene* scene, bool generateMipMaps,
                       bool invertY, unsigned int samplingMode)
    : Texture{nullptr, scene, !generateMipMaps, invertY}
    , format{iFormat}
    , wrapU{Texture::CLAMP_ADDRESSMODE}
    , wrapV{Texture::CLAMP_ADDRESSMODE}
{
  _texture = scene->getEngine()->createRawTexture(
    data, width, height, format, generateMipMaps, invertY, samplingMode);
}

RawTexture::~RawTexture()
{
}

void RawTexture::update(const Uint8Array& data)
{
  getScene()->getEngine()->updateRawTexture(_texture, data, format, _invertY);
}

std::unique_ptr<RawTexture> RawTexture::CreateLuminanceTexture(
  const Uint8Array& data, int width, int height, Scene* scene,
  bool generateMipMaps, bool invertY, unsigned int samplingMode)
{
  return std_util::make_unique<RawTexture>(
    data, width, height, Engine::TEXTUREFORMAT_LUMINANCE, scene,
    generateMipMaps, invertY, samplingMode);
}

std::unique_ptr<RawTexture> RawTexture::CreateLuminanceAlphaTexture(
  const Uint8Array& data, int width, int height, Scene* scene,
  bool generateMipMaps, bool invertY, unsigned int samplingMode)
{
  return std_util::make_unique<RawTexture>(
    data, width, height, Engine::TEXTUREFORMAT_LUMINANCE_ALPHA, scene,
    generateMipMaps, invertY, samplingMode);
}

std::unique_ptr<RawTexture>
RawTexture::CreateAlphaTexture(const Uint8Array& data, int width, int height,
                               Scene* scene, bool generateMipMaps, bool invertY,
                               unsigned int samplingMode)
{
  return std_util::make_unique<RawTexture>(
    data, width, height, Engine::TEXTUREFORMAT_ALPHA, scene, generateMipMaps,
    invertY, samplingMode);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRGBTexture(const Uint8Array& data, int width, int height,
                             Scene* scene, bool generateMipMaps, bool invertY,
                             unsigned int samplingMode)
{
  return std_util::make_unique<RawTexture>(
    data, width, height, Engine::TEXTUREFORMAT_RGB, scene, generateMipMaps,
    invertY, samplingMode);
}

std::unique_ptr<RawTexture>
RawTexture::CreateRGBATexture(const Uint8Array& data, int width, int height,
                              Scene* scene, bool generateMipMaps, bool invertY,
                              unsigned int samplingMode)
{
  return std_util::make_unique<RawTexture>(
    data, width, height, Engine::TEXTUREFORMAT_RGBA, scene, generateMipMaps,
    invertY, samplingMode);
}

} // end of namespace BABYLON
